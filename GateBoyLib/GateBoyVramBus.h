#pragma once
#include "GateBoyLib/Gates.h"

#include "GateBoyLib/GateBoyRegisters.h"

//-----------------------------------------------------------------------------

struct GateBoyVramBus {

  void reset_to_cart() {
    BUS_VRAM_A00n.reset(1);
    BUS_VRAM_A01n.reset(1);
    BUS_VRAM_A02n.reset(1);
    BUS_VRAM_A03n.reset(1);
    BUS_VRAM_A04n.reset(0);
    BUS_VRAM_A05n.reset(1);
    BUS_VRAM_A06n.reset(0);
    BUS_VRAM_A07n.reset(1);
    BUS_VRAM_A08n.reset(1);
    BUS_VRAM_A09n.reset(1);
    BUS_VRAM_A10n.reset(1);
    BUS_VRAM_A11n.reset(1);
    BUS_VRAM_A12n.reset(1);

    PIN34_VRAM_A00.reset(1);
    PIN35_VRAM_A01.reset(1);
    PIN36_VRAM_A02.reset(1);
    PIN37_VRAM_A03.reset(1);
    PIN38_VRAM_A04.reset(0);
    PIN39_VRAM_A05.reset(1);
    PIN40_VRAM_A06.reset(0);
    PIN41_VRAM_A07.reset(1);
    PIN48_VRAM_A08.reset(1);
    PIN47_VRAM_A09.reset(1);
    PIN44_VRAM_A10.reset(1);
    PIN46_VRAM_A11.reset(1);
    PIN42_VRAM_A12.reset(1);
  }

  void reset_buses() {
    BUS_VRAM_A00n.reset_for_pass();
    BUS_VRAM_A01n.reset_for_pass();
    BUS_VRAM_A02n.reset_for_pass();
    BUS_VRAM_A03n.reset_for_pass();
    BUS_VRAM_A04n.reset_for_pass();
    BUS_VRAM_A05n.reset_for_pass();
    BUS_VRAM_A06n.reset_for_pass();
    BUS_VRAM_A07n.reset_for_pass();
    BUS_VRAM_A08n.reset_for_pass();
    BUS_VRAM_A09n.reset_for_pass();
    BUS_VRAM_A10n.reset_for_pass();
    BUS_VRAM_A11n.reset_for_pass();
    BUS_VRAM_A12n.reset_for_pass();

    BUS_VRAM_D00p.reset_for_pass();
    BUS_VRAM_D01p.reset_for_pass();
    BUS_VRAM_D02p.reset_for_pass();
    BUS_VRAM_D03p.reset_for_pass();
    BUS_VRAM_D04p.reset_for_pass();
    BUS_VRAM_D05p.reset_for_pass();
    BUS_VRAM_D06p.reset_for_pass();
    BUS_VRAM_D07p.reset_for_pass();

    PIN43_VRAM_CSn.reset_for_pass();
    PIN45_VRAM_OEn.reset_for_pass();
    PIN49_VRAM_WRn.reset_for_pass();

    PIN34_VRAM_A00.reset_for_pass();
    PIN35_VRAM_A01.reset_for_pass();
    PIN36_VRAM_A02.reset_for_pass();
    PIN37_VRAM_A03.reset_for_pass();
    PIN38_VRAM_A04.reset_for_pass();
    PIN39_VRAM_A05.reset_for_pass();
    PIN40_VRAM_A06.reset_for_pass();
    PIN41_VRAM_A07.reset_for_pass();
    PIN48_VRAM_A08.reset_for_pass();
    PIN47_VRAM_A09.reset_for_pass();
    PIN44_VRAM_A10.reset_for_pass();
    PIN46_VRAM_A11.reset_for_pass();
    PIN42_VRAM_A12.reset_for_pass();

    PIN33_VRAM_D00.reset_for_pass();
    PIN31_VRAM_D01.reset_for_pass();
    PIN30_VRAM_D02.reset_for_pass();
    PIN29_VRAM_D03.reset_for_pass();
    PIN28_VRAM_D04.reset_for_pass();
    PIN27_VRAM_D05.reset_for_pass();
    PIN26_VRAM_D06.reset_for_pass();
    PIN25_VRAM_D07.reset_for_pass();
  }

  void cpu_addr_to_vram_addr(Signal BUS_CPU_A[16], wire XYMU_RENDERINGp, wire LUFA_DMA_VRAMp);
  void dma_addr_to_vram_addr(const GateBoyDMA& dma);
  void scroll_to_addr(BGScrollX scroll_x, BGScrollY scroll_y, wire POTU_BGW_MAP_READp, wire AXAD_WIN_MODEn, wire XAFO_LCDC_BGMAPp);
  void win_to_addr(const WinMapX& win_map_x, const WinLineY& win_line_y, wire POTU_BGW_MAP_READp, wire PORE_WIN_MODEp, wire WOKY_LCDC_WINMAPp);
  void tile_to_addr(const BGScrollY scroll_y, const WinLineY win_line_y, const TileTempB tile_temp_b, wire NETA_BGW_TILE_READp, wire XUHA_FETCH_HILOp, wire WEXU_LCDC_BGTILEp, wire PORE_WIN_MODEp, wire AXAD_WIN_MODEn);
  void sprite_to_addr(const SpriteStore& sprite_store, const OamTempA& oam_temp_a, const OamTempB& oam_temp_b, wire XUQU_SPRITE_AB, wire SAKY_SFETCHn, wire XYMU_RENDERINGp, wire XYMO_LCDC_SPSIZEp);
  void addr_to_pins();
  void cpu_data_to_bus_data(const GateBoyCpuBus& cpu_bus, wire SERE_CPU_VRAM_RDp, wire SALE_CPU_VRAM_WRn);
  void bus_data_to_pins(wire SERE_CPU_VRAM_RDp, wire SALE_CPU_VRAM_WRn);

  void set_pin_cs(wire TUTO_VRAM_DBGp, wire SERE_CPU_VRAM_RDp, wire LUFA_DMA_VRAMp, wire LENA_BFETCHINGp, wire TEXY_SFETCHINGp);
  void set_pin_wr(wire TUTO_VRAM_DBGp, wire SERE_CPU_VRAM_RDp, wire TUJA_CPU_VRAM_WRp);
  void set_pin_oe(wire TUTO_VRAM_DBGp, wire SALE_CPU_VRAM_WRn, wire LUFA_DMA_VRAMp, wire XYMU_RENDERINGp, wire LONY_BFETCHINGp, wire SOHO_SPR_VRAM_RDp);

  void read_vram(const uint8_t* vid_ram);
  void write_vram(uint8_t* vid_ram);

  void pins_to_data_bus(wire SERE_CPU_VRAM_RDp, wire SALE_CPU_VRAM_WRn);
  void data_bus_to_cpu_bus(GateBoyCpuBus& cpu_bus, wire SERE_CPU_VRAM_RDp);

  void dump(Dumper& d) {
    d.dump_bitp   ("PIN43_VRAM_CSn  : ", PIN43_VRAM_CSn.state);
    d.dump_bitp   ("PIN45_VRAM_OEn  : ", PIN45_VRAM_OEn.state);
    d.dump_bitp   ("PIN49_VRAM_WRn  : ", PIN49_VRAM_WRn.state);
    d.dump_slice2p("PIN34_VRAM_ADDR : ", &PIN34_VRAM_A00, 13);
    d.dump_slice2p("PIN25_VRAM_DATA : ", &PIN33_VRAM_D00, 8);
    d.dump_slice2n("BUS_VRAM_An     : ", &BUS_VRAM_A00n, 13);
    d.dump_slice2p("BUS_VRAM_Dp     : ", &BUS_VRAM_D00p, 8);
  }

  Bus BUS_VRAM_A00n;
  Bus BUS_VRAM_A01n;
  Bus BUS_VRAM_A02n;
  Bus BUS_VRAM_A03n;
  Bus BUS_VRAM_A04n;
  Bus BUS_VRAM_A05n;
  Bus BUS_VRAM_A06n;
  Bus BUS_VRAM_A07n;
  Bus BUS_VRAM_A08n;
  Bus BUS_VRAM_A09n;
  Bus BUS_VRAM_A10n;
  Bus BUS_VRAM_A11n;
  Bus BUS_VRAM_A12n;

  Bus BUS_VRAM_D00p;
  Bus BUS_VRAM_D01p;
  Bus BUS_VRAM_D02p;
  Bus BUS_VRAM_D03p;
  Bus BUS_VRAM_D04p;
  Bus BUS_VRAM_D05p;
  Bus BUS_VRAM_D06p;
  Bus BUS_VRAM_D07p;

  /*PIN43*/ PinOut PIN43_VRAM_CSn;
  /*PIN45*/ PinOut PIN45_VRAM_OEn;
  /*PIN49*/ PinOut PIN49_VRAM_WRn;

  /*PIN34*/ PinOut PIN34_VRAM_A00;
  /*PIN35*/ PinOut PIN35_VRAM_A01;
  /*PIN36*/ PinOut PIN36_VRAM_A02;
  /*PIN37*/ PinOut PIN37_VRAM_A03;
  /*PIN38*/ PinOut PIN38_VRAM_A04;
  /*PIN39*/ PinOut PIN39_VRAM_A05;
  /*PIN40*/ PinOut PIN40_VRAM_A06;
  /*PIN41*/ PinOut PIN41_VRAM_A07;
  /*PIN48*/ PinOut PIN48_VRAM_A08;
  /*PIN47*/ PinOut PIN47_VRAM_A09;
  /*PIN44*/ PinOut PIN44_VRAM_A10;
  /*PIN46*/ PinOut PIN46_VRAM_A11;
  /*PIN42*/ PinOut PIN42_VRAM_A12;

  /*PIN33*/ PinIO  PIN33_VRAM_D00;
  /*PIN31*/ PinIO  PIN31_VRAM_D01;
  /*PIN30*/ PinIO  PIN30_VRAM_D02;
  /*PIN29*/ PinIO  PIN29_VRAM_D03;
  /*PIN28*/ PinIO  PIN28_VRAM_D04;
  /*PIN27*/ PinIO  PIN27_VRAM_D05;
  /*PIN26*/ PinIO  PIN26_VRAM_D06;
  /*PIN25*/ PinIO  PIN25_VRAM_D07;
};

//-----------------------------------------------------------------------------
