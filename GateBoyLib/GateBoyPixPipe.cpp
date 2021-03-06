#include "GateBoyLib/GateBoyPixPipe.h"

#include "GateBoyLib/GateBoy.h"

// Sprite temp loads  on AB/CD/EF/GH
// Tile   temp loads  on AB/CD/EF/GH
// Pipes       shifts on HA/BC/DE/FG
// WUTY        fires  on HA/BC/DE/FG
// NYXU        fires  on HA/BC/DE/FG

//------------------------------------------------------------------------------------------------------------------------

void GateBoy::tock_window(wire SEGU_CLKPIPE_evn, wire REPU_VBLANKp) {
  /* p27.ROCO*/ wire _ROCO_CLKPIPE_odd = not1(SEGU_CLKPIPE_evn);

  /* p27.PYCO*/ win_reg.PYCO_WIN_MATCHp.dff17(_ROCO_CLKPIPE_odd, XAPO_VID_RSTn(), win_reg.NUKO_WX_MATCHp.qp_old());

  /*#p27.ROZE*/ wire ROZE_FINE_COUNT_7n_old = nand3(fine_scroll.RUBU_FINE_CNT2.qp_old(), fine_scroll.ROGA_FINE_CNT1.qp_old(), fine_scroll.RYKU_FINE_CNT0.qp_old());
  /* p27.PANY*/ wire PANY_WIN_FETCHn_old = nor2(win_reg.NUKO_WX_MATCHp.qp_old(), ROZE_FINE_COUNT_7n_old);

  /* p27.RENE*/ win_reg.RENE_WIN_FETCHn_B.dff17(ALET_xBxDxFxH(),  XYMU_RENDERINGn.qn_new(), win_reg.RYFA_WIN_FETCHn_A.qp_old());
  /* p27.RYFA*/ win_reg.RYFA_WIN_FETCHn_A.dff17(SEGU_CLKPIPE_evn, XYMU_RENDERINGn.qn_new(), PANY_WIN_FETCHn_old);

  /* p23.WEKO*/ wire _WEKO_FF4A_WRp = and2(CUPA_CPU_WRp(), new_bus.VYGA_FF4Ap());
  /* p23.VEFU*/ wire _VEFU_FF4A_WRn = not1(_WEKO_FF4A_WRp);
  /* p23.NESO*/ reg_wy.NESO_WY0n.dff9(_VEFU_FF4A_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D00p.qp_old());
  /* p23.NYRO*/ reg_wy.NYRO_WY1n.dff9(_VEFU_FF4A_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D01p.qp_old());
  /* p23.NAGA*/ reg_wy.NAGA_WY2n.dff9(_VEFU_FF4A_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D02p.qp_old());
  /* p23.MELA*/ reg_wy.MELA_WY3n.dff9(_VEFU_FF4A_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D03p.qp_old());
  /* p23.NULO*/ reg_wy.NULO_WY4n.dff9(_VEFU_FF4A_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D04p.qp_old());
  /* p23.NENE*/ reg_wy.NENE_WY5n.dff9(_VEFU_FF4A_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D05p.qp_old());
  /* p23.NUKA*/ reg_wy.NUKA_WY6n.dff9(_VEFU_FF4A_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D06p.qp_old());
  /* p23.NAFU*/ reg_wy.NAFU_WY7n.dff9(_VEFU_FF4A_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D07p.qp_old());

  /* p23.WUZA*/ wire _WUZA_FF4B_WRp = and2(CUPA_CPU_WRp(), new_bus.VUMY_FF4Bp());
  /* p23.VOXU*/ wire _VOXU_FF4B_WRn = not1(_WUZA_FF4B_WRp);
  /* p23.MYPA*/ reg_wx.MYPA_WX0n.dff9(_VOXU_FF4B_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D00p.qp_old());
  /* p23.NOFE*/ reg_wx.NOFE_WX1n.dff9(_VOXU_FF4B_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D01p.qp_old());
  /* p23.NOKE*/ reg_wx.NOKE_WX2n.dff9(_VOXU_FF4B_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D02p.qp_old());
  /* p23.MEBY*/ reg_wx.MEBY_WX3n.dff9(_VOXU_FF4B_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D03p.qp_old());
  /* p23.MYPU*/ reg_wx.MYPU_WX4n.dff9(_VOXU_FF4B_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D04p.qp_old());
  /* p23.MYCE*/ reg_wx.MYCE_WX5n.dff9(_VOXU_FF4B_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D05p.qp_old());
  /* p23.MUVO*/ reg_wx.MUVO_WX6n.dff9(_VOXU_FF4B_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D06p.qp_old());
  /* p23.NUKU*/ reg_wx.NUKU_WX7n.dff9(_VOXU_FF4B_WRn, WALU_SYS_RSTn(), old_bus.BUS_CPU_D07p.qp_old());

  /*#p27.NAZE*/ wire _NAZE_WY_MATCH0p = xnor2(reg_wy.NESO_WY0n.qn_new(), reg_ly.MUWY_LY0p.qp_new());
  /* p27.PEBO*/ wire _PEBO_WY_MATCH1p = xnor2(reg_wy.NYRO_WY1n.qn_new(), reg_ly.MYRO_LY1p.qp_new());
  /* p27.POMO*/ wire _POMO_WY_MATCH2p = xnor2(reg_wy.NAGA_WY2n.qn_new(), reg_ly.LEXA_LY2p.qp_new());
  /* p27.NEVU*/ wire _NEVU_WY_MATCH3p = xnor2(reg_wy.MELA_WY3n.qn_new(), reg_ly.LYDO_LY3p.qp_new());
  /* p27.NOJO*/ wire _NOJO_WY_MATCH4p = xnor2(reg_wy.NULO_WY4n.qn_new(), reg_ly.LOVU_LY4p.qp_new());
  /* p27.PAGA*/ wire _PAGA_WY_MATCH5p = xnor2(reg_wy.NENE_WY5n.qn_new(), reg_ly.LEMA_LY5p.qp_new());
  /* p27.PEZO*/ wire _PEZO_WY_MATCH6p = xnor2(reg_wy.NUKA_WY6n.qn_new(), reg_ly.MATO_LY6p.qp_new());
  /* p27.NUPA*/ wire _NUPA_WY_MATCH7p = xnor2(reg_wy.NAFU_WY7n.qn_new(), reg_ly.LAFO_LY7p.qp_new());

  /*#p27.MYLO*/ wire _MYLO_WX_MATCH0p = xnor2(pix_count.XEHO_PX0p.qp_new(), reg_wx.MYPA_WX0n.qn_new());
  /* p27.PUWU*/ wire _PUWU_WX_MATCH1p = xnor2(pix_count.SAVY_PX1p.qp_new(), reg_wx.NOFE_WX1n.qn_new());
  /* p27.PUHO*/ wire _PUHO_WX_MATCH2p = xnor2(pix_count.XODU_PX2p.qp_new(), reg_wx.NOKE_WX2n.qn_new());
  /* p27.NYTU*/ wire _NYTU_WX_MATCH3p = xnor2(pix_count.XYDO_PX3p.qp_new(), reg_wx.MEBY_WX3n.qn_new());
  /* p27.NEZO*/ wire _NEZO_WX_MATCH4p = xnor2(pix_count.TUHU_PX4p.qp_new(), reg_wx.MYPU_WX4n.qn_new());
  /* p27.NORY*/ wire _NORY_WX_MATCH5p = xnor2(pix_count.TUKY_PX5p.qp_new(), reg_wx.MYCE_WX5n.qn_new());
  /* p27.NONO*/ wire _NONO_WX_MATCH6p = xnor2(pix_count.TAKO_PX6p.qp_new(), reg_wx.MUVO_WX6n.qn_new());
  /* p27.PASE*/ wire _PASE_WX_MATCH7p = xnor2(pix_count.SYBE_PX7p.qp_new(), reg_wx.NUKU_WX7n.qn_new());

  /*#p27.PALO*/ wire _PALO_WY_MATCHn  = nand5(reg_lcdc.WYMO_LCDC_WINENn.qn_new(), _NOJO_WY_MATCH4p, _PAGA_WY_MATCH5p, _PEZO_WY_MATCH6p, _NUPA_WY_MATCH7p);
  /* p27.NELE*/ wire _NELE_WY_MATCHp  = not1(_PALO_WY_MATCHn);
  /* p27.PAFU*/ wire _PAFU_WY_MATCHn  = nand5(_NELE_WY_MATCHp, _NAZE_WY_MATCH0p, _PEBO_WY_MATCH1p, _POMO_WY_MATCH2p, _NEVU_WY_MATCH3p);

  /* p27.SARY*/ win_reg.SARY_WY_MATCHp.dff17(TALU_xxCDEFxx(), XAPO_VID_RSTn(), win_reg.ROGE_WY_MATCHp.qp_old());
  /* p27.ROGE*/ win_reg.ROGE_WY_MATCHp = not1(_PAFU_WY_MATCHn);
  /* p27.REJO*/ win_reg.REJO_WY_MATCH_LATCHp.nor_latch(win_reg.SARY_WY_MATCHp.qp_new(), REPU_VBLANKp);

  /* p27.PUKY*/ wire _PUKY_WX_MATCHn  = nand5(win_reg.REJO_WY_MATCH_LATCHp.qp_new(), _NEZO_WX_MATCH4p, _NORY_WX_MATCH5p, _NONO_WX_MATCH6p, _PASE_WX_MATCH7p);
  /* p27.NUFA*/ wire _NUFA_WX_MATCHp  = not1(_PUKY_WX_MATCHn);
  /* p27.NOGY*/ wire _NOGY_WX_MATCHn  = nand5(_NUFA_WX_MATCHp, _MYLO_WX_MATCH0p, _PUWU_WX_MATCH1p, _PUHO_WX_MATCH2p, _NYTU_WX_MATCH3p);
  /* p27.NUKO*/ win_reg.NUKO_WX_MATCHp  = not1(_NOGY_WX_MATCHn);

  /* p23.WAXU*/ wire _WAXU_FF4A_RDp = and2(ASOT_CPU_RDp(), new_bus.VYGA_FF4Ap());
  /* p23.VOMY*/ wire _VOMY_FF4A_RDn = not1(_WAXU_FF4A_RDp);
  /*#p23.PUNU_WY0_TO_CD0*/ new_bus.BUS_CPU_D00p.tri6_nn(_VOMY_FF4A_RDn, reg_wy.NESO_WY0n.qp_new());
  /* p23.PODA_WY1_TO_CD1*/ new_bus.BUS_CPU_D01p.tri6_nn(_VOMY_FF4A_RDn, reg_wy.NYRO_WY1n.qp_new());
  /* p23.PYGU_WY2_TO_CD2*/ new_bus.BUS_CPU_D02p.tri6_nn(_VOMY_FF4A_RDn, reg_wy.NAGA_WY2n.qp_new());
  /* p23.LOKA_WY3_TO_CD3*/ new_bus.BUS_CPU_D03p.tri6_nn(_VOMY_FF4A_RDn, reg_wy.MELA_WY3n.qp_new());
  /* p23.MEGA_WY4_TO_CD4*/ new_bus.BUS_CPU_D04p.tri6_nn(_VOMY_FF4A_RDn, reg_wy.NULO_WY4n.qp_new());
  /* p23.PELA_WY5_TO_CD5*/ new_bus.BUS_CPU_D05p.tri6_nn(_VOMY_FF4A_RDn, reg_wy.NENE_WY5n.qp_new());
  /* p23.POLO_WY6_TO_CD6*/ new_bus.BUS_CPU_D06p.tri6_nn(_VOMY_FF4A_RDn, reg_wy.NUKA_WY6n.qp_new());
  /* p23.MERA_WY7_TO_CD7*/ new_bus.BUS_CPU_D07p.tri6_nn(_VOMY_FF4A_RDn, reg_wy.NAFU_WY7n.qp_new());

  /* p23.WYZE*/ wire _WYZE_FF4B_RDp = and2(ASOT_CPU_RDp(), new_bus.VUMY_FF4Bp());
  /* p23.VYCU*/ wire _VYCU_FF4B_RDn = not1(_WYZE_FF4B_RDp);
  /*#p23.LOVA_WX0_TO_CD0*/ new_bus.BUS_CPU_D00p.tri6_nn(_VYCU_FF4B_RDn, reg_wx.MYPA_WX0n.qp_new());
  /* p23.MUKA_WX1_TO_CD1*/ new_bus.BUS_CPU_D01p.tri6_nn(_VYCU_FF4B_RDn, reg_wx.NOFE_WX1n.qp_new());
  /* p23.MOKO_WX2_TO_CD2*/ new_bus.BUS_CPU_D02p.tri6_nn(_VYCU_FF4B_RDn, reg_wx.NOKE_WX2n.qp_new());
  /* p23.LOLE_WX3_TO_CD3*/ new_bus.BUS_CPU_D03p.tri6_nn(_VYCU_FF4B_RDn, reg_wx.MEBY_WX3n.qp_new());
  /* p23.MELE_WX4_TO_CD4*/ new_bus.BUS_CPU_D04p.tri6_nn(_VYCU_FF4B_RDn, reg_wx.MYPU_WX4n.qp_new());
  /* p23.MUFE_WX5_TO_CD5*/ new_bus.BUS_CPU_D05p.tri6_nn(_VYCU_FF4B_RDn, reg_wx.MYCE_WX5n.qp_new());
  /* p23.MULY_WX6_TO_CD6*/ new_bus.BUS_CPU_D06p.tri6_nn(_VYCU_FF4B_RDn, reg_wx.MUVO_WX6n.qp_new());
  /* p23.MARA_WX7_TO_CD7*/ new_bus.BUS_CPU_D07p.tri6_nn(_VYCU_FF4B_RDn, reg_wx.NUKU_WX7n.qp_new());
}

//------------------------------------------------------------------------------------------------------------------------

void GateBoy::tock_pix_pipes(wire SACU_CLKPIPE_evn, wire NYXU_BFETCH_RSTn)
{
  /* p24.LOBY*/ wire _LOBY_RENDERINGn = not1(XYMU_RENDERINGn.qn_new());

  /*#p27.LAXE*/ wire _LAXE_BFETCH_S0n = not1(tile_fetcher._LAXU_BFETCH_S0p.qp_new());
  /*#p27.NAKO*/ wire _NAKO_BFETCH_S1n = not1(tile_fetcher._MESU_BFETCH_S1p.qp_new());
  /*#p27.NOFU*/ wire _NOFU_BFETCH_S2n = not1(tile_fetcher._NYVA_BFETCH_S2p.qp_new());
  /* p29.TYTU*/ wire _TYTU_SFETCH_S0n = not1(sprite_fetcher.TOXE_SFETCH_S0p.qp_new());
  /* p29.SYCU*/ wire _SYCU_SFETCH_S0pe = nor3(_TYTU_SFETCH_S0n, _LOBY_RENDERINGn, sprite_fetcher.TYFO_SFETCH_S0p_D1.qp_new());

  /*#p27.MYSO*/ wire _MYSO_STORE_VRAM_DATA_TRIGp = nor3(_LOBY_RENDERINGn, _LAXE_BFETCH_S0n, tile_fetcher.LYZU_BFETCH_S0p_D1.qp_new()); // MYSO fires on fetch phase 2, 6, 10

  /*#p27.NYDY*/ wire _NYDY_LATCH_TILE_DAn = nand3(_MYSO_STORE_VRAM_DATA_TRIGp, tile_fetcher._MESU_BFETCH_S1p.qp_new(), _NOFU_BFETCH_S2n); // NYDY on fetch phase 6
  /* p27.MOFU*/ wire _MOFU_LATCH_TILE_DBp = and2(_MYSO_STORE_VRAM_DATA_TRIGp, _NAKO_BFETCH_S1n); // MOFU fires on fetch phase 2 and 10
  /*#p29.TOPU*/ wire _TOPU_STORE_SPRITE_Ap = and2(sprite_fetcher.TULY_SFETCH_S1p.qp_new(), _SYCU_SFETCH_S0pe);
  /*#p29.RACA*/ wire _RACA_STORE_SPRITE_Bp = and2(sprite_fetcher.VONU_SFETCH_S1p_D4.qp_new(), _SYCU_SFETCH_S0pe);

  {
    /*#p32.METE*/ wire _METE_LATCH_TILE_DAp = not1(_NYDY_LATCH_TILE_DAn);
    /*#p32.LOMA*/ wire _LOMA_LATCH_TILE_DAn = not1(_METE_LATCH_TILE_DAp);
    /* p32.LEGU*/ tile_temp_a.LEGU_TILE_DA0n.dff8p(_LOMA_LATCH_TILE_DAn, vram_bus.BUS_VRAM_D00p.qp_old());
    /* p32.NUDU*/ tile_temp_a.NUDU_TILE_DA1n.dff8p(_LOMA_LATCH_TILE_DAn, vram_bus.BUS_VRAM_D01p.qp_old());
    /* p32.MUKU*/ tile_temp_a.MUKU_TILE_DA2n.dff8p(_LOMA_LATCH_TILE_DAn, vram_bus.BUS_VRAM_D02p.qp_old());
    /* p32.LUZO*/ tile_temp_a.LUZO_TILE_DA3n.dff8p(_LOMA_LATCH_TILE_DAn, vram_bus.BUS_VRAM_D03p.qp_old());
    /* p32.MEGU*/ tile_temp_a.MEGU_TILE_DA4n.dff8p(_LOMA_LATCH_TILE_DAn, vram_bus.BUS_VRAM_D04p.qp_old());
    /* p32.MYJY*/ tile_temp_a.MYJY_TILE_DA5n.dff8p(_LOMA_LATCH_TILE_DAn, vram_bus.BUS_VRAM_D05p.qp_old());
    /* p32.NASA*/ tile_temp_a.NASA_TILE_DA6n.dff8p(_LOMA_LATCH_TILE_DAn, vram_bus.BUS_VRAM_D06p.qp_old());
    /* p32.NEFO*/ tile_temp_a.NEFO_TILE_DA7n.dff8p(_LOMA_LATCH_TILE_DAn, vram_bus.BUS_VRAM_D07p.qp_old());
  }

  {
    // This is the only block of "dff11" on the chip. Not sure about clock polarity, it seems to work either way.

    /* p32.LESO*/ wire _LESO_LATCH_TILE_DBn = not1(_MOFU_LATCH_TILE_DBp);
    /* p32.LUVE*/ wire _LUVE_LATCH_TILE_DBp = not1(_LESO_LATCH_TILE_DBn); // Schematic wrong, was labeled AJAR
    /* p32.LABU*/ wire _LABU_LATCH_TILE_DBn = not1(_LUVE_LATCH_TILE_DBp);
    /* p32.RAWU*/ tile_temp_b.RAWU_TILE_DB0p.dff11(_LABU_LATCH_TILE_DBn, SIG_VCC.qp_new(), vram_bus.BUS_VRAM_D00p.qp_old());
    /* p32.POZO*/ tile_temp_b.POZO_TILE_DB1p.dff11(_LABU_LATCH_TILE_DBn, SIG_VCC.qp_new(), vram_bus.BUS_VRAM_D01p.qp_old());
    /* p32.PYZO*/ tile_temp_b.PYZO_TILE_DB2p.dff11(_LABU_LATCH_TILE_DBn, SIG_VCC.qp_new(), vram_bus.BUS_VRAM_D02p.qp_old());
    /* p32.POXA*/ tile_temp_b.POXA_TILE_DB3p.dff11(_LABU_LATCH_TILE_DBn, SIG_VCC.qp_new(), vram_bus.BUS_VRAM_D03p.qp_old());
    /* p32.PULO*/ tile_temp_b.PULO_TILE_DB4p.dff11(_LABU_LATCH_TILE_DBn, SIG_VCC.qp_new(), vram_bus.BUS_VRAM_D04p.qp_old());
    /* p32.POJU*/ tile_temp_b.POJU_TILE_DB5p.dff11(_LABU_LATCH_TILE_DBn, SIG_VCC.qp_new(), vram_bus.BUS_VRAM_D05p.qp_old());
    /* p32.POWY*/ tile_temp_b.POWY_TILE_DB6p.dff11(_LABU_LATCH_TILE_DBn, SIG_VCC.qp_new(), vram_bus.BUS_VRAM_D06p.qp_old());
    /* p32.PYJU*/ tile_temp_b.PYJU_TILE_DB7p.dff11(_LABU_LATCH_TILE_DBn, SIG_VCC.qp_new(), vram_bus.BUS_VRAM_D07p.qp_old());
  }

  {
    /*#p29.VYWA*/ wire _VYWA_STORE_SPRITE_An = not1(_TOPU_STORE_SPRITE_Ap);
    /*#p29.WENY*/ wire _WENY_STORE_SPRITE_Ap = not1(_VYWA_STORE_SPRITE_An);
    /*#p29.XADO*/ wire _XADO_STORE_SPRITE_An = not1(_WENY_STORE_SPRITE_Ap);
    /* p33.REWO*/ sprite_pix_a.REWO_SPRITE_DA0n.dff8n(_XADO_STORE_SPRITE_An, flipped_sprite.PUTE_FLIP0p.qp_old());
    /* p33.PEBA*/ sprite_pix_a.PEBA_SPRITE_DA1n.dff8n(_XADO_STORE_SPRITE_An, flipped_sprite.PELO_FLIP1p.qp_old());
    /* p33.MOFO*/ sprite_pix_a.MOFO_SPRITE_DA2n.dff8n(_XADO_STORE_SPRITE_An, flipped_sprite.PONO_FLIP2p.qp_old());
    /* p33.PUDU*/ sprite_pix_a.PUDU_SPRITE_DA3n.dff8n(_XADO_STORE_SPRITE_An, flipped_sprite.POBE_FLIP3p.qp_old());
    /* p33.SAJA*/ sprite_pix_a.SAJA_SPRITE_DA4n.dff8n(_XADO_STORE_SPRITE_An, flipped_sprite.PACY_FLIP4p.qp_old());
    /* p33.SUNY*/ sprite_pix_a.SUNY_SPRITE_DA5n.dff8n(_XADO_STORE_SPRITE_An, flipped_sprite.PUGU_FLIP5p.qp_old());
    /* p33.SEMO*/ sprite_pix_a.SEMO_SPRITE_DA6n.dff8n(_XADO_STORE_SPRITE_An, flipped_sprite.PAWE_FLIP6p.qp_old());
    /* p33.SEGA*/ sprite_pix_a.SEGA_SPRITE_DA7n.dff8n(_XADO_STORE_SPRITE_An, flipped_sprite.PULY_FLIP7p.qp_old());
  }

  {
    /*#p29.PEBY*/ wire _PEBY_STORE_SPRITE_Bn = not1(_RACA_STORE_SPRITE_Bp);
    /*#p29.NYBE*/ wire _NYBE_STORE_SPRITE_Bp = not1(_PEBY_STORE_SPRITE_Bn);
    /*#p29.PUCO*/ wire _PUCO_STORE_SPRITE_Bn = not1(_NYBE_STORE_SPRITE_Bp);
    /* p33.PEFO*/ sprite_pix_b.PEFO_SPRITE_DB0n.dff8n(_PUCO_STORE_SPRITE_Bn, flipped_sprite.PUTE_FLIP0p.qp_old());
    /* p33.ROKA*/ sprite_pix_b.ROKA_SPRITE_DB1n.dff8n(_PUCO_STORE_SPRITE_Bn, flipped_sprite.PELO_FLIP1p.qp_old());
    /* p33.MYTU*/ sprite_pix_b.MYTU_SPRITE_DB2n.dff8n(_PUCO_STORE_SPRITE_Bn, flipped_sprite.PONO_FLIP2p.qp_old());
    /* p33.RAMU*/ sprite_pix_b.RAMU_SPRITE_DB3n.dff8n(_PUCO_STORE_SPRITE_Bn, flipped_sprite.POBE_FLIP3p.qp_old());
    /* p33.SELE*/ sprite_pix_b.SELE_SPRITE_DB4n.dff8n(_PUCO_STORE_SPRITE_Bn, flipped_sprite.PACY_FLIP4p.qp_old());
    /* p33.SUTO*/ sprite_pix_b.SUTO_SPRITE_DB5n.dff8n(_PUCO_STORE_SPRITE_Bn, flipped_sprite.PUGU_FLIP5p.qp_old());
    /* p33.RAMA*/ sprite_pix_b.RAMA_SPRITE_DB6n.dff8n(_PUCO_STORE_SPRITE_Bn, flipped_sprite.PAWE_FLIP6p.qp_old());
    /* p33.RYDU*/ sprite_pix_b.RYDU_SPRITE_DB7n.dff8n(_PUCO_STORE_SPRITE_Bn, flipped_sprite.PULY_FLIP7p.qp_old());
  }

  /* p29.XEFY*/ wire _XEFY_SPRITE_DONEn  = not1(sprite_fetcher.WUTY_SFETCH_DONE_TRIGp.qp_new());

  // FIXME - old? well,i guess there's another feedback loop here...

  /* p34.MEFU*/ wire _MEFU_SPRITE_MASK0n = or3(_XEFY_SPRITE_DONEn, pix_pipes.NYLU_SPR_PIPE_B0.qp_old(), pix_pipes.NURO_SPR_PIPE_A0.qp_old());
  /* p34.MEVE*/ wire _MEVE_SPRITE_MASK1n = or3(_XEFY_SPRITE_DONEn, pix_pipes.PEFU_SPR_PIPE_B1.qp_old(), pix_pipes.MASO_SPR_PIPE_A1.qp_old());
  /* p34.MYZO*/ wire _MYZO_SPRITE_MASK2n = or3(_XEFY_SPRITE_DONEn, pix_pipes.NATY_SPR_PIPE_B2.qp_old(), pix_pipes.LEFE_SPR_PIPE_A2.qp_old());
  /* p34.RUDA*/ wire _RUDA_SPRITE_MASK3n = or3(_XEFY_SPRITE_DONEn, pix_pipes.PYJO_SPR_PIPE_B3.qp_old(), pix_pipes.LESU_SPR_PIPE_A3.qp_old());
  /* p34.VOTO*/ wire _VOTO_SPRITE_MASK4n = or3(_XEFY_SPRITE_DONEn, pix_pipes.VARE_SPR_PIPE_B4.qp_old(), pix_pipes.WYHO_SPR_PIPE_A4.qp_old());
  /* p34.VYSA*/ wire _VYSA_SPRITE_MASK5n = or3(_XEFY_SPRITE_DONEn, pix_pipes.WEBA_SPR_PIPE_B5.qp_old(), pix_pipes.WORA_SPR_PIPE_A5.qp_old());
  /* p34.TORY*/ wire _TORY_SPRITE_MASK6n = or3(_XEFY_SPRITE_DONEn, pix_pipes.VANU_SPR_PIPE_B6.qp_old(), pix_pipes.VAFO_SPR_PIPE_A6.qp_old());
  /* p34.WOPE*/ wire _WOPE_SPRITE_MASK7n = or3(_XEFY_SPRITE_DONEn, pix_pipes.VUPY_SPR_PIPE_B7.qp_old(), pix_pipes.WUFY_SPR_PIPE_A7.qp_old());

  /* p34.LESY*/ wire _LESY_SPRITE_MASK0p = not1(_MEFU_SPRITE_MASK0n);
  /* p34.LOTA*/ wire _LOTA_SPRITE_MASK1p = not1(_MEVE_SPRITE_MASK1n);
  /* p34.LYKU*/ wire _LYKU_SPRITE_MASK2p = not1(_MYZO_SPRITE_MASK2n);
  /* p34.ROBY*/ wire _ROBY_SPRITE_MASK3p = not1(_RUDA_SPRITE_MASK3n);
  /* p34.TYTA*/ wire _TYTA_SPRITE_MASK4p = not1(_VOTO_SPRITE_MASK4n);
  /* p34.TYCO*/ wire _TYCO_SPRITE_MASK5p = not1(_VYSA_SPRITE_MASK5n);
  /* p34.SOKA*/ wire _SOKA_SPRITE_MASK6p = not1(_TORY_SPRITE_MASK6n);
  /* p34.XOVU*/ wire _XOVU_SPRITE_MASK7p = not1(_WOPE_SPRITE_MASK7n);

  //----------------------------------------
  // Sprite pipe A
  {
    /* p33.PABE*/ wire _PABE_SPR_PIX_SET0 = nand2(_LESY_SPRITE_MASK0p, sprite_pix_a.REWO_SPRITE_DA0n.qn_new());
    /* p33.MYTO*/ wire _MYTO_SPR_PIX_SET1 = nand2(_LOTA_SPRITE_MASK1p, sprite_pix_a.PEBA_SPRITE_DA1n.qn_new());
    /* p33.LELA*/ wire _LELA_SPR_PIX_SET2 = nand2(_LYKU_SPRITE_MASK2p, sprite_pix_a.MOFO_SPRITE_DA2n.qn_new());
    /* p33.MAME*/ wire _MAME_SPR_PIX_SET3 = nand2(_ROBY_SPRITE_MASK3p, sprite_pix_a.PUDU_SPRITE_DA3n.qn_new());
    /* p33.VEXU*/ wire _VEXU_SPR_PIX_SET4 = nand2(_TYTA_SPRITE_MASK4p, sprite_pix_a.SAJA_SPRITE_DA4n.qn_new());
    /* p33.VABY*/ wire _VABY_SPR_PIX_SET5 = nand2(_TYCO_SPRITE_MASK5p, sprite_pix_a.SUNY_SPRITE_DA5n.qn_new());
    /* p33.TUXA*/ wire _TUXA_SPR_PIX_SET6 = nand2(_SOKA_SPRITE_MASK6p, sprite_pix_a.SEMO_SPRITE_DA6n.qn_new());
    /* p33.VUNE*/ wire _VUNE_SPR_PIX_SET7 = nand2(_XOVU_SPRITE_MASK7p, sprite_pix_a.SEGA_SPRITE_DA7n.qn_new());

    /* p33.RATA*/ wire _RATA_SPR_PIX_DA0n = not1(sprite_pix_a.REWO_SPRITE_DA0n.qn_new());
    /* p33.NUCA*/ wire _NUCA_SPR_PIX_DA1n = not1(sprite_pix_a.PEBA_SPRITE_DA1n.qn_new());
    /* p33.LASE*/ wire _LASE_SPR_PIX_DA2n = not1(sprite_pix_a.MOFO_SPRITE_DA2n.qn_new());
    /* p33.LUBO*/ wire _LUBO_SPR_PIX_DA3n = not1(sprite_pix_a.PUDU_SPRITE_DA3n.qn_new());
    /* p33.WERY*/ wire _WERY_SPR_PIX_DA4n = not1(sprite_pix_a.SAJA_SPRITE_DA4n.qn_new());
    /* p33.WURA*/ wire _WURA_SPR_PIX_DA5n = not1(sprite_pix_a.SUNY_SPRITE_DA5n.qn_new());
    /* p33.SULU*/ wire _SULU_SPR_PIX_DA6n = not1(sprite_pix_a.SEMO_SPRITE_DA6n.qn_new());
    /* p33.WAMY*/ wire _WAMY_SPR_PIX_DA7n = not1(sprite_pix_a.SEGA_SPRITE_DA7n.qn_new());

    /* p33.PYZU*/ wire _PYZU_SPR_PIX_RST0 = nand2(_LESY_SPRITE_MASK0p, _RATA_SPR_PIX_DA0n);
    /* p33.MADA*/ wire _MADA_SPR_PIX_RST1 = nand2(_LOTA_SPRITE_MASK1p, _NUCA_SPR_PIX_DA1n);
    /* p33.LYDE*/ wire _LYDE_SPR_PIX_RST2 = nand2(_LYKU_SPRITE_MASK2p, _LASE_SPR_PIX_DA2n);
    /* p33.LUFY*/ wire _LUFY_SPR_PIX_RST3 = nand2(_ROBY_SPRITE_MASK3p, _LUBO_SPR_PIX_DA3n);
    /* p33.XATO*/ wire _XATO_SPR_PIX_RST4 = nand2(_TYTA_SPRITE_MASK4p, _WERY_SPR_PIX_DA4n);
    /* p33.XEXU*/ wire _XEXU_SPR_PIX_RST5 = nand2(_TYCO_SPRITE_MASK5p, _WURA_SPR_PIX_DA5n);
    /* p33.TUPE*/ wire _TUPE_SPR_PIX_RST6 = nand2(_SOKA_SPRITE_MASK6p, _SULU_SPR_PIX_DA6n);
    /* p33.XYVE*/ wire _XYVE_SPR_PIX_RST7 = nand2(_XOVU_SPRITE_MASK7p, _WAMY_SPR_PIX_DA7n);

    /* p33.WUFY*/ pix_pipes.WUFY_SPR_PIPE_A7.dff22(SACU_CLKPIPE_evn, _VUNE_SPR_PIX_SET7, _XYVE_SPR_PIX_RST7, pix_pipes.VAFO_SPR_PIPE_A6.qp_old());
    /* p33.VAFO*/ pix_pipes.VAFO_SPR_PIPE_A6.dff22(SACU_CLKPIPE_evn, _TUXA_SPR_PIX_SET6, _TUPE_SPR_PIX_RST6, pix_pipes.WORA_SPR_PIPE_A5.qp_old());
    /* p33.WORA*/ pix_pipes.WORA_SPR_PIPE_A5.dff22(SACU_CLKPIPE_evn, _VABY_SPR_PIX_SET5, _XEXU_SPR_PIX_RST5, pix_pipes.WYHO_SPR_PIPE_A4.qp_old());
    /* p33.WYHO*/ pix_pipes.WYHO_SPR_PIPE_A4.dff22(SACU_CLKPIPE_evn, _VEXU_SPR_PIX_SET4, _XATO_SPR_PIX_RST4, pix_pipes.LESU_SPR_PIPE_A3.qp_old());
    /* p33.LESU*/ pix_pipes.LESU_SPR_PIPE_A3.dff22(SACU_CLKPIPE_evn, _MAME_SPR_PIX_SET3, _LUFY_SPR_PIX_RST3, pix_pipes.LEFE_SPR_PIPE_A2.qp_old());
    /* p33.LEFE*/ pix_pipes.LEFE_SPR_PIPE_A2.dff22(SACU_CLKPIPE_evn, _LELA_SPR_PIX_SET2, _LYDE_SPR_PIX_RST2, pix_pipes.MASO_SPR_PIPE_A1.qp_old());
    /* p33.MASO*/ pix_pipes.MASO_SPR_PIPE_A1.dff22(SACU_CLKPIPE_evn, _MYTO_SPR_PIX_SET1, _MADA_SPR_PIX_RST1, pix_pipes.NURO_SPR_PIPE_A0.qp_old());
    /* p33.NURO*/ pix_pipes.NURO_SPR_PIPE_A0.dff22(SACU_CLKPIPE_evn, _PABE_SPR_PIX_SET0, _PYZU_SPR_PIX_RST0, SIG_GND.qp_new());
  }

  //----------------------------------------
  // Sprite pipe B
  {
    /* p33.MEZU*/ wire _MEZU_SPR_PIX_SET0 = nand2(_LESY_SPRITE_MASK0p, sprite_pix_b.PEFO_SPRITE_DB0n.qn_new());
    /* p33.RUSY*/ wire _RUSY_SPR_PIX_SET1 = nand2(_LOTA_SPRITE_MASK1p, sprite_pix_b.ROKA_SPRITE_DB1n.qn_new());
    /* p33.MYXA*/ wire _MYXA_SPR_PIX_SET2 = nand2(_LYKU_SPRITE_MASK2p, sprite_pix_b.MYTU_SPRITE_DB2n.qn_new());
    /* p33.RANO*/ wire _RANO_SPR_PIX_SET3 = nand2(_ROBY_SPRITE_MASK3p, sprite_pix_b.RAMU_SPRITE_DB3n.qn_new());
    /* p33.TYGA*/ wire _TYGA_SPR_PIX_SET4 = nand2(_TYTA_SPRITE_MASK4p, sprite_pix_b.SELE_SPRITE_DB4n.qn_new());
    /* p33.VUME*/ wire _VUME_SPR_PIX_SET5 = nand2(_TYCO_SPRITE_MASK5p, sprite_pix_b.SUTO_SPRITE_DB5n.qn_new());
    /* p33.TAPO*/ wire _TAPO_SPR_PIX_SET6 = nand2(_SOKA_SPRITE_MASK6p, sprite_pix_b.RAMA_SPRITE_DB6n.qn_new());
    /* p33.TESO*/ wire _TESO_SPR_PIX_SET7 = nand2(_XOVU_SPRITE_MASK7p, sprite_pix_b.RYDU_SPRITE_DB7n.qn_new());

    /* p33.LOZA*/ wire _LOZA_SPR_PIX_DB0n = not1(sprite_pix_b.PEFO_SPRITE_DB0n.qn_new());
    /* p33.SYBO*/ wire _SYBO_SPR_PIX_DB1n = not1(sprite_pix_b.ROKA_SPRITE_DB1n.qn_new());
    /* p33.LUMO*/ wire _LUMO_SPR_PIX_DB2n = not1(sprite_pix_b.MYTU_SPRITE_DB2n.qn_new());
    /* p33.SOLO*/ wire _SOLO_SPR_PIX_DB3n = not1(sprite_pix_b.RAMU_SPRITE_DB3n.qn_new());
    /* p33.VOBY*/ wire _VOBY_SPR_PIX_DB4n = not1(sprite_pix_b.SELE_SPRITE_DB4n.qn_new());
    /* p33.WYCO*/ wire _WYCO_SPR_PIX_DB5n = not1(sprite_pix_b.SUTO_SPRITE_DB5n.qn_new());
    /* p33.SERY*/ wire _SERY_SPR_PIX_DB6n = not1(sprite_pix_b.RAMA_SPRITE_DB6n.qn_new());
    /* p33.SELU*/ wire _SELU_SPR_PIX_DB7n = not1(sprite_pix_b.RYDU_SPRITE_DB7n.qn_new());

    /* p33.MOFY*/ wire _MOFY_SPR_PIX_RST0 = nand2(_LESY_SPRITE_MASK0p, _LOZA_SPR_PIX_DB0n);
    /* p33.RUCA*/ wire _RUCA_SPR_PIX_RST1 = nand2(_LOTA_SPRITE_MASK1p, _SYBO_SPR_PIX_DB1n);
    /* p33.MAJO*/ wire _MAJO_SPR_PIX_RST2 = nand2(_LYKU_SPRITE_MASK2p, _LUMO_SPR_PIX_DB2n);
    /* p33.REHU*/ wire _REHU_SPR_PIX_RST3 = nand2(_ROBY_SPRITE_MASK3p, _SOLO_SPR_PIX_DB3n);
    /* p33.WAXO*/ wire _WAXO_SPR_PIX_RST4 = nand2(_TYTA_SPRITE_MASK4p, _VOBY_SPR_PIX_DB4n);
    /* p33.XOLE*/ wire _XOLE_SPR_PIX_RST5 = nand2(_TYCO_SPRITE_MASK5p, _WYCO_SPR_PIX_DB5n);
    /* p33.TABY*/ wire _TABY_SPR_PIX_RST6 = nand2(_SOKA_SPRITE_MASK6p, _SERY_SPR_PIX_DB6n);
    /* p33.TULA*/ wire _TULA_SPR_PIX_RST7 = nand2(_XOVU_SPRITE_MASK7p, _SELU_SPR_PIX_DB7n);

    /* p33.VUPY*/ pix_pipes.VUPY_SPR_PIPE_B7.dff22(SACU_CLKPIPE_evn, _TESO_SPR_PIX_SET7, _TULA_SPR_PIX_RST7, pix_pipes.VANU_SPR_PIPE_B6.qp_old());
    /* p33.VANU*/ pix_pipes.VANU_SPR_PIPE_B6.dff22(SACU_CLKPIPE_evn, _TAPO_SPR_PIX_SET6, _TABY_SPR_PIX_RST6, pix_pipes.WEBA_SPR_PIPE_B5.qp_old());
    /* p33.WEBA*/ pix_pipes.WEBA_SPR_PIPE_B5.dff22(SACU_CLKPIPE_evn, _VUME_SPR_PIX_SET5, _XOLE_SPR_PIX_RST5, pix_pipes.VARE_SPR_PIPE_B4.qp_old());
    /* p33.VARE*/ pix_pipes.VARE_SPR_PIPE_B4.dff22(SACU_CLKPIPE_evn, _TYGA_SPR_PIX_SET4, _WAXO_SPR_PIX_RST4, pix_pipes.PYJO_SPR_PIPE_B3.qp_old());
    /* p33.PYJO*/ pix_pipes.PYJO_SPR_PIPE_B3.dff22(SACU_CLKPIPE_evn, _RANO_SPR_PIX_SET3, _REHU_SPR_PIX_RST3, pix_pipes.NATY_SPR_PIPE_B2.qp_old());
    /* p33.NATY*/ pix_pipes.NATY_SPR_PIPE_B2.dff22(SACU_CLKPIPE_evn, _MYXA_SPR_PIX_SET2, _MAJO_SPR_PIX_RST2, pix_pipes.PEFU_SPR_PIPE_B1.qp_old());
    /* p33.PEFU*/ pix_pipes.PEFU_SPR_PIPE_B1.dff22(SACU_CLKPIPE_evn, _RUSY_SPR_PIX_SET1, _RUCA_SPR_PIX_RST1, pix_pipes.NYLU_SPR_PIPE_B0.qp_old());
    /* p33.NYLU*/ pix_pipes.NYLU_SPR_PIPE_B0.dff22(SACU_CLKPIPE_evn, _MEZU_SPR_PIX_SET0, _MOFY_SPR_PIX_RST0, SIG_GND.qp_new());
  }

  //----------------------------------------
  // Background pipe A
  {
    /* p32.LOZE*/ wire _LOZE_PIPE_A_LOADp = not1(NYXU_BFETCH_RSTn);
    /* p32.LAKY*/ wire _LAKY_BG_PIX_SET0 = nand2(_LOZE_PIPE_A_LOADp, tile_temp_a.LEGU_TILE_DA0n.qn_new());
    /* p32.NYXO*/ wire _NYXO_BG_PIX_SET1 = nand2(_LOZE_PIPE_A_LOADp, tile_temp_a.NUDU_TILE_DA1n.qn_new());
    /* p32.LOTO*/ wire _LOTO_BG_PIX_SET2 = nand2(_LOZE_PIPE_A_LOADp, tile_temp_a.MUKU_TILE_DA2n.qn_new());
    /* p32.LYDU*/ wire _LYDU_BG_PIX_SET3 = nand2(_LOZE_PIPE_A_LOADp, tile_temp_a.LUZO_TILE_DA3n.qn_new());
    /* p32.MYVY*/ wire _MYVY_BG_PIX_SET4 = nand2(_LOZE_PIPE_A_LOADp, tile_temp_a.MEGU_TILE_DA4n.qn_new());
    /* p32.LODO*/ wire _LODO_BG_PIX_SET5 = nand2(_LOZE_PIPE_A_LOADp, tile_temp_a.MYJY_TILE_DA5n.qn_new());
    /* p32.NUTE*/ wire _NUTE_BG_PIX_SET6 = nand2(_LOZE_PIPE_A_LOADp, tile_temp_a.NASA_TILE_DA6n.qn_new());
    /* p32.NAJA*/ wire _NAJA_BG_PIX_SET7 = nand2(_LOZE_PIPE_A_LOADp, tile_temp_a.NEFO_TILE_DA7n.qn_new());

    /* p32.LUHE*/ wire _LUHE_BG_PIX_DA0n = not1(tile_temp_a.LEGU_TILE_DA0n.qn_new());
    /* p32.NOLY*/ wire _NOLY_BG_PIX_DA1n = not1(tile_temp_a.NUDU_TILE_DA1n.qn_new());
    /* p32.LEKE*/ wire _LEKE_BG_PIX_DA2n = not1(tile_temp_a.MUKU_TILE_DA2n.qn_new());
    /* p32.LOMY*/ wire _LOMY_BG_PIX_DA3n = not1(tile_temp_a.LUZO_TILE_DA3n.qn_new());
    /* p32.LALA*/ wire _LALA_BG_PIX_DA4n = not1(tile_temp_a.MEGU_TILE_DA4n.qn_new());
    /* p32.LOXA*/ wire _LOXA_BG_PIX_DA5n = not1(tile_temp_a.MYJY_TILE_DA5n.qn_new());
    /* p32.NEZE*/ wire _NEZE_BG_PIX_DA6n = not1(tile_temp_a.NASA_TILE_DA6n.qn_new());
    /* p32.NOBO*/ wire _NOBO_BG_PIX_DA7n = not1(tile_temp_a.NEFO_TILE_DA7n.qn_new());

    /* p32.LOTY*/ wire _LOTY_BG_PIX_RST0 = nand2(_LOZE_PIPE_A_LOADp, _LUHE_BG_PIX_DA0n);
    /* p32.NEXA*/ wire _NEXA_BG_PIX_RST1 = nand2(_LOZE_PIPE_A_LOADp, _NOLY_BG_PIX_DA1n);
    /* p32.LUTU*/ wire _LUTU_BG_PIX_RST2 = nand2(_LOZE_PIPE_A_LOADp, _LEKE_BG_PIX_DA2n);
    /* p32.LUJA*/ wire _LUJA_BG_PIX_RST3 = nand2(_LOZE_PIPE_A_LOADp, _LOMY_BG_PIX_DA3n);
    /* p32.MOSY*/ wire _MOSY_BG_PIX_RST4 = nand2(_LOZE_PIPE_A_LOADp, _LALA_BG_PIX_DA4n);
    /* p32.LERU*/ wire _LERU_BG_PIX_RST5 = nand2(_LOZE_PIPE_A_LOADp, _LOXA_BG_PIX_DA5n);
    /* p32.NYHA*/ wire _NYHA_BG_PIX_RST6 = nand2(_LOZE_PIPE_A_LOADp, _NEZE_BG_PIX_DA6n);
    /* p32.NADY*/ wire _NADY_BG_PIX_RST7 = nand2(_LOZE_PIPE_A_LOADp, _NOBO_BG_PIX_DA7n);

    /* p32.PYBO*/ pix_pipes.PYBO_BGW_PIPE_A7.dff22(SACU_CLKPIPE_evn, _NAJA_BG_PIX_SET7, _NADY_BG_PIX_RST7, pix_pipes.NEDA_BGW_PIPE_A6.qp_old());
    /* p32.NEDA*/ pix_pipes.NEDA_BGW_PIPE_A6.dff22(SACU_CLKPIPE_evn, _NUTE_BG_PIX_SET6, _NYHA_BG_PIX_RST6, pix_pipes.MODU_BGW_PIPE_A5.qp_old());
    /* p32.MODU*/ pix_pipes.MODU_BGW_PIPE_A5.dff22(SACU_CLKPIPE_evn, _LODO_BG_PIX_SET5, _LERU_BG_PIX_RST5, pix_pipes.NEPO_BGW_PIPE_A4.qp_old());
    /* p32.NEPO*/ pix_pipes.NEPO_BGW_PIPE_A4.dff22(SACU_CLKPIPE_evn, _MYVY_BG_PIX_SET4, _MOSY_BG_PIX_RST4, pix_pipes.MACU_BGW_PIPE_A3.qp_old());
    /* p32.MACU*/ pix_pipes.MACU_BGW_PIPE_A3.dff22(SACU_CLKPIPE_evn, _LYDU_BG_PIX_SET3, _LUJA_BG_PIX_RST3, pix_pipes.MOJU_BGW_PIPE_A2.qp_old());
    /* p32.MOJU*/ pix_pipes.MOJU_BGW_PIPE_A2.dff22(SACU_CLKPIPE_evn, _LOTO_BG_PIX_SET2, _LUTU_BG_PIX_RST2, pix_pipes.NOZO_BGW_PIPE_A1.qp_old());
    /* p32.NOZO*/ pix_pipes.NOZO_BGW_PIPE_A1.dff22(SACU_CLKPIPE_evn, _NYXO_BG_PIX_SET1, _NEXA_BG_PIX_RST1, pix_pipes.MYDE_BGW_PIPE_A0.qp_old());
    /* p32.MYDE*/ pix_pipes.MYDE_BGW_PIPE_A0.dff22(SACU_CLKPIPE_evn, _LAKY_BG_PIX_SET0, _LOTY_BG_PIX_RST0, SIG_GND.qp_new());
  }

  //----------------------------------------
  // Background pipe B
  {
    /* p32.LUXA*/ wire _LUXA_PIPE_B_LOADp = not1(NYXU_BFETCH_RSTn);
    /* p32.TUXE*/ wire _TUXE_BG_PIX_SET0 = nand2(_LUXA_PIPE_B_LOADp, tile_temp_b.RAWU_TILE_DB0p.qp_new());
    /* p32.SOLY*/ wire _SOLY_BG_PIX_SET1 = nand2(_LUXA_PIPE_B_LOADp, tile_temp_b.POZO_TILE_DB1p.qp_new());
    /* p32.RUCE*/ wire _RUCE_BG_PIX_SET2 = nand2(_LUXA_PIPE_B_LOADp, tile_temp_b.PYZO_TILE_DB2p.qp_new());
    /* p32.RYJA*/ wire _RYJA_BG_PIX_SET3 = nand2(_LUXA_PIPE_B_LOADp, tile_temp_b.POXA_TILE_DB3p.qp_new());
    /* p32.RUTO*/ wire _RUTO_BG_PIX_SET4 = nand2(_LUXA_PIPE_B_LOADp, tile_temp_b.PULO_TILE_DB4p.qp_new());
    /* p32.RAJA*/ wire _RAJA_BG_PIX_SET5 = nand2(_LUXA_PIPE_B_LOADp, tile_temp_b.POJU_TILE_DB5p.qp_new());
    /* p32.RAJO*/ wire _RAJO_BG_PIX_SET6 = nand2(_LUXA_PIPE_B_LOADp, tile_temp_b.POWY_TILE_DB6p.qp_new());
    /* p32.RAGA*/ wire _RAGA_BG_PIX_SET7 = nand2(_LUXA_PIPE_B_LOADp, tile_temp_b.PYJU_TILE_DB7p.qp_new());

    /* p32.TOSA*/ wire _TOSA_BG_PIX_DB0n = not1(tile_temp_b.RAWU_TILE_DB0p.qp_new());
    /* p32.RUCO*/ wire _RUCO_BG_PIX_DB1n = not1(tile_temp_b.POZO_TILE_DB1p.qp_new());
    /* p32.TYCE*/ wire _TYCE_BG_PIX_DB2n = not1(tile_temp_b.PYZO_TILE_DB2p.qp_new());
    /* p32.REVY*/ wire _REVY_BG_PIX_DB3n = not1(tile_temp_b.POXA_TILE_DB3p.qp_new());
    /* p32.RYGA*/ wire _RYGA_BG_PIX_DB4n = not1(tile_temp_b.PULO_TILE_DB4p.qp_new());
    /* p32.RYLE*/ wire _RYLE_BG_PIX_DB5n = not1(tile_temp_b.POJU_TILE_DB5p.qp_new());
    /* p32.RAPU*/ wire _RAPU_BG_PIX_DB6n = not1(tile_temp_b.POWY_TILE_DB6p.qp_new());
    /* p32.SOJA*/ wire _SOJA_BG_PIX_DB7n = not1(tile_temp_b.PYJU_TILE_DB7p.qp_new());

    /* p32.SEJA*/ wire _SEJA_BG_PIX_RST0 = nand2(_LUXA_PIPE_B_LOADp, _TOSA_BG_PIX_DB0n);
    /* p32.SENO*/ wire _SENO_BG_PIX_RST1 = nand2(_LUXA_PIPE_B_LOADp, _RUCO_BG_PIX_DB1n);
    /* p32.SURE*/ wire _SURE_BG_PIX_RST2 = nand2(_LUXA_PIPE_B_LOADp, _TYCE_BG_PIX_DB2n);
    /* p32.SEBO*/ wire _SEBO_BG_PIX_RST3 = nand2(_LUXA_PIPE_B_LOADp, _REVY_BG_PIX_DB3n);
    /* p32.SUCA*/ wire _SUCA_BG_PIX_RST4 = nand2(_LUXA_PIPE_B_LOADp, _RYGA_BG_PIX_DB4n);
    /* p32.SYWE*/ wire _SYWE_BG_PIX_RST5 = nand2(_LUXA_PIPE_B_LOADp, _RYLE_BG_PIX_DB5n);
    /* p32.SUPU*/ wire _SUPU_BG_PIX_RST6 = nand2(_LUXA_PIPE_B_LOADp, _RAPU_BG_PIX_DB6n);
    /* p32.RYJY*/ wire _RYJY_BG_PIX_RST7 = nand2(_LUXA_PIPE_B_LOADp, _SOJA_BG_PIX_DB7n);

    /* p32.SOHU*/ pix_pipes.SOHU_BGW_PIPE_B7.dff22(SACU_CLKPIPE_evn, _RAGA_BG_PIX_SET7, _RYJY_BG_PIX_RST7, pix_pipes.RALU_BGW_PIPE_B6.qp_old());
    /* p32.RALU*/ pix_pipes.RALU_BGW_PIPE_B6.dff22(SACU_CLKPIPE_evn, _RAJO_BG_PIX_SET6, _SUPU_BG_PIX_RST6, pix_pipes.SETU_BGW_PIPE_B5.qp_old());
    /* p32.SETU*/ pix_pipes.SETU_BGW_PIPE_B5.dff22(SACU_CLKPIPE_evn, _RAJA_BG_PIX_SET5, _SYWE_BG_PIX_RST5, pix_pipes.SOBO_BGW_PIPE_B4.qp_old());
    /* p32.SOBO*/ pix_pipes.SOBO_BGW_PIPE_B4.dff22(SACU_CLKPIPE_evn, _RUTO_BG_PIX_SET4, _SUCA_BG_PIX_RST4, pix_pipes.RYSA_BGW_PIPE_B3.qp_old());
    /* p32.RYSA*/ pix_pipes.RYSA_BGW_PIPE_B3.dff22(SACU_CLKPIPE_evn, _RYJA_BG_PIX_SET3, _SEBO_BG_PIX_RST3, pix_pipes.SADY_BGW_PIPE_B2.qp_old());
    /* p32.SADY*/ pix_pipes.SADY_BGW_PIPE_B2.dff22(SACU_CLKPIPE_evn, _RUCE_BG_PIX_SET2, _SURE_BG_PIX_RST2, pix_pipes.TACA_BGW_PIPE_B1.qp_old());
    /* p32.TACA*/ pix_pipes.TACA_BGW_PIPE_B1.dff22(SACU_CLKPIPE_evn, _SOLY_BG_PIX_SET1, _SENO_BG_PIX_RST1, pix_pipes.TOMY_BGW_PIPE_B0.qp_old());
    /* p32.TOMY*/ pix_pipes.TOMY_BGW_PIPE_B0.dff22(SACU_CLKPIPE_evn, _TUXE_BG_PIX_SET0, _SEJA_BG_PIX_RST0, SIG_GND.qp_new());
  }

  //----------------------------------------
  // Mask pipe
  {
    /* p26.TEDE*/ wire _TEDE_MASK_PIPE_SET0 = nand2(_LESY_SPRITE_MASK0p, oam_temp_b.DEPO_OAM_DB7p.qp_new());
    /* p26.XALA*/ wire _XALA_MASK_PIPE_SET1 = nand2(_LOTA_SPRITE_MASK1p, oam_temp_b.DEPO_OAM_DB7p.qp_new());
    /* p26.TYRA*/ wire _TYRA_MASK_PIPE_SET2 = nand2(_LYKU_SPRITE_MASK2p, oam_temp_b.DEPO_OAM_DB7p.qp_new());
    /* p26.XYRU*/ wire _XYRU_MASK_PIPE_SET3 = nand2(_ROBY_SPRITE_MASK3p, oam_temp_b.DEPO_OAM_DB7p.qp_new());
    /* p26.XUKU*/ wire _XUKU_MASK_PIPE_SET4 = nand2(_TYTA_SPRITE_MASK4p, oam_temp_b.DEPO_OAM_DB7p.qp_new());
    /* p26.XELY*/ wire _XELY_MASK_PIPE_SET5 = nand2(_TYCO_SPRITE_MASK5p, oam_temp_b.DEPO_OAM_DB7p.qp_new());
    /* p26.TYKO*/ wire _TYKO_MASK_PIPE_SET6 = nand2(_SOKA_SPRITE_MASK6p, oam_temp_b.DEPO_OAM_DB7p.qp_new());
    /* p26.TUWU*/ wire _TUWU_MASK_PIPE_SET7 = nand2(_XOVU_SPRITE_MASK7p, oam_temp_b.DEPO_OAM_DB7p.qp_new());

    /* p26.XOGA*/ wire _XOGA_MASK_PIPE_DB7n = not1(oam_temp_b.DEPO_OAM_DB7p.qp_new());
    /* p26.XURA*/ wire _XURA_MASK_PIPE_DB7n = not1(oam_temp_b.DEPO_OAM_DB7p.qp_new());
    /* p26.TAJO*/ wire _TAJO_MASK_PIPE_DB7n = not1(oam_temp_b.DEPO_OAM_DB7p.qp_new());
    /* p26.XENU*/ wire _XENU_MASK_PIPE_DB7n = not1(oam_temp_b.DEPO_OAM_DB7p.qp_new());
    /* p26.XYKE*/ wire _XYKE_MASK_PIPE_DB7n = not1(oam_temp_b.DEPO_OAM_DB7p.qp_new());
    /* p26.XABA*/ wire _XABA_MASK_PIPE_DB7n = not1(oam_temp_b.DEPO_OAM_DB7p.qp_new());
    /* p26.TAFU*/ wire _TAFU_MASK_PIPE_DB7n = not1(oam_temp_b.DEPO_OAM_DB7p.qp_new());
    /* p26.XUHO*/ wire _XUHO_MASK_PIPE_DB7n = not1(oam_temp_b.DEPO_OAM_DB7p.qp_new());

    /* p26.WOKA*/ wire _WOKA_MASK_PIPE_RST0 = nand2(_LESY_SPRITE_MASK0p, _XOGA_MASK_PIPE_DB7n);
    /* p26.WEDE*/ wire _WEDE_MASK_PIPE_RST1 = nand2(_LOTA_SPRITE_MASK1p, _XURA_MASK_PIPE_DB7n);
    /* p26.TUFO*/ wire _TUFO_MASK_PIPE_RST2 = nand2(_LYKU_SPRITE_MASK2p, _TAJO_MASK_PIPE_DB7n);
    /* p26.WEVO*/ wire _WEVO_MASK_PIPE_RST3 = nand2(_ROBY_SPRITE_MASK3p, _XENU_MASK_PIPE_DB7n);
    /* p26.WEDY*/ wire _WEDY_MASK_PIPE_RST4 = nand2(_TYTA_SPRITE_MASK4p, _XYKE_MASK_PIPE_DB7n);
    /* p26.WUJA*/ wire _WUJA_MASK_PIPE_RST5 = nand2(_TYCO_SPRITE_MASK5p, _XABA_MASK_PIPE_DB7n);
    /* p26.TENA*/ wire _TENA_MASK_PIPE_RST6 = nand2(_SOKA_SPRITE_MASK6p, _TAFU_MASK_PIPE_DB7n);
    /* p26.WUBU*/ wire _WUBU_MASK_PIPE_RST7 = nand2(_XOVU_SPRITE_MASK7p, _XUHO_MASK_PIPE_DB7n);

    /* p26.VAVA*/ pix_pipes.VAVA_MASK_PIPE_7.dff22(SACU_CLKPIPE_evn, _TUWU_MASK_PIPE_SET7, _WUBU_MASK_PIPE_RST7, pix_pipes.VUMO_MASK_PIPE_6.qp_old());
    /* p26.VUMO*/ pix_pipes.VUMO_MASK_PIPE_6.dff22(SACU_CLKPIPE_evn, _TYKO_MASK_PIPE_SET6, _TENA_MASK_PIPE_RST6, pix_pipes.WODA_MASK_PIPE_5.qp_old());
    /* p26.WODA*/ pix_pipes.WODA_MASK_PIPE_5.dff22(SACU_CLKPIPE_evn, _XELY_MASK_PIPE_SET5, _WUJA_MASK_PIPE_RST5, pix_pipes.XETE_MASK_PIPE_4.qp_old());
    /* p26.XETE*/ pix_pipes.XETE_MASK_PIPE_4.dff22(SACU_CLKPIPE_evn, _XUKU_MASK_PIPE_SET4, _WEDY_MASK_PIPE_RST4, pix_pipes.WYFU_MASK_PIPE_3.qp_old());
    /* p26.WYFU*/ pix_pipes.WYFU_MASK_PIPE_3.dff22(SACU_CLKPIPE_evn, _XYRU_MASK_PIPE_SET3, _WEVO_MASK_PIPE_RST3, pix_pipes.VOSA_MASK_PIPE_2.qp_old());
    /* p26.VOSA*/ pix_pipes.VOSA_MASK_PIPE_2.dff22(SACU_CLKPIPE_evn, _TYRA_MASK_PIPE_SET2, _TUFO_MASK_PIPE_RST2, pix_pipes.WURU_MASK_PIPE_1.qp_old());
    /* p26.WURU*/ pix_pipes.WURU_MASK_PIPE_1.dff22(SACU_CLKPIPE_evn, _XALA_MASK_PIPE_SET1, _WEDE_MASK_PIPE_RST1, pix_pipes.VEZO_MASK_PIPE_0.qp_old());
    /* p26.VEZO*/ pix_pipes.VEZO_MASK_PIPE_0.dff22(SACU_CLKPIPE_evn, _TEDE_MASK_PIPE_SET0, _WOKA_MASK_PIPE_RST0, SIG_VCC.qp_new());
  }

  //----------------------------------------
  // Pal pipe
  {
    /* p34.PUME*/ wire _PUME_PAL_PIPE_SET0 = nand2(_LESY_SPRITE_MASK0p, oam_temp_b.GOMO_OAM_DB4p.qp_new());
    /* p34.SORO*/ wire _SORO_PAL_PIPE_SET1 = nand2(_LOTA_SPRITE_MASK1p, oam_temp_b.GOMO_OAM_DB4p.qp_new());
    /* p34.PAMO*/ wire _PAMO_PAL_PIPE_SET2 = nand2(_LYKU_SPRITE_MASK2p, oam_temp_b.GOMO_OAM_DB4p.qp_new());
    /* p34.SUKY*/ wire _SUKY_PAL_PIPE_SET3 = nand2(_ROBY_SPRITE_MASK3p, oam_temp_b.GOMO_OAM_DB4p.qp_new());
    /* p34.RORA*/ wire _RORA_PAL_PIPE_SET4 = nand2(_TYTA_SPRITE_MASK4p, oam_temp_b.GOMO_OAM_DB4p.qp_new());
    /* p34.MENE*/ wire _MENE_PAL_PIPE_SET5 = nand2(_TYCO_SPRITE_MASK5p, oam_temp_b.GOMO_OAM_DB4p.qp_new());
    /* p34.LUKE*/ wire _LUKE_PAL_PIPE_SET6 = nand2(_SOKA_SPRITE_MASK6p, oam_temp_b.GOMO_OAM_DB4p.qp_new());
    /* p34.LAMY*/ wire _LAMY_PAL_PIPE_SET7 = nand2(_XOVU_SPRITE_MASK7p, oam_temp_b.GOMO_OAM_DB4p.qp_new());

    /* p34.SYPY*/ wire _SYPY_PAL_PIPE_DB4n = not1(oam_temp_b.GOMO_OAM_DB4p.qp_new());
    /* p34.TOTU*/ wire _TOTU_PAL_PIPE_DB4n = not1(oam_temp_b.GOMO_OAM_DB4p.qp_new());
    /* p34.NARO*/ wire _NARO_PAL_PIPE_DB4n = not1(oam_temp_b.GOMO_OAM_DB4p.qp_new());
    /* p34.WEXY*/ wire _WEXY_PAL_PIPE_DB4n = not1(oam_temp_b.GOMO_OAM_DB4p.qp_new());
    /* p34.RYZY*/ wire _RYZY_PAL_PIPE_DB4n = not1(oam_temp_b.GOMO_OAM_DB4p.qp_new());
    /* p34.RYFE*/ wire _RYFE_PAL_PIPE_DB4n = not1(oam_temp_b.GOMO_OAM_DB4p.qp_new());
    /* p34.LADY*/ wire _LADY_PAL_PIPE_DB4n = not1(oam_temp_b.GOMO_OAM_DB4p.qp_new());
    /* p34.LAFY*/ wire _LAFY_PAL_PIPE_DB4n = not1(oam_temp_b.GOMO_OAM_DB4p.qp_new());

    /* p34.SUCO*/ wire _SUCO_PAL_PIPE_RST0 = nand2(_LESY_SPRITE_MASK0p, _SYPY_PAL_PIPE_DB4n);
    /* p34.TAFA*/ wire _TAFA_PAL_PIPE_RST1 = nand2(_LOTA_SPRITE_MASK1p, _TOTU_PAL_PIPE_DB4n);
    /* p34.PYZY*/ wire _PYZY_PAL_PIPE_RST2 = nand2(_LYKU_SPRITE_MASK2p, _NARO_PAL_PIPE_DB4n);
    /* p34.TOWA*/ wire _TOWA_PAL_PIPE_RST3 = nand2(_ROBY_SPRITE_MASK3p, _WEXY_PAL_PIPE_DB4n);
    /* p34.RUDU*/ wire _RUDU_PAL_PIPE_RST4 = nand2(_TYTA_SPRITE_MASK4p, _RYZY_PAL_PIPE_DB4n);
    /* p34.PAZO*/ wire _PAZO_PAL_PIPE_RST5 = nand2(_TYCO_SPRITE_MASK5p, _RYFE_PAL_PIPE_DB4n);
    /* p34.LOWA*/ wire _LOWA_PAL_PIPE_RST6 = nand2(_SOKA_SPRITE_MASK6p, _LADY_PAL_PIPE_DB4n);
    /* p34.LUNU*/ wire _LUNU_PAL_PIPE_RST7 = nand2(_XOVU_SPRITE_MASK7p, _LAFY_PAL_PIPE_DB4n);

    /* p34.LYME*/ pix_pipes.LYME_PAL_PIPE_D7.dff22(SACU_CLKPIPE_evn, _LAMY_PAL_PIPE_SET7, _LUNU_PAL_PIPE_RST7, pix_pipes.MODA_PAL_PIPE_D6.qp_old());
    /* p34.MODA*/ pix_pipes.MODA_PAL_PIPE_D6.dff22(SACU_CLKPIPE_evn, _LUKE_PAL_PIPE_SET6, _LOWA_PAL_PIPE_RST6, pix_pipes.NUKE_PAL_PIPE_D5.qp_old());
    /* p34.NUKE*/ pix_pipes.NUKE_PAL_PIPE_D5.dff22(SACU_CLKPIPE_evn, _MENE_PAL_PIPE_SET5, _PAZO_PAL_PIPE_RST5, pix_pipes.PALU_PAL_PIPE_D4.qp_old());
    /* p34.PALU*/ pix_pipes.PALU_PAL_PIPE_D4.dff22(SACU_CLKPIPE_evn, _RORA_PAL_PIPE_SET4, _RUDU_PAL_PIPE_RST4, pix_pipes.SOMY_PAL_PIPE_D3.qp_old());
    /* p34.SOMY*/ pix_pipes.SOMY_PAL_PIPE_D3.dff22(SACU_CLKPIPE_evn, _SUKY_PAL_PIPE_SET3, _TOWA_PAL_PIPE_RST3, pix_pipes.ROSA_PAL_PIPE_D2.qp_old());
    /* p34.ROSA*/ pix_pipes.ROSA_PAL_PIPE_D2.dff22(SACU_CLKPIPE_evn, _PAMO_PAL_PIPE_SET2, _PYZY_PAL_PIPE_RST2, pix_pipes.SATA_PAL_PIPE_D1.qp_old());
    /* p34.SATA*/ pix_pipes.SATA_PAL_PIPE_D1.dff22(SACU_CLKPIPE_evn, _SORO_PAL_PIPE_SET1, _TAFA_PAL_PIPE_RST1, pix_pipes.RUGO_PAL_PIPE_D0.qp_old());
    /* p34.RUGO*/ pix_pipes.RUGO_PAL_PIPE_D0.dff22(SACU_CLKPIPE_evn, _PUME_PAL_PIPE_SET0, _SUCO_PAL_PIPE_RST0, SIG_GND.qp_new());
  }

  //----------------------------------------
  // Pipe merge

  /*#p35.RAJY*/ wire RAJY_PIX_BG_LOp = and2(pix_pipes.PYBO_BGW_PIPE_A7.qp_new(), reg_lcdc.VYXE_LCDC_BGENn.qn_new());
  /*#p35.TADE*/ wire TADE_PIX_BG_HIp = and2(pix_pipes.SOHU_BGW_PIPE_B7.qp_new(), reg_lcdc.VYXE_LCDC_BGENn.qn_new());
  /*#p35.XULA*/ wire XULA_PIX_SP_LOp = and2(reg_lcdc.XYLO_LCDC_SPENn.qn_new(), pix_pipes.WUFY_SPR_PIPE_A7.qp_new());
  /*#p35.WOXA*/ wire WOXA_PIX_SP_HIp = and2(reg_lcdc.XYLO_LCDC_SPENn.qn_new(), pix_pipes.VUPY_SPR_PIPE_B7.qp_new());

  /*#p35.NULY*/ wire NULY_PIX_SP_MASKn = nor2(WOXA_PIX_SP_HIp, XULA_PIX_SP_LOp);

  /*#p35.RYFU*/ wire RYFU_MASK_BG0 = and2(RAJY_PIX_BG_LOp, pix_pipes.VAVA_MASK_PIPE_7.qp_new());
  /*#p35.RUTA*/ wire RUTA_MASK_BG1 = and2(TADE_PIX_BG_HIp, pix_pipes.VAVA_MASK_PIPE_7.qp_new());
  /*#p35.POKA*/ wire POKA_BGPIXELn = nor3(NULY_PIX_SP_MASKn, RUTA_MASK_BG1, RYFU_MASK_BG0);

  /*#p34.LOME*/ wire LOME_PAL_PIPE_7n = not1(pix_pipes.LYME_PAL_PIPE_D7.qp_new());
  /*#p34.LAFU*/ wire LAFU_OBP0PIXELn = nand2(LOME_PAL_PIPE_7n, POKA_BGPIXELn);
  /*#p34.LEKA*/ wire LEKA_OBP1PIXELn = nand2(pix_pipes.LYME_PAL_PIPE_D7.qp_new(), POKA_BGPIXELn);

  //----------------------------------------
  // Pal reg read/write

  /* p36.VELY*/ wire _VELY_FF47_WRp = and2(CUPA_CPU_WRp(), new_bus.WERA_FF47p());
  /* p36.TEPO*/ wire _TEPO_FF47_WRp = not1(_VELY_FF47_WRp);
  /*#p36.PAVO*/ reg_bgp.PAVO_BGP_D0n.dff8p(_TEPO_FF47_WRp, old_bus.BUS_CPU_D00p.qp_old());
  /* p36.NUSY*/ reg_bgp.NUSY_BGP_D1n.dff8p(_TEPO_FF47_WRp, old_bus.BUS_CPU_D01p.qp_old());
  /* p36.PYLU*/ reg_bgp.PYLU_BGP_D2n.dff8p(_TEPO_FF47_WRp, old_bus.BUS_CPU_D02p.qp_old());
  /* p36.MAXY*/ reg_bgp.MAXY_BGP_D3n.dff8p(_TEPO_FF47_WRp, old_bus.BUS_CPU_D03p.qp_old());
  /* p36.MUKE*/ reg_bgp.MUKE_BGP_D4n.dff8p(_TEPO_FF47_WRp, old_bus.BUS_CPU_D04p.qp_old());
  /* p36.MORU*/ reg_bgp.MORU_BGP_D5n.dff8p(_TEPO_FF47_WRp, old_bus.BUS_CPU_D05p.qp_old());
  /* p36.MOGY*/ reg_bgp.MOGY_BGP_D6n.dff8p(_TEPO_FF47_WRp, old_bus.BUS_CPU_D06p.qp_old());
  /* p36.MENA*/ reg_bgp.MENA_BGP_D7n.dff8p(_TEPO_FF47_WRp, old_bus.BUS_CPU_D07p.qp_old());

  /* p36.XOMA*/ wire _XOMA_FF48_WRp = and2(CUPA_CPU_WRp(), new_bus.XAYO_FF48p());
  /* p36.XELO*/ wire _XELO_FF48_WRn = not1(_XOMA_FF48_WRp);
  /* p36.XUFU*/ reg_obp0.XUFU_OBP0_D0n.dff8p(_XELO_FF48_WRn, old_bus.BUS_CPU_D00p.qp_old());
  /* p36.XUKY*/ reg_obp0.XUKY_OBP0_D1n.dff8p(_XELO_FF48_WRn, old_bus.BUS_CPU_D01p.qp_old());
  /* p36.XOVA*/ reg_obp0.XOVA_OBP0_D2n.dff8p(_XELO_FF48_WRn, old_bus.BUS_CPU_D02p.qp_old());
  /* p36.XALO*/ reg_obp0.XALO_OBP0_D3n.dff8p(_XELO_FF48_WRn, old_bus.BUS_CPU_D03p.qp_old());
  /* p36.XERU*/ reg_obp0.XERU_OBP0_D4n.dff8p(_XELO_FF48_WRn, old_bus.BUS_CPU_D04p.qp_old());
  /* p36.XYZE*/ reg_obp0.XYZE_OBP0_D5n.dff8p(_XELO_FF48_WRn, old_bus.BUS_CPU_D05p.qp_old());
  /* p36.XUPO*/ reg_obp0.XUPO_OBP0_D6n.dff8p(_XELO_FF48_WRn, old_bus.BUS_CPU_D06p.qp_old());
  /* p36.XANA*/ reg_obp0.XANA_OBP0_D7n.dff8p(_XELO_FF48_WRn, old_bus.BUS_CPU_D07p.qp_old());

  /* p36.MYXE*/ wire _MYXE_FF49_WRp = and2(CUPA_CPU_WRp(), new_bus.TEGO_FF49p());
  /* p36.LEHO*/ wire _LEHO_FF49_WRn = not1(_MYXE_FF49_WRp);
  /* p36.MOXY*/ reg_obp1.MOXY_OBP1_D0n.dff8p(_LEHO_FF49_WRn, old_bus.BUS_CPU_D00p.qp_old());
  /* p36.LAWO*/ reg_obp1.LAWO_OBP1_D1n.dff8p(_LEHO_FF49_WRn, old_bus.BUS_CPU_D01p.qp_old());
  /* p36.MOSA*/ reg_obp1.MOSA_OBP1_D2n.dff8p(_LEHO_FF49_WRn, old_bus.BUS_CPU_D02p.qp_old());
  /* p36.LOSE*/ reg_obp1.LOSE_OBP1_D3n.dff8p(_LEHO_FF49_WRn, old_bus.BUS_CPU_D03p.qp_old());
  /* p36.LUNE*/ reg_obp1.LUNE_OBP1_D4n.dff8p(_LEHO_FF49_WRn, old_bus.BUS_CPU_D04p.qp_old());
  /* p36.LUGU*/ reg_obp1.LUGU_OBP1_D5n.dff8p(_LEHO_FF49_WRn, old_bus.BUS_CPU_D05p.qp_old());
  /* p36.LEPU*/ reg_obp1.LEPU_OBP1_D6n.dff8p(_LEHO_FF49_WRn, old_bus.BUS_CPU_D06p.qp_old());
  /* p36.LUXO*/ reg_obp1.LUXO_OBP1_D7n.dff8p(_LEHO_FF49_WRn, old_bus.BUS_CPU_D07p.qp_old());

  /* p36.VUSO*/ wire _VUSO_FF47_RDp = and2(ASOT_CPU_RDp(), new_bus.WERA_FF47p());
  /* p36.TEPY*/ wire _TEPY_FF47_RDn = not1(_VUSO_FF47_RDp);
  /*#p36.RARO_BGP0_TO_CD0*/ new_bus.BUS_CPU_D00p.tri6_nn(_TEPY_FF47_RDn, reg_bgp.PAVO_BGP_D0n.qp_new());
  /* p36.PABA_BGP1_TO_CD1*/ new_bus.BUS_CPU_D01p.tri6_nn(_TEPY_FF47_RDn, reg_bgp.NUSY_BGP_D1n.qp_new());
  /* p36.REDO_BGP2_TO_CD2*/ new_bus.BUS_CPU_D02p.tri6_nn(_TEPY_FF47_RDn, reg_bgp.PYLU_BGP_D2n.qp_new());
  /* p36.LOBE_BGP3_TO_CD3*/ new_bus.BUS_CPU_D03p.tri6_nn(_TEPY_FF47_RDn, reg_bgp.MAXY_BGP_D3n.qp_new());
  /* p36.LACE_BGP4_TO_CD4*/ new_bus.BUS_CPU_D04p.tri6_nn(_TEPY_FF47_RDn, reg_bgp.MUKE_BGP_D4n.qp_new());
  /* p36.LYKA_BGP5_TO_CD5*/ new_bus.BUS_CPU_D05p.tri6_nn(_TEPY_FF47_RDn, reg_bgp.MORU_BGP_D5n.qp_new());
  /* p36.LODY_BGP6_TO_CD6*/ new_bus.BUS_CPU_D06p.tri6_nn(_TEPY_FF47_RDn, reg_bgp.MOGY_BGP_D6n.qp_new());
  /* p36.LARY_BGP7_TO_CD7*/ new_bus.BUS_CPU_D07p.tri6_nn(_TEPY_FF47_RDn, reg_bgp.MENA_BGP_D7n.qp_new());

  /* p36.XUFY*/ wire _XUFY_FF48_RDp = and2(ASOT_CPU_RDp(), new_bus.XAYO_FF48p());
  /* p36.XOZY*/ wire _XOZY_FF48_RDn = not1(_XUFY_FF48_RDp);
  /*#p36.XARY_OBP00_TO_CD0*/ new_bus.BUS_CPU_D00p.tri6_nn(_XOZY_FF48_RDn, reg_obp0.XUFU_OBP0_D0n.qp_new());
  /* p36.XOKE_OBP01_TO_CD1*/ new_bus.BUS_CPU_D01p.tri6_nn(_XOZY_FF48_RDn, reg_obp0.XUKY_OBP0_D1n.qp_new());
  /* p36.XUNO_OBP02_TO_CD2*/ new_bus.BUS_CPU_D02p.tri6_nn(_XOZY_FF48_RDn, reg_obp0.XOVA_OBP0_D2n.qp_new());
  /* p36.XUBY_OBP03_TO_CD3*/ new_bus.BUS_CPU_D03p.tri6_nn(_XOZY_FF48_RDn, reg_obp0.XALO_OBP0_D3n.qp_new());
  /* p36.XAJU_OBP04_TO_CD4*/ new_bus.BUS_CPU_D04p.tri6_nn(_XOZY_FF48_RDn, reg_obp0.XERU_OBP0_D4n.qp_new());
  /* p36.XOBO_OBP05_TO_CD5*/ new_bus.BUS_CPU_D05p.tri6_nn(_XOZY_FF48_RDn, reg_obp0.XYZE_OBP0_D5n.qp_new());
  /* p36.XAXA_OBP06_TO_CD6*/ new_bus.BUS_CPU_D06p.tri6_nn(_XOZY_FF48_RDn, reg_obp0.XUPO_OBP0_D6n.qp_new());
  /* p36.XAWO_OBP07_TO_CD7*/ new_bus.BUS_CPU_D07p.tri6_nn(_XOZY_FF48_RDn, reg_obp0.XANA_OBP0_D7n.qp_new());

  /* p36.MUMY*/ wire _MUMY_FF49_RDp = and2(ASOT_CPU_RDp(), new_bus.TEGO_FF49p());
  /* p36.LOTE*/ wire _LOTE_FF49_RDn = not1(_MUMY_FF49_RDp);
  /*#p36.LAJU_OBP10_TO_CD0*/ new_bus.BUS_CPU_D00p.tri6_nn(_LOTE_FF49_RDn, reg_obp1.MOXY_OBP1_D0n.qp_new());
  /* p36.LEPA_OBP11_TO_CD1*/ new_bus.BUS_CPU_D01p.tri6_nn(_LOTE_FF49_RDn, reg_obp1.LAWO_OBP1_D1n.qp_new());
  /* p36.LODE_OBP12_TO_CD2*/ new_bus.BUS_CPU_D02p.tri6_nn(_LOTE_FF49_RDn, reg_obp1.MOSA_OBP1_D2n.qp_new());
  /* p36.LYZA_OBP13_TO_CD3*/ new_bus.BUS_CPU_D03p.tri6_nn(_LOTE_FF49_RDn, reg_obp1.LOSE_OBP1_D3n.qp_new());
  /* p36.LUKY_OBP14_TO_CD4*/ new_bus.BUS_CPU_D04p.tri6_nn(_LOTE_FF49_RDn, reg_obp1.LUNE_OBP1_D4n.qp_new());
  /* p36.LUGA_OBP15_TO_CD5*/ new_bus.BUS_CPU_D05p.tri6_nn(_LOTE_FF49_RDn, reg_obp1.LUGU_OBP1_D5n.qp_new());
  /* p36.LEBA_OBP16_TO_CD6*/ new_bus.BUS_CPU_D06p.tri6_nn(_LOTE_FF49_RDn, reg_obp1.LEPU_OBP1_D6n.qp_new());
  /* p36.LELU_OBP17_TO_CD7*/ new_bus.BUS_CPU_D07p.tri6_nn(_LOTE_FF49_RDn, reg_obp1.LUXO_OBP1_D7n.qp_new());

  //----------------------------------------
  // Sprite palette 0 lookup

  /*#p35.WELE*/ wire WELE_PIX_SP_LOn = not1(XULA_PIX_SP_LOp);
  /*#p35.WOLO*/ wire WOLO_PIX_SP_LOp = not1(WELE_PIX_SP_LOn);
  /*#p35.VUMU*/ wire VUMU_PIX_SP_HIn = not1(WOXA_PIX_SP_HIp);
  /*#p35.WYRU*/ wire WYRU_PIX_SP_HIp = not1(VUMU_PIX_SP_HIn);

  /*#p35.LAVA*/ wire LAVA_MASK_OPB0  = not1(LAFU_OBP0PIXELn);

  /*#p35.VUGO*/ wire VUGO_PAL_OBP0A = and3(VUMU_PIX_SP_HIn, WELE_PIX_SP_LOn, LAVA_MASK_OPB0); // does not have vcc arm
  /*#p35.VOLO*/ wire VOLO_PAL_OBP0B = and3(VUMU_PIX_SP_HIn, WOLO_PIX_SP_LOp, LAVA_MASK_OPB0); // does not have vcc arm
  /*#p35.VATA*/ wire VATA_PAL_OBP0C = and3(WYRU_PIX_SP_HIp, WELE_PIX_SP_LOn, LAVA_MASK_OPB0); // does not have vcc arm
  /*#p35.VYRO*/ wire VYRO_PAL_OBP0D = and3(WYRU_PIX_SP_HIp, WOLO_PIX_SP_LOp, LAVA_MASK_OPB0); // does not have vcc arm

  /*#p35.WUFU*/ wire WUFU_COL_OBP0_HI = amux4(reg_obp0.XANA_OBP0_D7n.qp_any(), VYRO_PAL_OBP0D, reg_obp0.XYZE_OBP0_D5n.qp_any(), VATA_PAL_OBP0C, reg_obp0.XALO_OBP0_D3n.qp_any(), VOLO_PAL_OBP0B, reg_obp0.XUKY_OBP0_D1n.qp_any(), VUGO_PAL_OBP0A);
  /*#p35.WALY*/ wire WALY_COL_OBP0_LO = amux4(reg_obp0.XUPO_OBP0_D6n.qp_any(), VYRO_PAL_OBP0D, reg_obp0.XERU_OBP0_D4n.qp_any(), VATA_PAL_OBP0C, reg_obp0.XOVA_OBP0_D2n.qp_any(), VOLO_PAL_OBP0B, reg_obp0.XUFU_OBP0_D0n.qp_any(), VUGO_PAL_OBP0A);

  //----------------------------------------
  // Sprite palette 1 lookup

  /*#p35.MABY*/ wire MABY_PIX_SP_LOn = not1(XULA_PIX_SP_LOp);
  /*#p35.LYLE*/ wire LYLE_PIX_SP_LOp = not1(MABY_PIX_SP_LOn);
  /*#p35.MEXA*/ wire MEXA_PIX_SP_HIn = not1(WOXA_PIX_SP_HIp);
  /*#p35.LOZO*/ wire LOZO_PIX_SP_HIp = not1(MEXA_PIX_SP_HIn);

  /*#p35.LUKU*/ wire LUKU_MASK_OBP1  = not1(LEKA_OBP1PIXELn);

  /*#p35.LOPU*/ wire LOPU_PAL_OBP1A = and3(MEXA_PIX_SP_HIn, MABY_PIX_SP_LOn, LUKU_MASK_OBP1); // does not have vcc arm
  /*#p35.LYKY*/ wire LYKY_PAL_OBP1B = and3(MEXA_PIX_SP_HIn, LYLE_PIX_SP_LOp, LUKU_MASK_OBP1); // does not have vcc arm
  /*#p35.LARU*/ wire LARU_PAL_OBP1C = and3(LOZO_PIX_SP_HIp, MABY_PIX_SP_LOn, LUKU_MASK_OBP1); // does not have vcc arm
  /*#p35.LEDO*/ wire LEDO_PAL_OBP1D = and3(LOZO_PIX_SP_HIp, LYLE_PIX_SP_LOp, LUKU_MASK_OBP1); // does not have vcc arm

  /*#p35.MOKA*/ wire MOKA_COL_OBP1_HI = amux4(reg_obp1.LUXO_OBP1_D7n.qp_any(), LEDO_PAL_OBP1D, reg_obp1.LUGU_OBP1_D5n.qp_any(), LARU_PAL_OBP1C, reg_obp1.LOSE_OBP1_D3n.qp_any(), LYKY_PAL_OBP1B, reg_obp1.LAWO_OBP1_D1n.qp_any(), LOPU_PAL_OBP1A);
  /*#p35.MUFA*/ wire MUFA_COL_OBP1_LO = amux4(LEDO_PAL_OBP1D, reg_obp1.LEPU_OBP1_D6n.qp_any(), LARU_PAL_OBP1C, reg_obp1.LUNE_OBP1_D4n.qp_any(), LYKY_PAL_OBP1B, reg_obp1.MOSA_OBP1_D2n.qp_any(), LOPU_PAL_OBP1A, reg_obp1.MOXY_OBP1_D0n.qp_any());

  //----------------------------------------
  // Background/window palette lookup

  /* p35.SOBA*/ wire SOBA_PIX_BG_LOn = not1(RAJY_PIX_BG_LOp);
  /* p35.NUPO*/ wire NUPO_PIX_BG_LOp = not1(SOBA_PIX_BG_LOn);
  /* p35.VYCO*/ wire VYCO_PIX_BG_HIn = not1(TADE_PIX_BG_HIp);
  /* p35.NALE*/ wire NALE_PIX_BG_HIp = not1(VYCO_PIX_BG_HIn);

  /* p35.MUVE*/ wire MUVE_MASK_BGP = not1(POKA_BGPIXELn);

  /* p35.POBU*/ wire POBU_PAL_BGPA = and3(VYCO_PIX_BG_HIn, SOBA_PIX_BG_LOn, MUVE_MASK_BGP); // does not have vcc arm
  /* p35.NUXO*/ wire NUXO_PAL_BGPB = and3(VYCO_PIX_BG_HIn, NUPO_PIX_BG_LOp, MUVE_MASK_BGP); // does not have vcc arm
  /* p35.NUMA*/ wire NUMA_PAL_BGPC = and3(NALE_PIX_BG_HIp, SOBA_PIX_BG_LOn, MUVE_MASK_BGP); // does not have vcc arm
  /* p35.NYPO*/ wire NYPO_PAL_BGPD = and3(NALE_PIX_BG_HIp, NUPO_PIX_BG_LOp, MUVE_MASK_BGP); // does not have vcc arm

  /*#p35.NELO*/ wire NELO_COL_BG_LO = amux4(NYPO_PAL_BGPD, reg_bgp.MOGY_BGP_D6n.qp_any(), NUMA_PAL_BGPC, reg_bgp.MUKE_BGP_D4n.qp_any(), NUXO_PAL_BGPB, reg_bgp.PYLU_BGP_D2n.qp_any(), POBU_PAL_BGPA, reg_bgp.PAVO_BGP_D0n.qp_any());
  /*#p35.NURA*/ wire NURA_COL_BG_HI = amux4(reg_bgp.MENA_BGP_D7n.qp_any(), NYPO_PAL_BGPD, reg_bgp.MORU_BGP_D5n.qp_any(), NUMA_PAL_BGPC, reg_bgp.MAXY_BGP_D3n.qp_any(), NUXO_PAL_BGPB, reg_bgp.NUSY_BGP_D1n.qp_any(), POBU_PAL_BGPA);

  //----------------------------------------
  // Pixel merge and send

  /*#p35.PERO*/ wire _PERO_COL_LO = or3(NELO_COL_BG_LO, WALY_COL_OBP0_LO, MUFA_COL_OBP1_LO);
  /*#p35.PATY*/ wire _PATY_COL_HI = or3(NURA_COL_BG_HI, WUFU_COL_OBP0_HI, MOKA_COL_OBP1_HI);

  /*#p35.REMY*/ pix_pipes.REMY_LD0n = not1(_PERO_COL_LO);
  /*#p35.RAVO*/ pix_pipes.RAVO_LD1n = not1(_PATY_COL_HI);
}

//------------------------------------------------------------------------------------------------------------------------
