#pragma once
#include "Sch_Common.h"

namespace Schematics {

struct TestGB;

//-----------------------------------------------------------------------------

struct WindowSignals {
  /*p27.NOCU*/ wire NOCU_WIN_MODEn;
  /*p27.PORE*/ wire PORE_WIN_MODE;
  /*p27.NUNY*/ wire NUNY_WIN_MODE_TRIGp;
  /*p27.NYFO*/ wire NYFO_WIN_MODE_TRIGn;
  /*p27.MOSU*/ wire MOSU_WIN_MODE_TRIGp;

  /*p27.SYLO*/ wire SYLO_WIN_HITn;
  /*p24.TOMU*/ wire TOMU_WIN_HITp;
  /*p24.SOCY*/ wire SOCY_WIN_HITn;
  /*p27.TUKU*/ wire TUKU_WIN_HITn;
  /*p27.XOFO*/ wire XOFO_WIN_RSTp;
};

struct WindowRegisters {

  WindowSignals sig(const TestGB& gb) const;
  void tick(TestGB& gb);
  bool commit();

  /*p27.PYNU*/ NorLatch PYNU_WIN_MODE_LATCH;
  /*p27.RYDY*/ NorLatch RYDY_WIN_HIT_LATCH;

  /*p27.WYKA*/ Reg WIN_X3;
  /*p27.WODY*/ Reg WIN_X4;
  /*p27.WOBO*/ Reg WIN_X5;
  /*p27.WYKO*/ Reg WIN_X6;
  /*p27.XOLO*/ Reg WIN_X7;

  /*p27.TUFU*/ Reg WIN_Y3;
  /*p27.TAXA*/ Reg WIN_Y4;
  /*p27.TOZO*/ Reg WIN_Y5;
  /*p27.TATE*/ Reg WIN_Y6;
  /*p27.TEKE*/ Reg WIN_Y7;

  /*p27.VYNO*/ Reg WIN_Y0;
  /*p27.VUJO*/ Reg WIN_Y1;
  /*p27.VYMU*/ Reg WIN_Y2;

  /*p27.NOPA*/ Reg NOPA_WIN_MODE_SYNC;
  /*p27.SOVY*/ Reg SOVY_WIN_HIT_SYNC;
  /*p27.REJO*/ NorLatch REJO_WY_MATCH_LATCH;
  /*p27.SARY*/ Reg SARY_WIN_MATCH_Y_SYNC;
  /*p27.RYFA*/ Reg RYFA_WIN_MATCH_ONSCREEN_SYNC1;
  /*p27.RENE*/ Reg RENE_WIN_MATCH_ONSCREEN_SYNC2;
  /*p27.PYCO*/ Reg PYCO_WIN_MATCH_SYNC1;
  /*p27.NUNU*/ Reg NUNU_WIN_MATCH_SYNC2;
};


//-----------------------------------------------------------------------------

}; // namespace Schematics