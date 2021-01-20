#pragma once

#include "CoreLib/File.h"
#include "CoreLib/MetroBoyCPU.h"
#include "CoreLib/Constants.h"

#include "GateBoyLib/Probe.h"

#include "GateBoyLib/GateBoyBootrom.h"
#include "GateBoyLib/GateBoyBuses.h"
#include "GateBoyLib/GateBoyTimer.h"
#include "GateBoyLib/GateBoyLCD.h"
#include "GateBoyLib/GateBoyDMA.h"
#include "GateBoyLib/GateBoySpriteStore.h"
#include "GateBoyLib/GateBoyClocks.h"
#include "GateBoyLib/GateBoyPixPipe.h"
#include "GateBoyLib/GateBoyJoypad.h"
#include "GateBoyLib/GateBoySerial.h"
#include "GateBoyLib/GateBoyInterrupts.h"
#include "GateBoyLib/GateBoySpriteFetcher.h"
#include "GateBoyLib/GateBoyTileFetcher.h"
#include "GateBoyLib/GateBoySpriteScanner.h"
#include "GateBoyLib/GateBoyExtBus.h"
#include "GateBoyLib/GateBoyOamBus.h"
#include "GateBoyLib/GateBoyVramBus.h"
#include "GateBoyLib/GateBoyZramBus.h"
#include "GateBoyLib/GateBoyResetDebug.h"
#include "GateBoyLib/GateBoyCpuBus.h"

//-----------------------------------------------------------------------------

#pragma pack(push, 1)
struct GateBoy {

  void dump(Dumper& d) const;

  void reset_to_bootrom(bool fastboot);
  void reset_to_cart();
  void load_cart(uint8_t* _boot_buf, size_t _boot_size,
                uint8_t* _cart_buf, size_t _cart_size);

  //----------------------------------------

  void from_blob(const blob& b) {
    ASSERT_P(b.size() == sizeof(*this));
    memcpy(this, b.data(), sizeof(*this));
    check_sentinel();
  }

  void to_blob(blob& b) {
    uint8_t* bytes = (uint8_t*)this;
    b.insert(b.end(), bytes, bytes + sizeof(*this));
  }

  //----------------------------------------

  void check_div() const {
    int div_val = div.get_div();
    if (div_val != BOOT_DIV) {
      printf("div fail!\n");
      *reinterpret_cast<int*>(SENTINEL4) = 1;
    }
  }

  static bool check_sentinel(const blob& b) {
    if (b.size() != sizeof(GateBoy)) return false;

    GateBoy* gb = (GateBoy*)b.data();
    if (gb->sentinel1 != SENTINEL1) return false;
    if (gb->sentinel2 != SENTINEL2) return false;
    if (gb->sentinel3 != SENTINEL3) return false;
    if (gb->sentinel4 != SENTINEL4) return false;
    return true;
  }

  void check_sentinel() const {
    ASSERT_P(sentinel1 == SENTINEL1);
    ASSERT_P(sentinel2 == SENTINEL2);
    ASSERT_P(sentinel3 == SENTINEL3);
    ASSERT_P(sentinel4 == SENTINEL4);
  }

  uint8_t dbg_read (int addr);
  void dbg_write(int addr, uint8_t data);

  void set_boot_bit() {
    dbg_write(0xFF50, 0xFF);
  }

  //-----------------------------------------------------------------------------

  void run_phases(int phase_count) {
    for (int i = 0; i < phase_count; i++) {
      next_phase();
    }
  }

  void next_phase();

  void tock_slow(int pass_index);

  void update_framebuffer();

  uint8_t* reg_begin() { return (uint8_t*)(&sentinel1) + sizeof(sentinel1); }
  uint8_t* reg_end()   { return (uint8_t*)(&sentinel2); }

  int64_t commit_and_hash() {
    return ::commit_and_hash(reg_begin(), reg_end() - reg_begin());
  }

  void set_old_bits() {
    uint8_t* b = reg_begin();
    size_t s = reg_end() - reg_begin();
    for (size_t i = 0; i < s; i++) {
      b[i] |= 0b00010000;
    }
  }

  static wire ROGE_WY_MATCHp_old(const RegWY& reg_wy_old, const RegLY& reg_ly_old, DFF9 WYMO_LCDC_WINENn_old) {
    /*#p27.NAZE*/ wire _NAZE_WY_MATCH0p = xnor2(reg_wy_old.NESO_WY0n.qn_old(), reg_ly_old.MUWY_LY0p.qp_old());
    /* p27.PEBO*/ wire _PEBO_WY_MATCH1p = xnor2(reg_wy_old.NYRO_WY1n.qn_old(), reg_ly_old.MYRO_LY1p.qp_old());
    /* p27.POMO*/ wire _POMO_WY_MATCH2p = xnor2(reg_wy_old.NAGA_WY2n.qn_old(), reg_ly_old.LEXA_LY2p.qp_old());
    /* p27.NEVU*/ wire _NEVU_WY_MATCH3p = xnor2(reg_wy_old.MELA_WY3n.qn_old(), reg_ly_old.LYDO_LY3p.qp_old());
    /* p27.NOJO*/ wire _NOJO_WY_MATCH4p = xnor2(reg_wy_old.NULO_WY4n.qn_old(), reg_ly_old.LOVU_LY4p.qp_old());
    /* p27.PAGA*/ wire _PAGA_WY_MATCH5p = xnor2(reg_wy_old.NENE_WY5n.qn_old(), reg_ly_old.LEMA_LY5p.qp_old());
    /* p27.PEZO*/ wire _PEZO_WY_MATCH6p = xnor2(reg_wy_old.NUKA_WY6n.qn_old(), reg_ly_old.MATO_LY6p.qp_old());
    /* p27.NUPA*/ wire _NUPA_WY_MATCH7p = xnor2(reg_wy_old.NAFU_WY7n.qn_old(), reg_ly_old.LAFO_LY7p.qp_old());

    /*#p27.PALO*/ wire _PALO_WY_MATCHn  = nand5(WYMO_LCDC_WINENn_old.qn_old(), _NOJO_WY_MATCH4p, _PAGA_WY_MATCH5p, _PEZO_WY_MATCH6p, _NUPA_WY_MATCH7p);
    /* p27.NELE*/ wire _NELE_WY_MATCHp  = not1(_PALO_WY_MATCHn);
    /* p27.PAFU*/ wire _PAFU_WY_MATCHn  = nand5(_NELE_WY_MATCHp, _NAZE_WY_MATCH0p, _PEBO_WY_MATCH1p, _POMO_WY_MATCH2p, _NEVU_WY_MATCH3p);
    /* p27.ROGE*/ wire _ROGE_WY_MATCHp  = not1(_PAFU_WY_MATCHn);
    return _ROGE_WY_MATCHp;
  }

  static wire NUKO_WX_MATCHp_old(const PixCount& pix_count_old, const RegWX& reg_wx_old, NorLatch REJO_WY_MATCH_LATCHp_old) {
    /*#p27.MYLO*/ wire _MYLO_WX_MATCH0p = xnor2(pix_count_old.XEHO_PX0p.qp_old(), reg_wx_old.MYPA_WX0n.qn_old());
    /* p27.PUWU*/ wire _PUWU_WX_MATCH1p = xnor2(pix_count_old.SAVY_PX1p.qp_old(), reg_wx_old.NOFE_WX1n.qn_old());
    /* p27.PUHO*/ wire _PUHO_WX_MATCH2p = xnor2(pix_count_old.XODU_PX2p.qp_old(), reg_wx_old.NOKE_WX2n.qn_old());
    /* p27.NYTU*/ wire _NYTU_WX_MATCH3p = xnor2(pix_count_old.XYDO_PX3p.qp_old(), reg_wx_old.MEBY_WX3n.qn_old());
    /* p27.NEZO*/ wire _NEZO_WX_MATCH4p = xnor2(pix_count_old.TUHU_PX4p.qp_old(), reg_wx_old.MYPU_WX4n.qn_old());
    /* p27.NORY*/ wire _NORY_WX_MATCH5p = xnor2(pix_count_old.TUKY_PX5p.qp_old(), reg_wx_old.MYCE_WX5n.qn_old());
    /* p27.NONO*/ wire _NONO_WX_MATCH6p = xnor2(pix_count_old.TAKO_PX6p.qp_old(), reg_wx_old.MUVO_WX6n.qn_old());
    /* p27.PASE*/ wire _PASE_WX_MATCH7p = xnor2(pix_count_old.SYBE_PX7p.qp_old(), reg_wx_old.NUKU_WX7n.qn_old());

    /* p27.PUKY*/ wire _PUKY_WX_MATCHn  = nand5(REJO_WY_MATCH_LATCHp_old.qp_old(), _NEZO_WX_MATCH4p, _NORY_WX_MATCH5p, _NONO_WX_MATCH6p, _PASE_WX_MATCH7p);
    /* p27.NUFA*/ wire _NUFA_WX_MATCHp  = not1(_PUKY_WX_MATCHn);
    /* p27.NOGY*/ wire _NOGY_WX_MATCHn  = nand5(_NUFA_WX_MATCHp, _MYLO_WX_MATCH0p, _PUWU_WX_MATCH1p, _PUHO_WX_MATCH2p, _NYTU_WX_MATCH3p);
    /* p27.NUKO*/ wire _NUKO_WX_MATCHp  = not1(_NOGY_WX_MATCHn);
    return _NUKO_WX_MATCHp;
  }

  //-----------------------------------------------------------------------------

  void read_boot_bit();
  void write_boot_bit_sync();
  void read_bootrom();

  void reg_lcdc_read();
  void reg_lcdc_write();

  void reg_joy_read();
  void reg_joy_write();
  void reg_joy_tock2();

  /* p02.ASOK*/ wire ASOK_INT_JOYp() const { return and2(joy.APUG_JP_GLITCH3.qp_new(), joy.BATU_JP_GLITCH0.qp_new()); }

  void reg_stat_read(wire ACYL_SCANNINGp, wire PARU_VBLANKp);
  void reg_stat_write();
  void reg_stat_tock();

  void tock_interrupts(wire PARU_VBLANKp, wire PURE_LINE_ENDn, DFF17 MOBA_TIMER_OVERFLOWp, wire WODU_HBLANKp);
  void read_intf();
  void read_ie();
  void write_ie();

  void tock_clocks();
  void tock_vid_clocks();

  SpritePix flip_sprite_pix(wire TEXY_SFETCHINGp, DFF8n BAXO_OAM_DB5p);

  SpriteDeltaY sub_sprite_y();

  void tock_sprite_fetcher(wire ATEJ_LINE_RSTp, wire TAVE_PRELOAD_DONE_TRIGp, wire TEKY_SFETCH_REQp_old);

  void set_cpu_pins();

  void reg_wy_read();
  void reg_wy_write();

  void reg_wx_read();
  void reg_wx_write();

  void reg_bgp_read();
  void reg_bgp_write();

  void reg_obp0_read();
  void reg_obp0_write();

  void reg_obp1_read();
  void reg_obp1_write();

  void reg_div_tock();
  void reg_div_read();
  void reg_tima_read();
  void reg_tma_read();
  void reg_tma_write();
  void reg_tac_read();
  void reg_tac_write();
  void tock_timer();

  void tock_pix_counter(wire TADY_LINE_RSTn, wire SACU_CLKPIPE_evn);

  BGScrollX add_scx();
  BGScrollY add_scy();

  void reg_scy_read();
  void reg_scy_write();

  void reg_scx_read();
  void reg_scx_write();

  void reg_ly_read();
  void reg_ly_write();
  void reg_ly_tock2();

  wire TEGY_STROBE() const;
  void reg_lx_tock();
  void reg_lyc_read();
  void reg_lyc_write();
  void reg_lyc_tock2();

  void store_tile_temp_a(wire LOMA_LATCH_TILE_DAn);
  void store_tile_temp_b(wire LABU_LATCH_TILE_DBn);

  void store_sprite_pix_a(SpritePix sprite_pix_old, wire XADO_STORE_SPRITE_An);
  void store_sprite_pix_b(SpritePix sprite_pix_old, wire PUCO_STORE_SPRITE_Bn);

  void tock_win_map_x(wire TEVO_WIN_FETCH_TRIGp, wire PORE_WIN_MODEp, DFF9 WYMO_LCDC_WINENn, wire XAHY_LINE_RSTn);
  void tock_win_map_y(wire PORE_WIN_MODEp, wire PARU_VBLANKp);

  void tock_tile_fetcher(wire NYXU_BFETCH_RSTn, wire MOCE_BFETCH_DONEn_old);

  wire LOMA_LATCH_TILE_DAn() const;
  wire LABU_LATCH_TILE_DBn() const;

  void tock_reset(DFF17 UPOF_DIV15p);

  void set_lcd_pin_ctrl();

  void copy_cpu_addr_to_addr_latch();
  void copy_addr_latch_to_pins();
  void copy_pins_to_data_latch();
  void copy_data_latch_to_cpu_bus();
  void copy_cpu_data_to_pins();
  void set_ext_control_pins();
  void read_ext_to_pins();
  void write_pins_to_ext();

  //-----------------------------------------------------------------------------

  GateBoyBuses old_bus;

  //-----------------------------------------------------------------------------
  // All the SOC registers, pins, buses. Everything in this section should derive
  // from BitBase.

  uint64_t sentinel1 = SENTINEL1;

  //----------

  GateBoyBuses new_bus;

  /*p21.VOGA*/ DFF17 VOGA_HBLANKp;                   // ABxDxFxH Clocked on odd, reset on A
  /*p21.XYMU*/ NorLatch XYMU_RENDERINGn;             // ABxDxFxH Cleared on A, set on BDFH

  GateBoyCpuBus  cpu_bus;
  GateBoyExtBus  ext_bus;
  GateBoyVramBus vram_bus;
  GateBoyOamBus  oam_bus;
  GateBoyZramBus zram_bus;

  //----------
  GateBoyResetDebug rst;
  GateBoyClock      clk;
  GateBoyDiv        div;
  GateBoyTimer      timer;
  GateBoyDMA        dma;
  GateBoyInterrupts interrupts;
  GateBoyJoypad     joy;
  GateBoySerial     serial;

  //----------

  GateBoySpriteStore   sprite_store;
  SpriteScanner sprite_scanner;
  SpriteFetcher sprite_fetcher;
  TileFetcher   tile_fetcher;

  RegLCDC reg_lcdc;
  RegStat reg_stat;
  RegSCX  reg_scx;
  RegSCY  reg_scy;
  RegWY   reg_wy;
  RegWX   reg_wx;

  WinMapX         win_map_x;
  WinLineY        win_map_y;
  WindowRegisters win_reg;
  FineScroll      fine_scroll;

  PPURegisters ppu_reg;
  PixCount     pix_count;
  PixelPipes   pix_pipes;
  GateBoyLCD   lcd;

  //----------

  uint64_t sentinel2 = SENTINEL2;

  //-----------------------------------------------------------------------------
  // Control stuff

  uint8_t sys_rst = 0;
  uint8_t sys_t1 = 0;
  uint8_t sys_t2 = 0;
  uint8_t sys_clken = 0;
  uint8_t sys_clkgood = 0;
  uint8_t sys_clkreq = 0;
  uint8_t sys_cpu_en = 0;
  uint8_t sys_fastboot = 0;
  uint8_t sys_buttons = 0;

  //-----------------------------------------------------------------------------
  // CPU

  MetroBoyCPU      cpu;

  Req      bus_req_old = {0};
  Req      bus_req_new = {0};
  uint8_t  cpu_data_latch = 0;
  uint8_t  int_ack_latch = 0;
  uint8_t  intf_latch = 0;
  uint8_t  intf_latch_delay = 0;
  uint8_t  intf_halt_latch = 0;

  //-----------------------------------------------------------------------------
  // Memory

  uint8_t vid_ram [8192];
  uint8_t cart_ram[8192];
  uint8_t int_ram [8192];
  uint8_t oam_ram [256];
  uint8_t zero_ram[128];

  uint64_t sentinel3 = SENTINEL3;

  //-----------------------------------------------------------------------------
  // LCD and framebuffer

  int old_lcd_x = 0;
  int old_lcd_y = 0;

  uint8_t framebuffer[160*144];
  //uint8_t gb_screen_x = 0;
  //uint8_t gb_screen_y = 0;

  //-----------------------------------------------------------------------------

  double   sim_time = 0;
  uint64_t phase_total = 0;
  uint64_t phase_origin = 0;

  uint64_t phase_hash = 0;
  uint64_t cumulative_hash = 0;

  Probes   probes;

  uint8_t* boot_buf = nullptr;
  size_t   boot_size = 0;

  uint8_t* cart_buf = nullptr;
  size_t   cart_size = 0;

  uint64_t sentinel4 = SENTINEL4;
};
#pragma pack(pop)

//-----------------------------------------------------------------------------
