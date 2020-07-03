#pragma once
#include "Sch_Common.h"

namespace Schematics {

struct SchematicTop;

//-----------------------------------------------------------------------------

struct SpriteStoreSignals {
  /*p29.FEPO*/ bool FEPO_STORE_MATCHp;

  // to oam bus mux
  /*p28.GYBU*/ bool GYBU_IDX_0n; 
  /*p28.GYKA*/ bool GYKA_IDX_1n;
  /*p28.FABY*/ bool FABY_IDX_2n;
  /*p28.FACO*/ bool FACO_IDX_3n;
  /*p28.FUGU*/ bool FUGU_IDX_4n;
  /*p28.FYKE*/ bool FYKE_IDX_5n;

  // to ppu fetcher
  /*p30.WENU*/ bool WENU_TS_LINE_0; 
  /*p30.CUCU*/ bool CUCU_TS_LINE_1;
  /*p30.CUCA*/ bool CUCA_TS_LINE_2;
  /*p30.CEGA*/ bool CEGA_TS_LINE_3;

  // to sst.tick
  /*p29.DYTY*/ bool DYTY_STORE_ENn_xxCDxxGH;

  /*p29.YDUG*/ bool STORE0_MATCHn;
  /*p29.DYDU*/ bool STORE1_MATCHn;
  /*p29.DEGO*/ bool STORE2_MATCHn;
  /*p29.YLOZ*/ bool STORE3_MATCHn;
  /*p29.XAGE*/ bool STORE4_MATCHn;
  /*p29.EGOM*/ bool STORE5_MATCHn;
  /*p29.YBEZ*/ bool STORE6_MATCHn;
  /*p29.DYKA*/ bool STORE7_MATCHn;
  /*p29.EFYL*/ bool STORE8_MATCHn;
  /*p29.YGEM*/ bool STORE9_MATCHn;
};

//-----------------------------------------------------------------------------

struct SpriteStoreRegisters {

  SpriteStoreSignals sig(const SchematicTop& gb) const;
  void tick(SchematicTop& gb);
  SignalHash commit();

  void dump_regs(TextPainter& text_painter) {
    text_painter.dprintf("----- SPR_STORE -----\n");
    text_painter.dprintf("SPRITE_COUNT %d\n", pack(SPRITE_COUNT0.q(), SPRITE_COUNT1.q(), SPRITE_COUNT2.q(), SPRITE_COUNT3.q()));

    /*
    text_painter.dprintf("STORE0 %d %d %d %d\n",
                 SPRITE0_GET_SYNC.a.val,
                 pack(STORE0_X0,    STORE0_X1,    STORE0_X2,    STORE0_X3,   STORE0_X4,   STORE0_X5, STORE0_X6, STORE0_X7),
                 pack(STORE0_IDX0,  STORE0_IDX1,  STORE0_IDX2,  STORE0_IDX3, STORE0_IDX4, STORE0_IDX5),
                 pack(STORE0_LINE0, STORE0_LINE1, STORE0_LINE2, STORE0_LINE3));

    text_painter.dprintf("STORE1 %d %d %d %d\n",
                 SPRITE1_GET_SYNCn.a.val,
                 pack(STORE1_X0,    STORE1_X1,    STORE1_X2,    STORE1_X3,   STORE1_X4,   STORE1_X5, STORE1_X6, STORE1_X7),
                 pack(STORE1_IDX0,  STORE1_IDX1,  STORE1_IDX2,  STORE1_IDX3, STORE1_IDX4, STORE1_IDX5),
                 pack(STORE1_LINE0, STORE1_LINE1, STORE1_LINE2, STORE1_LINE3));

    text_painter.dprintf("STORE2 %d %d %d %d\n",
                 SPRITE2_GET_SYNCn.a.val,
                 pack(STORE2_X0,    STORE2_X1,    STORE2_X2,    STORE2_X3,   STORE2_X4,   STORE2_X5, STORE2_X6, STORE2_X7),
                 pack(STORE2_IDX0,  STORE2_IDX1,  STORE2_IDX2,  STORE2_IDX3, STORE2_IDX4, STORE2_IDX5),
                 pack(STORE2_LINE0, STORE2_LINE1, STORE2_LINE2, STORE2_LINE3));
    text_painter.dprintf("...\n");
    */
    text_painter.newline();
  }

private:

  /*p30.XADU*/ Reg13 XADU_SPRITE_IDX0;
  /*p30.XEDY*/ Reg13 XEDY_SPRITE_IDX1;
  /*p30.ZUZE*/ Reg13 ZUZE_SPRITE_IDX2;
  /*p30.XOBE*/ Reg13 XOBE_SPRITE_IDX3;
  /*p30.YDUF*/ Reg13 YDUF_SPRITE_IDX4;
  /*p30.XECU*/ Reg13 XECU_SPRITE_IDX5;

  /*p30.WUZY*/ Tribuf WUZY_TS_IDX_0;
  /*p30.WYSE*/ Tribuf WYSE_TS_IDX_1;
  /*p30.ZYSU*/ Tribuf ZYSU_TS_IDX_2;
  /*p30.WYDA*/ Tribuf WYDA_TS_IDX_3;
  /*p30.WUCO*/ Tribuf WUCO_TS_IDX_4;
  /*p30.WEZA*/ Tribuf WEZA_TS_IDX_5;

  /*p30.WENU*/ Tribuf WENU_TS_LINE_0;
  /*p30.CUCU*/ Tribuf CUCU_TS_LINE_1;
  /*p30.CUCA*/ Tribuf CUCA_TS_LINE_2;
  /*p30.CEGA*/ Tribuf CEGA_TS_LINE_3;

  /*p29.DEZY*/ Reg DEZY_STORE_ENn_SYNC;
  /*p29.BESE*/ Reg SPRITE_COUNT0;
  /*p29.CUXY*/ Reg SPRITE_COUNT1;
  /*p29.BEGO*/ Reg SPRITE_COUNT2;
  /*p29.DYBE*/ Reg SPRITE_COUNT3;

  //----------

  /*p29.EBOJ*/ Reg17 EBOJ_STORE0_RSTp;
  /*p30.YGUS*/ Reg8 YGUS_STORE0_IDX5;
  /*p30.YSOK*/ Reg8 YSOK_STORE0_IDX4;
  /*p30.YZEP*/ Reg8 YZEP_STORE0_IDX3;
  /*p30.WYTE*/ Reg8 WYTE_STORE0_IDX2;
  /*p30.ZONY*/ Reg8 ZONY_STORE0_IDX1;
  /*p30.YWAK*/ Reg8 YWAK_STORE0_IDX0;
  /*p30.FYHY*/ Reg8 STORE0_LINE0;
  /*p30.GYHO*/ Reg8 STORE0_LINE1;
  /*p30.BOZU*/ Reg8 STORE0_LINE2;
  /*p30.CUFO*/ Reg8 STORE0_LINE3;
  /*p31.XEPE*/ Reg9 STORE0_X0;
  /*p31.YLAH*/ Reg9 STORE0_X1;
  /*p31.ZOLA*/ Reg9 STORE0_X2;
  /*p31.ZULU*/ Reg9 STORE0_X3;
  /*p31.WELO*/ Reg9 STORE0_X4;
  /*p31.XUNY*/ Reg9 STORE0_X5;
  /*p31.WOTE*/ Reg9 STORE0_X6;
  /*p31.XAKO*/ Reg9 STORE0_X7;

  //----------

  /*p29.CEDY*/ Reg17 CEDY_STORE1_RSTp;
  /*p30.CAJY*/ Reg8 STORE1_IDX0;
  /*p30.CUZA*/ Reg8 STORE1_IDX1;
  /*p30.COMA*/ Reg8 STORE1_IDX2;
  /*p30.CUFA*/ Reg8 STORE1_IDX3;
  /*p30.CEBO*/ Reg8 STORE1_IDX4;
  /*p30.CADU*/ Reg8 STORE1_IDX5;
  /*p30.ABUG*/ Reg8 STORE1_LINE0;
  /*p30.AMES*/ Reg8 STORE1_LINE1;
  /*p30.ABOP*/ Reg8 STORE1_LINE2;
  /*p30.AROF*/ Reg8 STORE1_LINE3;
  /*p31.DANY*/ Reg9 STORE1_X0;
  /*p31.DUKO*/ Reg9 STORE1_X1;
  /*p31.DESU*/ Reg9 STORE1_X2;
  /*p31.DAZO*/ Reg9 STORE1_X3;
  /*p31.DAKE*/ Reg9 STORE1_X4;
  /*p31.CESO*/ Reg9 STORE1_X5;
  /*p31.DYFU*/ Reg9 STORE1_X6;
  /*p31.CUSY*/ Reg9 STORE1_X7;

  /*p29.EGAV*/ Reg17 EGAV_STORE2_RSTp;
  /*p30.BOXA*/ Reg8 STORE2_IDX0;
  /*p30.BUNA*/ Reg8 STORE2_IDX1;
  /*p30.BULU*/ Reg8 STORE2_IDX2;
  /*p30.BECA*/ Reg8 STORE2_IDX3;
  /*p30.BYHU*/ Reg8 STORE2_IDX4;
  /*p30.BUHE*/ Reg8 STORE2_IDX5;
  /*p30.YKUK*/ Reg8 STORE2_LINE0;
  /*p30.YLOV*/ Reg8 STORE2_LINE1;
  /*p30.XAZY*/ Reg8 STORE2_LINE2;
  /*p30.XOSY*/ Reg8 STORE2_LINE3;
  /*p31.FOKA*/ Reg9 STORE2_X0;
  /*p31.FYTY*/ Reg9 STORE2_X1;
  /*p31.FUBY*/ Reg9 STORE2_X2;
  /*p31.GOXU*/ Reg9 STORE2_X3;
  /*p31.DUHY*/ Reg9 STORE2_X4;
  /*p31.EJUF*/ Reg9 STORE2_X5;
  /*p31.ENOR*/ Reg9 STORE2_X6;
  /*p31.DEPY*/ Reg9 STORE2_X7;

  /*p29.GOTA*/ Reg17 GOTA_STORE3_RSTp;
  /*p30.DAFU*/ Reg8 STORE3_IDX0;
  /*p30.DEBA*/ Reg8 STORE3_IDX1;
  /*p30.DUHA*/ Reg8 STORE3_IDX2;
  /*p30.DUNY*/ Reg8 STORE3_IDX3;
  /*p30.DESE*/ Reg8 STORE3_IDX4;
  /*p30.DEVY*/ Reg8 STORE3_IDX5;
  /*p30.ZURY*/ Reg8 STORE3_LINE0;
  /*p30.ZURO*/ Reg8 STORE3_LINE1;
  /*p30.ZENE*/ Reg8 STORE3_LINE2;
  /*p30.ZYLU*/ Reg8 STORE3_LINE4;
  /*p31.XOLY*/ Reg9 STORE3_X0;
  /*p31.XYBA*/ Reg9 STORE3_X1;
  /*p31.XABE*/ Reg9 STORE3_X2;
  /*p31.XEKA*/ Reg9 STORE3_X3;
  /*p31.XOMY*/ Reg9 STORE3_X4;
  /*p31.WUHA*/ Reg9 STORE3_X5;
  /*p31.WYNA*/ Reg9 STORE3_X6;
  /*p31.WECO*/ Reg9 STORE3_X7;

  /*p29.XUDY*/ Reg17 XUDY_STORE4_RSTp;
  /*p30.XYNU*/ Reg8 XYNU;
  /*p30.XEGE*/ Reg8 XEGE;
  /*p30.XABO*/ Reg8 XABO;
  /*p30.WANU*/ Reg8 WANU;
  /*p30.XEFE*/ Reg8 XEFE;
  /*p30.XAVE*/ Reg8 XAVE;
  /*p30.CUMU*/ Reg8 CUMU;
  /*p30.CAPO*/ Reg8 CAPO;
  /*p30.CONO*/ Reg8 CONO;
  /*p30.CAJU*/ Reg8 CAJU;
  /*p31.WEDU*/ Reg9 STORE4_X0;
  /*p31.YGAJ*/ Reg9 STORE4_X1;
  /*p31.ZYJO*/ Reg9 STORE4_X2;
  /*p31.XURY*/ Reg9 STORE4_X3;
  /*p31.YBED*/ Reg9 STORE4_X4;
  /*p31.ZALA*/ Reg9 STORE4_X5;
  /*p31.WYDE*/ Reg9 STORE4_X6;
  /*p31.XEPA*/ Reg9 STORE4_X7;

  // store 5
  /*p29.WAFY*/ Reg17 WAFY_STORE5_RSTp;
  /*p30.EKAP*/ Reg8 EKAP;
  /*p30.ETAV*/ Reg8 ETAV;
  /*p30.EBEX*/ Reg8 EBEX;
  /*p30.GORU*/ Reg8 GORU;
  /*p30.ETYM*/ Reg8 ETYM;
  /*p30.EKOP*/ Reg8 EKOP;
  /*p30.ANED*/ Reg8 ANED;
  /*p30.ACEP*/ Reg8 ACEP;
  /*p30.ABUX*/ Reg8 ABUX;
  /*p30.ABEG*/ Reg8 ABEG;
  /*p31.FUSA*/ Reg9 STORE5_X0;
  /*p31.FAXA*/ Reg9 STORE5_X1;
  /*p31.FOZY*/ Reg9 STORE5_X2;
  /*p31.FESY*/ Reg9 STORE5_X3;
  /*p31.CYWE*/ Reg9 STORE5_X4;
  /*p31.DYBY*/ Reg9 STORE5_X5;
  /*p31.DURY*/ Reg9 STORE5_X6;
  /*p31.CUVY*/ Reg9 STORE5_X7;

  // sprite store 6
  /*p29.WOMY*/ Reg17 WOMY_STORE6_RSTp;
  /*p30.GECU*/ Reg8 GECU;
  /*p30.FOXY*/ Reg8 FOXY;
  /*p30.GOHU*/ Reg8 GOHU;
  /*p30.FOGO*/ Reg8 FOGO;
  /*p30.GACY*/ Reg8 GACY;
  /*p30.GABO*/ Reg8 GABO;
  /*p30.ZUBE*/ Reg8 ZUBE;
  /*p30.ZUMY*/ Reg8 ZUMY;
  /*p30.ZEXO*/ Reg8 ZEXO;
  /*p30.ZAFU*/ Reg8 ZAFU;
  /*p31.YCOL*/ Reg9 STORE6_X0;
  /*p31.YRAC*/ Reg9 STORE6_X1;
  /*p31.YMEM*/ Reg9 STORE6_X2;
  /*p31.YVAG*/ Reg9 STORE6_X3;
  /*p31.ZOLY*/ Reg9 STORE6_X4;
  /*p31.ZOGO*/ Reg9 STORE6_X5;
  /*p31.ZECU*/ Reg9 STORE6_X6;
  /*p31.ZESA*/ Reg9 STORE6_X7;

  // sprite store 7
  /*p29.WAPO*/ Reg17 WAPO_STORE7_RSTp;
  /*p30.FUZO*/ Reg8 FUZO;
  /*p30.GESY*/ Reg8 GESY;
  /*p30.FYSU*/ Reg8 FYSU;
  /*p30.FEFA*/ Reg8 FEFA;
  /*p30.GYNO*/ Reg8 GYNO;
  /*p30.GULE*/ Reg8 GULE;
  /*p30.XYGO*/ Reg8 XYGO;
  /*p30.XYNA*/ Reg8 XYNA;
  /*p30.XAKU*/ Reg8 XAKU;
  /*p30.YGUM*/ Reg8 YGUM;
  /*p31.ERAZ*/ Reg9 STORE7_X0;
  /*p31.EPUM*/ Reg9 STORE7_X1;
  /*p31.EROL*/ Reg9 STORE7_X2;
  /*p31.EHYN*/ Reg9 STORE7_X3;
  /*p31.FAZU*/ Reg9 STORE7_X4;
  /*p31.FAXE*/ Reg9 STORE7_X5;
  /*p31.EXUK*/ Reg9 STORE7_X6;
  /*p31.FEDE*/ Reg9 STORE7_X7;

  // sprite store 8
  /*p29.EXUQ*/ Reg17 EXUQ_STORE8_RSTp;
  /*p30.AXUV*/ Reg8 STORE8_IDX5;
  /*p30.BADA*/ Reg8 STORE8_IDX4;
  /*p30.APEV*/ Reg8 STORE8_IDX3;
  /*p30.BADO*/ Reg8 STORE8_IDX2;
  /*p30.BEXY*/ Reg8 STORE8_IDX1;
  /*p30.BYHE*/ Reg8 STORE8_IDX0;
  /*p30.AFYM*/ Reg8 STORE8_LINE0;
  /*p30.AZAP*/ Reg8 STORE8_LINE1;
  /*p30.AFUT*/ Reg8 STORE8_LINE2;
  /*p30.AFYX*/ Reg8 STORE8_LINE3;
  /*p31.GAVY*/ Reg9 STORE8_X4;
  /*p31.GYPU*/ Reg9 STORE8_X5;
  /*p31.GADY*/ Reg9 STORE8_X6;
  /*p31.GAZA*/ Reg9 STORE8_X7;
  /*p31.EZUF*/ Reg9 STORE8_X0;
  /*p31.ENAD*/ Reg9 STORE8_X1;
  /*p31.EBOW*/ Reg9 STORE8_X2;
  /*p31.FYCA*/ Reg9 STORE8_X3;

  // sprite store 9
  /*p29.FONO*/ Reg17 FONO_STORE9_RSTp;
  /*p30.XUFO*/ Reg8 STORE9_IDX0;
  /*p30.XUTE*/ Reg8 STORE9_IDX1;
  /*p30.XOTU*/ Reg8 STORE9_IDX2;
  /*p30.XYFE*/ Reg8 STORE9_IDX3;
  /*p30.YZOR*/ Reg8 STORE9_IDX4;
  /*p30.YBER*/ Reg8 STORE9_IDX5;
  /*p30.DEWU*/ Reg8 STORE9_LINE0;
  /*p30.CANA*/ Reg8 STORE9_LINE1;
  /*p30.DYSY*/ Reg8 STORE9_LINE2;
  /*p30.FOFO*/ Reg8 STORE9_LINE3;
  /*p31.XUVY*/ Reg9 STORE9_X0;
  /*p31.XERE*/ Reg9 STORE9_X1;
  /*p31.XUZO*/ Reg9 STORE9_X2;
  /*p31.XEXA*/ Reg9 STORE9_X3;
  /*p31.YPOD*/ Reg9 STORE9_X4;
  /*p31.YROP*/ Reg9 STORE9_X5;
  /*p31.YNEP*/ Reg9 STORE9_X6;
  /*p31.YZOF*/ Reg9 STORE9_X7;
};

//-----------------------------------------------------------------------------

}; // namespace Schematics