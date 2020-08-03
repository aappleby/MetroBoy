#include "Sch_Top.h"
#include <memory.h>

using namespace Schematics;

//-----------------------------------------------------------------------------

SchematicTop::SchematicTop() {
  //memset(this, 0, sizeof(*this));
}

//-----------------------------------------------------------------------------

void SchematicTop::tick_slow(int phase) {

  bool verbose = false;

  if (verbose) printf("SchematicTop::tick\n");

  clk_reg.tick_slow(*this);
  lcd_reg.tick(*this);
  sprite_scanner.tick(*this);
  sprite_store.tick(*this);
  pix_pipe.tick(*this);
  tile_fetcher.tick(*this);
  tim_reg.tick(*this);
  bootrom.tick(*this);
  dma_reg.tick(*this);
  ser_reg.tick(*this);
  joypad.tick(*this);
  sprite_fetcher.tick(*this);
  int_reg.tick(*this);

  clk_reg.tock_clk_slow(phase, *this);
  clk_reg.tock_rst_slow(phase, *this);
  clk_reg.tock_dbg_slow(phase, *this);
  clk_reg.tock_vid_slow(phase, *this);

  tim_reg.tock(*this, cpu_bus);
  bootrom.tock(*this, cpu_bus);
  dma_reg.tock(*this, cpu_bus);
  ser_reg.tock(*this, cpu_bus);
  joypad.tock(*this, cpu_bus);
  sprite_scanner.tock(*this);
  lcd_reg.tock(phase, *this, cpu_bus);
  sprite_store.tock(*this);

  pix_pipe.tock(*this, cpu_bus);
  sprite_fetcher.tock(*this);
  tile_fetcher.tock(*this, cpu_bus);
  int_reg.tock(*this, cpu_bus);

  cpu_bus.tock(*this);
  ext_bus.tock(*this);
  oam_bus.tock(*this);
  vram_bus.tock(*this);
}

//------------------------------------------------------------------------------

void SchematicTop::tick_fast(int phase) {

  bool verbose = false;

  if (verbose) printf("SchematicTop::tick\n");

  clk_reg.tick_fast(*this);
  lcd_reg.tick(*this);
  sprite_scanner.tick(*this);
  sprite_store.tick(*this);
  pix_pipe.tick(*this);
  tile_fetcher.tick(*this);
  tim_reg.tick(*this);
  bootrom.tick(*this);
  dma_reg.tick(*this);
  ser_reg.tick(*this);
  joypad.tick(*this);
  sprite_fetcher.tick(*this);
  int_reg.tick(*this);


  clk_reg.tock_clk_fast(phase, *this);
  clk_reg.tock_rst_fast(phase, *this);
  clk_reg.tock_dbg_fast(phase, *this);
  clk_reg.tock_vid_fast(phase, *this);

  tim_reg.tock(*this, cpu_bus);
  bootrom.tock(*this, cpu_bus);
  dma_reg.tock(*this, cpu_bus);
  ser_reg.tock(*this, cpu_bus);
  joypad.tock(*this, cpu_bus);
  sprite_scanner.tock(*this);
  lcd_reg.tock(phase, *this, cpu_bus);
  sprite_store.tock(*this);

  pix_pipe.tock(*this, cpu_bus);
  sprite_fetcher.tock(*this);
  tile_fetcher.tock(*this, cpu_bus);
  int_reg.tock(*this, cpu_bus);

  cpu_bus.tock(*this);
  ext_bus.tock(*this);
  oam_bus.tock(*this);
  vram_bus.tock(*this);
}

//------------------------------------------------------------------------------

#if 0
// Debug stuff I disabled

/*p07.APET*/ wire APET_MODE_DBG = or2(top.clk_reg.UMUT_MODE_DBG1p(), UNOR_MODE_DBG2p); // suggests UMUTp
/*p07.APER*/ wire FF60_WRn = nand2(APET_MODE_DBG, CPU_BUS_A05, CPU_BUS_A06, TAPU_CPUWR, ADDR_111111110xx00000);

//----------
// weird debug things, probably not right

/*p05.AXYN*/ wire AXYN_xBCDEFGH = not1(clk_reg.BEDO_Axxxxxxx);
/*p05.ADYR*/ wire ADYR_Axxxxxxx = not1(AXYN_xBCDEFGH);
/*p05.APYS*/ wire APYS_xBCDEFGH = nor4(sys_sig.MODE_DBG2, ADYR_Axxxxxxx);
/*p05.AFOP*/ wire AFOP_Axxxxxxx = not1(APYS_xBCDEFGH);
/*p07.LECO*/ wire LECO_xBCDEFGH = nor4(clk_reg.BEDO_Axxxxxxx, sys_sig.MODE_DBG2);

if (AFOP_Axxxxxxx) set_data(
  /*p05.ANOC*/ not1(top.GND),
  /*p05.ATAJ*/ not1(top.GND),
  /*p05.AJEC*/ not1(top.GND),
  /*p05.ASUZ*/ not1(top.GND),
  /*p05.BENU*/ not1(top.GND),
  /*p05.AKAJ*/ not1(top.GND),
  /*p05.ARAR*/ not1(top.GND),
  /*p05.BEDA*/ not1(top.GND)
);

if (LECO_xBCDEFGH) set_data(
  /*p07.ROMY*/ top.GND,
  /*p07.RYNE*/ top.GND,
  /*p07.REJY*/ top.GND,
  /*p07.RASE*/ top.GND,
  /*p07.REKA*/ top.GND,
  /*p07.ROWE*/ top.GND,
  /*p07.RYKE*/ top.GND,
  /*p07.RARU*/ top.GND
);



//----------
// more debug stuff

/*p25.TUSO*/ wire TUSO = nor4(MODE_DBG2, ff20.CPU_PIN_BOGA_xBCDEFGH);
/*p25.SOLE*/ wire SOLE = not1(TUSO);

if (top.VYPO_GND) bus_out.set_data(
  /*p25.TOVU*/ SOLE,
  /*p25.SOSA*/ SOLE,
  /*p25.SEDU*/ SOLE,
  /*p25.TAXO*/ SOLE,
  /*p25.TAHY*/ SOLE,
  /*p25.TESU*/ SOLE,
  /*p25.TAZU*/ SOLE,
  /*p25.TEWA*/ SOLE
);

// FIXME
///*p05.KORE*/ wire P05_NC0 = nand2(KERU_DBG_FF00_D7, FF60_0);
///*p05.KYWE*/ wire P05_NC1 = nor4 (KERU_DBG_FF00_D7, FF60_0o);

/*p08.LYRA*/ wire DBG_D_RDn = nand2(sys_sig.MODE_DBG2, bus_sig.CBUS_TO_CEXTn);
/*p08.TUTY*/ if (!DBG_D_RDn) CPU_BUS_D0 = not1(/*p08.TOVO*/ not1(pins.PIN_D0_C));
/*p08.SYWA*/ if (!DBG_D_RDn) CPU_BUS_D1 = not1(/*p08.RUZY*/ not1(pins.PIN_D1_C));
/*p08.SUGU*/ if (!DBG_D_RDn) CPU_BUS_D2 = not1(/*p08.ROME*/ not1(pins.PIN_D2_C));
/*p08.TAWO*/ if (!DBG_D_RDn) CPU_BUS_D3 = not1(/*p08.SAZA*/ not1(pins.PIN_D3_C));
/*p08.TUTE*/ if (!DBG_D_RDn) CPU_BUS_D4 = not1(/*p08.TEHE*/ not1(pins.PIN_D4_C));
/*p08.SAJO*/ if (!DBG_D_RDn) CPU_BUS_D5 = not1(/*p08.RATU*/ not1(pins.PIN_D5_C));
/*p08.TEMY*/ if (!DBG_D_RDn) CPU_BUS_D6 = not1(/*p08.SOCA*/ not1(pins.PIN_D6_C));
/*p08.ROPA*/ if (!DBG_D_RDn) CPU_BUS_D7 = not1(/*p08.RYBA*/ not1(pins.PIN_D7_C));
#endif

// hack, not correct
#if 0
{
  // FF60 debug state
  /*p07.APET*/ wire APET_MODE_DBG = or2(sys_sig.MODE_DBG1, sys_sig.MODE_DBG2);
  /*p07.APER*/ wire FF60_WRn = nand2(APET_MODE_DBG, CPU_BUS_A05, CPU_BUS_A06, bus_sig.TAPU_CPUWR, dec_sig.ADDR_111111110xx00000);

  /*p05.KURA*/ wire FF60_0n = not1(BURO_FF60_0);
  /*p05.JEVA*/ wire FF60_0o = not1(BURO_FF60_0);
  /*p07.BURO*/ BURO_FF60_0 = ff9(FF60_WRn, rst_sig.SYS_RESETn, CPU_BUS_D0);
  /*p07.AMUT*/ AMUT_FF60_1 = ff9(FF60_WRn, rst_sig.SYS_RESETn, CPU_BUS_D1);

  ///*p05.KURA*/ wire FF60_0n = not1(FF60);
  ///*p05.JEVA*/ wire FF60_0o = not1(FF60);
}
#endif


#if 0
// so the address bus is technically a tribuf, but we're going to ignore
// this debug circuit for now.
{
// If we're in debug mode 2, drive external address bus onto internal address
// these may be backwards, probably don't want to drive external address onto bus normally...

/*p08.KOVA*/ wire A00_Cn = not1(EXT_PIN_A00_C);
/*p08.CAMU*/ wire A01_Cn = not1(EXT_PIN_A01_C);
/*p08.BUXU*/ wire A02_Cn = not1(EXT_PIN_A02_C);
/*p08.BASE*/ wire A03_Cn = not1(EXT_PIN_A03_C);
/*p08.AFEC*/ wire A04_Cn = not1(EXT_PIN_A04_C);
/*p08.ABUP*/ wire A05_Cn = not1(EXT_PIN_A05_C);
/*p08.CYGU*/ wire A06_Cn = not1(EXT_PIN_A06_C);
/*p08.COGO*/ wire A07_Cn = not1(EXT_PIN_A07_C);
/*p08.MUJY*/ wire A08_Cn = not1(EXT_PIN_A08_C);
/*p08.NENA*/ wire A09_Cn = not1(EXT_PIN_A09_C);
/*p08.SURA*/ wire A10_Cn = not1(EXT_PIN_A10_C);
/*p08.MADY*/ wire A11_Cn = not1(EXT_PIN_A11_C);
/*p08.LAHE*/ wire A12_Cn = not1(EXT_PIN_A12_C);
/*p08.LURA*/ wire A13_Cn = not1(EXT_PIN_A13_C);
/*p08.PEVO*/ wire A14_Cn = not1(EXT_PIN_A14_C);
/*p08.RAZA*/ wire A15_Cn = not1(EXT_PIN_A15_C);

/*p08.KEJO*/ top.cpu_bus.CPU_BUS_A00 = tribuf_10n(TOVA_MODE_DBG2n, A00_Cn);
/*p08.BYXE*/ top.cpu_bus.CPU_BUS_A01 = tribuf_10n(TOVA_MODE_DBG2n, A01_Cn);
/*p08.AKAN*/ top.cpu_bus.CPU_BUS_A02 = tribuf_10n(TOVA_MODE_DBG2n, A02_Cn);
/*p08.ANAR*/ top.cpu_bus.CPU_BUS_A03 = tribuf_10n(TOVA_MODE_DBG2n, A03_Cn);
/*p08.AZUV*/ top.cpu_bus.CPU_BUS_A04 = tribuf_10n(TOVA_MODE_DBG2n, A04_Cn);
/*p08.AJOV*/ top.cpu_bus.CPU_BUS_A05 = tribuf_10n(TOVA_MODE_DBG2n, A05_Cn);
/*p08.BYNE*/ top.cpu_bus.CPU_BUS_A06 = tribuf_10n(TOVA_MODE_DBG2n, A06_Cn);
/*p08.BYNA*/ top.cpu_bus.CPU_BUS_A07 = tribuf_10n(TOVA_MODE_DBG2n, A07_Cn);
/*p08.LOFA*/ top.cpu_bus.CPU_BUS_A08 = tribuf_10n(TOVA_MODE_DBG2n, A08_Cn);
/*p08.MAPU*/ top.cpu_bus.CPU_BUS_A09 = tribuf_10n(TOVA_MODE_DBG2n, A09_Cn);
/*p08.RALA*/ top.cpu_bus.CPU_BUS_A10 = tribuf_10n(TOVA_MODE_DBG2n, A10_Cn);
/*p08.LORA*/ top.cpu_bus.CPU_BUS_A11 = tribuf_10n(TOVA_MODE_DBG2n, A11_Cn);
/*p08.LYNA*/ top.cpu_bus.CPU_BUS_A12 = tribuf_10n(TOVA_MODE_DBG2n, A12_Cn);
/*p08.LEFY*/ top.cpu_bus.CPU_BUS_A13 = tribuf_10n(TOVA_MODE_DBG2n, A13_Cn);
/*p08.NEFE*/ top.cpu_bus.CPU_BUS_A14 = tribuf_10n(TOVA_MODE_DBG2n, A14_Cn);
/*p08.SYZU*/ top.cpu_bus.CPU_BUS_A15 = tribuf_10n(TOVA_MODE_DBG2n, A15_Cn);
}
#endif

//-----------------------------------------------------------------------------
































































//------------------------------------------------------------------------------

//void preset(bool EXT_PIN_RDn_A, bool EXT_PIN_CSn_A);

/*
void preset_d(bool oe, uint8_t d) {
PIN_D0_C.preset(oe, d & 0x01);
PIN_D1_C.preset(oe, d & 0x02);
PIN_D2_C.preset(oe, d & 0x04);
PIN_D3_C.preset(oe, d & 0x08);
PIN_D4_C.preset(oe, d & 0x10);
PIN_D5_C.preset(oe, d & 0x20);
PIN_D6_C.preset(oe, d & 0x40);
PIN_D7_C.preset(oe, d & 0x80);
}
*/

#if 0
void ExtPinsOut::preset() {
  EXT_PIN_WRp_C.preset(true, 0);   // -> P07.UBAL
  EXT_PIN_RDp_C.preset(true, 0);   // -> P07.UJYV
  EXT_PIN_A00_C.preset(true, 0);   // -> P08.KOVA
  EXT_PIN_A01_C.preset(true, 0);   // -> P08.CAMU
  EXT_PIN_A02_C.preset(true, 0);   // -> P08.BUXU
  EXT_PIN_A03_C.preset(true, 0);   // -> P08.BASE
  EXT_PIN_A04_C.preset(true, 0);   // -> P08.AFEC
  EXT_PIN_A05_C.preset(true, 0);   // -> P08.ABUP
  EXT_PIN_A06_C.preset(true, 0);   // -> P08.CYGU
  EXT_PIN_A07_C.preset(true, 0);   // -> P08.COGO
  EXT_PIN_A08_C.preset(true, 0);   // -> P08.MUJY
  EXT_PIN_A09_C.preset(true, 0);   // -> P08.NENA
  EXT_PIN_A10_C.preset(true, 0);   // -> P08.SURA
  EXT_PIN_A11_C.preset(true, 0);   // -> P08.MADY
  EXT_PIN_A12_C.preset(true, 0);   // -> P08.LAHE
  EXT_PIN_A13_C.preset(true, 0);   // -> P08.LURA
  EXT_PIN_A14_C.preset(true, 0);   // -> P08.PEVO
  EXT_PIN_A15_C.preset(true, 0);   // -> P08.RAZA

  if (!EXT_PIN_RDn_A && !EXT_PIN_CSn_A) {
    uint16_t ext_addr = get_addr();

    if (0x0000 <= ext_addr && ext_addr <= 0x7FFF) {
      // Cart rom
      //uint8_t d = rom[ext_addr];
      uint8_t d = 0;
      preset_d(true, d);
    }
    else if (0xC000 <= ext_addr && ext_addr <= 0xDFFF) {
      // Main ram
      //uint8_t d = ram[ext_addr - 0xC000];
      uint8_t d = 0;
      preset_d(true, d);
    }
    else if (0xE000 <= ext_addr && ext_addr <= 0xFFFF) {
      // Echo ram
      //uint8_t d = ram[ext_addr - 0xE000];
      uint8_t d = 0;
      preset_d(true, d);
    }
    else {
      printf("Bad address?\n");
      __debugbreak();
    }
  }
  else {
    preset_d(false, 0);
  }
}
#endif





#if 0
inline void dump_pins(TextPainter& text_painter, const PinOut& a, const PinOut& d) {
  dump_pin(text_painter, a.a, d.a);
}

inline void dump_pins(TextPainter& text_painter, const PinOut& a, const PinOut& b, const PinIn& c, const PinOut& d) {
  dump_pin(text_painter, a.a, b.a, c.a, d.a);
}

void dump_regs(TextPainter& /*text_painter*/) {
  /*
  text_painter.dprintf(" ----- EXT_BUS -----\n");

  text_painter.dprintf("ADDR_LATCH ");
  dump2(text_painter, EXT_ADDR_LATCH_14.a);
  dump2(text_painter, EXT_ADDR_LATCH_13.a);
  dump2(text_painter, EXT_ADDR_LATCH_12.a);
  text_painter.add_char(':');
  dump2(text_painter, EXT_ADDR_LATCH_11.a);
  dump2(text_painter, EXT_ADDR_LATCH_10.a);
  dump2(text_painter, EXT_ADDR_LATCH_09.a);
  dump2(text_painter, EXT_ADDR_LATCH_08.a);
  text_painter.add_char(':');
  dump2(text_painter, EXT_ADDR_LATCH_07.a);
  dump2(text_painter, EXT_ADDR_LATCH_06.a);
  dump2(text_painter, EXT_ADDR_LATCH_05.a);
  dump2(text_painter, EXT_ADDR_LATCH_04.a);
  text_painter.add_char(':');
  dump2(text_painter, EXT_ADDR_LATCH_03.a);
  dump2(text_painter, EXT_ADDR_LATCH_02.a);
  dump2(text_painter, EXT_ADDR_LATCH_01.a);
  dump2(text_painter, EXT_ADDR_LATCH_00.a);
  text_painter.newline();

  text_painter.dprintf("DATA_LATCH ");
  dump2(text_painter, CART_DATA_LATCH_07.a);
  dump2(text_painter, CART_DATA_LATCH_06.a);
  dump2(text_painter, CART_DATA_LATCH_05.a);
  dump2(text_painter, CART_DATA_LATCH_04.a);
  text_painter.add_char(':');
  dump2(text_painter, CART_DATA_LATCH_03.a);
  dump2(text_painter, CART_DATA_LATCH_02.a);
  dump2(text_painter, CART_DATA_LATCH_01.a);
  dump2(text_painter, CART_DATA_LATCH_00.a);
  text_painter.newline();
  */
}

void dump_pins(TextPainter& /*text_painter*/) {
  /*
  text_painter.dprintf("----- EXT_PINS -----\n");

  text_painter.dprintf("EXT_PIN_CLK %d\n", EXT_PIN_CLK.a.val);
  text_painter.dprintf("WR  %d\n", EXT_PIN_WRn_A.a.val);
  text_painter.dprintf("RD  %d\n", EXT_PIN_RDn_A.a.val);
  text_painter.dprintf("CS  %d\n", EXT_PIN_CSn_A.a.val);

  text_painter.add_text("Axx ");
  dump_pins(text_painter, EXT_PIN_A15_A, EXT_PIN_A15_D);
  dump_pins(text_painter, EXT_PIN_A14_A, EXT_PIN_A14_D);
  dump_pins(text_painter, EXT_PIN_A13_A, EXT_PIN_A13_D);
  dump_pins(text_painter, EXT_PIN_A12_A, EXT_PIN_A12_D);
  text_painter.add_char(':');
  dump_pins(text_painter, EXT_PIN_A11_A, EXT_PIN_A11_D);
  dump_pins(text_painter, EXT_PIN_A10_A, EXT_PIN_A10_D);
  dump_pins(text_painter, EXT_PIN_A09_A, EXT_PIN_A09_D);
  dump_pins(text_painter, EXT_PIN_A08_A, EXT_PIN_A08_D);
  text_painter.add_char(':');
  dump_pins(text_painter, EXT_PIN_A07_A, EXT_PIN_A07_D);
  dump_pins(text_painter, EXT_PIN_A06_A, EXT_PIN_A06_D);
  dump_pins(text_painter, EXT_PIN_A05_A, EXT_PIN_A05_D);
  dump_pins(text_painter, EXT_PIN_A04_A, EXT_PIN_A04_D);
  text_painter.add_char(':');
  dump_pins(text_painter, EXT_PIN_A03_A, EXT_PIN_A03_D);
  dump_pins(text_painter, EXT_PIN_A02_A, EXT_PIN_A02_D);
  dump_pins(text_painter, EXT_PIN_A01_A, EXT_PIN_A01_D);
  dump_pins(text_painter, EXT_PIN_A00_A, EXT_PIN_A00_D);
  text_painter.newline();

  text_painter.add_text("Dxx ");
  dump_pins(text_painter, EXT_PIN_D7_A, EXT_PIN_D7_B, PIN_D7_C, EXT_PIN_D7_D);
  dump_pins(text_painter, EXT_PIN_D6_A, EXT_PIN_D6_B, PIN_D6_C, EXT_PIN_D6_D);
  dump_pins(text_painter, EXT_PIN_D5_A, EXT_PIN_D5_B, PIN_D5_C, EXT_PIN_D5_D);
  dump_pins(text_painter, EXT_PIN_D4_A, EXT_PIN_D4_B, PIN_D4_C, EXT_PIN_D4_D);
  dump_pins(text_painter, EXT_PIN_D3_A, EXT_PIN_D3_B, PIN_D3_C, EXT_PIN_D3_D);
  dump_pins(text_painter, EXT_PIN_D2_A, EXT_PIN_D2_B, PIN_D2_C, EXT_PIN_D2_D);
  dump_pins(text_painter, EXT_PIN_D1_A, EXT_PIN_D1_B, PIN_D1_C, EXT_PIN_D1_D);
  dump_pins(text_painter, EXT_PIN_D0_A, EXT_PIN_D0_B, PIN_D0_C, EXT_PIN_D0_D);
  text_painter.newline();
  */
}

uint16_t get_addr() const {
  uint16_t ext_addr = (uint16_t)pack(
    EXT_PIN_A00_A, EXT_PIN_A01_A, EXT_PIN_A02_A, EXT_PIN_A03_A,
    EXT_PIN_A04_A, EXT_PIN_A05_A, EXT_PIN_A06_A, EXT_PIN_A07_A,
    EXT_PIN_A08_A, EXT_PIN_A09_A, EXT_PIN_A10_A, EXT_PIN_A11_A,
    EXT_PIN_A12_A, EXT_PIN_A13_A, EXT_PIN_A14_A, EXT_PIN_A15_A);

  return ext_addr;
}

/*
void dump_pins(TextPainter& text_painter) {
text_painter.dprintf("----- SYS_PINS -----\n");
text_painter.dprintf("PIN_RST      %d\n", PIN_RST.a.val);
text_painter.dprintf("PIN_CLK_GOOD %d\n", PIN_CLK_GOOD.a.val);
text_painter.dprintf("SYS_PIN_CLK_xBxDxFxH   %d\n", SYS_PIN_CLK_xBxDxFxH.a.val);
text_painter.dprintf("SYS_PIN_T1       %d\n", SYS_PIN_T1.a.val);
text_painter.dprintf("SYS_PIN_T2       %d\n", SYS_PIN_T2.a.val);
text_painter.newline();
}
*/

#endif



#if 0
void dump(TextPainter& text_painter) {
  text_painter.dprintf("----- VRAM_PINS -----\n");

  text_painter.dprintf("MCS  %d:x:%d:%d\n", VRAM_PIN_MCSn_A.prev().val, VRAM_PIN_MCSn_C.prev().val, VRAM_PIN_MCSn_D.prev().val);
  text_painter.dprintf("MOE  %d:x:%d:%d\n", VRAM_PIN_MOEn_A.prev().val, VRAM_PIN_MOEn_C.prev().val, VRAM_PIN_MOEn_D.prev().val);
  text_painter.dprintf("MWR  %d:x:%d:%d\n", VRAM_PIN_MWRn_A.prev().val, VRAM_PIN_MWRn_C.prev().val, VRAM_PIN_MWRn_D.prev().val);
  text_painter.dprintf("MAxx 0x%04x\n", pack(VRAM_PIN_MA00_AD, VRAM_PIN_MA01_AD, VRAM_PIN_MA02_AD, VRAM_PIN_MA03_AD, VRAM_PIN_MA04_AD, VRAM_PIN_MA05_AD, VRAM_PIN_MA06_AD,
    VRAM_PIN_MA07_AD, VRAM_PIN_MA08_AD, VRAM_PIN_MA09_AD, VRAM_PIN_MA10_AD, VRAM_PIN_MA11_AD, VRAM_PIN_MA12_AD));

  text_painter.dprintf("MDx_A 0x%02x\n", pack(VRAM_PIN_MD0_A, VRAM_PIN_MD1_A, VRAM_PIN_MD2_A, VRAM_PIN_MD3_A, VRAM_PIN_MD4_A, VRAM_PIN_MD5_A, VRAM_PIN_MD6_A, VRAM_PIN_MD7_A));
  text_painter.dprintf("MDx_B 0x%02x\n", pack(VRAM_PIN_MD0_B, VRAM_PIN_MD1_B, VRAM_PIN_MD2_B, VRAM_PIN_MD3_B, VRAM_PIN_MD4_B, VRAM_PIN_MD5_B, VRAM_PIN_MD6_B, VRAM_PIN_MD7_B));
  text_painter.dprintf("MDx_C 0x%02x\n", pack(VRAM_PIN_MD0_C, VRAM_PIN_MD1_C, VRAM_PIN_MD2_C, VRAM_PIN_MD3_C, VRAM_PIN_MD4_C, VRAM_PIN_MD5_C, VRAM_PIN_MD6_C, VRAM_PIN_MD7_C));
  text_painter.dprintf("MDx_D 0x%02x\n", pack(VRAM_PIN_MD0_D, VRAM_PIN_MD1_D, VRAM_PIN_MD2_D, VRAM_PIN_MD3_D, VRAM_PIN_MD4_D, VRAM_PIN_MD5_D, VRAM_PIN_MD6_D, VRAM_PIN_MD7_D));
  text_painter.newline();
}
#endif
