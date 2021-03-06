#include "GateBoyLib/GateBoyOamBus.h"

#include "GateBoyLib/GateBoy.h"

//------------------------------------------------------------------------------------------------------------------------

void GateBoy::oam_latch_to_temp_a(wire _COTA_OAM_CLKn, const OamLatchA& oam_latch_a, OamTempA& oam_temp_a)
{
  /* p29.YWOK*/ wire _YWOK_OAM_CLKp = not1(_COTA_OAM_CLKn); // inverting this clock does not break anything
  /*#p29.XUSO*/ oam_temp_a.XUSO_OAM_DA0p.dff8n(_YWOK_OAM_CLKp, oam_latch_a.YDYV_OAM_LATCH_DA0n.qp_old());
  /* p29.XEGU*/ oam_temp_a.XEGU_OAM_DA1p.dff8n(_YWOK_OAM_CLKp, oam_latch_a.YCEB_OAM_LATCH_DA1n.qp_old());
  /* p29.YJEX*/ oam_temp_a.YJEX_OAM_DA2p.dff8n(_YWOK_OAM_CLKp, oam_latch_a.ZUCA_OAM_LATCH_DA2n.qp_old());
  /* p29.XYJU*/ oam_temp_a.XYJU_OAM_DA3p.dff8n(_YWOK_OAM_CLKp, oam_latch_a.WONE_OAM_LATCH_DA3n.qp_old());
  /* p29.YBOG*/ oam_temp_a.YBOG_OAM_DA4p.dff8n(_YWOK_OAM_CLKp, oam_latch_a.ZAXE_OAM_LATCH_DA4n.qp_old());
  /* p29.WYSO*/ oam_temp_a.WYSO_OAM_DA5p.dff8n(_YWOK_OAM_CLKp, oam_latch_a.XAFU_OAM_LATCH_DA5n.qp_old());
  /* p29.XOTE*/ oam_temp_a.XOTE_OAM_DA6p.dff8n(_YWOK_OAM_CLKp, oam_latch_a.YSES_OAM_LATCH_DA6n.qp_old());
  /* p29.YZAB*/ oam_temp_a.YZAB_OAM_DA7p.dff8n(_YWOK_OAM_CLKp, oam_latch_a.ZECA_OAM_LATCH_DA7n.qp_old());
}

void GateBoy::oam_latch_to_temp_b(wire _COTA_OAM_CLKn, const OamLatchB& oam_latch_b, OamTempB& oam_temp_b)
{
  /*#p31.XEGA*/ wire _XEGA_OAM_CLKp = not1(_COTA_OAM_CLKn); // inverting this clock does not break anything
  /* p31.YLOR*/ oam_temp_b.YLOR_OAM_DB0p.dff8n(_XEGA_OAM_CLKp, oam_latch_b.XYKY_OAM_LATCH_DB0n.qp_old());
  /* p31.ZYTY*/ oam_temp_b.ZYTY_OAM_DB1p.dff8n(_XEGA_OAM_CLKp, oam_latch_b.YRUM_OAM_LATCH_DB1n.qp_old());
  /* p31.ZYVE*/ oam_temp_b.ZYVE_OAM_DB2p.dff8n(_XEGA_OAM_CLKp, oam_latch_b.YSEX_OAM_LATCH_DB2n.qp_old());
  /* p31.ZEZY*/ oam_temp_b.ZEZY_OAM_DB3p.dff8n(_XEGA_OAM_CLKp, oam_latch_b.YVEL_OAM_LATCH_DB3n.qp_old());
  /* p31.GOMO*/ oam_temp_b.GOMO_OAM_DB4p.dff8n(_XEGA_OAM_CLKp, oam_latch_b.WYNO_OAM_LATCH_DB4n.qp_old());
  /* p31.BAXO*/ oam_temp_b.BAXO_OAM_DB5p.dff8n(_XEGA_OAM_CLKp, oam_latch_b.CYRA_OAM_LATCH_DB5n.qp_old());
  /* p31.YZOS*/ oam_temp_b.YZOS_OAM_DB6p.dff8n(_XEGA_OAM_CLKp, oam_latch_b.ZUVE_OAM_LATCH_DB6n.qp_old());
  /* p31.DEPO*/ oam_temp_b.DEPO_OAM_DB7p.dff8n(_XEGA_OAM_CLKp, oam_latch_b.ECED_OAM_LATCH_DB7n.qp_old());
}

//------------------------------------------------------------------------------------------------------------------------

void GateBoy::tock_oam_bus()
{
  memset(&oam_bus, BIT_NEW | BIT_PULLED | 1, sizeof(oam_bus));

  // OAM address from CPU
  /* p28.ASAM*/ wire _ASAM_CPU_OAM_RDn = or3(sprite_scanner.ACYL_SCANNINGp.qp_new(), XYMU_RENDERINGn.qn_new(), dma.MATU_DMA_RUNNINGp.qp_new());
  /* p28.GARO_CA0_TO_OA0*/ oam_bus.BUS_OAM_A00n.tri6_nn(_ASAM_CPU_OAM_RDn, new_bus.BUS_CPU_A00p.qp_new());
  /* p28.WACU_CA1_TO_OA1*/ oam_bus.BUS_OAM_A01n.tri6_nn(_ASAM_CPU_OAM_RDn, new_bus.BUS_CPU_A01p.qp_new());
  /* p28.GOSE_CA2_TO_OA2*/ oam_bus.BUS_OAM_A02n.tri6_nn(_ASAM_CPU_OAM_RDn, new_bus.BUS_CPU_A02p.qp_new());
  /* p28.WAPE_CA3_TO_OA3*/ oam_bus.BUS_OAM_A03n.tri6_nn(_ASAM_CPU_OAM_RDn, new_bus.BUS_CPU_A03p.qp_new());
  /* p28.FEVU_CA4_TO_OA4*/ oam_bus.BUS_OAM_A04n.tri6_nn(_ASAM_CPU_OAM_RDn, new_bus.BUS_CPU_A04p.qp_new());
  /* p28.GERA_CA5_TO_OA5*/ oam_bus.BUS_OAM_A05n.tri6_nn(_ASAM_CPU_OAM_RDn, new_bus.BUS_CPU_A05p.qp_new());
  /* p28.WAXA_CA6_TO_OA6*/ oam_bus.BUS_OAM_A06n.tri6_nn(_ASAM_CPU_OAM_RDn, new_bus.BUS_CPU_A06p.qp_new());
  /* p28.FOBY_CA7_TO_OA7*/ oam_bus.BUS_OAM_A07n.tri6_nn(_ASAM_CPU_OAM_RDn, new_bus.BUS_CPU_A07p.qp_new());

  // DMA OAM write address driver
  /* p04.DUGA*/ wire DUGA_DMA_RUNNINGn = not1(dma.MATU_DMA_RUNNINGp.qp_new());
  /* p28.FODO_DMA0_TO_OA0*/ oam_bus.BUS_OAM_A00n.tri6_nn(DUGA_DMA_RUNNINGn, dma.NAKY_DMA_A00p.qp_new());
  /* p28.FESA_DMA1_TO_OA1*/ oam_bus.BUS_OAM_A01n.tri6_nn(DUGA_DMA_RUNNINGn, dma.PYRO_DMA_A01p.qp_new());
  /* p28.FAGO_DMA2_TO_OA2*/ oam_bus.BUS_OAM_A02n.tri6_nn(DUGA_DMA_RUNNINGn, dma.NEFY_DMA_A02p.qp_new());
  /* p28.FYKY_DMA3_TO_OA3*/ oam_bus.BUS_OAM_A03n.tri6_nn(DUGA_DMA_RUNNINGn, dma.MUTY_DMA_A03p.qp_new());
  /* p28.ELUG_DMA4_TO_OA4*/ oam_bus.BUS_OAM_A04n.tri6_nn(DUGA_DMA_RUNNINGn, dma.NYKO_DMA_A04p.qp_new());
  /* p28.EDOL_DMA5_TO_OA5*/ oam_bus.BUS_OAM_A05n.tri6_nn(DUGA_DMA_RUNNINGn, dma.PYLO_DMA_A05p.qp_new());
  /* p28.FYDU_DMA6_TO_OA6*/ oam_bus.BUS_OAM_A06n.tri6_nn(DUGA_DMA_RUNNINGn, dma.NUTO_DMA_A06p.qp_new());
  /* p28.FETU_DMA7_TO_OA7*/ oam_bus.BUS_OAM_A07n.tri6_nn(DUGA_DMA_RUNNINGn, dma.MUGU_DMA_A07p.qp_new());

  // OAM address from sprite scanner
  /* p28.APAR*/ wire _APAR_SCANNINGn = not1(sprite_scanner.ACYL_SCANNINGp.qp_new());
  /* p28.GEFY_SCANX_TO_OA0*/ oam_bus.BUS_OAM_A00n.tri6_nn(_APAR_SCANNINGn, SIG_GND.qp_new());
  /* p28.WUWE_SCANX_TO_OA1*/ oam_bus.BUS_OAM_A01n.tri6_nn(_APAR_SCANNINGn, SIG_GND.qp_new());
  /* p28.GUSE_SCAN0_TO_OA2*/ oam_bus.BUS_OAM_A02n.tri6_nn(_APAR_SCANNINGn, sprite_scanner.YFEL_SCAN0.qp_new());
  /* p28.GEMA_SCAN1_TO_OA3*/ oam_bus.BUS_OAM_A03n.tri6_nn(_APAR_SCANNINGn, sprite_scanner.WEWY_SCAN1.qp_new());
  /* p28.FUTO_SCAN2_TO_OA4*/ oam_bus.BUS_OAM_A04n.tri6_nn(_APAR_SCANNINGn, sprite_scanner.GOSO_SCAN2.qp_new());
  /* p28.FAKU_SCAN3_TO_OA5*/ oam_bus.BUS_OAM_A05n.tri6_nn(_APAR_SCANNINGn, sprite_scanner.ELYN_SCAN3.qp_new());
  /* p28.GAMA_SCAN4_TO_OA6*/ oam_bus.BUS_OAM_A06n.tri6_nn(_APAR_SCANNINGn, sprite_scanner.FAHA_SCAN4.qp_new());
  /* p28.GOBY_SCAN5_TO_OA7*/ oam_bus.BUS_OAM_A07n.tri6_nn(_APAR_SCANNINGn, sprite_scanner.FONY_SCAN5.qp_new());

  // OAM address from sprite fetcher
  /*#p28.BOGE*/ wire BOGE_DMA_RUNNINGn = not1(dma.MATU_DMA_RUNNINGp.qp_new());
  /* p28.AJON*/ wire _AJON_RENDERINGp = and2(BOGE_DMA_RUNNINGn, XYMU_RENDERINGn.qn_new()); // def AND. ppu can read oam when there's rendering but no dma
  /* p28.BETE*/ wire _BETE_SPR_I_TO_OAM_An = not1(_AJON_RENDERINGp);
  /* p28.GECA_FETCHX_TO_OA0*/ oam_bus.BUS_OAM_A00n.tri6_nn(_BETE_SPR_I_TO_OAM_An, SIG_VCC.qp_new());
  /* p28.WYDU_FETCHX_TO_OA1*/ oam_bus.BUS_OAM_A01n.tri6_nn(_BETE_SPR_I_TO_OAM_An, SIG_VCC.qp_new());
  /* p28.GYBU_FETCH0_TO_OA2*/ oam_bus.BUS_OAM_A02n.tri6_nn(_BETE_SPR_I_TO_OAM_An, sprite_bus.BUS_SPR_I0.qp_new());
  /* p28.GYKA_FETCH1_TO_OA3*/ oam_bus.BUS_OAM_A03n.tri6_nn(_BETE_SPR_I_TO_OAM_An, sprite_bus.BUS_SPR_I1.qp_new());
  /* p28.FABY_FETCH2_TO_OA4*/ oam_bus.BUS_OAM_A04n.tri6_nn(_BETE_SPR_I_TO_OAM_An, sprite_bus.BUS_SPR_I2.qp_new());
  /* p28.FACO_FETCH3_TO_OA5*/ oam_bus.BUS_OAM_A05n.tri6_nn(_BETE_SPR_I_TO_OAM_An, sprite_bus.BUS_SPR_I3.qp_new());
  /* p28.FUGU_FETCH4_TO_OA6*/ oam_bus.BUS_OAM_A06n.tri6_nn(_BETE_SPR_I_TO_OAM_An, sprite_bus.BUS_SPR_I4.qp_new());
  /* p28.FYKE_FETCH5_TO_OA7*/ oam_bus.BUS_OAM_A07n.tri6_nn(_BETE_SPR_I_TO_OAM_An, sprite_bus.BUS_SPR_I5.qp_new());

  //probe_wire(16, "ASAM CPU", _ASAM_CPU_OAM_RDn);
  //probe_wire(17, "DUGA DMA", DUGA_DMA_RUNNINGn);
  //probe_wire(18, "APAR SCN", _APAR_SCANNINGn);
  //probe_wire(19, "BETE PPU", _BETE_SPR_I_TO_OAM_An);

  //if (!oam_bus.BUS_OAM_A00n.is_driven()) {
  //  printf("%03d oam bus not driven\n", line_phase_x);
  //}

  // cpu to oam data bus

  /* p28.XUTO*/ wire _XUTO_CPU_OAM_WRp = and2(new_bus.SARO_ADDR_OAMp(), CUPA_CPU_WRp());
  /* p28.WUJE*/ oam.WUJE_CPU_OAM_WRn.nor_latch(XYNY_ABCDxxxx(), _XUTO_CPU_OAM_WRp); // slightly weird

  /* p28.XUPA*/ wire _XUPA_CPU_OAM_WRp  = not1(oam.WUJE_CPU_OAM_WRn.qp_new());
  /*#p28.AJUJ*/ wire _AJUJ_OAM_BUSYn    = nor3(dma.MATU_DMA_RUNNINGp.qp_new(), sprite_scanner.ACYL_SCANNINGp.qp_new(), _AJON_RENDERINGp);
  /*#p28.AMAB*/ wire _AMAB_CPU_OAM_ENp  = and2(new_bus.SARO_ADDR_OAMp(), _AJUJ_OAM_BUSYn); // def and
  /*#p28.APAG*/ wire _APAG_CBD_TO_OBDp  = amux2(_XUPA_CPU_OAM_WRp, _AMAB_CPU_OAM_ENp, _AJUJ_OAM_BUSYn, new_bus.ADAH_FE00_FEFFn());
  /*#p28.AZUL*/ wire _AZUL_CBD_TO_OBDn  = not1(_APAG_CBD_TO_OBDp);

  /* p28.ZAMY_CD0_TO_ODB0*/ oam_bus.BUS_OAM_DA00n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D00p.qp_new());
  /* p28.ZOPU_CD1_TO_ODB1*/ oam_bus.BUS_OAM_DA01n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D01p.qp_new());
  /* p28.WYKY_CD2_TO_ODB2*/ oam_bus.BUS_OAM_DA02n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D02p.qp_new());
  /* p28.ZAJA_CD3_TO_ODB3*/ oam_bus.BUS_OAM_DA03n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D03p.qp_new());
  /* p28.ZUGA_CD4_TO_ODB4*/ oam_bus.BUS_OAM_DA04n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D04p.qp_new());
  /* p28.ZUMO_CD5_TO_ODB5*/ oam_bus.BUS_OAM_DA05n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D05p.qp_new());
  /* p28.XYTO_CD6_TO_ODB6*/ oam_bus.BUS_OAM_DA06n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D06p.qp_new());
  /* p28.ZYFA_CD7_TO_ODB7*/ oam_bus.BUS_OAM_DA07n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D07p.qp_new());

  /* p28.ZAXA_CD0_TO_ODA0*/ oam_bus.BUS_OAM_DB00n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D00p.qp_new());
  /* p28.ZAKY_CD1_TO_ODA1*/ oam_bus.BUS_OAM_DB01n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D01p.qp_new());
  /* p28.WULE_CD2_TO_ODA2*/ oam_bus.BUS_OAM_DB02n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D02p.qp_new());
  /* p28.ZOZO_CD3_TO_ODA3*/ oam_bus.BUS_OAM_DB03n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D03p.qp_new());
  /* p28.ZUFO_CD4_TO_ODA4*/ oam_bus.BUS_OAM_DB04n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D04p.qp_new());
  /* p28.ZATO_CD5_TO_ODA5*/ oam_bus.BUS_OAM_DB05n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D05p.qp_new());
  /* p28.YVUC_CD6_TO_ODA6*/ oam_bus.BUS_OAM_DB06n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D06p.qp_new());
  /* p28.ZUFE_CD7_TO_ODA7*/ oam_bus.BUS_OAM_DB07n.tri6_nn(_AZUL_CBD_TO_OBDn, new_bus.BUS_CPU_D07p.qp_new());

  // ext dma to oam data bus

  /* p25.RALO*/ wire _RALO_EXT_D0p = not1(ext_pins.PIN_17_D00.qp_int_new());
  /* p25.TUNE*/ wire _TUNE_EXT_D1p = not1(ext_pins.PIN_18_D01.qp_int_new());
  /* p25.SERA*/ wire _SERA_EXT_D2p = not1(ext_pins.PIN_19_D02.qp_int_new());
  /* p25.TENU*/ wire _TENU_EXT_D3p = not1(ext_pins.PIN_20_D03.qp_int_new());
  /* p25.SYSA*/ wire _SYSA_EXT_D4p = not1(ext_pins.PIN_21_D04.qp_int_new());
  /* p25.SUGY*/ wire _SUGY_EXT_D5p = not1(ext_pins.PIN_22_D05.qp_int_new());
  /* p25.TUBE*/ wire _TUBE_EXT_D6p = not1(ext_pins.PIN_23_D06.qp_int_new());
  /* p25.SYZO*/ wire _SYZO_EXT_D7p = not1(ext_pins.PIN_24_D07.qp_int_new());

  /*#p04.LEBU*/ wire _LEBU_DMA_A15n  = not1(dma.MARU_DMA_A15n.qn_new());
  /*#p04.MUDA*/ wire _MUDA_DMA_VRAMp = nor3(dma.PULA_DMA_A13n.qn_new(), dma.POKU_DMA_A14n.qn_new(), _LEBU_DMA_A15n);
  /* p04.LOGO*/ wire _LOGO_DMA_VRAMn = not1(_MUDA_DMA_VRAMp);
  /* p04.MORY*/ wire _MORY_DMA_CARTn = nand2(dma.MATU_DMA_RUNNINGp.qp_new(), _LOGO_DMA_VRAMn);
  /* p04.LUMA*/ wire _LUMA_DMA_CARTp = not1(_MORY_DMA_CARTn);
  /* p25.CEDE*/ wire _CEDE_EBD_TO_OBDn = not1(_LUMA_DMA_CARTp);

  /* p25.WASA_ED0_TO_ODA0*/ oam_bus.BUS_OAM_DA00n.tri6_nn(_CEDE_EBD_TO_OBDn, _RALO_EXT_D0p);
  /* p25.BOMO_ED1_TO_ODA1*/ oam_bus.BUS_OAM_DA01n.tri6_nn(_CEDE_EBD_TO_OBDn, _TUNE_EXT_D1p);
  /* p25.BASA_ED2_TO_ODA2*/ oam_bus.BUS_OAM_DA02n.tri6_nn(_CEDE_EBD_TO_OBDn, _SERA_EXT_D2p);
  /* p25.CAKO_ED3_TO_ODA3*/ oam_bus.BUS_OAM_DA03n.tri6_nn(_CEDE_EBD_TO_OBDn, _TENU_EXT_D3p);
  /* p25.BUMA_ED4_TO_ODA4*/ oam_bus.BUS_OAM_DA04n.tri6_nn(_CEDE_EBD_TO_OBDn, _SYSA_EXT_D4p);
  /* p25.BUPY_ED5_TO_ODA5*/ oam_bus.BUS_OAM_DA05n.tri6_nn(_CEDE_EBD_TO_OBDn, _SUGY_EXT_D5p);
  /* p25.BASY_ED6_TO_ODA6*/ oam_bus.BUS_OAM_DA06n.tri6_nn(_CEDE_EBD_TO_OBDn, _TUBE_EXT_D6p);
  /* p25.BAPE_ED7_TO_ODA7*/ oam_bus.BUS_OAM_DA07n.tri6_nn(_CEDE_EBD_TO_OBDn, _SYZO_EXT_D7p);

  /* p25.WEJO_ED0_TO_ODB0*/ oam_bus.BUS_OAM_DB00n.tri6_nn(_CEDE_EBD_TO_OBDn, _RALO_EXT_D0p);
  /* p25.BUBO_ED1_TO_ODB1*/ oam_bus.BUS_OAM_DB01n.tri6_nn(_CEDE_EBD_TO_OBDn, _TUNE_EXT_D1p);
  /* p25.BETU_ED2_TO_ODB2*/ oam_bus.BUS_OAM_DB02n.tri6_nn(_CEDE_EBD_TO_OBDn, _SERA_EXT_D2p);
  /* p25.CYME_ED3_TO_ODB3*/ oam_bus.BUS_OAM_DB03n.tri6_nn(_CEDE_EBD_TO_OBDn, _TENU_EXT_D3p);
  /* p25.BAXU_ED4_TO_ODB4*/ oam_bus.BUS_OAM_DB04n.tri6_nn(_CEDE_EBD_TO_OBDn, _SYSA_EXT_D4p);
  /* p25.BUHU_ED5_TO_ODB5*/ oam_bus.BUS_OAM_DB05n.tri6_nn(_CEDE_EBD_TO_OBDn, _SUGY_EXT_D5p);
  /* p25.BYNY_ED6_TO_ODB6*/ oam_bus.BUS_OAM_DB06n.tri6_nn(_CEDE_EBD_TO_OBDn, _TUBE_EXT_D6p);
  /* p25.BYPY_ED7_TO_ODB7*/ oam_bus.BUS_OAM_DB07n.tri6_nn(_CEDE_EBD_TO_OBDn, _SYZO_EXT_D7p);

  // vram dma to oam data bus

  /* p04.MUHO*/ wire _MUHO_DMA_VRAMp = nand2(dma.MATU_DMA_RUNNINGp.qp_new(), _MUDA_DMA_VRAMp);
  /* p04.LUFA*/ wire _LUFA_DMA_VRAMp = not1(_MUHO_DMA_VRAMp);
  /* p28.AZAR*/ wire _AZAR_VBD_TO_OBDn = not1(_LUFA_DMA_VRAMp);

  /* p28.WUZU_VD0_TO_ODA0*/ oam_bus.BUS_OAM_DA00n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D00p.qp_new());
  /* p28.AXER_VD1_TO_ODA1*/ oam_bus.BUS_OAM_DA01n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D01p.qp_new());
  /* p28.ASOX_VD2_TO_ODA2*/ oam_bus.BUS_OAM_DA02n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D02p.qp_new());
  /* p28.CETU_VD3_TO_ODA3*/ oam_bus.BUS_OAM_DA03n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D03p.qp_new());
  /* p28.ARYN_VD4_TO_ODA4*/ oam_bus.BUS_OAM_DA04n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D04p.qp_new());
  /* p28.ACOT_VD5_TO_ODA5*/ oam_bus.BUS_OAM_DA05n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D05p.qp_new());
  /* p28.CUJE_VD6_TO_ODA6*/ oam_bus.BUS_OAM_DA06n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D06p.qp_new());
  /* p28.ATER_VD7_TO_ODA7*/ oam_bus.BUS_OAM_DA07n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D07p.qp_new());

  /* p28.WOWA_VD0_TO_ODB0*/ oam_bus.BUS_OAM_DB00n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D00p.qp_new());
  /* p28.AVEB_VD1_TO_ODB1*/ oam_bus.BUS_OAM_DB01n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D01p.qp_new());
  /* p28.AMUH_VD2_TO_ODB2*/ oam_bus.BUS_OAM_DB02n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D02p.qp_new());
  /* p28.COFO_VD3_TO_ODB3*/ oam_bus.BUS_OAM_DB03n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D03p.qp_new());
  /* p28.AZOZ_VD4_TO_ODB4*/ oam_bus.BUS_OAM_DB04n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D04p.qp_new());
  /* p28.AGYK_VD5_TO_ODB5*/ oam_bus.BUS_OAM_DB05n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D05p.qp_new());
  /* p28.BUSE_VD6_TO_ODB6*/ oam_bus.BUS_OAM_DB06n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D06p.qp_new());
  /* p28.ANUM_VD7_TO_ODB7*/ oam_bus.BUS_OAM_DB07n.tri6_nn(_AZAR_VBD_TO_OBDn, vram_bus.BUS_VRAM_D07p.qp_new());

  /* p25.AVER*/ wire _AVER_AxxxExxx = nand2(sprite_scanner.ACYL_SCANNINGp.qp_new(), XYSO_xBCDxFGH());
  /* p25.CUFE*/ wire _CUFE_OAM_CLKp = not_or_and3(new_bus.SARO_ADDR_OAMp(), dma.MATU_DMA_RUNNINGp.qp_new(), MOPA_xxxxEFGH()); // CUFE looks like BYHA minus an inverter
  /* p29.TYTU*/ wire _TYTU_SFETCH_S0n = not1(sprite_fetcher.TOXE_SFETCH_S0p.qp_new());
  /* p29.TACU*/ wire _TACU_SPR_SEQ_5_TRIG = nand2(sprite_fetcher.TYFO_SFETCH_S0p_D1.qp_new(), _TYTU_SFETCH_S0n);
  /* p29.TEPA*/ wire _TEPA_RENDERINGp = not1(XYMU_RENDERINGn.qn_new());
  /* p29.TUVO*/ wire _TUVO_PPU_OAM_RDp = nor3(_TEPA_RENDERINGp, sprite_fetcher.TULY_SFETCH_S1p.qp_new(), sprite_fetcher.TESE_SFETCH_S2p.qp_new());
  /* p25.VAPE*/ wire _VAPE_OAM_CLKENn = and2(_TUVO_PPU_OAM_RDp, _TACU_SPR_SEQ_5_TRIG);
  /* p25.XUJY*/ wire _XUJY_OAM_CLKENp = not1(_VAPE_OAM_CLKENn);
  /* p25.BYCU*/ wire _BYCU_OAM_CLKp = nand3(_AVER_AxxxExxx, _XUJY_OAM_CLKENp, _CUFE_OAM_CLKp);
  /* p25.COTA*/ wire _COTA_OAM_CLKn = not1(_BYCU_OAM_CLKp);

  /*SIG_OAM_CLKn*/ oam.SIG_OAM_CLKn.sig_out(_COTA_OAM_CLKn);

  /*#p28.GEKA*/ wire _GEKA_OAM_A0p = not1(oam_bus.BUS_OAM_A00n.qp_new());
  /* p28.WAFO*/ wire _WAFO_OAM_A0n = not1(_GEKA_OAM_A0p);

  /* p04.MAKA*/ oam.MAKA_LATCH_EXTp.dff17(ZEME_AxCxExGx(), AVOR_SYS_RSTp(), CATY_LATCH_EXTp());

  /* p04.NAXY*/ wire _NAXY_DMA_OAM_WRp = nor2(UVYT_ABCDxxxx(), oam.MAKA_LATCH_EXTp.qp_new()); // def nor2
  /* p04.POWU*/ wire _POWU_DMA_OAM_WRp = and2(dma.MATU_DMA_RUNNINGp.qp_new(), _NAXY_DMA_OAM_WRp); // def and
  /* p04.WYJA*/ wire _WYJA_OAM_WRp     = and_or3(_AMAB_CPU_OAM_ENp, CUPA_CPU_WRp(), _POWU_DMA_OAM_WRp);
  /* p28.YNYC*/ wire _YNYC_OAM_A_WRp = and2(_WYJA_OAM_WRp, _WAFO_OAM_A0n);
  /* p28.YLYC*/ wire _YLYC_OAM_B_WRp = and2(_WYJA_OAM_WRp, _GEKA_OAM_A0p);
  /* p28.ZOFE*/ wire _ZOFE_OAM_A_WRn = not1(_YNYC_OAM_A_WRp);
  /* p28.ZONE*/ wire _ZONE_OAM_B_WRn = not1(_YLYC_OAM_B_WRp);

  /*SIG_OAM_WRn_A*/ oam.SIG_OAM_WRn_A.sig_out(_ZOFE_OAM_A_WRn);
  /*SIG_OAM_WRn_B*/ oam.SIG_OAM_WRn_B.sig_out(_ZONE_OAM_B_WRn);

  /*#p28.AJEP*/ wire _AJEP_SCAN_OAM_LATCHn = nand2(sprite_scanner.ACYL_SCANNINGp.qp_new(), XOCE_xBCxxFGx()); // schematic wrong, is def nand2
  /* p28.WEFY*/ wire _WEFY_SPR_READp = and2(_TUVO_PPU_OAM_RDp, sprite_fetcher.TYFO_SFETCH_S0p_D1.qp_new());
  /*#p28.XUJA*/ wire _XUJA_SPR_OAM_LATCHn  = not1(_WEFY_SPR_READp);

  /*#p28.BOTA*/ wire _BOTA_OAM_OEn  = nand3(BOFE_LATCH_EXTn(), new_bus.SARO_ADDR_OAMp(), ASOT_CPU_RDp()); // Schematic wrong, this is NAND
  /*#p28.ASYT*/ wire _ASYT_OAM_OEn = and3(_AJEP_SCAN_OAM_LATCHn, _XUJA_SPR_OAM_LATCHn, _BOTA_OAM_OEn); // def and

  /*#p28.BODE*/ wire _BODE_OAM_OEp = not1(_ASYT_OAM_OEn);
  /*#p28.YVAL*/ wire _YVAL_OAM_OEn = not1(_BODE_OAM_OEp);
  /*#p28.YRYV*/ wire _YRYV_OAM_OEp = not1(_YVAL_OAM_OEn);
  /*#p28.ZODO*/ wire _ZODO_OAM_OEn = not1(_YRYV_OAM_OEp);   // schematic thinks this is OAM_CLK?
  /*SIG_OAM_OEn*/ oam.SIG_OAM_OEn.sig_out(_ZODO_OAM_OEn);

  uint8_t oam_addr   = (uint8_t)BitBase::pack_newn(7, (BitBase*)&oam_bus.BUS_OAM_A01n);
  uint8_t oam_data_a = (uint8_t)BitBase::pack_newn(8, (BitBase*)&oam_bus.BUS_OAM_DA00n);
  uint8_t oam_data_b = (uint8_t)BitBase::pack_newn(8, (BitBase*)&oam_bus.BUS_OAM_DB00n);

  if (bit(~oam.old_oam_clk.qp_old()) && bit(~oam.SIG_OAM_CLKn.qp_new())) {
    if (bit(~oam.SIG_OAM_WRn_A.qp_new())) oam_ram[(oam_addr << 1) + 0] = oam_data_a;
    if (bit(~oam.SIG_OAM_WRn_B.qp_new())) oam_ram[(oam_addr << 1) + 1] = oam_data_b;
  }

  oam_data_a = oam_ram[(oam_addr << 1) + 0];
  oam_data_b = oam_ram[(oam_addr << 1) + 1];

  oam.old_oam_clk = bit(~oam.SIG_OAM_CLKn.qp_new());

  oam_bus.BUS_OAM_DA00n.tri6_nn(_ZODO_OAM_OEn, (oam_data_a >> 0) & 1);
  oam_bus.BUS_OAM_DA01n.tri6_nn(_ZODO_OAM_OEn, (oam_data_a >> 1) & 1);
  oam_bus.BUS_OAM_DA02n.tri6_nn(_ZODO_OAM_OEn, (oam_data_a >> 2) & 1);
  oam_bus.BUS_OAM_DA03n.tri6_nn(_ZODO_OAM_OEn, (oam_data_a >> 3) & 1);
  oam_bus.BUS_OAM_DA04n.tri6_nn(_ZODO_OAM_OEn, (oam_data_a >> 4) & 1);
  oam_bus.BUS_OAM_DA05n.tri6_nn(_ZODO_OAM_OEn, (oam_data_a >> 5) & 1);
  oam_bus.BUS_OAM_DA06n.tri6_nn(_ZODO_OAM_OEn, (oam_data_a >> 6) & 1);
  oam_bus.BUS_OAM_DA07n.tri6_nn(_ZODO_OAM_OEn, (oam_data_a >> 7) & 1);

  oam_bus.BUS_OAM_DB00n.tri6_nn(_ZODO_OAM_OEn, (oam_data_b >> 0) & 1);
  oam_bus.BUS_OAM_DB01n.tri6_nn(_ZODO_OAM_OEn, (oam_data_b >> 1) & 1);
  oam_bus.BUS_OAM_DB02n.tri6_nn(_ZODO_OAM_OEn, (oam_data_b >> 2) & 1);
  oam_bus.BUS_OAM_DB03n.tri6_nn(_ZODO_OAM_OEn, (oam_data_b >> 3) & 1);
  oam_bus.BUS_OAM_DB04n.tri6_nn(_ZODO_OAM_OEn, (oam_data_b >> 4) & 1);
  oam_bus.BUS_OAM_DB05n.tri6_nn(_ZODO_OAM_OEn, (oam_data_b >> 5) & 1);
  oam_bus.BUS_OAM_DB06n.tri6_nn(_ZODO_OAM_OEn, (oam_data_b >> 6) & 1);
  oam_bus.BUS_OAM_DB07n.tri6_nn(_ZODO_OAM_OEn, (oam_data_b >> 7) & 1);

  /*#p29.YDYV*/ oam_latch_a.YDYV_OAM_LATCH_DA0n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DA00n.qp_new());
  /* p29.YCEB*/ oam_latch_a.YCEB_OAM_LATCH_DA1n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DA01n.qp_new());
  /* p29.ZUCA*/ oam_latch_a.ZUCA_OAM_LATCH_DA2n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DA02n.qp_new());
  /* p29.WONE*/ oam_latch_a.WONE_OAM_LATCH_DA3n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DA03n.qp_new());
  /* p29.ZAXE*/ oam_latch_a.ZAXE_OAM_LATCH_DA4n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DA04n.qp_new());
  /* p29.XAFU*/ oam_latch_a.XAFU_OAM_LATCH_DA5n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DA05n.qp_new());
  /* p29.YSES*/ oam_latch_a.YSES_OAM_LATCH_DA6n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DA06n.qp_new());
  /* p29.ZECA*/ oam_latch_a.ZECA_OAM_LATCH_DA7n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DA07n.qp_new());

  /*#p31.XYKY*/ oam_latch_b.XYKY_OAM_LATCH_DB0n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DB00n.qp_new());
  /* p31.YRUM*/ oam_latch_b.YRUM_OAM_LATCH_DB1n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DB01n.qp_new());
  /* p31.YSEX*/ oam_latch_b.YSEX_OAM_LATCH_DB2n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DB02n.qp_new());
  /* p31.YVEL*/ oam_latch_b.YVEL_OAM_LATCH_DB3n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DB03n.qp_new());
  /* p31.WYNO*/ oam_latch_b.WYNO_OAM_LATCH_DB4n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DB04n.qp_new());
  /* p31.CYRA*/ oam_latch_b.CYRA_OAM_LATCH_DB5n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DB05n.qp_new());
  /* p31.ZUVE*/ oam_latch_b.ZUVE_OAM_LATCH_DB6n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DB06n.qp_new());
  /* p31.ECED*/ oam_latch_b.ECED_OAM_LATCH_DB7n.tp_latchn(_BODE_OAM_OEp, oam_bus.BUS_OAM_DB07n.qp_new());

  /* p28.GUKO*/ wire _GUKO_OBL_TO_CBDp = and3(LEKO_CPU_RDp(), _AMAB_CPU_OAM_ENp, _WAFO_OAM_A0n);
  /* p28.WUME*/ wire _WUME_OBL_TO_CBDn = not1(_GUKO_OBL_TO_CBDp);
  /* p29.YFAP_OLA_TO_CBD0*/ new_bus.BUS_CPU_D00p.tri10_np(_WUME_OBL_TO_CBDn, oam_latch_a.YDYV_OAM_LATCH_DA0n.qn_new());
  /* p29.XELE_OLA_TO_CBD1*/ new_bus.BUS_CPU_D01p.tri10_np(_WUME_OBL_TO_CBDn, oam_latch_a.YCEB_OAM_LATCH_DA1n.qn_new());
  /* p29.YPON_OLA_TO_CBD2*/ new_bus.BUS_CPU_D02p.tri10_np(_WUME_OBL_TO_CBDn, oam_latch_a.ZUCA_OAM_LATCH_DA2n.qn_new());
  /* p29.XUVO_OLA_TO_CBD3*/ new_bus.BUS_CPU_D03p.tri10_np(_WUME_OBL_TO_CBDn, oam_latch_a.WONE_OAM_LATCH_DA3n.qn_new());
  /* p29.ZYSA_OLA_TO_CBD4*/ new_bus.BUS_CPU_D04p.tri10_np(_WUME_OBL_TO_CBDn, oam_latch_a.ZAXE_OAM_LATCH_DA4n.qn_new());
  /* p29.YWEG_OLA_TO_CBD5*/ new_bus.BUS_CPU_D05p.tri10_np(_WUME_OBL_TO_CBDn, oam_latch_a.XAFU_OAM_LATCH_DA5n.qn_new());
  /* p29.XABU_OLA_TO_CBD6*/ new_bus.BUS_CPU_D06p.tri10_np(_WUME_OBL_TO_CBDn, oam_latch_a.YSES_OAM_LATCH_DA6n.qn_new());
  /* p29.YTUX_OLA_TO_CBD7*/ new_bus.BUS_CPU_D07p.tri10_np(_WUME_OBL_TO_CBDn, oam_latch_a.ZECA_OAM_LATCH_DA7n.qn_new());

  /* p28.WUKU*/ wire _WUKU_OBL_TO_CBDp = and3(LEKO_CPU_RDp(), _AMAB_CPU_OAM_ENp, _GEKA_OAM_A0p);
  /* p28.WEWU*/ wire _WEWU_OBL_TO_CBDn = not1(_WUKU_OBL_TO_CBDp);
  /* p31.XACA_OLB_TO_CBD0*/ new_bus.BUS_CPU_D00p.tri10_np(_WEWU_OBL_TO_CBDn, oam_latch_b.XYKY_OAM_LATCH_DB0n.qn_new());
  /* p31.XAGU_OLB_TO_CBD1*/ new_bus.BUS_CPU_D01p.tri10_np(_WEWU_OBL_TO_CBDn, oam_latch_b.YRUM_OAM_LATCH_DB1n.qn_new());
  /* p31.XEPU_OLB_TO_CBD2*/ new_bus.BUS_CPU_D02p.tri10_np(_WEWU_OBL_TO_CBDn, oam_latch_b.YSEX_OAM_LATCH_DB2n.qn_new());
  /* p31.XYGU_OLB_TO_CBD3*/ new_bus.BUS_CPU_D03p.tri10_np(_WEWU_OBL_TO_CBDn, oam_latch_b.YVEL_OAM_LATCH_DB3n.qn_new());
  /* p31.XUNA_OLB_TO_CBD4*/ new_bus.BUS_CPU_D04p.tri10_np(_WEWU_OBL_TO_CBDn, oam_latch_b.WYNO_OAM_LATCH_DB4n.qn_new());
  /* p31.DEVE_OLB_TO_CBD5*/ new_bus.BUS_CPU_D05p.tri10_np(_WEWU_OBL_TO_CBDn, oam_latch_b.CYRA_OAM_LATCH_DB5n.qn_new());
  /* p31.ZEHA_OLB_TO_CBD6*/ new_bus.BUS_CPU_D06p.tri10_np(_WEWU_OBL_TO_CBDn, oam_latch_b.ZUVE_OAM_LATCH_DB6n.qn_new());
  /* p31.FYRA_OLB_TO_CBD7*/ new_bus.BUS_CPU_D07p.tri10_np(_WEWU_OBL_TO_CBDn, oam_latch_b.ECED_OAM_LATCH_DB7n.qn_new());
}

//------------------------------------------------------------------------------------------------------------------------
