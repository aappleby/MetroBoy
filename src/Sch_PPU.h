#pragma once
#include "Cells.h"

namespace Schematics {

struct SchematicTop;

//-----------------------------------------------------------------------------

struct PpuRegisters {

  void tick(SchematicTop& top);
  void tock(SchematicTop& top);
  SignalHash commit(SchematicTop& top);

  /*p21.XYMU*/ wire XYMU_RENDERINGp() const { return _XYMU_RENDERINGp.q(); }
  /*p24.LOBY*/ wire LOBY_RENDERINGn() const { return not(_XYMU_RENDERINGp.q()); }
  /*p25.ROPY*/ wire ROPY_RENDERINGn() const { return not(_XYMU_RENDERINGp.q()); }
  /*p29.TEPA*/ wire TEPA_RENDERINGn() const { return not(_XYMU_RENDERINGp.q()); }

  /*p21.XUGU*/ wire XUGU_X_167n() const { return nand(XEHO_X0.q(), SAVY_X1.q(), XODU_X2.q(), TUKY_X5.q(), SYBE_X7.q()); } // 128 + 32 + 4 + 2 + 1 = 167
  /*p21.XANO*/ wire XANO_X_167p() const { return not(XUGU_X_167n()); }

  /*p27.ROZE*/ wire ROZE_FINE_COUNT_7n() const { return nand(RYKU_FINE_CNT0.q(), ROGA_FINE_CNT1.q(), RUBU_FINE_CNT2.q()); }

  /*p21.XEHO*/ Reg17 XEHO_X0;
  /*p21.SAVY*/ Reg17 SAVY_X1;
  /*p21.XODU*/ Reg17 XODU_X2;
  /*p21.XYDO*/ Reg17 XYDO_X3;
  /*p21.TUHU*/ Reg17 TUHU_X4;
  /*p21.TUKY*/ Reg17 TUKY_X5;
  /*p21.TAKO*/ Reg17 TAKO_X6;
  /*p21.SYBE*/ Reg17 SYBE_X7;

private:
  friend struct SchematicTop;

  /*p27.RYKU*/ Reg17 RYKU_FINE_CNT0;
  /*p27.ROGA*/ Reg17 ROGA_FINE_CNT1;
  /*p27.RUBU*/ Reg17 RUBU_FINE_CNT2;

  /*p24.PAHO*/ Reg17 PAHO_X_8_SYNC;
  /*p24.RUJU*/ NorLatch POFY_ST_LATCH; // nor latch with p24.RUJU, p24.POME
  /*p21.WUSA*/ NorLatch _WUSA_LCD_CLOCK_GATE;

  /*p??.ROXY*/ NorLatch ROXY_FINE_MATCH_LATCHn;
  /*p??.PUXA*/ Reg17 PUXA_FINE_MATCH_A;
  /*p27.NYZE*/ Reg17 NYZE_FINE_MATCH_B;

  /*p21.XYMU*/ NorLatch _XYMU_RENDERINGp; // this must be positive polarity, or stat read doesn't work

  /*p21.VOGA*/ Reg17 VOGA_RENDER_DONE_SYNC;

  /*p21.RUPO*/ NorLatch RUPO_LYC_MATCH_LATCHn;

  // FF41 - STAT
  /*p21.ROXE*/ Reg9 ROXE_INT_HBL_EN;
  /*p21.RUFO*/ Reg9 RUFO_INT_VBL_EN;
  /*p21.REFE*/ Reg9 REFE_INT_OAM_EN;
  /*p21.RUGU*/ Reg9 RUGU_INT_LYC_EN;
};

//-----------------------------------------------------------------------------

}; // namespace Schematics