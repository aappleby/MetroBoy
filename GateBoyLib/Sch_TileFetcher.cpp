#include "GateBoyLib/Sch_TileFetcher.h"
#include "GateBoyLib/Sch_Top.h"
#include "GateBoyLib/Probe.h"

using namespace Schematics;

#pragma warning(disable:4189)

//------------------------------------------------------------------------------

void TileFetcher::dump(Dumper& d, const SchematicTop& /*top*/) const {
  d("\002=====TileFetcher=====\001\n");
  d("LAXU_BFETCH_S0           %c\n", LAXU_BFETCH_S0.c());
  d("MESU_BFETCH_S1           %c\n", MESU_BFETCH_S1.c());
  d("NYVA_BFETCH_S2           %c\n", NYVA_BFETCH_S2.c());
  d("LYZU_BFETCH_S0_D1        %c\n", LYZU_BFETCH_S0_D1.c());
  d("\n");
  d("NYKA_FETCH_DONE_P11      %c\n", NYKA_FETCH_DONE_P11.c());
  d("PORY_FETCH_DONE_P12      %c\n", PORY_FETCH_DONE_P12.c());
  d("PYGO_FETCH_DONE_P13      %c\n", PYGO_FETCH_DONE_P13.c());
  d("POKY_PRELOAD_DONEp       %c\n", POKY_PRELOAD_LATCHp.c());
  d("\n");
  d("LONY_FETCH_RUNNINGp      %c\n", LONY_BG_FETCH_RUNNINGp.c()); // 1 for phases 0-11, 0 for 12-15
  d("LOVY_FETCH_DONEp         %c\n", LOVY_BG_FETCH_DONEp.c());    // 0 for phases 0-11, 1 for 12-15
  d("\n");
}

//------------------------------------------------------------------------------

void TileFetcher::tick(const SchematicTop& top) {
  /* p26.AXAD*/ wire _AXAD_WIN_MODEn = not1(top.pix_pipe.PORE_WIN_MODEp);

  /*#p27.LAXE*/ wire _LAXE_BFETCH_S0n = not1(LAXU_BFETCH_S0.qp17());
  /*#p27.NAKO*/ wire _NAKO_BFETCH_S1n = not1(MESU_BFETCH_S1.qp17());
  /*#p27.NOFU*/ wire _NOFU_BFETCH_S2n = not1(NYVA_BFETCH_S2.qp17());

  /* p27.NOGU*/ wire _NOGU_FETCH_01p = nand2(_NAKO_BFETCH_S1n, _NOFU_BFETCH_S2n);
  /* p27.NENY*/ wire _NENY_FETCH_01n = not1(_NOGU_FETCH_01p);

  /* p27.LUSU*/ wire _LUSU_BGW_VRAM_RDn = not1(LONY_BG_FETCH_RUNNINGp.qp03());
  /* p27.LENA*/ LENA_BGW_VRM_RDp = not1(_LUSU_BGW_VRAM_RDn);

  /* p27.POTU*/ wire _POTU_BG_MAP_READp  = and2(LENA_BGW_VRM_RDp, _NENY_FETCH_01n);
  /*#p25.XEZE*/ wire _XEZE_WIN_MAP_READp = and2(_POTU_BG_MAP_READp, top.pix_pipe.PORE_WIN_MODEp);
  /* p26.ACEN*/ wire _ACEN_BG_MAP_READp  = and2(_POTU_BG_MAP_READp, _AXAD_WIN_MODEn);

  /* p27.NETA*/ NETA_TILE_READp = and2(LENA_BGW_VRM_RDp, _NOGU_FETCH_01p);
  /* p26.ASUL*/ wire _ASUL_BG_TILE_READp  = and2(NETA_TILE_READp, _AXAD_WIN_MODEn);
  /* p25.XUCY*/ XUCY_WIN_TILE_READn = nand2(NETA_TILE_READp, top.pix_pipe.PORE_WIN_MODEp);
  /*#p25.WUKO*/ WUKO_WIN_MAP_READn  = not1(_XEZE_WIN_MAP_READp);
  /* p26.BAFY*/ BAFY_BG_MAP_READn   = not1(_ACEN_BG_MAP_READp);
  /* p26.BEJE*/ BEJE_BG_TILE_READn = not1(_ASUL_BG_TILE_READp);

  /* p27.MOCE*/ MOCE_BFETCH_DONEn = nand3(LAXU_BFETCH_S0.qp17(), NYVA_BFETCH_S2.qp17(), top.pix_pipe.NYXU_FETCH_TRIGn);

  /*#p27.XUHA*/ XUHA_FETCH_S2p  = not1(_NOFU_BFETCH_S2n);

  // MYSO fires on fetch phase 2, 6, 10
  /*#p27.MYSO*/ wire _MYSO_BG_TRIGp   = nor3(top.pix_pipe.LOBY_RENDERINGn, _LAXE_BFETCH_S0n, LYZU_BFETCH_S0_D1.qp17());

  // NYDY on fetch phase 6
  /*#p27.NYDY*/ wire _NYDY_LATCH_TILE_DAn = nand3(_MYSO_BG_TRIGp, MESU_BFETCH_S1.qp17(), _NOFU_BFETCH_S2n);
  /*#p32.METE*/ METE_LATCH_TILE_DAp   = not1(_NYDY_LATCH_TILE_DAn);

  // MOFU fires on fetch phase 2 and 10
  /*p27.MOFU*/ wire _MOFU_LATCH_TILE_DBp = and2(_MYSO_BG_TRIGp, _NAKO_BFETCH_S1n);
  /*p32.LESO*/ LESO_LATCH_TILE_DBn   = not1(_MOFU_LATCH_TILE_DBp);

  /*p27.LYRY*/ LYRY_BFETCH_DONEp = not1(MOCE_BFETCH_DONEn);
}

//------------------------------------------------------------------------------

void TileFetcher::tock(SchematicTop& top) {
  /*p27.LEBO*/ wire _LEBO_AxCxExGx = nand2(top.clk_reg.ALET_xBxDxFxH, MOCE_BFETCH_DONEn);

  /*p27.LAXU*/ LAXU_BFETCH_S0   .dff17(_LEBO_AxCxExGx,            top.pix_pipe.NYXU_FETCH_TRIGn,       LAXU_BFETCH_S0.qn16());
  /*p27.MESU*/ MESU_BFETCH_S1   .dff17(LAXU_BFETCH_S0.qn16(),     top.pix_pipe.NYXU_FETCH_TRIGn,       MESU_BFETCH_S1.qn16());
  /*p27.NYVA*/ NYVA_BFETCH_S2   .dff17(MESU_BFETCH_S1.qn16(),     top.pix_pipe.NYXU_FETCH_TRIGn,       NYVA_BFETCH_S2.qn16());
  /*p27.LYZU*/ LYZU_BFETCH_S0_D1.dff17(top.clk_reg.ALET_xBxDxFxH, top.pix_pipe.XYMU_RENDERINGn.qn03(), LAXU_BFETCH_S0.qp17());

  /*p24.NAFY*/ wire _NAFY_RENDERING_AND_NOT_WIN_TRIG = nor2(top.pix_pipe.MOSU_WIN_FETCH_TRIGp, top.pix_pipe.LOBY_RENDERINGn);

  /*p24.NYKA*/ NYKA_FETCH_DONE_P11.dff17(top.clk_reg.ALET_xBxDxFxH, _NAFY_RENDERING_AND_NOT_WIN_TRIG, LYRY_BFETCH_DONEp);
  /*p24.PORY*/ PORY_FETCH_DONE_P12.dff17(top.clk_reg.MYVO_AxCxExGx, _NAFY_RENDERING_AND_NOT_WIN_TRIG, NYKA_FETCH_DONE_P11.qp17());
  /*p24.PYGO*/ PYGO_FETCH_DONE_P13.dff17(top.clk_reg.ALET_xBxDxFxH, top.pix_pipe.XYMU_RENDERINGn.qn03(),   PORY_FETCH_DONE_P12.qp17());

  /*p24.POKY*/ POKY_PRELOAD_LATCHp.nor_latch(PYGO_FETCH_DONE_P13.qp17(), top.pix_pipe.LOBY_RENDERINGn);

  /*p27.LOVY*/ LOVY_BG_FETCH_DONEp  .dff17(top.clk_reg.MYVO_AxCxExGx, top.pix_pipe.NYXU_FETCH_TRIGn,     LYRY_BFETCH_DONEp);
  /*p27.LURY*/ wire _LURY_BG_FETCH_DONEn = and2(LOVY_BG_FETCH_DONEp.qn16(), top.pix_pipe.XYMU_RENDERINGn.qn03());
  /*p27.LONY*/ LONY_BG_FETCH_RUNNINGp.nand_latch(top.pix_pipe.NYXU_FETCH_TRIGn, _LURY_BG_FETCH_DONEn);
}

//------------------------------------------------------------------------------
