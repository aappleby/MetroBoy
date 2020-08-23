#pragma once
#include "GateBoyLib/Cells.h"

namespace Schematics {

struct SchematicTop;

//-----------------------------------------------------------------------------

struct SpriteStore {
  void tick(const SchematicTop& top);
  void tock(wire CLK, const SchematicTop& top);
  void dump(Dumper& d) const;

  /*p29.FEPO*/ Sig FEPO_STORE_MATCHp;
  /*p21.XENA*/ wire XENA_STORE_MATCHn() const { return not1(FEPO_STORE_MATCHp); }

  Tri SPR_TRI_I0p = TRI_HZPU;
  Tri SPR_TRI_I1p = TRI_HZPU;
  Tri SPR_TRI_I2p = TRI_HZPU;
  Tri SPR_TRI_I3p = TRI_HZPU;
  Tri SPR_TRI_I4p = TRI_HZPU;
  Tri SPR_TRI_I5p = TRI_HZPU;

  Tri SPR_TRI_L0 = TRI_HZPU;
  Tri SPR_TRI_L1 = TRI_HZPU;
  Tri SPR_TRI_L2 = TRI_HZPU;
  Tri SPR_TRI_L3 = TRI_HZPU;

//private:
//  friend struct SchematicTop;

  /*p29.DEZY*/ RegQP  DEZY_STORE_ENn = REG_D0C0;
  /*p29.BESE*/ RegQPN BESE_SPRITE_COUNT0 = REG_D0C0;
  /*p29.CUXY*/ RegQPN CUXY_SPRITE_COUNT1 = REG_D0C0;
  /*p29.BEGO*/ RegQPN BEGO_SPRITE_COUNT2 = REG_D0C0;
  /*p29.DYBE*/ RegQPN DYBE_SPRITE_COUNT3 = REG_D0C0;

  /*p30.XADU*/ RegQN XADU_SPRITE_IDX0p = REG_D0C0;
  /*p30.XEDY*/ RegQN XEDY_SPRITE_IDX1p = REG_D0C0;
  /*p30.ZUZE*/ RegQN ZUZE_SPRITE_IDX2p = REG_D0C0;
  /*p30.XOBE*/ RegQN XOBE_SPRITE_IDX3p = REG_D0C0;
  /*p30.YDUF*/ RegQN YDUF_SPRITE_IDX4p = REG_D0C0;
  /*p30.XECU*/ RegQN XECU_SPRITE_IDX5p = REG_D0C0;

  /*p29.YDUG*/ Sig YDUG_STORE0_MATCHn;
  /*p29.DYDU*/ Sig DYDU_STORE1_MATCHn;
  /*p29.DEGO*/ Sig DEGO_STORE2_MATCHn;
  /*p29.YLOZ*/ Sig YLOZ_STORE3_MATCHn;
  /*p29.XAGE*/ Sig XAGE_STORE4_MATCHn;
  /*p29.EGOM*/ Sig EGOM_STORE5_MATCHn;
  /*p29.YBEZ*/ Sig YBEZ_STORE6_MATCHn;
  /*p29.DYKA*/ Sig DYKA_STORE7_MATCHn;
  /*p29.EFYL*/ Sig EFYL_STORE8_MATCHn;
  /*p29.YGEM*/ Sig YGEM_STORE9_MATCHn;

  /*p29.EBOJ*/ RegQP EBOJ_STORE0_RSTp = REG_D0C0;
  /*p29.CEDY*/ RegQP CEDY_STORE1_RSTp = REG_D0C0;
  /*p29.EGAV*/ RegQP EGAV_STORE2_RSTp = REG_D0C0;
  /*p29.GOTA*/ RegQP GOTA_STORE3_RSTp = REG_D0C0;
  /*p29.XUDY*/ RegQP XUDY_STORE4_RSTp = REG_D0C0;
  /*p29.WAFY*/ RegQP WAFY_STORE5_RSTp = REG_D0C0;
  /*p29.WOMY*/ RegQP WOMY_STORE6_RSTp = REG_D0C0;
  /*p29.WAPO*/ RegQP WAPO_STORE7_RSTp = REG_D0C0;
  /*p29.EXUQ*/ RegQP EXUQ_STORE8_RSTp = REG_D0C0;
  /*p29.FONO*/ RegQP FONO_STORE9_RSTp = REG_D0C0;

  //----------

  /*p30.YGUS*/ RegQP YGUS_STORE0_I0n = REG_D0C0;
  /*p30.YSOK*/ RegQP YSOK_STORE0_I1n = REG_D0C0;
  /*p30.YZEP*/ RegQP YZEP_STORE0_I2n = REG_D0C0;
  /*p30.WYTE*/ RegQP WYTE_STORE0_I3n = REG_D0C0;
  /*p30.ZONY*/ RegQP ZONY_STORE0_I4n = REG_D0C0;
  /*p30.YWAK*/ RegQP YWAK_STORE0_I5n = REG_D0C0;

  /*p30.GYHO*/ RegQP GYHO_STORE0_L0 = REG_D0C0;
  /*p30.CUFO*/ RegQP CUFO_STORE0_L1 = REG_D0C0;
  /*p30.BOZU*/ RegQP BOZU_STORE0_L2 = REG_D0C0;
  /*p30.FYHY*/ RegQP FYHY_STORE0_L3 = REG_D0C0;

  /*p31.XEPE*/ RegQN XEPE_STORE0_X0p = REG_D0C0;
  /*p31.YLAH*/ RegQN YLAH_STORE0_X1p = REG_D0C0;
  /*p31.ZOLA*/ RegQN ZOLA_STORE0_X2p = REG_D0C0;
  /*p31.ZULU*/ RegQN ZULU_STORE0_X3p = REG_D0C0;
  /*p31.WELO*/ RegQN WELO_STORE0_X4p = REG_D0C0;
  /*p31.XUNY*/ RegQN XUNY_STORE0_X5p = REG_D0C0;
  /*p31.WOTE*/ RegQN WOTE_STORE0_X6p = REG_D0C0;
  /*p31.XAKO*/ RegQN XAKO_STORE0_X7p = REG_D0C0;

  //----------

  /*p30.CADU*/ RegQP CADU_STORE1_I0n = REG_D0C0;
  /*p30.CEBO*/ RegQP CEBO_STORE1_I1n = REG_D0C0;
  /*p30.CUFA*/ RegQP CUFA_STORE1_I2n = REG_D0C0;
  /*p30.COMA*/ RegQP COMA_STORE1_I3n = REG_D0C0;
  /*p30.CUZA*/ RegQP CUZA_STORE1_I4n = REG_D0C0;
  /*p30.CAJY*/ RegQP CAJY_STORE1_I5n = REG_D0C0;

  /*p30.AMES*/ RegQP AMES_STORE1_L0n = REG_D0C0;
  /*p30.AROF*/ RegQP AROF_STORE1_L1n = REG_D0C0;
  /*p30.ABOP*/ RegQP ABOP_STORE1_L2n = REG_D0C0;
  /*p30.ABUG*/ RegQP ABUG_STORE1_L3n = REG_D0C0;

  /*p31.DANY*/ RegQN DANY_STORE1_X0p = REG_D0C0;
  /*p31.DUKO*/ RegQN DUKO_STORE1_X1p = REG_D0C0;
  /*p31.DESU*/ RegQN DESU_STORE1_X2p = REG_D0C0;
  /*p31.DAZO*/ RegQN DAZO_STORE1_X3p = REG_D0C0;
  /*p31.DAKE*/ RegQN DAKE_STORE1_X4p = REG_D0C0;
  /*p31.CESO*/ RegQN CESO_STORE1_X5p = REG_D0C0;
  /*p31.DYFU*/ RegQN DYFU_STORE1_X6p = REG_D0C0;
  /*p31.CUSY*/ RegQN CUSY_STORE1_X7p = REG_D0C0;

  //----------

  /*p30.BUHE*/ RegQP BUHE_STORE2_I0n = REG_D0C0;
  /*p30.BYHU*/ RegQP BYHU_STORE2_I1n = REG_D0C0;
  /*p30.BECA*/ RegQP BECA_STORE2_I2n = REG_D0C0;
  /*p30.BULU*/ RegQP BULU_STORE2_I3n = REG_D0C0;
  /*p30.BUNA*/ RegQP BUNA_STORE2_I4n = REG_D0C0;
  /*p30.BOXA*/ RegQP BOXA_STORE2_I5n = REG_D0C0;

  /*p30.YLOV*/ RegQP YLOV_STORE2_L0n = REG_D0C0;
  /*p30.XOSY*/ RegQP XOSY_STORE2_L1n = REG_D0C0;
  /*p30.XAZY*/ RegQP XAZY_STORE2_L2n = REG_D0C0;
  /*p30.YKUK*/ RegQP YKUK_STORE2_L3n = REG_D0C0;

  /*p31.FOKA*/ RegQN FOKA_STORE2_X0p = REG_D0C0;
  /*p31.FYTY*/ RegQN FYTY_STORE2_X1p = REG_D0C0;
  /*p31.FUBY*/ RegQN FUBY_STORE2_X2p = REG_D0C0;
  /*p31.GOXU*/ RegQN GOXU_STORE2_X3p = REG_D0C0;
  /*p31.DUHY*/ RegQN DUHY_STORE2_X4p = REG_D0C0;
  /*p31.EJUF*/ RegQN EJUF_STORE2_X5p = REG_D0C0;
  /*p31.ENOR*/ RegQN ENOR_STORE2_X6p = REG_D0C0;
  /*p31.DEPY*/ RegQN DEPY_STORE2_X7p = REG_D0C0;

  //----------
  
  /*p30.DEVY*/ RegQP DEVY_STORE3_I0n = REG_D0C0;
  /*p30.DESE*/ RegQP DESE_STORE3_I1n = REG_D0C0;
  /*p30.DUNY*/ RegQP DUNY_STORE3_I2n = REG_D0C0;
  /*p30.DUHA*/ RegQP DUHA_STORE3_I3n = REG_D0C0;
  /*p30.DEBA*/ RegQP DEBA_STORE3_I4n = REG_D0C0;
  /*p30.DAFU*/ RegQP DAFU_STORE3_I5n = REG_D0C0;

  /*p30.ZURO*/ RegQP ZURO_STORE3_L0n = REG_D0C0;
  /*p30.ZYLU*/ RegQP ZYLU_STORE3_L1n = REG_D0C0;
  /*p30.ZENE*/ RegQP ZENE_STORE3_L2n = REG_D0C0;
  /*p30.ZURY*/ RegQP ZURY_STORE3_L3n = REG_D0C0;

  /*p31.XOLY*/ RegQN XOLY_STORE3_X0p = REG_D0C0;
  /*p31.XYBA*/ RegQN XYBA_STORE3_X1p = REG_D0C0;
  /*p31.XABE*/ RegQN XABE_STORE3_X2p = REG_D0C0;
  /*p31.XEKA*/ RegQN XEKA_STORE3_X3p = REG_D0C0;
  /*p31.XOMY*/ RegQN XOMY_STORE3_X4p = REG_D0C0;
  /*p31.WUHA*/ RegQN WUHA_STORE3_X5p = REG_D0C0;
  /*p31.WYNA*/ RegQN WYNA_STORE3_X6p = REG_D0C0;
  /*p31.WECO*/ RegQN WECO_STORE3_X7p = REG_D0C0;

  //----------

  /*p30.XAVE*/ RegQP XAVE_STORE4_I0n = REG_D0C0;
  /*p30.XEFE*/ RegQP XEFE_STORE4_I1n = REG_D0C0;
  /*p30.WANU*/ RegQP WANU_STORE4_I2n = REG_D0C0;
  /*p30.XABO*/ RegQP XABO_STORE4_I3n = REG_D0C0;
  /*p30.XEGE*/ RegQP XEGE_STORE4_I4n = REG_D0C0;
  /*p30.XYNU*/ RegQP XYNU_STORE4_I5n = REG_D0C0;

  /*p30.CAPO*/ RegQP CAPO_STORE4_L0n = REG_D0C0;
  /*p30.CAJU*/ RegQP CAJU_STORE4_L1n = REG_D0C0;
  /*p30.CONO*/ RegQP CONO_STORE4_L2n = REG_D0C0;
  /*p30.CUMU*/ RegQP CUMU_STORE4_L3n = REG_D0C0;
  
  /*p31.WEDU*/ RegQN WEDU_STORE4_X0p = REG_D0C0;
  /*p31.YGAJ*/ RegQN YGAJ_STORE4_X1p = REG_D0C0;
  /*p31.ZYJO*/ RegQN ZYJO_STORE4_X2p = REG_D0C0;
  /*p31.XURY*/ RegQN XURY_STORE4_X3p = REG_D0C0;
  /*p31.YBED*/ RegQN YBED_STORE4_X4p = REG_D0C0;
  /*p31.ZALA*/ RegQN ZALA_STORE4_X5p = REG_D0C0;
  /*p31.WYDE*/ RegQN WYDE_STORE4_X6p = REG_D0C0;
  /*p31.XEPA*/ RegQN XEPA_STORE4_X7p = REG_D0C0;

  //----------

  /*p30.EKOP*/ RegQP EKOP_STORE5_I0n = REG_D0C0;
  /*p30.ETYM*/ RegQP ETYM_STORE5_I1n = REG_D0C0;
  /*p30.GORU*/ RegQP GORU_STORE5_I2n = REG_D0C0;
  /*p30.EBEX*/ RegQP EBEX_STORE5_I3n = REG_D0C0;
  /*p30.ETAV*/ RegQP ETAV_STORE5_I4n = REG_D0C0;
  /*p30.EKAP*/ RegQP EKAP_STORE5_I5n = REG_D0C0;

  /*p30.ACEP*/ RegQP ACEP_STORE5_L0n = REG_D0C0;
  /*p30.ABEG*/ RegQP ABEG_STORE5_L1n = REG_D0C0;
  /*p30.ABUX*/ RegQP ABUX_STORE5_L2n = REG_D0C0;
  /*p30.ANED*/ RegQP ANED_STORE5_L3n = REG_D0C0;
  
  /*p31.FUSA*/ RegQN FUSA_STORE5_X0p = REG_D0C0;
  /*p31.FAXA*/ RegQN FAXA_STORE5_X1p = REG_D0C0;
  /*p31.FOZY*/ RegQN FOZY_STORE5_X2p = REG_D0C0;
  /*p31.FESY*/ RegQN FESY_STORE5_X3p = REG_D0C0;
  /*p31.CYWE*/ RegQN CYWE_STORE5_X4p = REG_D0C0;
  /*p31.DYBY*/ RegQN DYBY_STORE5_X5p = REG_D0C0;
  /*p31.DURY*/ RegQN DURY_STORE5_X6p = REG_D0C0;
  /*p31.CUVY*/ RegQN CUVY_STORE5_X7p = REG_D0C0;

  //----------

  /*p30.GABO*/ RegQP GABO_STORE6_I0n = REG_D0C0;
  /*p30.GACY*/ RegQP GACY_STORE6_I1n = REG_D0C0;
  /*p30.FOGO*/ RegQP FOGO_STORE6_I2n = REG_D0C0;
  /*p30.GOHU*/ RegQP GOHU_STORE6_I3n = REG_D0C0;
  /*p30.FOXY*/ RegQP FOXY_STORE6_I4n = REG_D0C0;
  /*p30.GECU*/ RegQP GECU_STORE6_I5n = REG_D0C0;

  /*p30.ZUMY*/ RegQP ZUMY_STORE6_L0n = REG_D0C0;
  /*p30.ZAFU*/ RegQP ZAFU_STORE6_L1n = REG_D0C0;
  /*p30.ZEXO*/ RegQP ZEXO_STORE6_L2n = REG_D0C0;
  /*p30.ZUBE*/ RegQP ZUBE_STORE6_L3n = REG_D0C0;

  /*p31.YCOL*/ RegQN YCOL_STORE6_X0p = REG_D0C0;
  /*p31.YRAC*/ RegQN YRAC_STORE6_X1p = REG_D0C0;
  /*p31.YMEM*/ RegQN YMEM_STORE6_X2p = REG_D0C0;
  /*p31.YVAG*/ RegQN YVAG_STORE6_X3p = REG_D0C0;
  /*p31.ZOLY*/ RegQN ZOLY_STORE6_X4p = REG_D0C0;
  /*p31.ZOGO*/ RegQN ZOGO_STORE6_X5p = REG_D0C0;
  /*p31.ZECU*/ RegQN ZECU_STORE6_X6p = REG_D0C0;
  /*p31.ZESA*/ RegQN ZESA_STORE6_X7p = REG_D0C0;

  //----------

  /*p30.GULE*/ RegQP GULE_STORE7_I0n = REG_D0C0;
  /*p30.GYNO*/ RegQP GYNO_STORE7_I1n = REG_D0C0;
  /*p30.FEFA*/ RegQP FEFA_STORE7_I2n = REG_D0C0;
  /*p30.FYSU*/ RegQP FYSU_STORE7_I3n = REG_D0C0;
  /*p30.GESY*/ RegQP GESY_STORE7_I4n = REG_D0C0;
  /*p30.FUZO*/ RegQP FUZO_STORE7_I5n = REG_D0C0;

  /*p30.XYNA*/ RegQP XYNA_STORE7_L0n = REG_D0C0;
  /*p30.YGUM*/ RegQP YGUM_STORE7_L1n = REG_D0C0;
  /*p30.XAKU*/ RegQP XAKU_STORE7_L2n = REG_D0C0;
  /*p30.XYGO*/ RegQP XYGO_STORE7_L3n = REG_D0C0;

  /*p31.ERAZ*/ RegQN ERAZ_STORE7_X0p = REG_D0C0;
  /*p31.EPUM*/ RegQN EPUM_STORE7_X1p = REG_D0C0;
  /*p31.EROL*/ RegQN EROL_STORE7_X2p = REG_D0C0;
  /*p31.EHYN*/ RegQN EHYN_STORE7_X3p = REG_D0C0;
  /*p31.FAZU*/ RegQN FAZU_STORE7_X4p = REG_D0C0;
  /*p31.FAXE*/ RegQN FAXE_STORE7_X5p = REG_D0C0;
  /*p31.EXUK*/ RegQN EXUK_STORE7_X6p = REG_D0C0;
  /*p31.FEDE*/ RegQN FEDE_STORE7_X7p = REG_D0C0;

  //----------

  /*p30.AXUV*/ RegQP AXUV_STORE8_I0n = REG_D0C0;
  /*p30.BADA*/ RegQP BADA_STORE8_I1n = REG_D0C0;
  /*p30.APEV*/ RegQP APEV_STORE8_I2n = REG_D0C0;
  /*p30.BADO*/ RegQP BADO_STORE8_I3n = REG_D0C0;
  /*p30.BEXY*/ RegQP BEXY_STORE8_I4n = REG_D0C0;
  /*p30.BYHE*/ RegQP BYHE_STORE8_I5n = REG_D0C0;

  /*p30.AZAP*/ RegQP AZAP_STORE8_L0n = REG_D0C0;
  /*p30.AFYX*/ RegQP AFYX_STORE8_L1n = REG_D0C0;
  /*p30.AFUT*/ RegQP AFUT_STORE8_L2n = REG_D0C0;
  /*p30.AFYM*/ RegQP AFYM_STORE8_L3n = REG_D0C0;

  /*p31.EZUF*/ RegQN EZUF_STORE8_X0p = REG_D0C0;
  /*p31.ENAD*/ RegQN ENAD_STORE8_X1p = REG_D0C0;
  /*p31.EBOW*/ RegQN EBOW_STORE8_X2p = REG_D0C0;
  /*p31.FYCA*/ RegQN FYCA_STORE8_X3p = REG_D0C0;
  /*p31.GAVY*/ RegQN GAVY_STORE8_X4p = REG_D0C0;
  /*p31.GYPU*/ RegQN GYPU_STORE8_X5p = REG_D0C0;
  /*p31.GADY*/ RegQN GADY_STORE8_X6p = REG_D0C0;
  /*p31.GAZA*/ RegQN GAZA_STORE8_X7p = REG_D0C0;

  //----------

  /*p30.YBER*/ RegQP YBER_STORE9_I0n = REG_D0C0;
  /*p30.YZOR*/ RegQP YZOR_STORE9_I1n = REG_D0C0;
  /*p30.XYFE*/ RegQP XYFE_STORE9_I2n = REG_D0C0;
  /*p30.XOTU*/ RegQP XOTU_STORE9_I3n = REG_D0C0;
  /*p30.XUTE*/ RegQP XUTE_STORE9_I4n = REG_D0C0;
  /*p30.XUFO*/ RegQP XUFO_STORE9_I5n = REG_D0C0;

  /*p30.CANA*/ RegQP CANA_STORE9_L0n = REG_D0C0;
  /*p30.FOFO*/ RegQP FOFO_STORE9_L1n = REG_D0C0;
  /*p30.DYSY*/ RegQP DYSY_STORE9_L2n = REG_D0C0;
  /*p30.DEWU*/ RegQP DEWU_STORE9_L3n = REG_D0C0;

  /*p31.XUVY*/ RegQN XUVY_STORE9_X0p = REG_D0C0;
  /*p31.XERE*/ RegQN XERE_STORE9_X1p = REG_D0C0;
  /*p31.XUZO*/ RegQN XUZO_STORE9_X2p = REG_D0C0;
  /*p31.XEXA*/ RegQN XEXA_STORE9_X3p = REG_D0C0;
  /*p31.YPOD*/ RegQN YPOD_STORE9_X4p = REG_D0C0;
  /*p31.YROP*/ RegQN YROP_STORE9_X5p = REG_D0C0;
  /*p31.YNEP*/ RegQN YNEP_STORE9_X6p = REG_D0C0;
  /*p31.YZOF*/ RegQN YZOF_STORE9_X7p = REG_D0C0;
};

//-----------------------------------------------------------------------------

}; // namespace Schematics