#include "GateBoyLib/Sch_Top.h"
#include <memory.h>
#include <stdio.h>

#include "GateBoyLib/Probe.h"

using namespace Schematics;

#pragma warning(disable:4100)

//-----------------------------------------------------------------------------

void SchematicTop::tick_slow(wire RST, wire CLK, wire CLKGOOD, wire T1n, wire T2n, wire CPUREADY) {
  const auto& top = *this;

  RegBase::tick_running = true;

  //----------------------------------------
  // dma_reg.tick();

  /*#p28.BOGE*/ dma_reg.BOGE_DMA_RUNNINGn = not1(dma_reg.MATU_DMA_RUNNINGp.qp17());

  /*#p04.LEBU*/ wire _LEBU_DMA_A15n    = not1(dma_reg.MARU_DMA_A15n.qn07());
  /*#p04.MUDA*/ wire _MUDA_DMA_VRAMp   = nor3(dma_reg.PULA_DMA_A13n.qn07(), dma_reg.POKU_DMA_A14n.qn07(), _LEBU_DMA_A15n);
  /* p04.LOGO*/ wire _LOGO_DMA_VRAMn   = not1(_MUDA_DMA_VRAMp);
  /* p04.MORY*/ wire _MORY_DMA_CARTn   = nand2(dma_reg.MATU_DMA_RUNNINGp.qp17(), _LOGO_DMA_VRAMn);
  /* p04.LUMA*/ dma_reg.LUMA_DMA_CARTp = not1(_MORY_DMA_CARTn);

  /* p04.MUHO*/ wire _MUHO_DMA_VRAMp   = nand2(dma_reg.MATU_DMA_RUNNINGp.qp17(), _MUDA_DMA_VRAMp);
  /* p04.LUFA*/ dma_reg.LUFA_DMA_VRAMp = not1(_MUHO_DMA_VRAMp);

  //----------------------------------------

  {
    /*p07.UBET*/ wire _UBET_T1p  = not1(T1n);
    /*p07.UVAR*/ wire _UVAR_T2p  = not1(T2n);
    /*p07.UMUT*/ UMUT_MODE_DBG1p = and2(T1n, _UVAR_T2p);
    /*p07.UNOR*/ UNOR_MODE_DBG2p = and2(T2n, _UBET_T1p);
    /*p07.UPOJ*/ UPOJ_MODE_PRODn = nand3(_UBET_T1p, _UVAR_T2p, RST);
    /*p25.TUTO*/ TUTO_DBG_VRAMp  = and2(UNOR_MODE_DBG2p, SOTO_DBG_VRAM.qn16());
  }

  /* p07.UJYV*/ wire _UJYV_CPU_RDn = mux2n(UNOR_MODE_DBG2p, ext_bus.PIN_EXT_RDn.qn(), cpu_bus.PIN_CPU_RDp.qp());
  /* p07.TEDO*/ TEDO_CPU_RDp = not1(_UJYV_CPU_RDn);
  /* p07.AJAS*/ AJAS_CPU_RDn = not1(TEDO_CPU_RDp);
  /* p07.ASOT*/ ASOT_CPU_RDp = not1(AJAS_CPU_RDn);



  {
    /*#p01.AGUT*/ wire _AGUT_xxCDEFGH = or_and3(clk_reg.AROV_xxCDEFxx.qp(), clk_reg.AJAX_xxxxEFGH.qp(), cpu_bus.PIN_CPU_EXT_BUSp.qp());
    /*#p01.AWOD*/ wire _AWOD_ABxxxxxx = nor2(UNOR_MODE_DBG2p, _AGUT_xxCDEFGH);
    /*#p01.ABUZ*/ ABUZ_xxCDEFGH = not1(_AWOD_ABxxxxxx);

    /*#p08.SORE*/ wire _SORE_0000_7FFFp = not1(cpu_bus.BUS_CPU_A15.qp());
    /*#p08.TEVY*/ wire _TEVY_8000_9FFFn = or3(cpu_bus.BUS_CPU_A13.qp(), cpu_bus.BUS_CPU_A14.qp(), _SORE_0000_7FFFp);
    /*#p08.TEXO*/ TEXO_ADDR_EXT_AND_NOT_VRAM = and2(cpu_bus.PIN_CPU_EXT_BUSp.qp(), _TEVY_8000_9FFFn);
  }

  {
    /* p04.DECY*/ wire _DECY_LATCH_EXTn = not1(cpu_bus.PIN_CPU_LATCH_EXT.qp());
    /* p04.CATY*/ CATY_LATCH_EXTp = not1(_DECY_LATCH_EXTn);

    /* p28.MYNU*/ wire _MYNU_CPU_RDn = nand2(ASOT_CPU_RDp, CATY_LATCH_EXTp);
    /* p28.LEKO*/ LEKO_CPU_RDp = not1(_MYNU_CPU_RDn);
  }

  //----------------------------------------

  clk_reg.tick_slow(CLK, CLKGOOD, CPUREADY, top);

  //----------------------------------------

  lcd_reg.tick(top);

  //----------------------------------------

  sprite_scanner.tick(top);

  //----------------------------------------

  sprite_store.tick(top);

  //----------------------------------------

  pix_pipe.tick(top);

  //----------------------------------------

  {
    /*p01.AREV*/ wire _AREV_CPU_WRn_ABCDxxxH = nand2(cpu_bus.PIN_CPU_WRp.qp(), clk_reg.AFAS_xxxxEFGx);
    /*p01.APOV*/ APOV_CPU_WRp_xxxxEFGx      = not1(_AREV_CPU_WRn_ABCDxxxH);
    /*p07.UBAL*/ wire _UBAL_CPU_WRn_ABCDxxxH = mux2n(UNOR_MODE_DBG2p, ext_bus.PIN_EXT_WRn.qn(), APOV_CPU_WRp_xxxxEFGx);
    /*p07.TAPU*/ TAPU_CPU_WRp_xxxxEFGx      = not1(_UBAL_CPU_WRn_ABCDxxxH);
    /*p07.DYKY*/ wire _DYKY_CPU_WRn_ABCDxxxH = not1(TAPU_CPU_WRp_xxxxEFGx);
    /*p07.CUPA*/ CUPA_CPU_WRp_xxxxEFGx      = not1(_DYKY_CPU_WRn_ABCDxxxH);
  }

  {
    /*p07.TERA*/ wire _TERA_BOOT_BITp  = not1(bootrom.BOOT_BITn.qp17());
    /*p07.TUTU*/ TUTU_ADDR_BOOTp = and2(_TERA_BOOT_BITp, cpu_bus.TULO_ADDR_00XXp());
  }

  //----------------------------------------

  tim_reg.tick(top);

  //----------------------------------------

  joypad.tick(top);

  //----------------------------------------

  tile_fetcher.tick(top);

  //----------------------------------------

  sprite_fetcher.tick(top);

  //----------------------------------------

  RegBase::tick_running = false;
}

//------------------------------------------------------------------------------

void SchematicTop::tock_slow(wire RST, wire CLK, wire CLKGOOD, wire T1n, wire T2n, wire CPUREADY) {
  auto& top = *this;

  {
    /*#p25.SYCY*/ wire SYCY_DBG_CLOCKn = not1(UNOR_MODE_DBG2p);
    /*#p25.SOTO*/ SOTO_DBG_VRAM.dff17(SYCY_DBG_CLOCKn, clk_reg.CUNU_SYS_RSTn, SOTO_DBG_VRAM.qn16());
  }

  clk_reg.tock_clk_slow(CLKGOOD, top);
  clk_reg.tock_rst_slow(RST, CLKGOOD, CPUREADY, top);
  clk_reg.tock_dbg_slow(top);
  clk_reg.tock_vid_slow(CLK, top);
  tim_reg.tock(RST, top, cpu_bus);
  bootrom.tock(top, cpu_bus);

  //------------------------------------------------------------------------------
  // dma_reg.tock(top, cpu_bus);

  /*#p22.WATE*/ wire _WATE_FF46n    = nand5(cpu_bus.WERO_FF4Xp(), cpu_bus.XOLA_A00n(), cpu_bus.WESA_A01p(), cpu_bus.WALO_A02p(), cpu_bus.XERA_A03n());
  /*#p22.XEDA*/ wire _XEDA_FF46p    = not1(_WATE_FF46n);
  /*#p04.LAVY*/ wire _LAVY_FF46_WRp = and2(CUPA_CPU_WRp_xxxxEFGx, _XEDA_FF46p);

  /*#p04.LOKO*/ wire _LOKO_DMA_RSTp = nand2(dma_reg.LENE_DMA_TRIG_d4.qn16(), clk_reg.CUNU_SYS_RSTn);
  /*#p04.LAPA*/ wire _LAPA_DMA_RSTn = not1(_LOKO_DMA_RSTp);

  {
    /*#p04.LYXE*/ dma_reg.LYXE_DMA_LATCHp.nor_latch(_LAVY_FF46_WRp, _LOKO_DMA_RSTp);

    /*#p04.LUPA*/ wire _LUPA_DMA_TRIG = nor2(_LAVY_FF46_WRp, dma_reg.LYXE_DMA_LATCHp.qn03());
    /*#p04.LUVY*/ dma_reg.LUVY_DMA_TRIG_d0  .dff17(clk_reg.UVYT_ABCDxxxx, clk_reg.CUNU_SYS_RSTn, _LUPA_DMA_TRIG);
    /*#p04.LENE*/ dma_reg.LENE_DMA_TRIG_d4  .dff17(clk_reg.MOPA_xxxxEFGH, clk_reg.CUNU_SYS_RSTn, dma_reg.LUVY_DMA_TRIG_d0.qp17());

    /*#p04.LOKY*/ dma_reg.LOKY_DMA_LATCHp.nand_latch(dma_reg.LENE_DMA_TRIG_d4.qn16(), and2(dma_reg.MYTE_DMA_DONE.qn16(), clk_reg.CUNU_SYS_RSTn));
    /*#p04.MATU*/ dma_reg.MATU_DMA_RUNNINGp .dff17(clk_reg.UVYT_ABCDxxxx, clk_reg.CUNU_SYS_RSTn, dma_reg.LOKY_DMA_LATCHp.qp03());
  }

  {
    // 128+16+8+4+2+1 = 159, this must be "dma done"

    /*#p04.NAVO*/ wire _NAVO_DMA_DONEn = nand6(dma_reg.NAKY_DMA_A00p.qp17(), dma_reg.PYRO_DMA_A01p.qp17(),
                                               dma_reg.NEFY_DMA_A02p.qp17(), dma_reg.MUTY_DMA_A03p.qp17(),
                                               dma_reg.NYKO_DMA_A04p.qp17(), dma_reg.MUGU_DMA_A07p.qp17());
    /*#p04.NOLO*/ wire _NOLO_DMA_DONEp = not1(_NAVO_DMA_DONEn);
    /*#p04.MYTE*/ dma_reg.MYTE_DMA_DONE.dff17(clk_reg.MOPA_xxxxEFGH, _LAPA_DMA_RSTn, _NOLO_DMA_DONEp);
  }

  {
    /*#p04.META*/ wire _META_DMA_CLKp = and2(clk_reg.UVYT_ABCDxxxx, dma_reg.LOKY_DMA_LATCHp.qp03());

    /*#p04.NAKY*/ dma_reg.NAKY_DMA_A00p.dff17(_META_DMA_CLKp,                   _LAPA_DMA_RSTn, dma_reg.NAKY_DMA_A00p.qn16());
    /*#p04.PYRO*/ dma_reg.PYRO_DMA_A01p.dff17(dma_reg.NAKY_DMA_A00p.qn16(), _LAPA_DMA_RSTn, dma_reg.PYRO_DMA_A01p.qn16());
    /* p04.NEFY*/ dma_reg.NEFY_DMA_A02p.dff17(dma_reg.PYRO_DMA_A01p.qn16(), _LAPA_DMA_RSTn, dma_reg.NEFY_DMA_A02p.qn16());
    /* p04.MUTY*/ dma_reg.MUTY_DMA_A03p.dff17(dma_reg.NEFY_DMA_A02p.qn16(), _LAPA_DMA_RSTn, dma_reg.MUTY_DMA_A03p.qn16());
    /* p04.NYKO*/ dma_reg.NYKO_DMA_A04p.dff17(dma_reg.MUTY_DMA_A03p.qn16(), _LAPA_DMA_RSTn, dma_reg.NYKO_DMA_A04p.qn16());
    /* p04.PYLO*/ dma_reg.PYLO_DMA_A05p.dff17(dma_reg.NYKO_DMA_A04p.qn16(), _LAPA_DMA_RSTn, dma_reg.PYLO_DMA_A05p.qn16());
    /* p04.NUTO*/ dma_reg.NUTO_DMA_A06p.dff17(dma_reg.PYLO_DMA_A05p.qn16(), _LAPA_DMA_RSTn, dma_reg.NUTO_DMA_A06p.qn16());
    /* p04.MUGU*/ dma_reg.MUGU_DMA_A07p.dff17(dma_reg.NUTO_DMA_A06p.qn16(), _LAPA_DMA_RSTn, dma_reg.MUGU_DMA_A07p.qn16());
  }

  // FF46 DMA
  {
    /*#p04.LORU*/ wire _LORU_FF46_WRn = not1(_LAVY_FF46_WRp);
    ///*#p??.PYSU*/ wire _PYSU_FF46_WRp = not1(LORU_FF46_WRn);

    /*#p04.NAFA*/ dma_reg.NAFA_DMA_A08n.dff8p(_LORU_FF46_WRn, cpu_bus.BUS_CPU_D0p.qp()); // BOTH OUTPUTS USED
    /* p04.PYNE*/ dma_reg.PYNE_DMA_A09n.dff8p(_LORU_FF46_WRn, cpu_bus.BUS_CPU_D1p.qp());
    /* p04.PARA*/ dma_reg.PARA_DMA_A10n.dff8p(_LORU_FF46_WRn, cpu_bus.BUS_CPU_D2p.qp());
    /* p04.NYDO*/ dma_reg.NYDO_DMA_A11n.dff8p(_LORU_FF46_WRn, cpu_bus.BUS_CPU_D3p.qp());
    /* p04.NYGY*/ dma_reg.NYGY_DMA_A12n.dff8p(_LORU_FF46_WRn, cpu_bus.BUS_CPU_D4p.qp());
    /* p04.PULA*/ dma_reg.PULA_DMA_A13n.dff8p(_LORU_FF46_WRn, cpu_bus.BUS_CPU_D5p.qp());
    /* p04.POKU*/ dma_reg.POKU_DMA_A14n.dff8p(_LORU_FF46_WRn, cpu_bus.BUS_CPU_D6p.qp());
    /* p04.MARU*/ dma_reg.MARU_DMA_A15n.dff8p(_LORU_FF46_WRn, cpu_bus.BUS_CPU_D7p.qp());

    /*#p04.MOLU*/ wire _MOLU_FF46_RDp = and2(_XEDA_FF46p, ASOT_CPU_RDp);
    /*#p04.NYGO*/ wire _NYGO_FF46_RDn = not1(_MOLU_FF46_RDp);
    /*#p04.PUSY*/ wire _PUSY_FF46_RDp = not1(_NYGO_FF46_RDn);

    /*#p04.POLY*/ cpu_bus.BUS_CPU_D0p.tri_6pn(_PUSY_FF46_RDp, dma_reg.NAFA_DMA_A08n.qp08());
    /* p04.ROFO*/ cpu_bus.BUS_CPU_D1p.tri_6pn(_PUSY_FF46_RDp, dma_reg.PYNE_DMA_A09n.qp08());
    /* p04.REMA*/ cpu_bus.BUS_CPU_D2p.tri_6pn(_PUSY_FF46_RDp, dma_reg.PARA_DMA_A10n.qp08());
    /* p04.PANE*/ cpu_bus.BUS_CPU_D3p.tri_6pn(_PUSY_FF46_RDp, dma_reg.NYDO_DMA_A11n.qp08());
    /* p04.PARE*/ cpu_bus.BUS_CPU_D4p.tri_6pn(_PUSY_FF46_RDp, dma_reg.NYGY_DMA_A12n.qp08());
    /* p04.RALY*/ cpu_bus.BUS_CPU_D5p.tri_6pn(_PUSY_FF46_RDp, dma_reg.PULA_DMA_A13n.qp08());
    /* p04.RESU*/ cpu_bus.BUS_CPU_D6p.tri_6pn(_PUSY_FF46_RDp, dma_reg.POKU_DMA_A14n.qp08());
    /* p04.NUVY*/ cpu_bus.BUS_CPU_D7p.tri_6pn(_PUSY_FF46_RDp, dma_reg.MARU_DMA_A15n.qp08());
  }

  //------------------------------------------------------------------------------
  // ser_reg.tock(top, cpu_bus);

  /*p06.SANO*/ wire _SANO_ADDR_FF00_FF03 = and3(cpu_bus.SARE_XX00_XX07p(), cpu_bus.SEFY_A02n(), cpu_bus.SYKE_FF00_FFFFp());
  /*p06.URYS*/ wire _URYS_FF01_WRn = nand4(top.TAPU_CPU_WRp_xxxxEFGx, _SANO_ADDR_FF00_FF03, cpu_bus.BUS_CPU_A00.qp(), cpu_bus.TOLA_A01n());
  /*p06.DAKU*/ wire _DAKU_FF01_WRp = not1(_URYS_FF01_WRn);
  /*p06.UFEG*/ wire _UFEG_FF01_RDp = and4(top.TEDO_CPU_RDp, _SANO_ADDR_FF00_FF03, cpu_bus.BUS_CPU_A00.qp(), cpu_bus.TOLA_A01n());

  /*p06.COBA*/ wire _COBA_SER_CNT3n = not1(top.ser_reg.CALY_INT_SERp.qp17());
  /*p06.CABY*/ wire _CABY_XFER_RESET = and2(_COBA_SER_CNT3n, top.clk_reg.ALUR_SYS_RSTn);

  /*p06.UWAM*/ wire _UWAM_FF02_WRn = nand4(top.TAPU_CPU_WRp_xxxxEFGx, _SANO_ADDR_FF00_FF03, cpu_bus.TOVY_A00n(), cpu_bus.BUS_CPU_A01.qp());
  /*p06.ETAF*/ top.ser_reg.ETAF_XFER_START.dff17(_UWAM_FF02_WRn, _CABY_XFER_RESET, cpu_bus.BUS_CPU_D7p.qp());
  /*p06.CULY*/ top.ser_reg.CULY_XFER_DIR  .dff17(_UWAM_FF02_WRn, top.clk_reg.ALUR_SYS_RSTn, cpu_bus.BUS_CPU_D0p.qp());

  /*p06.UCOM*/ wire _UCOM_FF02_RD = and4(top.TEDO_CPU_RDp, _SANO_ADDR_FF00_FF03, cpu_bus.TOVY_A00n(), cpu_bus.BUS_CPU_A01.qp());
  /*p06.ELUV*/ cpu_bus.BUS_CPU_D7p.tri_6pn(_UCOM_FF02_RD, top.ser_reg.ETAF_XFER_START.qn16());
  /*p06.CORE*/ cpu_bus.BUS_CPU_D0p.tri_6pn(_UCOM_FF02_RD, top.ser_reg.CULY_XFER_DIR.qn16());

  /*p06.COTY*/ top.ser_reg.COTY_SER_CLK.dff17(top.tim_reg.UVYN_DIV_05n, _UWAM_FF02_WRn, top.ser_reg.COTY_SER_CLK.qn16());

  /* p06.CAVE*/ wire _CAVE_SER_CLK_MUXn = mux2n(top.ser_reg.CULY_XFER_DIR.qp17(), top.ser_reg.COTY_SER_CLK.qp17(), top.ser_reg.PIN_SCK.qn());

  /* p06.DAWA*/ wire _DAWA_SER_CLK = or2(_CAVE_SER_CLK_MUXn, top.ser_reg.ETAF_XFER_START.qn16()); // this must stop the clock or something when the transmit's done
  /* p06.EDYL*/ wire _EDYL_SER_CLK = not1(_DAWA_SER_CLK);
  /* p06.EPYT*/ wire _EPYT_SER_CLK = not1(_EDYL_SER_CLK);
  /* p06.DEHO*/ wire _DEHO_SER_CLK = not1(_EPYT_SER_CLK);
  /* p06.DAWE*/ wire _DAWE_SER_CLK = not1(_DEHO_SER_CLK);

  /* p06.CARO*/ wire _CARO_SER_RST = and2(_UWAM_FF02_WRn, top.clk_reg.ALUR_SYS_RSTn);
  /* p06.CAFA*/ top.ser_reg.CAFA_SER_CNT0.dff17(_DAWA_SER_CLK,                    _CARO_SER_RST, top.ser_reg.CAFA_SER_CNT0.qn16());
  /* p06.CYLO*/ top.ser_reg.CYLO_SER_CNT1.dff17(top.ser_reg.CAFA_SER_CNT0.qn16(), _CARO_SER_RST, top.ser_reg.CYLO_SER_CNT1.qn16());
  /* p06.CYDE*/ top.ser_reg.CYDE_SER_CNT2.dff17(top.ser_reg.CYLO_SER_CNT1.qn16(), _CARO_SER_RST, top.ser_reg.CYDE_SER_CNT2.qn16());
  /* p06.CALY*/ top.ser_reg.CALY_INT_SERp.dff17(top.ser_reg.CYDE_SER_CNT2.qn16(), _CARO_SER_RST, top.ser_reg.CALY_INT_SERp.qn16());

  /* p06.CUFU*/ wire _CUFU_SER_DATA0_SETn = nand2(cpu_bus.BUS_CPU_D0p.qp(), _DAKU_FF01_WRp);
  /* p06.DOCU*/ wire _DOCU_SER_DATA1_SETn = nand2(cpu_bus.BUS_CPU_D1p.qp(), _DAKU_FF01_WRp);
  /* p06.DELA*/ wire _DELA_SER_DATA2_SETn = nand2(cpu_bus.BUS_CPU_D2p.qp(), _DAKU_FF01_WRp);
  /* p06.DYGE*/ wire _DYGE_SER_DATA3_SETn = nand2(cpu_bus.BUS_CPU_D3p.qp(), _DAKU_FF01_WRp);
  /* p06.DOLA*/ wire _DOLA_SER_DATA4_SETn = nand2(cpu_bus.BUS_CPU_D4p.qp(), _DAKU_FF01_WRp);
  /* p06.ELOK*/ wire _ELOK_SER_DATA5_SETn = nand2(cpu_bus.BUS_CPU_D5p.qp(), _DAKU_FF01_WRp);
  /* p06.EDEL*/ wire _EDEL_SER_DATA6_SETn = nand2(cpu_bus.BUS_CPU_D6p.qp(), _DAKU_FF01_WRp);
  /* p06.EFEF*/ wire _EFEL_SER_DATA7_SETn = nand2(cpu_bus.BUS_CPU_D7p.qp(), _DAKU_FF01_WRp);

  // COHY matches BYHA, and BYHA's C input _must_ be and, so this is (A | B) & C
  // and dff22's SET and RST _must_ be SETn/RSTn

  /* p06.COHY*/ wire _COHY_SER_DATA0_RSTn = or_and3(_URYS_FF01_WRn, cpu_bus.BUS_CPU_D0p.qp(), top.clk_reg.ALUR_SYS_RSTn);
  /* p06.DUMO*/ wire _DUMO_SER_DATA1_RSTn = or_and3(_URYS_FF01_WRn, cpu_bus.BUS_CPU_D1p.qp(), top.clk_reg.ALUR_SYS_RSTn);
  /* p06.DYBO*/ wire _DYBO_SER_DATA2_RSTn = or_and3(_URYS_FF01_WRn, cpu_bus.BUS_CPU_D2p.qp(), top.clk_reg.ALUR_SYS_RSTn);
  /* p06.DAJU*/ wire _DAJU_SER_DATA3_RSTn = or_and3(_URYS_FF01_WRn, cpu_bus.BUS_CPU_D3p.qp(), top.clk_reg.ALUR_SYS_RSTn);
  /* p06.DYLY*/ wire _DYLY_SER_DATA4_RSTn = or_and3(_URYS_FF01_WRn, cpu_bus.BUS_CPU_D4p.qp(), top.clk_reg.ALUR_SYS_RSTn);
  /* p06.EHUJ*/ wire _EHUJ_SER_DATA5_RSTn = or_and3(_URYS_FF01_WRn, cpu_bus.BUS_CPU_D5p.qp(), top.clk_reg.ALUR_SYS_RSTn);
  /* p06.EFAK*/ wire _EFAK_SER_DATA6_RSTn = or_and3(_URYS_FF01_WRn, cpu_bus.BUS_CPU_D6p.qp(), top.clk_reg.ALUR_SYS_RSTn);
  /* p06.EGUV*/ wire _EGUV_SER_DATA7_RSTn = or_and3(_URYS_FF01_WRn, cpu_bus.BUS_CPU_D7p.qp(), top.clk_reg.ALUR_SYS_RSTn);

  /* p06.CAGE*/ wire _CAGE_SIN_Cp = not1(top.ser_reg.PIN_SIN.qn());
  /* p06.CUBA*/ top.ser_reg.CUBA_SER_DATA0.dff22(_DAWE_SER_CLK, _CUFU_SER_DATA0_SETn, _COHY_SER_DATA0_RSTn, _CAGE_SIN_Cp);
  /* p06.DEGU*/ top.ser_reg.DEGU_SER_DATA1.dff22(_DAWE_SER_CLK, _DOCU_SER_DATA1_SETn, _DUMO_SER_DATA1_RSTn, top.ser_reg.CUBA_SER_DATA0.qp16());
  /* p06.DYRA*/ top.ser_reg.DYRA_SER_DATA2.dff22(_DAWE_SER_CLK, _DELA_SER_DATA2_SETn, _DYBO_SER_DATA2_RSTn, top.ser_reg.DEGU_SER_DATA1.qp16());
  /* p06.DOJO*/ top.ser_reg.DOJO_SER_DATA3.dff22(_DAWE_SER_CLK, _DYGE_SER_DATA3_SETn, _DAJU_SER_DATA3_RSTn, top.ser_reg.DYRA_SER_DATA2.qp16());
  /* p06.DOVU*/ top.ser_reg.DOVU_SER_DATA4.dff22(_EPYT_SER_CLK, _DOLA_SER_DATA4_SETn, _DYLY_SER_DATA4_RSTn, top.ser_reg.DOJO_SER_DATA3.qp16());
  /* p06.EJAB*/ top.ser_reg.EJAB_SER_DATA5.dff22(_EPYT_SER_CLK, _ELOK_SER_DATA5_SETn, _EHUJ_SER_DATA5_RSTn, top.ser_reg.DOVU_SER_DATA4.qp16());
  /* p06.EROD*/ top.ser_reg.EROD_SER_DATA6.dff22(_EPYT_SER_CLK, _EDEL_SER_DATA6_SETn, _EFAK_SER_DATA6_RSTn, top.ser_reg.EJAB_SER_DATA5.qp16());
  /* p06.EDER*/ top.ser_reg.EDER_SER_DATA7.dff22(_EPYT_SER_CLK, _EFEL_SER_DATA7_SETn, _EGUV_SER_DATA7_RSTn, top.ser_reg.EROD_SER_DATA6.qp16());

  /* p06.ELYS*/ top.ser_reg.ELYS_SER_OUT  .dff17(_EDYL_SER_CLK, top.clk_reg.ALUR_SYS_RSTn, top.ser_reg.EDER_SER_DATA7.qp16());

  // FIXME hacking out debug stuff
  ///*p05.KENA*/ ser_reg.SOUT  = mux2n(KUKO_DBG_FF00_D6, ser_reg.SER_OUT, FF60_0);

  /* p06.KEXU*/ wire _KEXU = nand2(top.ser_reg.COTY_SER_CLK.qp17(), top.ser_reg.CULY_XFER_DIR.qp17());
  /* p06.KUJO*/ wire _KUJO = nor2 (top.ser_reg.COTY_SER_CLK.qp17(), /*p06.JAGO*/ not1(top.ser_reg.CULY_XFER_DIR.qp17()));

  /* p06.KEXU*/ top.ser_reg.PIN_SCK.io_pin(_KEXU, _KUJO, top.ser_reg.CULY_XFER_DIR.qp17());
  /* p05.KENA*/ top.ser_reg.PIN_SOUT.io_pin(top.ser_reg.ELYS_SER_OUT.qp17(), top.ser_reg.ELYS_SER_OUT.qp17());

  /*#p06.CUGY*/ cpu_bus.BUS_CPU_D0p.tri_6pn(_UFEG_FF01_RDp, top.ser_reg.CUBA_SER_DATA0.qn15());
  /* p06.DUDE*/ cpu_bus.BUS_CPU_D1p.tri_6pn(_UFEG_FF01_RDp, top.ser_reg.DEGU_SER_DATA1.qn15());
  /* p06.DETU*/ cpu_bus.BUS_CPU_D2p.tri_6pn(_UFEG_FF01_RDp, top.ser_reg.DYRA_SER_DATA2.qn15());
  /* p06.DASO*/ cpu_bus.BUS_CPU_D3p.tri_6pn(_UFEG_FF01_RDp, top.ser_reg.DOJO_SER_DATA3.qn15());
  /* p06.DAME*/ cpu_bus.BUS_CPU_D4p.tri_6pn(_UFEG_FF01_RDp, top.ser_reg.DOVU_SER_DATA4.qn15());
  /* p06.EVOK*/ cpu_bus.BUS_CPU_D5p.tri_6pn(_UFEG_FF01_RDp, top.ser_reg.EJAB_SER_DATA5.qn15());
  /* p06.EFAB*/ cpu_bus.BUS_CPU_D6p.tri_6pn(_UFEG_FF01_RDp, top.ser_reg.EROD_SER_DATA6.qn15());
  /*#p06.ETAK*/ cpu_bus.BUS_CPU_D7p.tri_6pn(_UFEG_FF01_RDp, top.ser_reg.EDER_SER_DATA7.qn15());


  //------------------------------------------------------------------------------

  joypad.tock(top, cpu_bus);

  //------------------------------------------------------------------------------

  sprite_scanner.tock(top);

  //------------------------------------------------------------------------------

  lcd_reg.tock(top, cpu_bus);

  //------------------------------------------------------------------------------

  sprite_store.tock(top);

  //------------------------------------------------------------------------------

  pix_pipe.tock(top, cpu_bus);

  //------------------------------------------------------------------------------

  sprite_fetcher.tock(top);

  //------------------------------------------------------------------------------

  tile_fetcher.tock(top);

  //------------------------------------------------------------------------------

  int_reg.tock(top, cpu_bus);

  //------------------------------------------------------------------------------

  cpu_bus.PIN_CPU_ADDR_HIp.set(cpu_bus.SYRO_FE00_FFFFp());
  cpu_bus.PIN_CPU_BOOTp.set(TUTU_ADDR_BOOTp);

  //------------------------------------------------------------------------------
  // ext_bus.tock(top);


  /*p08.MOCA*/ wire _MOCA_DBG_EXT_RD = nor2(top.TEXO_ADDR_EXT_AND_NOT_VRAM, top.UMUT_MODE_DBG1p);
  /*p08.LEVO*/ wire _LEVO_ADDR_INT_OR_ADDR_VRAM = not1(top.TEXO_ADDR_EXT_AND_NOT_VRAM);
  /*p08.LAGU*/ wire _LAGU = and_or3(top.cpu_bus.PIN_CPU_RDp.qp(), _LEVO_ADDR_INT_OR_ADDR_VRAM, top.cpu_bus.PIN_CPU_WRp.qp());
  /*p08.LYWE*/ wire _LYWE = not1(_LAGU);
  /*p08.MOTY*/ wire _MOTY_CPU_EXT_RD = or2(_MOCA_DBG_EXT_RD, _LYWE);

  /*p08.TOVA*/ wire _TOVA_MODE_DBG2n = not1(top.UNOR_MODE_DBG2p);

  //----------------------------------------
  // Ext pins

  {
    /*p08.TYMU*/ wire _TYMU_EXT_RDn = nor2(top.dma_reg.LUMA_DMA_CARTp, _MOTY_CPU_EXT_RD);
    /*p08.UGAC*/ wire _UGAC_RD_A = nand2(_TYMU_EXT_RDn, _TOVA_MODE_DBG2n);
    /*p08.URUN*/ wire _URUN_RD_D = nor2 (_TYMU_EXT_RDn, top.UNOR_MODE_DBG2p);
    top.ext_bus.PIN_EXT_RDn.io_pin(_UGAC_RD_A, _URUN_RD_D);
  }

  {

    /*p08.MEXO*/ wire _MEXO_CPU_WRn_ABCDxxxH = not1(top.APOV_CPU_WRp_xxxxEFGx);
    /*p08.NEVY*/ wire _NEVY = or2(_MEXO_CPU_WRn_ABCDxxxH, _MOCA_DBG_EXT_RD);
    /*p08.PUVA*/ wire _PUVA_EXT_WRn = or2(_NEVY, top.dma_reg.LUMA_DMA_CARTp);
    /*p08.UVER*/ wire _UVER_WR_A = nand2(_PUVA_EXT_WRn, _TOVA_MODE_DBG2n);
    /*p08.USUF*/ wire _USUF_WR_D = nor2 (_PUVA_EXT_WRn, top.UNOR_MODE_DBG2p);
    top.ext_bus.PIN_EXT_WRn.io_pin(_UVER_WR_A, _USUF_WR_D);
  }


  {
    /*p08.SOGY*/ wire _SOGY_A14n = not1(top.cpu_bus.BUS_CPU_A14.qp());
    /*p08.TUMA*/ wire _TUMA_CART_RAM = and3(top.cpu_bus.BUS_CPU_A13.qp(), _SOGY_A14n, top.cpu_bus.BUS_CPU_A15.qp());
    /*p08.TYNU*/ wire _TYNU_ADDR_RAM = and_or3(top.cpu_bus.BUS_CPU_A15.qp(), top.cpu_bus.BUS_CPU_A14.qp(), _TUMA_CART_RAM);

    /*p08.TOZA*/ wire _TOZA_PIN_EXT_CS_A_xxCDEFGH = and3(top.ABUZ_xxCDEFGH, _TYNU_ADDR_RAM, top.cpu_bus.TUNA_0000_FDFFp());
    /*p08.TYHO*/ wire _TYHO_PIN_EXT_CS_A_xxCDEFGH = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.MARU_DMA_A15n.qn07(), _TOZA_PIN_EXT_CS_A_xxCDEFGH);
    top.ext_bus.PIN_EXT_CSn.io_pin(_TYHO_PIN_EXT_CS_A_xxCDEFGH, _TYHO_PIN_EXT_CS_A_xxCDEFGH);
  }


  //----------------------------------------
  // Ext address

  // DMA address / CPU address latch -> ext addr pins
  {
    /*p08.MULE*/ wire _MULE_MODE_DBG1n  = not1(top.UMUT_MODE_DBG1p);
    /*p08.LOXO*/ wire _LOXO_HOLDn = and_or3(_MULE_MODE_DBG1n, top.TEXO_ADDR_EXT_AND_NOT_VRAM, top.UMUT_MODE_DBG1p);
    /*p08.LASY*/ wire _LASY_HOLDp = not1(_LOXO_HOLDn);
    /*p08.MATE*/ wire _MATE_HOLDn = not1(_LASY_HOLDp);

    /* p08.ALOR*/ top.ext_bus.ALOR_EXT_ADDR_LATCH_00p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A00.qp());
    /* p08.APUR*/ top.ext_bus.APUR_EXT_ADDR_LATCH_01p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A01.qp());
    /* p08.ALYR*/ top.ext_bus.ALYR_EXT_ADDR_LATCH_02p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A02.qp());
    /* p08.ARET*/ top.ext_bus.ARET_EXT_ADDR_LATCH_03p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A03.qp());
    /* p08.AVYS*/ top.ext_bus.AVYS_EXT_ADDR_LATCH_04p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A04.qp());
    /* p08.ATEV*/ top.ext_bus.ATEV_EXT_ADDR_LATCH_05p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A05.qp());
    /* p08.AROS*/ top.ext_bus.AROS_EXT_ADDR_LATCH_06p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A06.qp());
    /* p08.ARYM*/ top.ext_bus.ARYM_EXT_ADDR_LATCH_07p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A07.qp());
    /* p08.LUNO*/ top.ext_bus.LUNO_EXT_ADDR_LATCH_08p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A08.qp());
    /* p08.LYSA*/ top.ext_bus.LYSA_EXT_ADDR_LATCH_09p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A09.qp());
    /* p08.PATE*/ top.ext_bus.PATE_EXT_ADDR_LATCH_10p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A10.qp());
    /* p08.LUMY*/ top.ext_bus.LUMY_EXT_ADDR_LATCH_11p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A11.qp());
    /* p08.LOBU*/ top.ext_bus.LOBU_EXT_ADDR_LATCH_12p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A12.qp());
    /* p08.LONU*/ top.ext_bus.LONU_EXT_ADDR_LATCH_13p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A13.qp());
    /* p08.NYRE*/ top.ext_bus.NYRE_EXT_ADDR_LATCH_14p.tp_latch(_MATE_HOLDn, top.cpu_bus.BUS_CPU_A14.qp());


    /* p08.AMET*/ wire _AMET_A00p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.NAKY_DMA_A00p.qp17(), top.ext_bus.ALOR_EXT_ADDR_LATCH_00p.qp08());
    /* p08.ATOL*/ wire _ATOL_A01p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.PYRO_DMA_A01p.qp17(), top.ext_bus.APUR_EXT_ADDR_LATCH_01p.qp08());
    /* p08.APOK*/ wire _APOK_A02p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.NEFY_DMA_A02p.qp17(), top.ext_bus.ALYR_EXT_ADDR_LATCH_02p.qp08());
    /* p08.AMER*/ wire _AMER_A03p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.MUTY_DMA_A03p.qp17(), top.ext_bus.ARET_EXT_ADDR_LATCH_03p.qp08());
    /* p08.ATEM*/ wire _ATEM_A04p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.NYKO_DMA_A04p.qp17(), top.ext_bus.AVYS_EXT_ADDR_LATCH_04p.qp08());
    /* p08.ATOV*/ wire _ATOV_A05p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.PYLO_DMA_A05p.qp17(), top.ext_bus.ATEV_EXT_ADDR_LATCH_05p.qp08());
    /* p08.ATYR*/ wire _ATYR_A06p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.NUTO_DMA_A06p.qp17(), top.ext_bus.AROS_EXT_ADDR_LATCH_06p.qp08());
    /*#p08.ASUR*/ wire _ASUR_A07p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.MUGU_DMA_A07p.qp17(), top.ext_bus.ARYM_EXT_ADDR_LATCH_07p.qp08());
    /*#p08.MANO*/ wire _MANO_A08p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.NAFA_DMA_A08n.qn07(), top.ext_bus.LUNO_EXT_ADDR_LATCH_08p.qp08());
    /* p08.MASU*/ wire _MASU_A09p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.PYNE_DMA_A09n.qn07(), top.ext_bus.LYSA_EXT_ADDR_LATCH_09p.qp08());
    /* p08.PAMY*/ wire _PAMY_A10p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.PARA_DMA_A10n.qn07(), top.ext_bus.PATE_EXT_ADDR_LATCH_10p.qp08());
    /* p08.MALE*/ wire _MALE_A11p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.NYDO_DMA_A11n.qn07(), top.ext_bus.LUMY_EXT_ADDR_LATCH_11p.qp08());
    /* p08.MOJY*/ wire _MOJY_A12p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.NYGY_DMA_A12n.qn07(), top.ext_bus.LOBU_EXT_ADDR_LATCH_12p.qp08());
    /* p08.MUCE*/ wire _MUCE_A13p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.PULA_DMA_A13n.qn07(), top.ext_bus.LONU_EXT_ADDR_LATCH_13p.qp08());
    /* p08.PEGE*/ wire _PEGE_A14p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.POKU_DMA_A14n.qn07(), top.ext_bus.NYRE_EXT_ADDR_LATCH_14p.qp08());

    /*p08.KUPO*/ wire _KUPO = nand2(_AMET_A00p, _TOVA_MODE_DBG2n);
    /*p08.CABA*/ wire _CABA = nand2(_ATOL_A01p, _TOVA_MODE_DBG2n);
    /*p08.BOKU*/ wire _BOKU = nand2(_APOK_A02p, _TOVA_MODE_DBG2n);
    /*p08.BOTY*/ wire _BOTY = nand2(_AMER_A03p, _TOVA_MODE_DBG2n);
    /*p08.BYLA*/ wire _BYLA = nand2(_ATEM_A04p, _TOVA_MODE_DBG2n);
    /*p08.BADU*/ wire _BADU = nand2(_ATOV_A05p, _TOVA_MODE_DBG2n);
    /*p08.CEPU*/ wire _CEPU = nand2(_ATYR_A06p, _TOVA_MODE_DBG2n);
    /*p08.DEFY*/ wire _DEFY = nand2(_ASUR_A07p, _TOVA_MODE_DBG2n);
    /*p08.MYNY*/ wire _MYNY = nand2(_MANO_A08p, _TOVA_MODE_DBG2n);
    /*p08.MUNE*/ wire _MUNE = nand2(_MASU_A09p, _TOVA_MODE_DBG2n);
    /*p08.ROXU*/ wire _ROXU = nand2(_PAMY_A10p, _TOVA_MODE_DBG2n);
    /*p08.LEPY*/ wire _LEPY = nand2(_MALE_A11p, _TOVA_MODE_DBG2n);
    /*p08.LUCE*/ wire _LUCE = nand2(_MOJY_A12p, _TOVA_MODE_DBG2n);
    /*p08.LABE*/ wire _LABE = nand2(_MUCE_A13p, _TOVA_MODE_DBG2n);
    /*p08.PUHE*/ wire _PUHE = nand2(_PEGE_A14p, _TOVA_MODE_DBG2n);

    /*p08.KOTY*/ wire _KOTY = nor2 (_AMET_A00p, top.UNOR_MODE_DBG2p);
    /*p08.COTU*/ wire _COTU = nor2 (_ATOL_A01p, top.UNOR_MODE_DBG2p);
    /*p08.BAJO*/ wire _BAJO = nor2 (_APOK_A02p, top.UNOR_MODE_DBG2p);
    /*p08.BOLA*/ wire _BOLA = nor2 (_AMER_A03p, top.UNOR_MODE_DBG2p);
    /*p08.BEVO*/ wire _BEVO = nor2 (_ATEM_A04p, top.UNOR_MODE_DBG2p);
    /*p08.AJAV*/ wire _AJAV = nor2 (_ATOV_A05p, top.UNOR_MODE_DBG2p);
    /*p08.CYKA*/ wire _CYKA = nor2 (_ATYR_A06p, top.UNOR_MODE_DBG2p);
    /*p08.COLO*/ wire _COLO = nor2 (_ASUR_A07p, top.UNOR_MODE_DBG2p);
    /*p08.MEGO*/ wire _MEGO = nor2 (_MANO_A08p, top.UNOR_MODE_DBG2p);
    /*p08.MENY*/ wire _MENY = nor2 (_MASU_A09p, top.UNOR_MODE_DBG2p);
    /*p08.RORE*/ wire _RORE = nor2 (_PAMY_A10p, top.UNOR_MODE_DBG2p);
    /*p08.LYNY*/ wire _LYNY = nor2 (_MALE_A11p, top.UNOR_MODE_DBG2p);
    /*p08.LOSO*/ wire _LOSO = nor2 (_MOJY_A12p, top.UNOR_MODE_DBG2p);
    /*p08.LEVA*/ wire _LEVA = nor2 (_MUCE_A13p, top.UNOR_MODE_DBG2p);
    /*p08.PAHY*/ wire _PAHY = nor2 (_PEGE_A14p, top.UNOR_MODE_DBG2p);

    top.ext_bus.PIN_EXT_A00p.io_pin(_KUPO, _KOTY);
    top.ext_bus.PIN_EXT_A01p.io_pin(_CABA, _COTU);
    top.ext_bus.PIN_EXT_A02p.io_pin(_BOKU, _BAJO);
    top.ext_bus.PIN_EXT_A03p.io_pin(_BOTY, _BOLA);
    top.ext_bus.PIN_EXT_A04p.io_pin(_BYLA, _BEVO);
    top.ext_bus.PIN_EXT_A05p.io_pin(_BADU, _AJAV);
    top.ext_bus.PIN_EXT_A06p.io_pin(_CEPU, _CYKA);
    top.ext_bus.PIN_EXT_A07p.io_pin(_DEFY, _COLO);
    top.ext_bus.PIN_EXT_A08p.io_pin(_MYNY, _MEGO);
    top.ext_bus.PIN_EXT_A09p.io_pin(_MUNE, _MENY);
    top.ext_bus.PIN_EXT_A10p.io_pin(_ROXU, _RORE);
    top.ext_bus.PIN_EXT_A11p.io_pin(_LEPY, _LYNY);
    top.ext_bus.PIN_EXT_A12p.io_pin(_LUCE, _LOSO);
    top.ext_bus.PIN_EXT_A13p.io_pin(_LABE, _LEVA);
    top.ext_bus.PIN_EXT_A14p.io_pin(_PUHE, _PAHY);
  }


  {
    // A15 is "special"

    /* p08.RYCA*/ wire _RYCA_MODE_DBG2n = not1(top.UNOR_MODE_DBG2p);
    /* p08.SOBY*/ wire _SOBY_A15n = nor2(top.cpu_bus.BUS_CPU_A15.qp(), top.TUTU_ADDR_BOOTp);
    /* p08.SEPY*/ wire _SEPY_A15p_ABxxxxxx = nand2(top.ABUZ_xxCDEFGH, _SOBY_A15n);
    /* p08.TAZY*/ wire _TAZY_A15p = mux2p(top.dma_reg.LUMA_DMA_CARTp, top.dma_reg.MARU_DMA_A15n.qn07(), _SEPY_A15p_ABxxxxxx);
    /* p08.SUZE*/ wire _SUZE_PIN_EXT_A15n = nand2(_TAZY_A15p, _RYCA_MODE_DBG2n);
    /* p08.RULO*/ wire _RULO_PIN_EXT_A15n = nor2 (_TAZY_A15p, top.UNOR_MODE_DBG2p);

    top.ext_bus.PIN_EXT_A15p.io_pin(_SUZE_PIN_EXT_A15n, _RULO_PIN_EXT_A15n);
  }

  //----------------------------------------
  // Ext data out


  // CPU data bus -> external data bus
  {
    // FIXME So does this mean that if the CPU writes to the external bus during dma, that data
    // will actually end up in oam?

    /*p08.REDU*/ wire _REDU_CPU_RDn = not1(top.TEDO_CPU_RDp);
    /*p08.RORU*/ wire _RORU_CBD_TO_EPDn = mux2p(top.UNOR_MODE_DBG2p, _REDU_CPU_RDn, _MOTY_CPU_EXT_RD);
    /*p08.LULA*/ wire _LULA_CBD_TO_EPDp = not1(_RORU_CBD_TO_EPDn);

    /*p25.RUXA*/ wire _RUXA = nand2(top.cpu_bus.BUS_CPU_D0p.qp(), _LULA_CBD_TO_EPDp);
    /*p25.RUJA*/ wire _RUJA = nand2(top.cpu_bus.BUS_CPU_D1p.qp(), _LULA_CBD_TO_EPDp);
    /*p25.RABY*/ wire _RABY = nand2(top.cpu_bus.BUS_CPU_D2p.qp(), _LULA_CBD_TO_EPDp);
    /*p25.RERA*/ wire _RERA = nand2(top.cpu_bus.BUS_CPU_D3p.qp(), _LULA_CBD_TO_EPDp);
    /*p25.RORY*/ wire _RORY = nand2(top.cpu_bus.BUS_CPU_D4p.qp(), _LULA_CBD_TO_EPDp);
    /*p25.RYVO*/ wire _RYVO = nand2(top.cpu_bus.BUS_CPU_D5p.qp(), _LULA_CBD_TO_EPDp);
    /*p25.RAFY*/ wire _RAFY = nand2(top.cpu_bus.BUS_CPU_D6p.qp(), _LULA_CBD_TO_EPDp);
    /*p25.RAVU*/ wire _RAVU = nand2(top.cpu_bus.BUS_CPU_D7p.qp(), _LULA_CBD_TO_EPDp);

    /*p08.RUNE*/ wire _RUNE = nor2 (top.cpu_bus.BUS_CPU_D0p.qp(), _RORU_CBD_TO_EPDn);
    /*p08.RYPU*/ wire _RYPU = nor2 (top.cpu_bus.BUS_CPU_D1p.qp(), _RORU_CBD_TO_EPDn);
    /*p08.SULY*/ wire _SULY = nor2 (top.cpu_bus.BUS_CPU_D2p.qp(), _RORU_CBD_TO_EPDn);
    /*p08.SEZE*/ wire _SEZE = nor2 (top.cpu_bus.BUS_CPU_D3p.qp(), _RORU_CBD_TO_EPDn);
    /*p08.RESY*/ wire _RESY = nor2 (top.cpu_bus.BUS_CPU_D4p.qp(), _RORU_CBD_TO_EPDn);
    /*p08.TAMU*/ wire _TAMU = nor2 (top.cpu_bus.BUS_CPU_D5p.qp(), _RORU_CBD_TO_EPDn);
    /*p08.ROGY*/ wire _ROGY = nor2 (top.cpu_bus.BUS_CPU_D6p.qp(), _RORU_CBD_TO_EPDn);
    /*p08.RYDA*/ wire _RYDA = nor2 (top.cpu_bus.BUS_CPU_D7p.qp(), _RORU_CBD_TO_EPDn);

    top.ext_bus.PIN_EXT_D00p.io_pin(_RUXA, _RUNE, _LULA_CBD_TO_EPDp);
    top.ext_bus.PIN_EXT_D01p.io_pin(_RUJA, _RYPU, _LULA_CBD_TO_EPDp);
    top.ext_bus.PIN_EXT_D02p.io_pin(_RABY, _SULY, _LULA_CBD_TO_EPDp);
    top.ext_bus.PIN_EXT_D03p.io_pin(_RERA, _SEZE, _LULA_CBD_TO_EPDp);
    top.ext_bus.PIN_EXT_D04p.io_pin(_RORY, _RESY, _LULA_CBD_TO_EPDp);
    top.ext_bus.PIN_EXT_D05p.io_pin(_RYVO, _TAMU, _LULA_CBD_TO_EPDp);
    top.ext_bus.PIN_EXT_D06p.io_pin(_RAFY, _ROGY, _LULA_CBD_TO_EPDp);
    top.ext_bus.PIN_EXT_D07p.io_pin(_RAVU, _RYDA, _LULA_CBD_TO_EPDp);
  }

  //----------------------------------------
  // Ext data in
  {
    // -> ext bus
    /*p08.LAVO*/ wire _LAVO_HOLDn = nand3(top.cpu_bus.PIN_CPU_RDp.qp(), top.TEXO_ADDR_EXT_AND_NOT_VRAM, top.cpu_bus.PIN_CPU_LATCH_EXT.qp());

    // Ext pin -> Ext latch
    /*#p08.SOMA*/ top.ext_bus.SOMA_EXT_DATA_LATCH_D0n.tp_latch(_LAVO_HOLDn, top.ext_bus.PIN_EXT_D00p.qn());
    /* p08.RONY*/ top.ext_bus.RONY_EXT_DATA_LATCH_D1n.tp_latch(_LAVO_HOLDn, top.ext_bus.PIN_EXT_D01p.qn());
    /* p08.RAXY*/ top.ext_bus.RAXY_EXT_DATA_LATCH_D2n.tp_latch(_LAVO_HOLDn, top.ext_bus.PIN_EXT_D02p.qn());
    /* p08.SELO*/ top.ext_bus.SELO_EXT_DATA_LATCH_D3n.tp_latch(_LAVO_HOLDn, top.ext_bus.PIN_EXT_D03p.qn());
    /* p08.SODY*/ top.ext_bus.SODY_EXT_DATA_LATCH_D4n.tp_latch(_LAVO_HOLDn, top.ext_bus.PIN_EXT_D04p.qn());
    /* p08.SAGO*/ top.ext_bus.SAGO_EXT_DATA_LATCH_D5n.tp_latch(_LAVO_HOLDn, top.ext_bus.PIN_EXT_D05p.qn());
    /* p08.RUPA*/ top.ext_bus.RUPA_EXT_DATA_LATCH_D6n.tp_latch(_LAVO_HOLDn, top.ext_bus.PIN_EXT_D06p.qn());
    /* p08.SAZY*/ top.ext_bus.SAZY_EXT_DATA_LATCH_D7n.tp_latch(_LAVO_HOLDn, top.ext_bus.PIN_EXT_D07p.qn());

    // Ext latch -> int bus
    /*#p08.RYMA*/ top.cpu_bus.BUS_CPU_D0p.tri_6nn(_LAVO_HOLDn, top.ext_bus.SOMA_EXT_DATA_LATCH_D0n.qp08());
    /* p08.RUVO*/ top.cpu_bus.BUS_CPU_D1p.tri_6nn(_LAVO_HOLDn, top.ext_bus.RONY_EXT_DATA_LATCH_D1n.qp08());
    /* p08.RYKO*/ top.cpu_bus.BUS_CPU_D2p.tri_6nn(_LAVO_HOLDn, top.ext_bus.RAXY_EXT_DATA_LATCH_D2n.qp08());
    /* p08.TAVO*/ top.cpu_bus.BUS_CPU_D3p.tri_6nn(_LAVO_HOLDn, top.ext_bus.SELO_EXT_DATA_LATCH_D3n.qp08());
    /* p08.TEPE*/ top.cpu_bus.BUS_CPU_D4p.tri_6nn(_LAVO_HOLDn, top.ext_bus.SODY_EXT_DATA_LATCH_D4n.qp08());
    /* p08.SAFO*/ top.cpu_bus.BUS_CPU_D5p.tri_6nn(_LAVO_HOLDn, top.ext_bus.SAGO_EXT_DATA_LATCH_D5n.qp08());
    /* p08.SEVU*/ top.cpu_bus.BUS_CPU_D6p.tri_6nn(_LAVO_HOLDn, top.ext_bus.RUPA_EXT_DATA_LATCH_D6n.qp08());
    /* p08.TAJU*/ top.cpu_bus.BUS_CPU_D7p.tri_6nn(_LAVO_HOLDn, top.ext_bus.SAZY_EXT_DATA_LATCH_D7n.qp08());
  }

  //------------------------------------------------------------------------------
  //oam_bus.tock(top);

  wire GND = 0;
  wire WEFE_VCC = 1;

  //----------------------------------------
  // OAM signals

  /* p28.XYNY*/ wire _XYNY_ABCDxxxx = not1(top.clk_reg.MOPA_xxxxEFGH);

  /*#p28.GEKA*/ wire _GEKA_OAM_A0p = not1(top.oam_bus.BUS_OAM_A0n.qp()); // -> WAFO, YLYC, WUKU
  /* p28.WAFO*/ wire _WAFO_OAM_A0n = not1(_GEKA_OAM_A0p);

  /* p28.AJON*/ wire _AJON_PPU_OAM_ENp = and2(top.dma_reg.BOGE_DMA_RUNNINGn, top.pix_pipe.XYMU_RENDERINGn.qn03()); // def AND. ppu can read oam when there's rendering but no dma

  /*#p28.AJEP*/ wire _AJEP_SCAN_OAM_LATCHn = nand2(top.sprite_scanner.ACYL_SCANNINGp, top.clk_reg.XOCE_xBCxxFGx); // schematic wrong, is def nand2
  /*#p28.XUJA*/ wire _XUJA_SPR_OAM_LATCHn  = not1(top.sprite_fetcher.WEFY_SPR_READp);
  /*#p28.BOFE*/ wire _BOFE_LATCH_EXTn = not1(top.CATY_LATCH_EXTp);
  /*#p28.BOTA*/ wire _BOTA_CPU_OAM_OEn  = nand3(_BOFE_LATCH_EXTn, top.cpu_bus.SARO_FE00_FEFFp(), top.ASOT_CPU_RDp); // Schematic wrong, this is NAND

  //probe(16, "PIN_CPU_LATCH_EXT", top.cpu_bus.PIN_CPU_LATCH_EXT.qp());
  //probe(17, "SARO_FE00_FEFFp",   top.cpu_bus.SARO_FE00_FEFFp());
  //probe(18, "ASOT_CPU_RDp",      top.ASOT_CPU_RDp);
  //probe(15, "BOTA_CPU_OAM_OEn",  _BOTA_CPU_OAM_OEn);

  /*#p28.ASYT*/ wire _ASYT_OAM_OEn = and3(_AJEP_SCAN_OAM_LATCHn, _XUJA_SPR_OAM_LATCHn, _BOTA_CPU_OAM_OEn); // def and
  /*#p28.BODE*/ wire _BODE_OAM_OEp = not1(_ASYT_OAM_OEn);

  /* p28.AJUJ*/ wire _AJUJ_OAM_BUSYn = nor3(top.dma_reg.MATU_DMA_RUNNINGp.qp17(), top.sprite_scanner.ACYL_SCANNINGp, _AJON_PPU_OAM_ENp); // def nor
  /* p28.AMAB*/ wire _AMAB_CPU_OAM_ENp = and2(top.cpu_bus.SARO_FE00_FEFFp(), _AJUJ_OAM_BUSYn); // def and

  // CUFE looks like BYHA minus an inverter

  bool _COTA_OAM_CLKn;
  {
    /* p29.XYSO*/ wire _XYSO_xBCDxFGH = not1(top.clk_reg.WOJO_AxxxExxx);
    /* p25.AVER*/ wire _AVER_SCAN_OAM_CLK = nand2(top.sprite_scanner.ACYL_SCANNINGp, _XYSO_xBCDxFGH);
    /* p25.XUJY*/ wire _XUJY_PPU_OAM_CLK  = not1(top.sprite_fetcher.VAPE_FETCH_OAM_CLK);
    /* p25.CUFE*/ wire _CUFE_OAM_CLK  = not1(or_and3(top.cpu_bus.SARO_FE00_FEFFp(), top.dma_reg.MATU_DMA_RUNNINGp.qp17(), top.clk_reg.MOPA_xxxxEFGH));

    /* p25.BYCU*/ wire _BYCU_OAM_CLKp = nand3(_AVER_SCAN_OAM_CLK, _XUJY_PPU_OAM_CLK, _CUFE_OAM_CLK);
    /* p25.COTA*/ _COTA_OAM_CLKn = not1(_BYCU_OAM_CLKp);
  }

  //----------------------------------------
  // OAM pins
  // PIN_CPU_LATCH_EXT _blocks_ DMA from writing to OAM? wat?

  {
    top.oam_bus.PIN_OAM_CLK.set(_COTA_OAM_CLKn);

    /*#p28.YVAL*/ wire _YVAL_OAM_OEn = not1(_BODE_OAM_OEp);
    /*#p28.YRYV*/ wire _YRYU_OAM_OEp = not1(_YVAL_OAM_OEn);
    /*#p28.ZODO*/ wire _ZODO_OAM_OEn = not1(_YRYU_OAM_OEp);
    top.oam_bus.PIN_OAM_OE.set(_ZODO_OAM_OEn);

    /*p04.MAKA*/ top.oam_bus.MAKA_HOLD_MEMp.dff17(top.clk_reg.ZEME_AxCxExGx, top.clk_reg.CUNU_SYS_RSTn, top.CATY_LATCH_EXTp);

    /*p04.NAXY*/ wire _NAXY_DMA_OAM_WRp = nor2(top.clk_reg.UVYT_ABCDxxxx, top.oam_bus.MAKA_HOLD_MEMp.qp17()); // def nor2
    /*p04.POWU*/ wire _POWU_DMA_OAM_WRp  = and2(top.dma_reg.MATU_DMA_RUNNINGp.qp17(), _NAXY_DMA_OAM_WRp); // def and
    /*p04.WYJA*/ wire _WYJA_OAM_WRp      = and_or3(_AMAB_CPU_OAM_ENp, top.CUPA_CPU_WRp_xxxxEFGx, _POWU_DMA_OAM_WRp);
    /*p28.YNYC*/ wire _YNYC_OAM_A_WRp = and2(_WYJA_OAM_WRp, _WAFO_OAM_A0n);
    /*p28.YLYC*/ wire _YLYC_OAM_B_WRp = and2(_WYJA_OAM_WRp, _GEKA_OAM_A0p);
    /*p28.ZOFE*/ wire _ZOFE_OAM_A_WRn = not1(_YNYC_OAM_A_WRp);
    /*p28.ZONE*/ wire _ZONE_OAM_B_WRn = not1(_YLYC_OAM_B_WRp);
    top.oam_bus.PIN_OAM_WR_A.set(_ZOFE_OAM_A_WRn);
    top.oam_bus.PIN_OAM_WR_B.set(_ZONE_OAM_B_WRn);
  }

  //----------------------------------------
  // OAM address mux

  {
    /*p28.APAR*/ wire _APAR_SCAN_OAM_RDn = not1(top.sprite_scanner.ACYL_SCANNINGp);
    /*p04.DUGA*/ wire _DUGA_DMA_OAM_RDn  = not1(top.dma_reg.MATU_DMA_RUNNINGp.qp17());
    /*p28.ASAM*/ wire _ASAM_CPU_OAM_RDn  = or3(top.sprite_scanner.ACYL_SCANNINGp, top.pix_pipe.XYMU_RENDERINGn.qn03(), top.dma_reg.MATU_DMA_RUNNINGp.qp17());
    /*p28.BETE*/ wire _BETE_PPU_OAM_RDn  = not1(_AJON_PPU_OAM_ENp);

    //probe(20, "_APAR_SCAN_OAM_RDn", _APAR_SCAN_OAM_RDn);
    //probe(21, "_DUGA_DMA_OAM_RDn ", _DUGA_DMA_OAM_RDn );
    //probe(22, "_ASAM_CPU_OAM_RDn ", _ASAM_CPU_OAM_RDn );
    //probe(23, "_BETE_PPU_OAM_RDn ", _BETE_PPU_OAM_RDn );

    // Scanner addr -> OAM addr
    /*p28.GEFY*/ top.oam_bus.BUS_OAM_A0n.tri_6nn(_APAR_SCAN_OAM_RDn, GND);
    /*p28.WUWE*/ top.oam_bus.BUS_OAM_A1n.tri_6nn(_APAR_SCAN_OAM_RDn, GND);
    /*p28.GUSE*/ top.oam_bus.BUS_OAM_A2n.tri_6nn(_APAR_SCAN_OAM_RDn, top.sprite_scanner.YFEL_SCAN0.qp17());
    /*p28.GEMA*/ top.oam_bus.BUS_OAM_A3n.tri_6nn(_APAR_SCAN_OAM_RDn, top.sprite_scanner.WEWY_SCAN1.qp17());
    /*p28.FUTO*/ top.oam_bus.BUS_OAM_A4n.tri_6nn(_APAR_SCAN_OAM_RDn, top.sprite_scanner.GOSO_SCAN2.qp17());
    /*p28.FAKU*/ top.oam_bus.BUS_OAM_A5n.tri_6nn(_APAR_SCAN_OAM_RDn, top.sprite_scanner.ELYN_SCAN3.qp17());
    /*p28.GAMA*/ top.oam_bus.BUS_OAM_A6n.tri_6nn(_APAR_SCAN_OAM_RDn, top.sprite_scanner.FAHA_SCAN4.qp17());
    /*p28.GOBY*/ top.oam_bus.BUS_OAM_A7n.tri_6nn(_APAR_SCAN_OAM_RDn, top.sprite_scanner.FONY_SCAN5.qp17());

    // DMA addr -> OAM addr
    /*p28.FODO*/ top.oam_bus.BUS_OAM_A0n.tri_6nn(_DUGA_DMA_OAM_RDn, top.dma_reg.NAKY_DMA_A00p.qp17());
    /*p28.FESA*/ top.oam_bus.BUS_OAM_A1n.tri_6nn(_DUGA_DMA_OAM_RDn, top.dma_reg.PYRO_DMA_A01p.qp17());
    /*p28.FAGO*/ top.oam_bus.BUS_OAM_A2n.tri_6nn(_DUGA_DMA_OAM_RDn, top.dma_reg.NEFY_DMA_A02p.qp17());
    /*p28.FYKY*/ top.oam_bus.BUS_OAM_A3n.tri_6nn(_DUGA_DMA_OAM_RDn, top.dma_reg.MUTY_DMA_A03p.qp17());
    /*p28.ELUG*/ top.oam_bus.BUS_OAM_A4n.tri_6nn(_DUGA_DMA_OAM_RDn, top.dma_reg.NYKO_DMA_A04p.qp17());
    /*p28.EDOL*/ top.oam_bus.BUS_OAM_A5n.tri_6nn(_DUGA_DMA_OAM_RDn, top.dma_reg.PYLO_DMA_A05p.qp17());
    /*p28.FYDU*/ top.oam_bus.BUS_OAM_A6n.tri_6nn(_DUGA_DMA_OAM_RDn, top.dma_reg.NUTO_DMA_A06p.qp17());
    /*p28.FETU*/ top.oam_bus.BUS_OAM_A7n.tri_6nn(_DUGA_DMA_OAM_RDn, top.dma_reg.MUGU_DMA_A07p.qp17());

    // CPU addr -> OAM addr
    /*p28.GARO*/ top.oam_bus.BUS_OAM_A0n.tri_6nn(_ASAM_CPU_OAM_RDn, top.cpu_bus.BUS_CPU_A00.qp());
    /*p28.WACU*/ top.oam_bus.BUS_OAM_A1n.tri_6nn(_ASAM_CPU_OAM_RDn, top.cpu_bus.BUS_CPU_A01.qp());
    /*p28.GOSE*/ top.oam_bus.BUS_OAM_A2n.tri_6nn(_ASAM_CPU_OAM_RDn, top.cpu_bus.BUS_CPU_A02.qp());
    /*p28.WAPE*/ top.oam_bus.BUS_OAM_A3n.tri_6nn(_ASAM_CPU_OAM_RDn, top.cpu_bus.BUS_CPU_A03.qp());
    /*p28.FEVU*/ top.oam_bus.BUS_OAM_A4n.tri_6nn(_ASAM_CPU_OAM_RDn, top.cpu_bus.BUS_CPU_A04.qp());
    /*p28.GERA*/ top.oam_bus.BUS_OAM_A5n.tri_6nn(_ASAM_CPU_OAM_RDn, top.cpu_bus.BUS_CPU_A05.qp());
    /*p28.WAXA*/ top.oam_bus.BUS_OAM_A6n.tri_6nn(_ASAM_CPU_OAM_RDn, top.cpu_bus.BUS_CPU_A06.qp());
    /*p28.FOBY*/ top.oam_bus.BUS_OAM_A7n.tri_6nn(_ASAM_CPU_OAM_RDn, top.cpu_bus.BUS_CPU_A07.qp());

    // PPU addr -> OAM addr
    /*p28.GECA*/ top.oam_bus.BUS_OAM_A0n.tri_6nn(_BETE_PPU_OAM_RDn, WEFE_VCC);
    /*p28.WYDU*/ top.oam_bus.BUS_OAM_A1n.tri_6nn(_BETE_PPU_OAM_RDn, WEFE_VCC);
    /*p28.GYBU*/ top.oam_bus.BUS_OAM_A2n.tri_6nn(_BETE_PPU_OAM_RDn, top.sprite_store.SPR_TRI_I0p.qp());
    /*p28.GYKA*/ top.oam_bus.BUS_OAM_A3n.tri_6nn(_BETE_PPU_OAM_RDn, top.sprite_store.SPR_TRI_I1p.qp());
    /*p28.FABY*/ top.oam_bus.BUS_OAM_A4n.tri_6nn(_BETE_PPU_OAM_RDn, top.sprite_store.SPR_TRI_I2p.qp());
    /*p28.FACO*/ top.oam_bus.BUS_OAM_A5n.tri_6nn(_BETE_PPU_OAM_RDn, top.sprite_store.SPR_TRI_I3p.qp());
    /*p28.FUGU*/ top.oam_bus.BUS_OAM_A6n.tri_6nn(_BETE_PPU_OAM_RDn, top.sprite_store.SPR_TRI_I4p.qp());
    /*p28.FYKE*/ top.oam_bus.BUS_OAM_A7n.tri_6nn(_BETE_PPU_OAM_RDn, top.sprite_store.SPR_TRI_I5p.qp());
  }

  //----------------------------------------
  // OAM data out

  // CBD -> OBD
  {
    /*p28.XUTO*/ wire _XUTO_CPU_OAM_WRp = and2(top.cpu_bus.SARO_FE00_FEFFp(), top.CUPA_CPU_WRp_xxxxEFGx);
    /*p28.WUJE*/ top.oam_bus.WUJE_CPU_OAM_WRn.nor_latch(_XYNY_ABCDxxxx, _XUTO_CPU_OAM_WRp);

    /*p28.XUPA*/ wire _XUPA_CPU_OAM_WRp = not1(top.oam_bus.WUJE_CPU_OAM_WRn.qp04());
    /*p28.APAG*/ wire _APAG_CBD_TO_OBDp = amux2(_XUPA_CPU_OAM_WRp, _AMAB_CPU_OAM_ENp, _AJUJ_OAM_BUSYn, top.cpu_bus.ADAH_FE00_FEFFn());
    /*p28.AZUL*/ wire _AZUL_CBD_TO_OBDn = not1(_APAG_CBD_TO_OBDp);

    /*p28.ZAXA*/ top.oam_bus.BUS_OAM_DA0n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D0p.qp());
    /*p28.ZAKY*/ top.oam_bus.BUS_OAM_DA1n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D1p.qp());
    /*p28.WULE*/ top.oam_bus.BUS_OAM_DA2n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D2p.qp());
    /*p28.ZOZO*/ top.oam_bus.BUS_OAM_DA3n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D3p.qp());
    /*p28.ZUFO*/ top.oam_bus.BUS_OAM_DA4n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D4p.qp());
    /*p28.ZATO*/ top.oam_bus.BUS_OAM_DA5n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D5p.qp());
    /*p28.YVUC*/ top.oam_bus.BUS_OAM_DA6n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D6p.qp());
    /*p28.ZUFE*/ top.oam_bus.BUS_OAM_DA7n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D7p.qp());

    /*p28.ZAMY*/ top.oam_bus.BUS_OAM_DB0n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D0p.qp());
    /*p28.ZOPU*/ top.oam_bus.BUS_OAM_DB1n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D1p.qp());
    /*p28.WYKY*/ top.oam_bus.BUS_OAM_DB2n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D2p.qp());
    /*p28.ZAJA*/ top.oam_bus.BUS_OAM_DB3n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D3p.qp());
    /*p28.ZUGA*/ top.oam_bus.BUS_OAM_DB4n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D4p.qp());
    /*p28.ZUMO*/ top.oam_bus.BUS_OAM_DB5n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D5p.qp());
    /*p28.XYTO*/ top.oam_bus.BUS_OAM_DB6n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D6p.qp());
    /*p28.ZYFA*/ top.oam_bus.BUS_OAM_DB7n.tri_6nn(_AZUL_CBD_TO_OBDn, top.cpu_bus.BUS_CPU_D7p.qp());
  }

  // EBD -> OPD
  {
    /*p25.RALO*/ wire _RALO_EXT_D0p = not1(top.ext_bus.PIN_EXT_D00p.qn());
    /*p25.TUNE*/ wire _TUNE_EXT_D1p = not1(top.ext_bus.PIN_EXT_D01p.qn());
    /*p25.SERA*/ wire _SERA_EXT_D2p = not1(top.ext_bus.PIN_EXT_D02p.qn());
    /*p25.TENU*/ wire _TENU_EXT_D3p = not1(top.ext_bus.PIN_EXT_D03p.qn());
    /*p25.SYSA*/ wire _SYSA_EXT_D4p = not1(top.ext_bus.PIN_EXT_D04p.qn());
    /*p25.SUGY*/ wire _SUGY_EXT_D5p = not1(top.ext_bus.PIN_EXT_D05p.qn());
    /*p25.TUBE*/ wire _TUBE_EXT_D6p = not1(top.ext_bus.PIN_EXT_D06p.qn());
    /*p25.SYZO*/ wire _SYZO_EXT_D7p = not1(top.ext_bus.PIN_EXT_D07p.qn());

    /*p25.CEDE*/ wire _CEDE_EBD_TO_OBDn = not1(top.dma_reg.LUMA_DMA_CARTp);
    /*p25.WASA*/ top.oam_bus.BUS_OAM_DA0n.tri_6nn(_CEDE_EBD_TO_OBDn, _RALO_EXT_D0p);
    /*p25.BOMO*/ top.oam_bus.BUS_OAM_DA1n.tri_6nn(_CEDE_EBD_TO_OBDn, _TUNE_EXT_D1p);
    /*p25.BASA*/ top.oam_bus.BUS_OAM_DA2n.tri_6nn(_CEDE_EBD_TO_OBDn, _SERA_EXT_D2p);
    /*p25.CAKO*/ top.oam_bus.BUS_OAM_DA3n.tri_6nn(_CEDE_EBD_TO_OBDn, _TENU_EXT_D3p);
    /*p25.BUMA*/ top.oam_bus.BUS_OAM_DA4n.tri_6nn(_CEDE_EBD_TO_OBDn, _SYSA_EXT_D4p);
    /*p25.BUPY*/ top.oam_bus.BUS_OAM_DA5n.tri_6nn(_CEDE_EBD_TO_OBDn, _SUGY_EXT_D5p);
    /*p25.BASY*/ top.oam_bus.BUS_OAM_DA6n.tri_6nn(_CEDE_EBD_TO_OBDn, _TUBE_EXT_D6p);
    /*p25.BAPE*/ top.oam_bus.BUS_OAM_DA7n.tri_6nn(_CEDE_EBD_TO_OBDn, _SYZO_EXT_D7p);

    /*p25.WEJO*/ top.oam_bus.BUS_OAM_DB0n.tri_6nn(_CEDE_EBD_TO_OBDn, _RALO_EXT_D0p);
    /*p25.BUBO*/ top.oam_bus.BUS_OAM_DB1n.tri_6nn(_CEDE_EBD_TO_OBDn, _TUNE_EXT_D1p);
    /*p25.BETU*/ top.oam_bus.BUS_OAM_DB2n.tri_6nn(_CEDE_EBD_TO_OBDn, _SERA_EXT_D2p);
    /*p25.CYME*/ top.oam_bus.BUS_OAM_DB3n.tri_6nn(_CEDE_EBD_TO_OBDn, _TENU_EXT_D3p);
    /*p25.BAXU*/ top.oam_bus.BUS_OAM_DB4n.tri_6nn(_CEDE_EBD_TO_OBDn, _SYSA_EXT_D4p);
    /*p25.BUHU*/ top.oam_bus.BUS_OAM_DB5n.tri_6nn(_CEDE_EBD_TO_OBDn, _SUGY_EXT_D5p);
    /*p25.BYNY*/ top.oam_bus.BUS_OAM_DB6n.tri_6nn(_CEDE_EBD_TO_OBDn, _TUBE_EXT_D6p);
    /*p25.BYPY*/ top.oam_bus.BUS_OAM_DB7n.tri_6nn(_CEDE_EBD_TO_OBDn, _SYZO_EXT_D7p);
  }

  // VBD -> OPD
  {
    /*p28.AZAR*/ wire _AZAR_VBD_TO_OBDn = not1(top.dma_reg.LUFA_DMA_VRAMp);
    /*p28.WUZU*/ top.oam_bus.BUS_OAM_DA0n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D0p.qp());
    /*p28.AXER*/ top.oam_bus.BUS_OAM_DA1n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D1p.qp());
    /*p28.ASOX*/ top.oam_bus.BUS_OAM_DA2n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D2p.qp());
    /*p28.CETU*/ top.oam_bus.BUS_OAM_DA3n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D3p.qp());
    /*p28.ARYN*/ top.oam_bus.BUS_OAM_DA4n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D4p.qp());
    /*p28.ACOT*/ top.oam_bus.BUS_OAM_DA5n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D5p.qp());
    /*p28.CUJE*/ top.oam_bus.BUS_OAM_DA6n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D6p.qp());
    /*p28.ATER*/ top.oam_bus.BUS_OAM_DA7n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D7p.qp());

    /*p28.WOWA*/ top.oam_bus.BUS_OAM_DB0n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D0p.qp());
    /*p28.AVEB*/ top.oam_bus.BUS_OAM_DB1n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D1p.qp());
    /*p28.AMUH*/ top.oam_bus.BUS_OAM_DB2n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D2p.qp());
    /*p28.COFO*/ top.oam_bus.BUS_OAM_DB3n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D3p.qp());
    /*p28.AZOZ*/ top.oam_bus.BUS_OAM_DB4n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D4p.qp());
    /*p28.AGYK*/ top.oam_bus.BUS_OAM_DB5n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D5p.qp());
    /*p28.BUSE*/ top.oam_bus.BUS_OAM_DB6n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D6p.qp());
    /*p28.ANUM*/ top.oam_bus.BUS_OAM_DB7n.tri_6nn(_AZAR_VBD_TO_OBDn, top.vram_bus.BUS_VRAM_D7p.qp());
  }

  //----------------------------------------
  // OAM data in

  // OPD -> OBL
  {
    /* p29.YDYV*/ top.oam_bus.YDYV_OAM_LATCH_DA0n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DA0n.qp());
    /* p29.YCEB*/ top.oam_bus.YCEB_OAM_LATCH_DA1n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DA1n.qp());
    /* p29.ZUCA*/ top.oam_bus.ZUCA_OAM_LATCH_DA2n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DA2n.qp());
    /* p29.WONE*/ top.oam_bus.WONE_OAM_LATCH_DA3n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DA3n.qp());
    /* p29.ZAXE*/ top.oam_bus.ZAXE_OAM_LATCH_DA4n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DA4n.qp());
    /* p29.XAFU*/ top.oam_bus.XAFU_OAM_LATCH_DA5n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DA5n.qp());
    /* p29.YSES*/ top.oam_bus.YSES_OAM_LATCH_DA6n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DA6n.qp());
    /* p29.ZECA*/ top.oam_bus.ZECA_OAM_LATCH_DA7n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DA7n.qp());

    /*#p31.XYKY*/ top.oam_bus.XYKY_OAM_LATCH_DB0n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DB0n.qp());
    /* p31.YRUM*/ top.oam_bus.YRUM_OAM_LATCH_DB1n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DB1n.qp());
    /* p31.YSEX*/ top.oam_bus.YSEX_OAM_LATCH_DB2n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DB2n.qp());
    /* p31.YVEL*/ top.oam_bus.YVEL_OAM_LATCH_DB3n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DB3n.qp());
    /* p31.WYNO*/ top.oam_bus.WYNO_OAM_LATCH_DB4n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DB4n.qp());
    /* p31.CYRA*/ top.oam_bus.CYRA_OAM_LATCH_DB5n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DB5n.qp());
    /* p31.ZUVE*/ top.oam_bus.ZUVE_OAM_LATCH_DB6n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DB6n.qp());
    /* p31.ECED*/ top.oam_bus.ECED_OAM_LATCH_DB7n.tp_latch(_BODE_OAM_OEp, top.oam_bus.BUS_OAM_DB7n.qp());
  }

  // OBL -> temp
  {
    /*p29.YWOK*/ wire _YWOK_OAM_CLKn = not1(_COTA_OAM_CLKn);
    /*p29.YWOK*/ wire _YWOK_OAM_CLKp = not1(_YWOK_OAM_CLKn);

    /*#p29.XUSO*/ top.oam_bus.XUSO_OAM_DA0p.dff8n(_YWOK_OAM_CLKn, _YWOK_OAM_CLKp, top.oam_bus.YDYV_OAM_LATCH_DA0n.qp08());
    /* p29.XEGU*/ top.oam_bus.XEGU_OAM_DA1p.dff8n(_YWOK_OAM_CLKn, _YWOK_OAM_CLKp, top.oam_bus.YCEB_OAM_LATCH_DA1n.qp08());
    /* p29.YJEX*/ top.oam_bus.YJEX_OAM_DA2p.dff8n(_YWOK_OAM_CLKn, _YWOK_OAM_CLKp, top.oam_bus.ZUCA_OAM_LATCH_DA2n.qp08());
    /* p29.XYJU*/ top.oam_bus.XYJU_OAM_DA3p.dff8n(_YWOK_OAM_CLKn, _YWOK_OAM_CLKp, top.oam_bus.WONE_OAM_LATCH_DA3n.qp08());
    /* p29.YBOG*/ top.oam_bus.YBOG_OAM_DA4p.dff8n(_YWOK_OAM_CLKn, _YWOK_OAM_CLKp, top.oam_bus.ZAXE_OAM_LATCH_DA4n.qp08());
    /* p29.WYSO*/ top.oam_bus.WYSO_OAM_DA5p.dff8n(_YWOK_OAM_CLKn, _YWOK_OAM_CLKp, top.oam_bus.XAFU_OAM_LATCH_DA5n.qp08());
    /* p29.XOTE*/ top.oam_bus.XOTE_OAM_DA6p.dff8n(_YWOK_OAM_CLKn, _YWOK_OAM_CLKp, top.oam_bus.YSES_OAM_LATCH_DA6n.qp08());
    /* p29.YZAB*/ top.oam_bus.YZAB_OAM_DA7p.dff8n(_YWOK_OAM_CLKn, _YWOK_OAM_CLKp, top.oam_bus.ZECA_OAM_LATCH_DA7n.qp08());

    /* p31.XEGA*/ wire _XEGA_OAM_CLKp = not1(_COTA_OAM_CLKn);
    /* p31.XEGA*/ wire _XEGA_OAM_CLKn = not1(_XEGA_OAM_CLKp);

    /* p31.YLOR*/ top.oam_bus.YLOR_OAM_DB0p.dff8n(_XEGA_OAM_CLKp, _XEGA_OAM_CLKn, top.oam_bus.XYKY_OAM_LATCH_DB0n.qp08());
    /* p31.ZYTY*/ top.oam_bus.ZYTY_OAM_DB1p.dff8n(_XEGA_OAM_CLKp, _XEGA_OAM_CLKn, top.oam_bus.YRUM_OAM_LATCH_DB1n.qp08());
    /* p31.ZYVE*/ top.oam_bus.ZYVE_OAM_DB2p.dff8n(_XEGA_OAM_CLKp, _XEGA_OAM_CLKn, top.oam_bus.YSEX_OAM_LATCH_DB2n.qp08());
    /* p31.ZEZY*/ top.oam_bus.ZEZY_OAM_DB3p.dff8n(_XEGA_OAM_CLKp, _XEGA_OAM_CLKn, top.oam_bus.YVEL_OAM_LATCH_DB3n.qp08());
    /* p31.GOMO*/ top.oam_bus.GOMO_OAM_DB4p.dff8n(_XEGA_OAM_CLKp, _XEGA_OAM_CLKn, top.oam_bus.WYNO_OAM_LATCH_DB4n.qp08());
    /* p31.BAXO*/ top.oam_bus.BAXO_OAM_DB5p.dff8n(_XEGA_OAM_CLKp, _XEGA_OAM_CLKn, top.oam_bus.CYRA_OAM_LATCH_DB5n.qp08());
    /* p31.YZOS*/ top.oam_bus.YZOS_OAM_DB6p.dff8n(_XEGA_OAM_CLKp, _XEGA_OAM_CLKn, top.oam_bus.ZUVE_OAM_LATCH_DB6n.qp08());
    /* p31.DEPO*/ top.oam_bus.DEPO_OAM_DB7p.dff8n(_XEGA_OAM_CLKp, _XEGA_OAM_CLKn, top.oam_bus.ECED_OAM_LATCH_DB7n.qp08());
  }

  // OBL -> CBD
  {
    /* p28.GUKO*/ wire _GUKO_OBL_TO_CBDp = and3(top.LEKO_CPU_RDp, _AMAB_CPU_OAM_ENp, _WAFO_OAM_A0n);
    /* p28.WUKU*/ wire _WUKU_OBL_TO_CBDp = and3(top.LEKO_CPU_RDp, _AMAB_CPU_OAM_ENp, _GEKA_OAM_A0p);

    /* p28.WUME*/ wire _WUME_OBL_TO_CBDn = not1(_GUKO_OBL_TO_CBDp);
    /* p29.YFAP*/ top.cpu_bus.BUS_CPU_D0p.tri10_np(_WUME_OBL_TO_CBDn, top.oam_bus.YDYV_OAM_LATCH_DA0n.qn10());
    /* p29.XELE*/ top.cpu_bus.BUS_CPU_D1p.tri10_np(_WUME_OBL_TO_CBDn, top.oam_bus.YCEB_OAM_LATCH_DA1n.qn10());
    /* p29.YPON*/ top.cpu_bus.BUS_CPU_D2p.tri10_np(_WUME_OBL_TO_CBDn, top.oam_bus.ZUCA_OAM_LATCH_DA2n.qn10());
    /* p29.XUVO*/ top.cpu_bus.BUS_CPU_D3p.tri10_np(_WUME_OBL_TO_CBDn, top.oam_bus.WONE_OAM_LATCH_DA3n.qn10());
    /* p29.ZYSA*/ top.cpu_bus.BUS_CPU_D4p.tri10_np(_WUME_OBL_TO_CBDn, top.oam_bus.ZAXE_OAM_LATCH_DA4n.qn10());
    /* p29.YWEG*/ top.cpu_bus.BUS_CPU_D5p.tri10_np(_WUME_OBL_TO_CBDn, top.oam_bus.XAFU_OAM_LATCH_DA5n.qn10());
    /* p29.XABU*/ top.cpu_bus.BUS_CPU_D6p.tri10_np(_WUME_OBL_TO_CBDn, top.oam_bus.YSES_OAM_LATCH_DA6n.qn10());
    /* p29.YTUX*/ top.cpu_bus.BUS_CPU_D7p.tri10_np(_WUME_OBL_TO_CBDn, top.oam_bus.ZECA_OAM_LATCH_DA7n.qn10());

    /* p28.WEWU*/ wire _WEWU_OBL_TO_CBDn = not1(_WUKU_OBL_TO_CBDp);
    /* p31.XACA*/ top.cpu_bus.BUS_CPU_D0p.tri10_np(_WEWU_OBL_TO_CBDn, top.oam_bus.XYKY_OAM_LATCH_DB0n.qn10());
    /* p31.XAGU*/ top.cpu_bus.BUS_CPU_D1p.tri10_np(_WEWU_OBL_TO_CBDn, top.oam_bus.YRUM_OAM_LATCH_DB1n.qn10());
    /* p31.XEPU*/ top.cpu_bus.BUS_CPU_D2p.tri10_np(_WEWU_OBL_TO_CBDn, top.oam_bus.YSEX_OAM_LATCH_DB2n.qn10());
    /* p31.XYGU*/ top.cpu_bus.BUS_CPU_D3p.tri10_np(_WEWU_OBL_TO_CBDn, top.oam_bus.YVEL_OAM_LATCH_DB3n.qn10());
    /* p31.XUNA*/ top.cpu_bus.BUS_CPU_D4p.tri10_np(_WEWU_OBL_TO_CBDn, top.oam_bus.WYNO_OAM_LATCH_DB4n.qn10());
    /* p31.DEVE*/ top.cpu_bus.BUS_CPU_D5p.tri10_np(_WEWU_OBL_TO_CBDn, top.oam_bus.CYRA_OAM_LATCH_DB5n.qn10());
    /* p31.ZEHA*/ top.cpu_bus.BUS_CPU_D6p.tri10_np(_WEWU_OBL_TO_CBDn, top.oam_bus.ZUVE_OAM_LATCH_DB6n.qn10());
    /* p31.FYRA*/ top.cpu_bus.BUS_CPU_D7p.tri10_np(_WEWU_OBL_TO_CBDn, top.oam_bus.ECED_OAM_LATCH_DB7n.qn10());
  }

  //------------------------------------------------------------------------------

  wire VYPO = 1;

  /*#p25.TEFA*/ wire _TEFA_8000_9FFFp = nor2(top.cpu_bus.SYRO_FE00_FFFFp(), top.TEXO_ADDR_EXT_AND_NOT_VRAM);
  /*#p25.SOSE*/ wire _SOSE_8000_9FFFp = and2(top.cpu_bus.BUS_CPU_A15.qp(), _TEFA_8000_9FFFp);

  /*#p25.ROPY*/ wire _ROPY_RENDERINGn = not1(top.pix_pipe.XYMU_RENDERINGn.qn03());

  /*#p25.TEFY*/ wire _TEFY_VRAM_MCSp = not1(top.vram_bus.PIN_VRAM_CSn.qn());
  /*#p25.TUCA*/ wire _TUCA_CPU_VRAM_RDp = and2(_SOSE_8000_9FFFp, top.ABUZ_xxCDEFGH);

  /*#p25.TOLE*/ wire _TOLE_VRAM_RDp     = mux2p(top.TUTO_DBG_VRAMp, _TEFY_VRAM_MCSp, _TUCA_CPU_VRAM_RDp);
  /*#p25.SERE*/ wire _SERE_CPU_VRM_RDp = and2(_TOLE_VRAM_RDp, _ROPY_RENDERINGn);

  /*#p25.TAVY*/ wire _TAVY_MOEp = not1(top.vram_bus.PIN_VRAM_OEn.qn());
  /*#p25.TEGU*/ wire _TEGU_CPU_VRAM_WRn = nand2(_SOSE_8000_9FFFp, top.cpu_bus.PIN_CPU_WRp.qp());  // Schematic wrong, second input is PIN_CPU_WRp
  /*#p25.SALE*/ wire _SALE_CPU_VRAM_WRn = mux2p(top.TUTO_DBG_VRAMp, _TAVY_MOEp, _TEGU_CPU_VRAM_WRn);
  /*#p25.RUVY*/ wire _RUVY_VRAM_WRp    = not1(_SALE_CPU_VRAM_WRn);
  /*#p25.SAZO*/ wire _SAZO_CBD_TO_VPDp = and2(_SERE_CPU_VRM_RDp, _RUVY_VRAM_WRp);
  /*#p25.RYJE*/ wire _RYJE_CBD_TO_VPDn = not1(_SAZO_CBD_TO_VPDp);
  /*#p25.REVO*/ wire _REVO_CBD_TO_VPDp = not1(_RYJE_CBD_TO_VPDn);
  /*#p25.RELA*/ wire _RELA_CBD_TO_VPDp = or2(_REVO_CBD_TO_VPDp, _SAZO_CBD_TO_VPDp);
  /*#p25.RENA*/ wire _RENA_CBD_TO_VPDn = not1(_RELA_CBD_TO_VPDp);

  /*#p25.XANE*/ wire _XANE_VRAM_LOCKn = nor2(top.dma_reg.LUFA_DMA_VRAMp, top.pix_pipe.XYMU_RENDERINGn.qn03());

  /*#p25.RACO*/ wire _RACO_DBG_VRAMn = not1(top.TUTO_DBG_VRAMp);
  (void)_RACO_DBG_VRAMn;

  //----------------------------------------
  // VRAM pins

  {
    /*p25.TUJA*/ wire _TUJA_CPU_VRAM_WRp = and2(_SOSE_8000_9FFFp, top.APOV_CPU_WRp_xxxxEFGx);
    /*p25.SUDO*/ wire _SUDO_MWRp_C = not1(top.vram_bus.PIN_VRAM_WRn.qn());
    /*p25.TYJY*/ wire _TYJY_VRAM_WRp = mux2p(top.TUTO_DBG_VRAMp, _SUDO_MWRp_C, _TUJA_CPU_VRAM_WRp);

    /*p25.SOHY*/ wire _SOHY_MWRn = nand2(_TYJY_VRAM_WRp, _SERE_CPU_VRM_RDp);

    /*p25.TAXY*/ wire _TAXY_MWRn_A = and2(_SOHY_MWRn, _RACO_DBG_VRAMn);
    /*p25.SOFY*/ wire _SOFY_MWRn_D = or2(_SOHY_MWRn, top.TUTO_DBG_VRAMp);
    /*p25.SYSY*/ wire _SYSY_MWRp_A = not1(_TAXY_MWRn_A);
    /*p25.RAGU*/ wire _RAGU_MWRp_D = not1(_SOFY_MWRn_D);

    top.vram_bus.PIN_VRAM_WRn.io_pin(_SYSY_MWRp_A, _RAGU_MWRp_D);
  }

  {
    /*p25.RYLU*/ wire _RYLU_CPU_VRAM_RDn = nand2(_SALE_CPU_VRAM_WRn, _XANE_VRAM_LOCKn);

    /*p25.RAWA*/ wire _RAWA_SPR_VRAM_RDn = not1(top.sprite_fetcher.SOHO_SPR_VRAM_RDp);
    /*p25.APAM*/ wire _APAM_DMA_VRAM_RDn = not1(top.dma_reg.LUFA_DMA_VRAMp);
    /*p27.MYMA*/ wire _MYMA_BGW_VRAM_RDn = not1(top.tile_fetcher.LONY_BG_FETCH_RUNNINGp.qp03());

    /*p25.RACU*/ wire _RACU_MOEn = and4(_RYLU_CPU_VRAM_RDn, _RAWA_SPR_VRAM_RDn, _MYMA_BGW_VRAM_RDn, _APAM_DMA_VRAM_RDn); // def and

    /*p25.SEMA*/ wire _SEMA_MOEn_A = and2(_RACU_MOEn, _RACO_DBG_VRAMn);
    /*p25.RUTE*/ wire _RUTE_MOEn_D = or2(_RACU_MOEn, top.TUTO_DBG_VRAMp); // schematic wrong, second input is RACU
    /*p25.REFO*/ wire _REFO_MOEn_A = not1(_SEMA_MOEn_A);
    /*p25.SAHA*/ wire _SAHA_MOEn_D = not1(_RUTE_MOEn_D);

    top.vram_bus.PIN_VRAM_OEn.io_pin(_REFO_MOEn_A, _SAHA_MOEn_D);
  }

  {
    /*#p25.SUTU*/ wire _SUTU_MCSn = nor4(top.tile_fetcher.LENA_BGW_VRM_RDp,
                                         top.dma_reg.LUFA_DMA_VRAMp,
                                         top.sprite_fetcher.TEXY_SPR_READ_VRAMp,
                                         _SERE_CPU_VRM_RDp);

    /*p25.TODE*/ wire _TODE_MCSn_A = and2(_SUTU_MCSn, _RACO_DBG_VRAMn);
    /*p25.SEWO*/ wire _SEWO_MCSn_D = or2(_SUTU_MCSn, top.TUTO_DBG_VRAMp);
    /*p25.SOKY*/ wire _SOKY_MCSp_A = not1(_TODE_MCSn_A);
    /*p25.SETY*/ wire _SETY_MCSp_D = not1(_SEWO_MCSn_D);

    top.vram_bus.PIN_VRAM_CSn.io_pin(_SOKY_MCSp_A, _SETY_MCSp_D);
  }

  //----------------------------------------
  // VRAM address

  // CPU address -> vram address
  {
    /* p25.XEDU*/ wire _XEDU_CPU_VRAM_RDn = not1(_XANE_VRAM_LOCKn);
    /* p25.XAKY*/ top.vram_bus.BUS_VRAM_A00n.tri_6nn(_XEDU_CPU_VRAM_RDn, top.cpu_bus.BUS_CPU_A00.qp());
    /* p25.XUXU*/ top.vram_bus.BUS_VRAM_A01n.tri_6nn(_XEDU_CPU_VRAM_RDn, top.cpu_bus.BUS_CPU_A01.qp());
    /* p25.XYNE*/ top.vram_bus.BUS_VRAM_A02n.tri_6nn(_XEDU_CPU_VRAM_RDn, top.cpu_bus.BUS_CPU_A02.qp());
    /* p25.XODY*/ top.vram_bus.BUS_VRAM_A03n.tri_6nn(_XEDU_CPU_VRAM_RDn, top.cpu_bus.BUS_CPU_A03.qp());
    /* p25.XECA*/ top.vram_bus.BUS_VRAM_A04n.tri_6nn(_XEDU_CPU_VRAM_RDn, top.cpu_bus.BUS_CPU_A04.qp());
    /* p25.XOBA*/ top.vram_bus.BUS_VRAM_A05n.tri_6nn(_XEDU_CPU_VRAM_RDn, top.cpu_bus.BUS_CPU_A05.qp());
    /* p25.XOPO*/ top.vram_bus.BUS_VRAM_A06n.tri_6nn(_XEDU_CPU_VRAM_RDn, top.cpu_bus.BUS_CPU_A06.qp());
    /* p25.XYBO*/ top.vram_bus.BUS_VRAM_A07n.tri_6nn(_XEDU_CPU_VRAM_RDn, top.cpu_bus.BUS_CPU_A07.qp());
    /* p25.RYSU*/ top.vram_bus.BUS_VRAM_A08n.tri_6nn(_XEDU_CPU_VRAM_RDn, top.cpu_bus.BUS_CPU_A08.qp());
    /* p25.RESE*/ top.vram_bus.BUS_VRAM_A09n.tri_6nn(_XEDU_CPU_VRAM_RDn, top.cpu_bus.BUS_CPU_A09.qp());
    /* p25.RUSE*/ top.vram_bus.BUS_VRAM_A10n.tri_6nn(_XEDU_CPU_VRAM_RDn, top.cpu_bus.BUS_CPU_A10.qp());
    /* p25.RYNA*/ top.vram_bus.BUS_VRAM_A11n.tri_6nn(_XEDU_CPU_VRAM_RDn, top.cpu_bus.BUS_CPU_A11.qp());
    /* p25.RUMO*/ top.vram_bus.BUS_VRAM_A12n.tri_6nn(_XEDU_CPU_VRAM_RDn, top.cpu_bus.BUS_CPU_A12.qp());
  }

  // DMA address -> vram address
  {
    /* p04.AHOC*/ wire _AHOC_DMA_VRAM_RDn = not1(top.dma_reg.LUFA_DMA_VRAMp);
    /* p04.ECAL*/ top.vram_bus.BUS_VRAM_A00n.tri_6nn(_AHOC_DMA_VRAM_RDn, top.dma_reg.NAKY_DMA_A00p.qp17());
    /* p04.EGEZ*/ top.vram_bus.BUS_VRAM_A01n.tri_6nn(_AHOC_DMA_VRAM_RDn, top.dma_reg.PYRO_DMA_A01p.qp17());
    /* p04.FUHE*/ top.vram_bus.BUS_VRAM_A02n.tri_6nn(_AHOC_DMA_VRAM_RDn, top.dma_reg.NEFY_DMA_A02p.qp17());
    /* p04.FYZY*/ top.vram_bus.BUS_VRAM_A03n.tri_6nn(_AHOC_DMA_VRAM_RDn, top.dma_reg.MUTY_DMA_A03p.qp17());
    /* p04.DAMU*/ top.vram_bus.BUS_VRAM_A04n.tri_6nn(_AHOC_DMA_VRAM_RDn, top.dma_reg.NYKO_DMA_A04p.qp17());
    /* p04.DAVA*/ top.vram_bus.BUS_VRAM_A05n.tri_6nn(_AHOC_DMA_VRAM_RDn, top.dma_reg.PYLO_DMA_A05p.qp17());
    /* p04.ETEG*/ top.vram_bus.BUS_VRAM_A06n.tri_6nn(_AHOC_DMA_VRAM_RDn, top.dma_reg.NUTO_DMA_A06p.qp17());
    /*#p04.EREW*/ top.vram_bus.BUS_VRAM_A07n.tri_6nn(_AHOC_DMA_VRAM_RDn, top.dma_reg.MUGU_DMA_A07p.qp17());
    /*#p04.EVAX*/ top.vram_bus.BUS_VRAM_A08n.tri_6nn(_AHOC_DMA_VRAM_RDn, top.dma_reg.NAFA_DMA_A08n.qn07());
    /* p04.DUVE*/ top.vram_bus.BUS_VRAM_A09n.tri_6nn(_AHOC_DMA_VRAM_RDn, top.dma_reg.PYNE_DMA_A09n.qn07());
    /* p04.ERAF*/ top.vram_bus.BUS_VRAM_A10n.tri_6nn(_AHOC_DMA_VRAM_RDn, top.dma_reg.PARA_DMA_A10n.qn07());
    /* p04.FUSY*/ top.vram_bus.BUS_VRAM_A11n.tri_6nn(_AHOC_DMA_VRAM_RDn, top.dma_reg.NYDO_DMA_A11n.qn07());
    /* p04.EXYF*/ top.vram_bus.BUS_VRAM_A12n.tri_6nn(_AHOC_DMA_VRAM_RDn, top.dma_reg.NYGY_DMA_A12n.qn07());
  }

  // Sprite fetcher read
  {
    /*#p29.WUKY*/ wire _WUKY_FLIP_Yp = not1(top.oam_bus.YZOS_OAM_DB6p.qp08());
    /*#p29.FUFO*/ wire _FUFO_LCDC_SPSIZEn = not1(top.pix_pipe.XYMO_LCDC_SPSIZEn.qn08());

    /*#p29.CYVU*/ wire _CYVU_L0 = xor2(_WUKY_FLIP_Yp, top.sprite_store.SPR_TRI_L0.qp());
    /*#p29.BORE*/ wire _BORE_L1 = xor2(_WUKY_FLIP_Yp, top.sprite_store.SPR_TRI_L1.qp());
    /*#p29.BUVY*/ wire _BUVY_L2 = xor2(_WUKY_FLIP_Yp, top.sprite_store.SPR_TRI_L2.qp());
    /*#p29.WAGO*/ wire _WAGO_L3 = xor2(_WUKY_FLIP_Yp, top.sprite_store.SPR_TRI_L3.qp());
    /*#p29.GEJY*/ wire _GEJY_L3 = amux2(top.oam_bus.XUSO_OAM_DA0p.qp08(), _FUFO_LCDC_SPSIZEn,
                                        top.pix_pipe.XYMO_LCDC_SPSIZEn.qn08(), _WAGO_L3);

    /* p29.ABEM*/ top.vram_bus.BUS_VRAM_A00n.tri_6nn(top.sprite_fetcher.ABON_SPR_VRM_RDn, top.sprite_fetcher.XUQU_SPRITE_AB);
    /* p29.BAXE*/ top.vram_bus.BUS_VRAM_A01n.tri_6nn(top.sprite_fetcher.ABON_SPR_VRM_RDn, _CYVU_L0);
    /* p29.ARAS*/ top.vram_bus.BUS_VRAM_A02n.tri_6nn(top.sprite_fetcher.ABON_SPR_VRM_RDn, _BORE_L1);
    /* p29.AGAG*/ top.vram_bus.BUS_VRAM_A03n.tri_6nn(top.sprite_fetcher.ABON_SPR_VRM_RDn, _BUVY_L2);
    /* p29.FAMU*/ top.vram_bus.BUS_VRAM_A04n.tri_6nn(top.sprite_fetcher.ABON_SPR_VRM_RDn, _GEJY_L3);
    /*#p29.FUGY*/ top.vram_bus.BUS_VRAM_A05n.tri_6nn(top.sprite_fetcher.ABON_SPR_VRM_RDn, top.oam_bus.XEGU_OAM_DA1p.qp08());
    /* p29.GAVO*/ top.vram_bus.BUS_VRAM_A06n.tri_6nn(top.sprite_fetcher.ABON_SPR_VRM_RDn, top.oam_bus.YJEX_OAM_DA2p.qp08());
    /* p29.WYGA*/ top.vram_bus.BUS_VRAM_A07n.tri_6nn(top.sprite_fetcher.ABON_SPR_VRM_RDn, top.oam_bus.XYJU_OAM_DA3p.qp08());
    /* p29.WUNE*/ top.vram_bus.BUS_VRAM_A08n.tri_6nn(top.sprite_fetcher.ABON_SPR_VRM_RDn, top.oam_bus.YBOG_OAM_DA4p.qp08());
    /* p29.GOTU*/ top.vram_bus.BUS_VRAM_A09n.tri_6nn(top.sprite_fetcher.ABON_SPR_VRM_RDn, top.oam_bus.WYSO_OAM_DA5p.qp08());
    /* p29.GEGU*/ top.vram_bus.BUS_VRAM_A10n.tri_6nn(top.sprite_fetcher.ABON_SPR_VRM_RDn, top.oam_bus.XOTE_OAM_DA6p.qp08());
    /* p29.XEHE*/ top.vram_bus.BUS_VRAM_A11n.tri_6nn(top.sprite_fetcher.ABON_SPR_VRM_RDn, top.oam_bus.YZAB_OAM_DA7p.qp08());
    /* p29.DYSO*/ top.vram_bus.BUS_VRAM_A12n.tri_6nn(top.sprite_fetcher.ABON_SPR_VRM_RDn, 0);   // sprites always in low half of tile store
  }

  {
    // Map scroll adder
#pragma warning(push)
#pragma warning(disable:4189)
    /*#p26.FAFO*/ wire _FAFO_TILE_Y0S = add_s(top.lcd_reg.MUWY_Y0p.qp17(), top.pix_pipe.GAVE_SCY0n.qn08(), 0);
    /*#p26.FAFO*/ wire _FAFO_TILE_Y0C = add_c(top.lcd_reg.MUWY_Y0p.qp17(), top.pix_pipe.GAVE_SCY0n.qn08(), 0);
    /* p26.EMUX*/ wire _EMUX_TILE_Y1S = add_s(top.lcd_reg.MYRO_Y1p.qp17(), top.pix_pipe.FYMO_SCY1n.qn08(), _FAFO_TILE_Y0C);
    /* p26.EMUX*/ wire _EMUX_TILE_Y1C = add_c(top.lcd_reg.MYRO_Y1p.qp17(), top.pix_pipe.FYMO_SCY1n.qn08(), _FAFO_TILE_Y0C);
    /* p26.ECAB*/ wire _ECAB_TILE_Y2S = add_s(top.lcd_reg.LEXA_Y2p.qp17(), top.pix_pipe.FEZU_SCY2n.qn08(), _EMUX_TILE_Y1C);
    /* p26.ECAB*/ wire _ECAB_TILE_Y2C = add_c(top.lcd_reg.LEXA_Y2p.qp17(), top.pix_pipe.FEZU_SCY2n.qn08(), _EMUX_TILE_Y1C);
    /* p26.ETAM*/ wire _ETAM_MAP_Y0S  = add_s(top.lcd_reg.LYDO_Y3p.qp17(), top.pix_pipe.FUJO_SCY3n.qn08(), _ECAB_TILE_Y2C);
    /* p26.ETAM*/ wire _ETAM_MAP_Y0C  = add_c(top.lcd_reg.LYDO_Y3p.qp17(), top.pix_pipe.FUJO_SCY3n.qn08(), _ECAB_TILE_Y2C);
    /* p26.DOTO*/ wire _DOTO_MAP_Y1S  = add_s(top.lcd_reg.LOVU_Y4p.qp17(), top.pix_pipe.DEDE_SCY4n.qn08(), _ETAM_MAP_Y0C);
    /* p26.DOTO*/ wire _DOTO_MAP_Y1C  = add_c(top.lcd_reg.LOVU_Y4p.qp17(), top.pix_pipe.DEDE_SCY4n.qn08(), _ETAM_MAP_Y0C);
    /* p26.DABA*/ wire _DABA_MAP_Y2S  = add_s(top.lcd_reg.LEMA_Y5p.qp17(), top.pix_pipe.FOTY_SCY5n.qn08(), _DOTO_MAP_Y1C);
    /* p26.DABA*/ wire _DABA_MAP_Y2C  = add_c(top.lcd_reg.LEMA_Y5p.qp17(), top.pix_pipe.FOTY_SCY5n.qn08(), _DOTO_MAP_Y1C);
    /* p26.EFYK*/ wire _EFYK_MAP_Y3S  = add_s(top.lcd_reg.MATO_Y6p.qp17(), top.pix_pipe.FOHA_SCY6n.qn08(), _DABA_MAP_Y2C);
    /* p26.EFYK*/ wire _EFYK_MAP_Y3C  = add_c(top.lcd_reg.MATO_Y6p.qp17(), top.pix_pipe.FOHA_SCY6n.qn08(), _DABA_MAP_Y2C);
    /* p26.EJOK*/ wire _EJOK_MAP_Y4S  = add_s(top.lcd_reg.LAFO_Y7p.qp17(), top.pix_pipe.FUNY_SCY7n.qn08(), _EFYK_MAP_Y3C);
    /* p26.EJOK*/ wire _EJOK_MAP_Y4C  = add_c(top.lcd_reg.LAFO_Y7p.qp17(), top.pix_pipe.FUNY_SCY7n.qn08(), _EFYK_MAP_Y3C);

    /*#p26.ATAD*/ wire _ATAD_TILE_X0S = add_s(top.pix_pipe.XEHO_X0p.qp17(), top.pix_pipe.DATY_SCX0n.qn08(), 0);
    /*#p26.ATAD*/ wire _ATAD_TILE_X0C = add_c(top.pix_pipe.XEHO_X0p.qp17(), top.pix_pipe.DATY_SCX0n.qn08(), 0);
    /* p26.BEHU*/ wire _BEHU_TILE_X1S = add_s(top.pix_pipe.SAVY_X1p.qp17(), top.pix_pipe.DUZU_SCX1n.qn08(), _ATAD_TILE_X0C);
    /* p26.BEHU*/ wire _BEHU_TILE_X1C = add_c(top.pix_pipe.SAVY_X1p.qp17(), top.pix_pipe.DUZU_SCX1n.qn08(), _ATAD_TILE_X0C);
    /* p26.APYH*/ wire _APYH_TILE_X2S = add_s(top.pix_pipe.XODU_X2p.qp17(), top.pix_pipe.CYXU_SCX2n.qn08(), _BEHU_TILE_X1C);
    /* p26.APYH*/ wire _APYH_TILE_X2C = add_c(top.pix_pipe.XODU_X2p.qp17(), top.pix_pipe.CYXU_SCX2n.qn08(), _BEHU_TILE_X1C);
    /* p26.BABE*/ wire _BABE_MAP_X0S  = add_s(top.pix_pipe.XYDO_X3p.qp17(), top.pix_pipe.GUBO_SCX3n.qn08(), _APYH_TILE_X2C);
    /* p26.BABE*/ wire _BABE_MAP_X0C  = add_c(top.pix_pipe.XYDO_X3p.qp17(), top.pix_pipe.GUBO_SCX3n.qn08(), _APYH_TILE_X2C);
    /* p26.ABOD*/ wire _ABOD_MAP_X1S  = add_s(top.pix_pipe.TUHU_X4p.qp17(), top.pix_pipe.BEMY_SCX4n.qn08(), _BABE_MAP_X0C);
    /* p26.ABOD*/ wire _ABOD_MAP_X1C  = add_c(top.pix_pipe.TUHU_X4p.qp17(), top.pix_pipe.BEMY_SCX4n.qn08(), _BABE_MAP_X0C);
    /* p26.BEWY*/ wire _BEWY_MAP_X2S  = add_s(top.pix_pipe.TUKY_X5p.qp17(), top.pix_pipe.CUZY_SCX5n.qn08(), _ABOD_MAP_X1C);
    /* p26.BEWY*/ wire _BEWY_MAP_X2C  = add_c(top.pix_pipe.TUKY_X5p.qp17(), top.pix_pipe.CUZY_SCX5n.qn08(), _ABOD_MAP_X1C);
    /* p26.BYCA*/ wire _BYCA_MAP_X3S  = add_s(top.pix_pipe.TAKO_X6p.qp17(), top.pix_pipe.CABU_SCX6n.qn08(), _BEWY_MAP_X2C);
    /* p26.BYCA*/ wire _BYCA_MAP_X3C  = add_c(top.pix_pipe.TAKO_X6p.qp17(), top.pix_pipe.CABU_SCX6n.qn08(), _BEWY_MAP_X2C);
    /* p26.ACUL*/ wire _ACUL_MAP_X4S  = add_s(top.pix_pipe.SYBE_X7p.qp17(), top.pix_pipe.BAKE_SCX7n.qn08(), _BYCA_MAP_X3C);
    /* p26.ACUL*/ wire _ACUL_MAP_X4C  = add_c(top.pix_pipe.SYBE_X7p.qp17(), top.pix_pipe.BAKE_SCX7n.qn08(), _BYCA_MAP_X3C);
#pragma warning(pop)

    // Background map read
    {
      /* p26.AXEP*/ top.vram_bus.BUS_VRAM_A00n.tri_6nn(top.tile_fetcher.BAFY_BG_MAP_READn, _BABE_MAP_X0S);
      /* p26.AFEB*/ top.vram_bus.BUS_VRAM_A01n.tri_6nn(top.tile_fetcher.BAFY_BG_MAP_READn, _ABOD_MAP_X1S);
      /* p26.ALEL*/ top.vram_bus.BUS_VRAM_A02n.tri_6nn(top.tile_fetcher.BAFY_BG_MAP_READn, _BEWY_MAP_X2S);
      /* p26.COLY*/ top.vram_bus.BUS_VRAM_A03n.tri_6nn(top.tile_fetcher.BAFY_BG_MAP_READn, _BYCA_MAP_X3S);
      /* p26.AJAN*/ top.vram_bus.BUS_VRAM_A04n.tri_6nn(top.tile_fetcher.BAFY_BG_MAP_READn, _ACUL_MAP_X4S);
      /* p26.DUHO*/ top.vram_bus.BUS_VRAM_A05n.tri_6nn(top.tile_fetcher.BAFY_BG_MAP_READn, _ETAM_MAP_Y0S);
      /* p26.CASE*/ top.vram_bus.BUS_VRAM_A06n.tri_6nn(top.tile_fetcher.BAFY_BG_MAP_READn, _DOTO_MAP_Y1S);
      /* p26.CYPO*/ top.vram_bus.BUS_VRAM_A07n.tri_6nn(top.tile_fetcher.BAFY_BG_MAP_READn, _DABA_MAP_Y2S);
      /* p26.CETA*/ top.vram_bus.BUS_VRAM_A08n.tri_6nn(top.tile_fetcher.BAFY_BG_MAP_READn, _EFYK_MAP_Y3S);
      /* p26.DAFE*/ top.vram_bus.BUS_VRAM_A09n.tri_6nn(top.tile_fetcher.BAFY_BG_MAP_READn, _EJOK_MAP_Y4S);
      /*#p26.AMUV*/ top.vram_bus.BUS_VRAM_A10n.tri_6nn(top.tile_fetcher.BAFY_BG_MAP_READn, top.pix_pipe.XAFO_LCDC_BGMAPn.qn08());
      /* p26.COVE*/ top.vram_bus.BUS_VRAM_A11n.tri_6nn(top.tile_fetcher.BAFY_BG_MAP_READn, 1);
      /* p26.COXO*/ top.vram_bus.BUS_VRAM_A12n.tri_6nn(top.tile_fetcher.BAFY_BG_MAP_READn, 1);
    }

    // Window map read
    {
      /*#p27.XEJA*/ top.vram_bus.BUS_VRAM_A00n.tri_6nn(top.tile_fetcher.WUKO_WIN_MAP_READn, top.pix_pipe.WYKA_WIN_X3.qp17());
      /* p27.XAMO*/ top.vram_bus.BUS_VRAM_A01n.tri_6nn(top.tile_fetcher.WUKO_WIN_MAP_READn, top.pix_pipe.WODY_WIN_X4.qp17());
      /* p27.XAHE*/ top.vram_bus.BUS_VRAM_A02n.tri_6nn(top.tile_fetcher.WUKO_WIN_MAP_READn, top.pix_pipe.WOBO_WIN_X5.qp17());
      /* p27.XULO*/ top.vram_bus.BUS_VRAM_A03n.tri_6nn(top.tile_fetcher.WUKO_WIN_MAP_READn, top.pix_pipe.WYKO_WIN_X6.qp17());
      /* p27.WUJU*/ top.vram_bus.BUS_VRAM_A04n.tri_6nn(top.tile_fetcher.WUKO_WIN_MAP_READn, top.pix_pipe.XOLO_WIN_X7.qp17());
      /*#p27.VYTO*/ top.vram_bus.BUS_VRAM_A05n.tri_6nn(top.tile_fetcher.WUKO_WIN_MAP_READn, top.pix_pipe.TUFU_WIN_Y3.qp17());
      /* p27.VEHA*/ top.vram_bus.BUS_VRAM_A06n.tri_6nn(top.tile_fetcher.WUKO_WIN_MAP_READn, top.pix_pipe.TAXA_WIN_Y4.qp17());
      /* p27.VACE*/ top.vram_bus.BUS_VRAM_A07n.tri_6nn(top.tile_fetcher.WUKO_WIN_MAP_READn, top.pix_pipe.TOZO_WIN_Y5.qp17());
      /* p27.VOVO*/ top.vram_bus.BUS_VRAM_A08n.tri_6nn(top.tile_fetcher.WUKO_WIN_MAP_READn, top.pix_pipe.TATE_WIN_Y6.qp17());
      /* p27.VULO*/ top.vram_bus.BUS_VRAM_A09n.tri_6nn(top.tile_fetcher.WUKO_WIN_MAP_READn, top.pix_pipe.TEKE_WIN_Y7.qp17());
      /*#p27.VEVY*/ top.vram_bus.BUS_VRAM_A10n.tri_6nn(top.tile_fetcher.WUKO_WIN_MAP_READn, top.pix_pipe.WOKY_LCDC_WINMAPn.qn08());
      /* p27.VEZA*/ top.vram_bus.BUS_VRAM_A11n.tri_6nn(top.tile_fetcher.WUKO_WIN_MAP_READn, 1);
      /* p27.VOGU*/ top.vram_bus.BUS_VRAM_A12n.tri_6nn(top.tile_fetcher.WUKO_WIN_MAP_READn, 1);
    }

    // Background/window tile read
    {
      /*#p26.ASUM*/ top.vram_bus.BUS_VRAM_A00n.tri_6nn(top.tile_fetcher.BEJE_BG_TILE_READn,  top.tile_fetcher.XUHA_FETCH_S2p);
      /* p26.EVAD*/ top.vram_bus.BUS_VRAM_A01n.tri_6nn(top.tile_fetcher.BEJE_BG_TILE_READn,  _FAFO_TILE_Y0S);
      /* p26.DAHU*/ top.vram_bus.BUS_VRAM_A02n.tri_6nn(top.tile_fetcher.BEJE_BG_TILE_READn,  _EMUX_TILE_Y1S);
      /* p26.DODE*/ top.vram_bus.BUS_VRAM_A03n.tri_6nn(top.tile_fetcher.BEJE_BG_TILE_READn,  _ECAB_TILE_Y2S); // check outputs of ECAB

      /*#p25.XONU*/ top.vram_bus.BUS_VRAM_A00n.tri_6nn(top.tile_fetcher.XUCY_WIN_TILE_READn, top.tile_fetcher.XUHA_FETCH_S2p);
      /*#p25.WUDO*/ top.vram_bus.BUS_VRAM_A01n.tri_6nn(top.tile_fetcher.XUCY_WIN_TILE_READn, top.pix_pipe.VYNO_WIN_Y0.qp17());
      /*#p25.WAWE*/ top.vram_bus.BUS_VRAM_A02n.tri_6nn(top.tile_fetcher.XUCY_WIN_TILE_READn, top.pix_pipe.VUJO_WIN_Y1.qp17());
      /*#p25.WOLU*/ top.vram_bus.BUS_VRAM_A03n.tri_6nn(top.tile_fetcher.XUCY_WIN_TILE_READn, top.pix_pipe.VYMU_WIN_Y2.qp17());

      /*#p25.VUZA*/ wire _VUZA_TILE_BANKp = nor2(top.vram_bus.PYJU_TILE_DB7p.q11p(), top.pix_pipe.WEXU_LCDC_BGTILEn.qn08());

      /*#p25.VAPY*/ top.vram_bus.BUS_VRAM_A04n.tri_6pn(top.tile_fetcher.NETA_TILE_READp,     top.vram_bus.RAWU_TILE_DB0p.q11p());
      /*#p25.SEZU*/ top.vram_bus.BUS_VRAM_A05n.tri_6pn(top.tile_fetcher.NETA_TILE_READp,     top.vram_bus.POZO_TILE_DB1p.q11p());
      /*#p25.VEJY*/ top.vram_bus.BUS_VRAM_A06n.tri_6pn(top.tile_fetcher.NETA_TILE_READp,     top.vram_bus.PYZO_TILE_DB2p.q11p());
      /*#p25.RUSA*/ top.vram_bus.BUS_VRAM_A07n.tri_6pn(top.tile_fetcher.NETA_TILE_READp,     top.vram_bus.POXA_TILE_DB3p.q11p());
      /*#p25.ROHA*/ top.vram_bus.BUS_VRAM_A08n.tri_6pn(top.tile_fetcher.NETA_TILE_READp,     top.vram_bus.PULO_TILE_DB4p.q11p());
      /*#p25.RESO*/ top.vram_bus.BUS_VRAM_A09n.tri_6pn(top.tile_fetcher.NETA_TILE_READp,     top.vram_bus.POJU_TILE_DB5p.q11p());
      /*#p25.SUVO*/ top.vram_bus.BUS_VRAM_A10n.tri_6pn(top.tile_fetcher.NETA_TILE_READp,     top.vram_bus.POWY_TILE_DB6p.q11p());
      /*#p25.TOBO*/ top.vram_bus.BUS_VRAM_A11n.tri_6pn(top.tile_fetcher.NETA_TILE_READp,     top.vram_bus.PYJU_TILE_DB7p.q11p());
      /*#p25.VURY*/ top.vram_bus.BUS_VRAM_A12n.tri_6pn(top.tile_fetcher.NETA_TILE_READp,     _VUZA_TILE_BANKp);
    }
  }

  // VRAM addr bus -> VRAM addr pin
  {
    /*p25.MYFU*/ wire _MYFUp = not1(top.vram_bus.BUS_VRAM_A00n.qp());
    /*p25.MASA*/ wire _MASAp = not1(top.vram_bus.BUS_VRAM_A01n.qp());
    /*p25.MYRE*/ wire _MYREp = not1(top.vram_bus.BUS_VRAM_A02n.qp());
    /*p25.MAVU*/ wire _MAVUp = not1(top.vram_bus.BUS_VRAM_A03n.qp());
    /*p25.MEPA*/ wire _MEPAp = not1(top.vram_bus.BUS_VRAM_A04n.qp());
    /*p25.MYSA*/ wire _MYSAp = not1(top.vram_bus.BUS_VRAM_A05n.qp());
    /*p25.MEWY*/ wire _MEWYp = not1(top.vram_bus.BUS_VRAM_A06n.qp());
    /*p25.MUME*/ wire _MUMEp = not1(top.vram_bus.BUS_VRAM_A07n.qp());
    /*p25.VOVA*/ wire _VOVAp = not1(top.vram_bus.BUS_VRAM_A08n.qp());
    /*p25.VODE*/ wire _VODEp = not1(top.vram_bus.BUS_VRAM_A09n.qp());
    /*p25.RUKY*/ wire _RUKYp = not1(top.vram_bus.BUS_VRAM_A10n.qp());
    /*p25.RUMA*/ wire _RUMAp = not1(top.vram_bus.BUS_VRAM_A11n.qp());
    /*p25.REHO*/ wire _REHOp = not1(top.vram_bus.BUS_VRAM_A12n.qp());

    /*p25.LEXE*/ wire _LEXEn = not1(_MYFUp);
    /*p25.LOZU*/ wire _LOZUn = not1(_MASAp);
    /*p25.LACA*/ wire _LACAn = not1(_MYREp);
    /*p25.LUVO*/ wire _LUVOn = not1(_MAVUp);
    /*p25.LOLY*/ wire _LOLYn = not1(_MEPAp);
    /*p25.LALO*/ wire _LALOn = not1(_MYSAp);
    /*p25.LEFA*/ wire _LEFAn = not1(_MEWYp);
    /*p25.LUBY*/ wire _LUBYn = not1(_MUMEp);
    /*p25.TUJY*/ wire _TUJYn = not1(_VOVAp);
    /*p25.TAGO*/ wire _TAGOn = not1(_VODEp);
    /*p25.NUVA*/ wire _NUVAn = not1(_RUKYp);
    /*p25.PEDU*/ wire _PEDUn = not1(_RUMAp);
    /*p25.PONY*/ wire _PONYn = not1(_REHOp);

    top.vram_bus.PIN_VRAM_A00p.io_pin(_LEXEn, _LEXEn);
    top.vram_bus.PIN_VRAM_A01p.io_pin(_LOZUn, _LOZUn);
    top.vram_bus.PIN_VRAM_A02p.io_pin(_LACAn, _LACAn);
    top.vram_bus.PIN_VRAM_A03p.io_pin(_LUVOn, _LUVOn);
    top.vram_bus.PIN_VRAM_A04p.io_pin(_LOLYn, _LOLYn);
    top.vram_bus.PIN_VRAM_A05p.io_pin(_LALOn, _LALOn);
    top.vram_bus.PIN_VRAM_A06p.io_pin(_LEFAn, _LEFAn);
    top.vram_bus.PIN_VRAM_A07p.io_pin(_LUBYn, _LUBYn);
    top.vram_bus.PIN_VRAM_A08p.io_pin(_TUJYn, _TUJYn);
    top.vram_bus.PIN_VRAM_A09p.io_pin(_TAGOn, _TAGOn);
    top.vram_bus.PIN_VRAM_A10p.io_pin(_NUVAn, _NUVAn);
    top.vram_bus.PIN_VRAM_A11p.io_pin(_PEDUn, _PEDUn);
    top.vram_bus.PIN_VRAM_A12p.io_pin(_PONYn, _PONYn);
  }

  //----------------------------------------
  // VRAM data out

  // CBD->VBD->VPD
  {
    /*p25.ROCY*/ wire _ROCY_CBD_TO_VPDp = and2(_REVO_CBD_TO_VPDp, _SAZO_CBD_TO_VPDp);
    /*p25.RAHU*/ wire _RAHU_CBD_TO_VPDn = not1(_ROCY_CBD_TO_VPDp);

    /* p25.TEME*/ top.vram_bus.BUS_VRAM_D0p.tri10_np(_RAHU_CBD_TO_VPDn, top.cpu_bus.BUS_CPU_D0p.qp());
    /* p25.TEWU*/ top.vram_bus.BUS_VRAM_D1p.tri10_np(_RAHU_CBD_TO_VPDn, top.cpu_bus.BUS_CPU_D1p.qp());
    /*#p25.TYGO*/ top.vram_bus.BUS_VRAM_D2p.tri10_np(_RAHU_CBD_TO_VPDn, top.cpu_bus.BUS_CPU_D2p.qp());
    /* p25.SOTE*/ top.vram_bus.BUS_VRAM_D3p.tri10_np(_RAHU_CBD_TO_VPDn, top.cpu_bus.BUS_CPU_D3p.qp());
    /* p25.SEKE*/ top.vram_bus.BUS_VRAM_D4p.tri10_np(_RAHU_CBD_TO_VPDn, top.cpu_bus.BUS_CPU_D4p.qp());
    /* p25.RUJO*/ top.vram_bus.BUS_VRAM_D5p.tri10_np(_RAHU_CBD_TO_VPDn, top.cpu_bus.BUS_CPU_D5p.qp());
    /* p25.TOFA*/ top.vram_bus.BUS_VRAM_D6p.tri10_np(_RAHU_CBD_TO_VPDn, top.cpu_bus.BUS_CPU_D6p.qp());
    /* p25.SUZA*/ top.vram_bus.BUS_VRAM_D7p.tri10_np(_RAHU_CBD_TO_VPDn, top.cpu_bus.BUS_CPU_D7p.qp());

    /*#p25.ROVE*/ wire _ROVE_CBD_TO_VPDp = not1(_RAHU_CBD_TO_VPDn);
    /*#p25.SEFA*/ wire _SEFA_D0p = and2(top.vram_bus.BUS_VRAM_D0p.qp(), _ROVE_CBD_TO_VPDp);
    /* p25.SOGO*/ wire _SOGO_D1p = and2(top.vram_bus.BUS_VRAM_D1p.qp(), _ROVE_CBD_TO_VPDp);
    /* p25.SEFU*/ wire _SEFU_D2p = and2(top.vram_bus.BUS_VRAM_D2p.qp(), _ROVE_CBD_TO_VPDp);
    /* p25.SUNA*/ wire _SUNA_D3p = and2(top.vram_bus.BUS_VRAM_D3p.qp(), _ROVE_CBD_TO_VPDp);
    /* p25.SUMO*/ wire _SUMO_D4p = and2(top.vram_bus.BUS_VRAM_D4p.qp(), _ROVE_CBD_TO_VPDp);
    /* p25.SAZU*/ wire _SAZU_D5p = and2(top.vram_bus.BUS_VRAM_D5p.qp(), _ROVE_CBD_TO_VPDp);
    /* p25.SAMO*/ wire _SAMO_D6p = and2(top.vram_bus.BUS_VRAM_D6p.qp(), _ROVE_CBD_TO_VPDp);
    /* p25.SUKE*/ wire _SUKE_D7p = and2(top.vram_bus.BUS_VRAM_D7p.qp(), _ROVE_CBD_TO_VPDp);

    /*#p25.REGE*/ wire _REGE_D0n = not1(_SEFA_D0p);
    /* p25.RYKY*/ wire _RYKY_D1n = not1(_SOGO_D1p);
    /* p25.RAZO*/ wire _RAZO_D2n = not1(_SEFU_D2p);
    /* p25.RADA*/ wire _RADA_D3n = not1(_SUNA_D3p);
    /* p25.RYRO*/ wire _RYRO_D4n = not1(_SUMO_D4p);
    /* p25.REVU*/ wire _REVU_D5n = not1(_SAZU_D5p);
    /* p25.REKU*/ wire _REKU_D6n = not1(_SAMO_D6p);
    /* p25.RYZE*/ wire _RYZE_D7n = not1(_SUKE_D7p);

    /*#p25.SYNU*/ wire _SYNU_D0p = or2(_RAHU_CBD_TO_VPDn, top.vram_bus.BUS_VRAM_D0p.qp());
    /* p25.SYMA*/ wire _SYMA_D1p = or2(_RAHU_CBD_TO_VPDn, top.vram_bus.BUS_VRAM_D1p.qp());
    /* p25.ROKO*/ wire _ROKO_D2p = or2(_RAHU_CBD_TO_VPDn, top.vram_bus.BUS_VRAM_D2p.qp());
    /* p25.SYBU*/ wire _SYBU_D3p = or2(_RAHU_CBD_TO_VPDn, top.vram_bus.BUS_VRAM_D3p.qp());
    /* p25.SAKO*/ wire _SAKO_D4p = or2(_RAHU_CBD_TO_VPDn, top.vram_bus.BUS_VRAM_D4p.qp());
    /* p25.SEJY*/ wire _SEJY_D5p = or2(_RAHU_CBD_TO_VPDn, top.vram_bus.BUS_VRAM_D5p.qp());
    /* p25.SEDO*/ wire _SEDO_D6p = or2(_RAHU_CBD_TO_VPDn, top.vram_bus.BUS_VRAM_D6p.qp());
    /* p25.SAWU*/ wire _SAWU_D7p = or2(_RAHU_CBD_TO_VPDn, top.vram_bus.BUS_VRAM_D7p.qp());

    /*#p25.RURA*/ wire _RURA_D0n = not1(_SYNU_D0p);
    /* p25.RULY*/ wire _RULY_D1n = not1(_SYMA_D1p);
    /* p25.RARE*/ wire _RARE_D2n = not1(_ROKO_D2p);
    /* p25.RODU*/ wire _RODU_D3n = not1(_SYBU_D3p);
    /* p25.RUBE*/ wire _RUBE_D4n = not1(_SAKO_D4p);
    /* p25.RUMU*/ wire _RUMU_D5n = not1(_SEJY_D5p);
    /* p25.RYTY*/ wire _RYTY_D6n = not1(_SEDO_D6p);
    /* p25.RADY*/ wire _RADY_D7n = not1(_SAWU_D7p);

    /*#p25.ROFA*/ wire _ROFA_CBD_TO_VPDp = not1(_RENA_CBD_TO_VPDn);

    /*#*/ top.vram_bus.PIN_VRAM_D00p.io_pin(_REGE_D0n, _RURA_D0n, _ROFA_CBD_TO_VPDp);
    /* */ top.vram_bus.PIN_VRAM_D01p.io_pin(_RYKY_D1n, _RULY_D1n, _ROFA_CBD_TO_VPDp);
    /* */ top.vram_bus.PIN_VRAM_D02p.io_pin(_RAZO_D2n, _RARE_D2n, _ROFA_CBD_TO_VPDp);
    /* */ top.vram_bus.PIN_VRAM_D03p.io_pin(_RADA_D3n, _RODU_D3n, _ROFA_CBD_TO_VPDp);
    /* */ top.vram_bus.PIN_VRAM_D04p.io_pin(_RYRO_D4n, _RUBE_D4n, _ROFA_CBD_TO_VPDp);
    /* */ top.vram_bus.PIN_VRAM_D05p.io_pin(_REVU_D5n, _RUMU_D5n, _ROFA_CBD_TO_VPDp);
    /* */ top.vram_bus.PIN_VRAM_D06p.io_pin(_REKU_D6n, _RYTY_D6n, _ROFA_CBD_TO_VPDp);
    /* */ top.vram_bus.PIN_VRAM_D07p.io_pin(_RYZE_D7n, _RADY_D7n, _ROFA_CBD_TO_VPDp);
  }

  //----------------------------------------
  // VRAM data in

  // VPD -> VBD
  {
    /*p25.RODY*/ top.vram_bus.BUS_VRAM_D0p.tri_6pn(_RENA_CBD_TO_VPDn, top.vram_bus.PIN_VRAM_D00p.qn());
    /*p25.REBA*/ top.vram_bus.BUS_VRAM_D1p.tri_6pn(_RENA_CBD_TO_VPDn, top.vram_bus.PIN_VRAM_D01p.qn());
    /*p25.RYDO*/ top.vram_bus.BUS_VRAM_D2p.tri_6pn(_RENA_CBD_TO_VPDn, top.vram_bus.PIN_VRAM_D02p.qn());
    /*p25.REMO*/ top.vram_bus.BUS_VRAM_D3p.tri_6pn(_RENA_CBD_TO_VPDn, top.vram_bus.PIN_VRAM_D03p.qn());
    /*p25.ROCE*/ top.vram_bus.BUS_VRAM_D4p.tri_6pn(_RENA_CBD_TO_VPDn, top.vram_bus.PIN_VRAM_D04p.qn());
    /*p25.ROPU*/ top.vram_bus.BUS_VRAM_D5p.tri_6pn(_RENA_CBD_TO_VPDn, top.vram_bus.PIN_VRAM_D05p.qn());
    /*p25.RETA*/ top.vram_bus.BUS_VRAM_D6p.tri_6pn(_RENA_CBD_TO_VPDn, top.vram_bus.PIN_VRAM_D06p.qn());
    /*p25.RAKU*/ top.vram_bus.BUS_VRAM_D7p.tri_6pn(_RENA_CBD_TO_VPDn, top.vram_bus.PIN_VRAM_D07p.qn());
  }

  // VBD -> CBD
  {
    /* p25.TYVY*/ wire _TYVY_VBD_TO_CBDn = nand2(_SERE_CPU_VRM_RDp, top.LEKO_CPU_RDp);
    /* p25.SEBY*/ wire _SEBY_VBD_TO_CBDp = not1(_TYVY_VBD_TO_CBDn);

    /* p25.RERY*/ wire _RERY_VBUS_D0n = not1(top.vram_bus.BUS_VRAM_D0p.qp());
    /* p25.RUNA*/ wire _RUNA_VBUS_D1n = not1(top.vram_bus.BUS_VRAM_D1p.qp());
    /* p25.RONA*/ wire _RONA_VBUS_D2n = not1(top.vram_bus.BUS_VRAM_D2p.qp());
    /* p25.RUNO*/ wire _RUNO_VBUS_D3n = not1(top.vram_bus.BUS_VRAM_D3p.qp());
    /* p25.SANA*/ wire _SANA_VBUS_D4n = not1(top.vram_bus.BUS_VRAM_D4p.qp());
    /* p25.RORO*/ wire _RORO_VBUS_D5n = not1(top.vram_bus.BUS_VRAM_D5p.qp());
    /* p25.RABO*/ wire _RABO_VBUS_D6n = not1(top.vram_bus.BUS_VRAM_D6p.qp());
    /* p25.SAME*/ wire _SAME_VBUS_D7n = not1(top.vram_bus.BUS_VRAM_D7p.qp());

    /*#p25.RUGA*/ top.cpu_bus.BUS_CPU_D0p.tri_6pn(_SEBY_VBD_TO_CBDp, _RERY_VBUS_D0n);
    /* p25.ROTA*/ top.cpu_bus.BUS_CPU_D1p.tri_6pn(_SEBY_VBD_TO_CBDp, _RUNA_VBUS_D1n);
    /* p25.RYBU*/ top.cpu_bus.BUS_CPU_D2p.tri_6pn(_SEBY_VBD_TO_CBDp, _RONA_VBUS_D2n);
    /* p25.RAJU*/ top.cpu_bus.BUS_CPU_D3p.tri_6pn(_SEBY_VBD_TO_CBDp, _RUNO_VBUS_D3n);
    /* p25.TYJA*/ top.cpu_bus.BUS_CPU_D4p.tri_6pn(_SEBY_VBD_TO_CBDp, _SANA_VBUS_D4n);
    /* p25.REXU*/ top.cpu_bus.BUS_CPU_D5p.tri_6pn(_SEBY_VBD_TO_CBDp, _RORO_VBUS_D5n);
    /* p25.RUPY*/ top.cpu_bus.BUS_CPU_D6p.tri_6pn(_SEBY_VBD_TO_CBDp, _RABO_VBUS_D6n);
    /* p25.TOKU*/ top.cpu_bus.BUS_CPU_D7p.tri_6pn(_SEBY_VBD_TO_CBDp, _SAME_VBUS_D7n);
  }

  // VBD -> tile pix temp
  {
    /*#p32.LOMA*/ wire _LOMA_LATCH_TILE_DAn = not1(top.tile_fetcher.METE_LATCH_TILE_DAp);

    /*p32.LEGU*/ top.vram_bus.LEGU_TILE_DA0n.dff8p(_LOMA_LATCH_TILE_DAn, top.vram_bus.BUS_VRAM_D0p.qp());
    /*p32.NUDU*/ top.vram_bus.NUDU_TILE_DA1n.dff8p(_LOMA_LATCH_TILE_DAn, top.vram_bus.BUS_VRAM_D1p.qp());
    /*p32.MUKU*/ top.vram_bus.MUKU_TILE_DA2n.dff8p(_LOMA_LATCH_TILE_DAn, top.vram_bus.BUS_VRAM_D2p.qp());
    /*p32.LUZO*/ top.vram_bus.LUZO_TILE_DA3n.dff8p(_LOMA_LATCH_TILE_DAn, top.vram_bus.BUS_VRAM_D3p.qp());
    /*p32.MEGU*/ top.vram_bus.MEGU_TILE_DA4n.dff8p(_LOMA_LATCH_TILE_DAn, top.vram_bus.BUS_VRAM_D4p.qp());
    /*p32.MYJY*/ top.vram_bus.MYJY_TILE_DA5n.dff8p(_LOMA_LATCH_TILE_DAn, top.vram_bus.BUS_VRAM_D5p.qp());
    /*p32.NASA*/ top.vram_bus.NASA_TILE_DA6n.dff8p(_LOMA_LATCH_TILE_DAn, top.vram_bus.BUS_VRAM_D6p.qp());
    /*p32.NEFO*/ top.vram_bus.NEFO_TILE_DA7n.dff8p(_LOMA_LATCH_TILE_DAn, top.vram_bus.BUS_VRAM_D7p.qp());

    // This is the only block of "dff11" on the chip. Not sure about clock polarity, it seems to work either way.
    /*p??.LUVE*/ wire _LUVE_MATCH_TILE_DBp = not1(top.tile_fetcher.LESO_LATCH_TILE_DBn); // Schematic wrong, was labeled AJAR
    /*p32.LABU*/ wire _LABU_LATCH_TILE_DBn = not1(_LUVE_MATCH_TILE_DBp);

    /*p32.RAWU*/ top.vram_bus.RAWU_TILE_DB0p.dff11(_LABU_LATCH_TILE_DBn, VYPO, top.vram_bus.BUS_VRAM_D0p.qp());
    /*p32.POZO*/ top.vram_bus.POZO_TILE_DB1p.dff11(_LABU_LATCH_TILE_DBn, VYPO, top.vram_bus.BUS_VRAM_D1p.qp());
    /*p32.PYZO*/ top.vram_bus.PYZO_TILE_DB2p.dff11(_LABU_LATCH_TILE_DBn, VYPO, top.vram_bus.BUS_VRAM_D2p.qp());
    /*p32.POXA*/ top.vram_bus.POXA_TILE_DB3p.dff11(_LABU_LATCH_TILE_DBn, VYPO, top.vram_bus.BUS_VRAM_D3p.qp());
    /*p32.PULO*/ top.vram_bus.PULO_TILE_DB4p.dff11(_LABU_LATCH_TILE_DBn, VYPO, top.vram_bus.BUS_VRAM_D4p.qp());
    /*p32.POJU*/ top.vram_bus.POJU_TILE_DB5p.dff11(_LABU_LATCH_TILE_DBn, VYPO, top.vram_bus.BUS_VRAM_D5p.qp());
    /*p32.POWY*/ top.vram_bus.POWY_TILE_DB6p.dff11(_LABU_LATCH_TILE_DBn, VYPO, top.vram_bus.BUS_VRAM_D6p.qp());
    /*p32.PYJU*/ top.vram_bus.PYJU_TILE_DB7p.dff11(_LABU_LATCH_TILE_DBn, VYPO, top.vram_bus.BUS_VRAM_D7p.qp());
  }

  // VRAM tri -> sprite pix temp + x flip
  {
    /*#p29.XONO*/ wire _XONO_FLIP_X = and2(top.oam_bus.BAXO_OAM_DB5p.qp08(), top.sprite_fetcher.TEXY_SPR_READ_VRAMp);

    /*p33.PUTE*/ wire _PUTE_FLIP0p = mux2p(_XONO_FLIP_X, top.vram_bus.BUS_VRAM_D7p.qp(), top.vram_bus.BUS_VRAM_D0p.qp());
    /*p33.PELO*/ wire _PELO_FLIP1p = mux2p(_XONO_FLIP_X, top.vram_bus.BUS_VRAM_D6p.qp(), top.vram_bus.BUS_VRAM_D1p.qp());
    /*p33.PONO*/ wire _PONO_FLIP2p = mux2p(_XONO_FLIP_X, top.vram_bus.BUS_VRAM_D5p.qp(), top.vram_bus.BUS_VRAM_D2p.qp());
    /*p33.POBE*/ wire _POBE_FLIP3p = mux2p(_XONO_FLIP_X, top.vram_bus.BUS_VRAM_D4p.qp(), top.vram_bus.BUS_VRAM_D3p.qp());
    /*p33.PACY*/ wire _PACY_FLIP4p = mux2p(_XONO_FLIP_X, top.vram_bus.BUS_VRAM_D3p.qp(), top.vram_bus.BUS_VRAM_D4p.qp());
    /*p33.PUGU*/ wire _PUGU_FLIP5p = mux2p(_XONO_FLIP_X, top.vram_bus.BUS_VRAM_D2p.qp(), top.vram_bus.BUS_VRAM_D5p.qp());
    /*p33.PAWE*/ wire _PAWE_FLIP6p = mux2p(_XONO_FLIP_X, top.vram_bus.BUS_VRAM_D1p.qp(), top.vram_bus.BUS_VRAM_D6p.qp());
    /*p33.PULY*/ wire _PULY_FLIP7p = mux2p(_XONO_FLIP_X, top.vram_bus.BUS_VRAM_D0p.qp(), top.vram_bus.BUS_VRAM_D7p.qp());

    /*#p29.PEBY*/ wire _PEBY_CLKp = not1(top.sprite_fetcher.RACA_LATCH_SPPIXB);
    /*#p29.NYBE*/ wire _NYBE_CLKn = not1(_PEBY_CLKp);
    /*#p29.PUCO*/ wire _PUCO_CLKp = not1(_NYBE_CLKn);

    top.vram_bus.latch_sprite_a_delay.set(_PUCO_CLKp);

    /*p33.PEFO*/ top.vram_bus.PEFO_SPRITE_DB0n.dff8n(top.vram_bus.latch_sprite_a_delay.q6(), _PUTE_FLIP0p);
    /*p33.ROKA*/ top.vram_bus.ROKA_SPRITE_DB1n.dff8n(top.vram_bus.latch_sprite_a_delay.q6(), _PELO_FLIP1p);
    /*p33.MYTU*/ top.vram_bus.MYTU_SPRITE_DB2n.dff8n(top.vram_bus.latch_sprite_a_delay.q6(), _PONO_FLIP2p);
    /*p33.RAMU*/ top.vram_bus.RAMU_SPRITE_DB3n.dff8n(top.vram_bus.latch_sprite_a_delay.q6(), _POBE_FLIP3p);
    /*p33.SELE*/ top.vram_bus.SELE_SPRITE_DB4n.dff8n(top.vram_bus.latch_sprite_a_delay.q6(), _PACY_FLIP4p);
    /*p33.SUTO*/ top.vram_bus.SUTO_SPRITE_DB5n.dff8n(top.vram_bus.latch_sprite_a_delay.q6(), _PUGU_FLIP5p);
    /*p33.RAMA*/ top.vram_bus.RAMA_SPRITE_DB6n.dff8n(top.vram_bus.latch_sprite_a_delay.q6(), _PAWE_FLIP6p);
    /*p33.RYDU*/ top.vram_bus.RYDU_SPRITE_DB7n.dff8n(top.vram_bus.latch_sprite_a_delay.q6(), _PULY_FLIP7p);

    /*#p29.VYWA*/ wire _VYWA_CLKp = not1(top.sprite_fetcher.TOPU_LATCH_SPPIXA);
    /*#p29.WENY*/ wire _WENY_CLKn = not1(_VYWA_CLKp);
    /*#p29.XADO*/ wire _XADO_CLKp = not1(_WENY_CLKn);

    top.vram_bus.latch_sprite_b_delay.set(_XADO_CLKp);

    /*p33.REWO*/ top.vram_bus.REWO_SPRITE_DA0n.dff8n(top.vram_bus.latch_sprite_b_delay.q6(), _PUTE_FLIP0p);
    /*p33.PEBA*/ top.vram_bus.PEBA_SPRITE_DA1n.dff8n(top.vram_bus.latch_sprite_b_delay.q6(), _PELO_FLIP1p);
    /*p33.MOFO*/ top.vram_bus.MOFO_SPRITE_DA2n.dff8n(top.vram_bus.latch_sprite_b_delay.q6(), _PONO_FLIP2p);
    /*p33.PUDU*/ top.vram_bus.PUDU_SPRITE_DA3n.dff8n(top.vram_bus.latch_sprite_b_delay.q6(), _POBE_FLIP3p);
    /*p33.SAJA*/ top.vram_bus.SAJA_SPRITE_DA4n.dff8n(top.vram_bus.latch_sprite_b_delay.q6(), _PACY_FLIP4p);
    /*p33.SUNY*/ top.vram_bus.SUNY_SPRITE_DA5n.dff8n(top.vram_bus.latch_sprite_b_delay.q6(), _PUGU_FLIP5p);
    /*p33.SEMO*/ top.vram_bus.SEMO_SPRITE_DA6n.dff8n(top.vram_bus.latch_sprite_b_delay.q6(), _PAWE_FLIP6p);
    /*p33.SEGA*/ top.vram_bus.SEGA_SPRITE_DA7n.dff8n(top.vram_bus.latch_sprite_b_delay.q6(), _PULY_FLIP7p);
  }

  //------------------------------------------------------------------------------
  // Interrupts

  {
    // IE is technically in the CPU, but we're going to implement it here for now.
    wire FFFF_HIT = cpu_bus.get_bus_addr() == 0xFFFF;
    wire FFFF_WRn = nand2(TAPU_CPU_WRp_xxxxEFGx, FFFF_HIT);

    IE_D0.dff(FFFF_WRn, !RST, cpu_bus.BUS_CPU_D0p.qp());
    IE_D1.dff(FFFF_WRn, !RST, cpu_bus.BUS_CPU_D1p.qp());
    IE_D2.dff(FFFF_WRn, !RST, cpu_bus.BUS_CPU_D2p.qp());
    IE_D3.dff(FFFF_WRn, !RST, cpu_bus.BUS_CPU_D3p.qp());
    IE_D4.dff(FFFF_WRn, !RST, cpu_bus.BUS_CPU_D4p.qp());

    wire FFFF_RDn = nand2(TEDO_CPU_RDp, FFFF_HIT);

    cpu_bus.BUS_CPU_D0p.tri_6nn(FFFF_RDn, IE_D0.qn());
    cpu_bus.BUS_CPU_D1p.tri_6nn(FFFF_RDn, IE_D1.qn());
    cpu_bus.BUS_CPU_D2p.tri_6nn(FFFF_RDn, IE_D2.qn());
    cpu_bus.BUS_CPU_D3p.tri_6nn(FFFF_RDn, IE_D3.qn());
    cpu_bus.BUS_CPU_D4p.tri_6nn(FFFF_RDn, IE_D4.qn());
  }

  //------------------------------------------------------------------------------
  // LCD pixel pipe

  lcd_pix_lo.nor_latch(PIN_LCD_DATA0.qp(), PIN_LCD_CLOCK.qp() | PIN_LCD_HSYNC.qp());
  lcd_pix_hi.nor_latch(PIN_LCD_DATA1.qp(), PIN_LCD_CLOCK.qp() | PIN_LCD_HSYNC.qp());

  for (int i = 0; i < 159; i++) {
    lcd_pipe_lo[i].dff(PIN_LCD_CLOCK.qp(), lcd_pipe_lo[i + 1].qp());
    lcd_pipe_hi[i].dff(PIN_LCD_CLOCK.qp(), lcd_pipe_hi[i + 1].qp());
  }

  lcd_pipe_lo[159].dff(PIN_LCD_CLOCK.qp(), lcd_pix_lo.qp04());
  lcd_pipe_hi[159].dff(PIN_LCD_CLOCK.qp(), lcd_pix_hi.qp04());
}

//-----------------------------------------------------------------------------

void SchematicTop::tock_ext_bus(wire RST, const uint8_t* cart_rom, uint8_t* cart_ram, uint8_t* ext_ram) {
  if (RST) return;

  uint16_t ext_addr = ext_bus.get_pin_addr();

  // ROM read
  {
    uint16_t rom_addr = ext_addr & 0x7FFF;
    wire OEn = ext_bus.PIN_EXT_RDn.qp();
    wire CEn = ext_bus.PIN_EXT_A15p.qp();

    if (!CEn && !OEn) {
      if (cart_rom) {
        ext_bus.set_pin_data(cart_rom[rom_addr]);
      }
      else {
        printf("no cart loaded!\n");
      }
    }
  }

  // Ext RAM read/write (also echo RAM)
  {
    uint16_t ram_addr = (ext_addr & 0x1FFF);

    wire WRn  = ext_bus.PIN_EXT_WRn.qp();
    wire CE1n = ext_bus.PIN_EXT_CSn.qp();
    wire CE2  = ext_bus.PIN_EXT_A14p.qp();
    wire OEn  = ext_bus.PIN_EXT_RDn.qp();

    // Write
    if (!CE1n && CE2 && !WRn) {
      ext_ram[ram_addr] = ext_bus.get_pin_data();
    }

    // Read
    if (!CE1n && CE2 && WRn && !OEn) {
      ext_bus.set_pin_data(ext_ram[ram_addr]);
    }
  }

  // Cart RAM read/write
  {
    // A000-BFFF
    // 0b101xxxxxxxxxxxxx

    uint16_t ram_addr = (ext_addr & 0x1FFF);

    wire WRn  = ext_bus.PIN_EXT_WRn.qp();
    wire CS1n = ext_bus.PIN_EXT_CSn.qp();
    wire CS2  = ext_bus.PIN_EXT_A13p.qp() && !ext_bus.PIN_EXT_A14p.qp() && ext_bus.PIN_EXT_A15p.qp();
    wire OEn = ext_bus.PIN_EXT_RDn.qp();

    // Write
    if (!CS1n && CS2 && !WRn) {
      cart_ram[ram_addr] = ext_bus.get_pin_data();
    }

    // Read
    if (!CS1n && CS2 && !OEn) {
      ext_bus.set_pin_data(cart_ram[ram_addr]);
    }
  }

  // FIXME - implement MBC1

  // 0000-3FFF - ROM Bank 00 (Read Only) This area always contains the first 16KBytes of the cartridge ROM.
  // 4000-7FFF - ROM Bank 01-7F (Read Only) This area may contain any of the further 16KByte banks of the ROM, allowing to address up to 125 ROM Banks (almost 2MByte). As described below, bank numbers 20h, 40h, and 60h cannot be used, resulting in the odd amount of 125 banks.
  // A000-BFFF - RAM Bank 00-03, if any (Read/Write) This area is used to address external RAM in the cartridge (if any). External RAM is often battery buffered, allowing to store game positions or high score tables, even if the gameboy is turned off, or if the cartridge is removed from the gameboy. Available RAM sizes are: 2KByte (at A000-A7FF), 8KByte (at A000-BFFF), and 32KByte (in form of four 8K banks at A000-BFFF).

  // 0000-1FFF - RAM Enable (Write Only)   00h  Disable RAM (default)   ?Ah  Enable RAM Practically any value with 0Ah in the lower 4 bits enables RAM, and any other value disables RAM.
  // 2000-3FFF - ROM Bank Number (Write Only) Writing to this address space selects the lower 5 bits of the ROM Bank Number (in range 01-1Fh). When 00h is written, the MBC translates that to bank 01h also. That doesn't harm so far, because ROM Bank 00h can be always directly accessed by reading from 0000-3FFF.
  // But (when using the register below to specify the upper ROM Bank bits), the same happens for Bank 20h, 40h, and 60h. Any attempt to address these ROM Banks will select Bank 21h, 41h, and 61h instead.
  // 4000-5FFF - RAM Bank Number - or - Upper Bits of ROM Bank Number (Write Only) This 2bit register can be used to select a RAM Bank in range from 00-03h, or to specify the upper two bits (Bit 5-6) of the ROM Bank number, depending on the current ROM/RAM Mode. (See below.)
  // 6000-7FFF - ROM/RAM Mode Select (Write Only)  00h = ROM Banking Mode (up to 8KByte RAM, 2MByte ROM) (default)   01h = RAM Banking Mode (up to 32KByte RAM, 512KByte ROM)

  // MBC1_RAM_EN

  // MBC1_BANK_D0
  // MBC1_BANK_D1
  // MBC1_BANK_D2
  // MBC1_BANK_D3
  // MBC1_BANK_D4
  // MBC1_BANK_D5
  // MBC1_BANK_D6

  // MBC1_MODE

  /*
  {

    bool bank_0 = nor(MBC1_BANK_D0, MBC1_BANK_D1, MBC1_BANK_D2, MBC1_BANK_D3, MBC1_BANK_D4);

    wire cart_rom_a14 = bank_0 ? 1 : MBC1_BANK_D0.qp();
    wire cart_rom_a15 = bank_0 ? 0 : MBC1_BANK_D1.qp();
    wire cart_rom_a16 = bank_0 ? 0 : MBC1_BANK_D2.qp();
    wire cart_rom_a17 = bank_0 ? 0 : MBC1_BANK_D3.qp();
    wire cart_rom_a18 = bank_0 ? 0 : MBC1_BANK_D4.qp();
    wire cart_rom_a19 = MBC1_MODE.qp() ? 0 : bank_0 ? 0 : MBC1_BANK_D5.qp();
    wire cart_rom_a20 = MBC1_MODE.qp() ? 0 : bank_0 ? 0 : MBC1_BANK_D6.qp();

    wire cart_ram_a13 = MBC1_MODE.qp() ? MBC1_BANK_D5.qp() : 0;
    wire cart_ram_a14 = MBC1_MODE.qp() ? MBC1_BANK_D6.qp() : 0;

    // ROM read
    {
      uint16_t rom_addr = ext_addr & 0x7FFF;
      wire OEn = ext_bus.PIN_EXT_RDn.qp();
      wire CEn = ext_bus.PIN_EXT_A15p.qp();

      if (!CEn && !OEn) {
        ext_bus.set_pin_data(cart_rom[rom_addr]);
      }
    }
  }
  */
}

//-----------------------------------------------------------------------------

void SchematicTop::tock_vram_bus(wire RST, uint8_t* vid_ram) {
  if (RST) return;

  int vram_addr = vram_bus.get_pin_addr();
  uint8_t& vram_data = vid_ram[vram_addr];

  // We're getting a fake write on the first phase because PIN_VRAM_WRn resets to 0...
  // ignore it if we're in reset

  if (!RST) {
    if (!vram_bus.PIN_VRAM_WRn.qp()) {
      vram_data = (uint8_t)vram_bus.get_pin_data();
    }

    if (!vram_bus.PIN_VRAM_OEn.qp()) {
      vram_bus.set_pin_data_in(vram_data);
    }
    else {
      vram_bus.set_pin_data_z();
    }
  }
}

//-----------------------------------------------------------------------------
// FIXME This should be using PIN_OAM_CLK (which might actually be PIN_OAM_CSn?)

void SchematicTop::tock_oam_bus(wire RST, uint8_t* oam_ram) {
  if (RST) return;

  uint16_t oam_addr = oam_bus.get_oam_pin_addr();
  uint8_t& oam_data_a = oam_ram[(oam_addr << 1) + 0];
  uint8_t& oam_data_b = oam_ram[(oam_addr << 1) + 1];

  if (!oam_bus.PIN_OAM_WR_A.qp()) oam_data_a = oam_bus.get_oam_pin_data_a();
  if (!oam_bus.PIN_OAM_WR_B.qp()) oam_data_b = oam_bus.get_oam_pin_data_b();

  if (!oam_bus.PIN_OAM_OE.qp()) oam_bus.set_pin_data_a(oam_data_a);
  if (!oam_bus.PIN_OAM_OE.qp()) oam_bus.set_pin_data_b(oam_data_b);
}

//-----------------------------------------------------------------------------

void SchematicTop::tock_zram_bus(wire RST, uint8_t* zero_ram) {
  if (RST) return;

  // ZRAM control signals are

  // clk_reg.PIN_CPU_BUKE_AxxxxxGH
  // TEDO_CPU_RDp();
  // TAPU_CPU_WRp_xxxxEFGx()
  // cpu_bus.SYKE_FF00_FFFFp()

  // and there's somes gates WUTA/WOLY/WALE that do the check for FFXX && !FFFF

  int addr = cpu_bus.get_bus_addr();
  bool hit_zram = (addr >= 0xFF80) && (addr <= 0xFFFE);

  if (hit_zram) {
    uint8_t& data = zero_ram[addr & 0x007F];
    if (TAPU_CPU_WRp_xxxxEFGx) data = cpu_bus.get_bus_data();
    if (TEDO_CPU_RDp) cpu_bus.set_data(data);
  }
}

//------------------------------------------------------------------------------

#if 0
// Debug stuff I disabled

/*p07.APET*/ wire APET_MODE_DBG = or2(clk_reg.UMUT_MODE_DBG1p(), UNOR_MODE_DBG2p); // suggests UMUTp
/*p07.APER*/ wire FF60_WRn = nand2(APET_MODE_DBG, BUS_CPU_A05, BUS_CPU_A06, TAPU_CPUWR, ADDR_111111110xx00000);

//----------
// weird debug things, probably not right

/*p05.AXYN*/ wire AXYN_xBCDEFGH = not1(clk_reg.BEDO_Axxxxxxx);
/*p05.ADYR*/ wire ADYR_Axxxxxxx = not1(AXYN_xBCDEFGH);
/*p05.APYS*/ wire APYS_xBCDEFGH = nor4(sys_sig.MODE_DBG2, ADYR_Axxxxxxx);
/*p05.AFOP*/ wire AFOP_Axxxxxxx = not1(APYS_xBCDEFGH);
/*p07.LECO*/ wire LECO_xBCDEFGH = nor4(clk_reg.BEDO_Axxxxxxx, sys_sig.MODE_DBG2);

if (AFOP_Axxxxxxx) set_data(
  /*p05.ANOC*/ not1(GND),
  /*p05.ATAJ*/ not1(GND),
  /*p05.AJEC*/ not1(GND),
  /*p05.ASUZ*/ not1(GND),
  /*p05.BENU*/ not1(GND),
  /*p05.AKAJ*/ not1(GND),
  /*p05.ARAR*/ not1(GND),
  /*p05.BEDA*/ not1(GND)
);

if (LECO_xBCDEFGH) set_data(
  /*p07.ROMY*/ GND,
  /*p07.RYNE*/ GND,
  /*p07.REJY*/ GND,
  /*p07.RASE*/ GND,
  /*p07.REKA*/ GND,
  /*p07.ROWE*/ GND,
  /*p07.RYKE*/ GND,
  /*p07.RARU*/ GND
);



//----------
// more debug stuff

/*p25.TUSO*/ wire TUSO = nor4(MODE_DBG2, dff20.PIN_CPU_BOGA_xBCDEFGH);
/*p25.SOLE*/ wire SOLE = not1(TUSO);

if (VYPO_GND) bus_out.set_data(
  /*p25.TOVU*/ SOLE,
  /*p25.SOSA*/ SOLE,
  /*p25.SEDU*/ SOLE,
  /*p25.TAXO*/ SOLE,
  /*p25.TAHY*/ SOLE,
  /*p25.TESU*/ SOLE,
  /*p25.TAZU*/ SOLE,
  /*p25.TEWA*/ SOLE
);

///*p05.KORE*/ wire P05_NC0 = nand2(KERU_DBG_FF00_D7, FF60_0);
///*p05.KYWE*/ wire P05_NC1 = nor4 (KERU_DBG_FF00_D7, FF60_0o);

/*p08.LYRA*/ wire DBG_D_RDn = nand2(sys_sig.MODE_DBG2, bus_sig.CBUS_TO_CEXTn);
/*p08.TUTY*/ if (DBG_D_RDn) BUS_CPU_D0p = not1(/*p08.TOVO*/ not1(pins.PIN_D0_C));
/*p08.SYWA*/ if (DBG_D_RDn) BUS_CPU_D1p = not1(/*p08.RUZY*/ not1(pins.PIN_D1_C));
/*p08.SUGU*/ if (DBG_D_RDn) BUS_CPU_D2p = not1(/*p08.ROME*/ not1(pins.PIN_D2_C));
/*p08.TAWO*/ if (DBG_D_RDn) BUS_CPU_D3p = not1(/*p08.SAZA*/ not1(pins.PIN_D3_C));
/*p08.TUTE*/ if (DBG_D_RDn) BUS_CPU_D4p = not1(/*p08.TEHE*/ not1(pins.PIN_D4_C));
/*p08.SAJO*/ if (DBG_D_RDn) BUS_CPU_D5p = not1(/*p08.RATU*/ not1(pins.PIN_D5_C));
/*p08.TEMY*/ if (DBG_D_RDn) BUS_CPU_D6p = not1(/*p08.SOCA*/ not1(pins.PIN_D6_C));
/*p08.ROPA*/ if (DBG_D_RDn) BUS_CPU_D7p = not1(/*p08.RYBA*/ not1(pins.PIN_D7_C));
#endif

// hack, not correct
#if 0
{
  // FF60 debug state
  /*p07.APET*/ wire APET_MODE_DBG = or2(sys_sig.MODE_DBG1, sys_sig.MODE_DBG2);
  /*p07.APER*/ wire FF60_WRn = nand2(APET_MODE_DBG, BUS_CPU_A05, BUS_CPU_A06, bus_sig.TAPU_CPUWR, dec_sig.ADDR_111111110xx00000);

  /*p05.KURA*/ wire FF60_0n = not1(BURO_FF60_0);
  /*p05.JEVA*/ wire FF60_0o = not1(BURO_FF60_0);
  /*p07.BURO*/ BURO_FF60_0 = ff9(FF60_WRn, rst_sig.SYS_RESETn, BUS_CPU_D0p);
  /*p07.AMUT*/ AMUT_FF60_1 = ff9(FF60_WRn, rst_sig.SYS_RESETn, BUS_CPU_D1p);

  ///*p05.KURA*/ wire FF60_0n = not1(FF60);
  ///*p05.JEVA*/ wire FF60_0o = not1(FF60);
}
#endif


#if 0
// so the address bus is technically a tribuf, but we're going to ignore
// this debug circuit for now.
{
  // If we're in debug mode 2, drive external address bus onto internal address

  /*#p08.KOVA*/ wire KOVA_A00p = not1(PIN_EXT_A00p.qn());
  /* p08.CAMU*/ wire CAMU_A01p = not1(PIN_EXT_A01p.qn());
  /* p08.BUXU*/ wire BUXU_A02p = not1(PIN_EXT_A02p.qn());
  /* p08.BASE*/ wire BASE_A03p = not1(PIN_EXT_A03p.qn());
  /* p08.AFEC*/ wire AFEC_A04p = not1(PIN_EXT_A04p.qn());
  /* p08.ABUP*/ wire ABUP_A05p = not1(PIN_EXT_A05p.qn());
  /* p08.CYGU*/ wire CYGU_A06p = not1(PIN_EXT_A06p.qn());
  /* p08.COGO*/ wire COGO_A07p = not1(PIN_EXT_A07p.qn());
  /* p08.MUJY*/ wire MUJY_A08p = not1(PIN_EXT_A08p.qn());
  /* p08.NENA*/ wire NENA_A09p = not1(PIN_EXT_A09p.qn());
  /* p08.SURA*/ wire SURA_A10p = not1(PIN_EXT_A10p.qn());
  /* p08.MADY*/ wire MADY_A11p = not1(PIN_EXT_A11p.qn());
  /* p08.LAHE*/ wire LAHE_A12p = not1(PIN_EXT_A12p.qn());
  /* p08.LURA*/ wire LURA_A13p = not1(PIN_EXT_A13p.qn());
  /* p08.PEVO*/ wire PEVO_A14p = not1(PIN_EXT_A14p.qn());
  /* p08.RAZA*/ wire RAZA_A15p = not1(PIN_EXT_A15p.qn());

  // KEJO_01 << KOVA_02
  // KEJO_02
  // KEJO_03
  // KEJO_04 << TOVA_02
  // KEJO_05 << KOVA_02
  // KEJO_06
  // KEJO_07
  // KEJO_08
  // KEJO_09 >> BUS_CPU_A00p
  // KEJO_10

  /* p08.KEJO*/ cpu_bus.BUS_CPU_A00 = tribuf_10np(TOVA_MODE_DBG2n, KOVA_A00p);
  /* p08.BYXE*/ cpu_bus.BUS_CPU_A01 = tribuf_10np(TOVA_MODE_DBG2n, CAMU_A01p);
  /* p08.AKAN*/ cpu_bus.BUS_CPU_A02 = tribuf_10np(TOVA_MODE_DBG2n, BUXU_A02p);
  /* p08.ANAR*/ cpu_bus.BUS_CPU_A03 = tribuf_10np(TOVA_MODE_DBG2n, BASE_A03p);
  /* p08.AZUV*/ cpu_bus.BUS_CPU_A04 = tribuf_10np(TOVA_MODE_DBG2n, AFEC_A04p);
  /* p08.AJOV*/ cpu_bus.BUS_CPU_A05 = tribuf_10np(TOVA_MODE_DBG2n, ABUP_A05p);
  /* p08.BYNE*/ cpu_bus.BUS_CPU_A06 = tribuf_10np(TOVA_MODE_DBG2n, CYGU_A06p);
  /* p08.BYNA*/ cpu_bus.BUS_CPU_A07 = tribuf_10np(TOVA_MODE_DBG2n, COGO_A07p);
  /* p08.LOFA*/ cpu_bus.BUS_CPU_A08 = tribuf_10np(TOVA_MODE_DBG2n, MUJY_A08p);
  /* p08.MAPU*/ cpu_bus.BUS_CPU_A09 = tribuf_10np(TOVA_MODE_DBG2n, NENA_A09p);
  /* p08.RALA*/ cpu_bus.BUS_CPU_A10 = tribuf_10np(TOVA_MODE_DBG2n, SURA_A10p);
  /* p08.LORA*/ cpu_bus.BUS_CPU_A11 = tribuf_10np(TOVA_MODE_DBG2n, MADY_A11p);
  /* p08.LYNA*/ cpu_bus.BUS_CPU_A12 = tribuf_10np(TOVA_MODE_DBG2n, LAHE_A12p);
  /* p08.LEFY*/ cpu_bus.BUS_CPU_A13 = tribuf_10np(TOVA_MODE_DBG2n, LURA_A13p);
  /* p08.NEFE*/ cpu_bus.BUS_CPU_A14 = tribuf_10np(TOVA_MODE_DBG2n, PEVO_A14p);
  /* p08.SYZU*/ cpu_bus.BUS_CPU_A15 = tribuf_10np(TOVA_MODE_DBG2n, RAZA_A15p);
}
#endif

//-----------------------------------------------------------------------------
