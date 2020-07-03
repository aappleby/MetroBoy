#include "Sch_Serial.h"

#include "Sch_Top.h"

using namespace Schematics;

//------------------------------------------------------------------------------

SerialSignals SerialRegisters::sig(const SchematicTop& /*gb*/) const {
  return {
    .CALY_INT_SERIALp = CALY_INT_SERIALp,
  };
}

void SerialRegisters::tick(SchematicTop& gb) {

  auto cpu_sig = gb.cpu_bus.sig(gb);
  auto rst_sig = gb.rst_reg.sig(gb);
  auto tim_sig = gb.tim_reg.sig();
  auto& cpu_bus = gb.cpu_bus;

  wire TAPU_CPU_WR_xxxxxFGH = cpu_sig.TAPU_CPU_WR_xxxxxFGH;
  wire TEDO_CPU_RD = cpu_sig.TEDO_CPU_RD;

  //----------------------------------------

  /*p06.SANO*/ wire _ADDR_FF00_FF03 = and (cpu_sig.SARE_XX00_XX07p, cpu_sig.SEFY_A02n, cpu_sig.SYKE_FF00_FFFFp);
  /*p06.URYS*/ wire _FF01_WR_xxxxxFGHn = nand(TAPU_CPU_WR_xxxxxFGH, _ADDR_FF00_FF03, cpu_bus.PIN_A00, cpu_sig.TOLA_A01n);
  /*p06.DAKU*/ wire _FF01_WR_xxxxxFGHp = not (_FF01_WR_xxxxxFGHn);
  /*p06.UWAM*/ wire _FF02_WRn_xxxxxFGH = nand(TAPU_CPU_WR_xxxxxFGH, _ADDR_FF00_FF03, cpu_bus.PIN_A01, cpu_sig.TOVY_A00n);
  /*p06.UFEG*/ wire _FF01_RD = and (TEDO_CPU_RD, _ADDR_FF00_FF03, cpu_bus.PIN_A00, cpu_sig.TOLA_A01n);
  /*p06.UCOM*/ wire _FF02_RD = and (TEDO_CPU_RD, _ADDR_FF00_FF03, cpu_bus.PIN_A01, cpu_sig.TOVY_A00n);

  /*p06.COBA*/ wire _SER_CNT3n = not(CALY_INT_SERIALp.q());
  /*p01.ALUR*/ wire ALUR_RSTn = not(rst_sig.AVOR_RSTp);   // this goes all over the place
  /*p06.CABY*/ wire _XFER_RESET = and (_SER_CNT3n, ALUR_RSTn);
  /*p06.ETAF*/ XFER_START.set(_FF02_WRn_xxxxxFGH, _XFER_RESET, cpu_bus.TRI_D0);
  /*p06.CULY*/ XFER_DIR.set(_FF02_WRn_xxxxxFGH, ALUR_RSTn, cpu_bus.TRI_D1);

  /*p06.COTY*/ SER_CLK.set(tim_sig.UVYN_DIV_05n, _FF02_WRn_xxxxxFGH, !SER_CLK.q());

  // XFER_DIR 0 = ext clk
  // XFER DIR 1 = int clk

  // CAVE01 << CULY17 (xfer dir)
  // CAVE02 << COTY17 (mux 1 = int clk)
  // CAVE03 << SCK_C  (mux 0 = ext clk)
  // CAVE04 nc
  // CAVE05 >> DAWA01

  /*p06.CAVE*/ wire _SER_CLK_MUXn = mux2_n(SER_CLK.q(), SCK_C.q(), XFER_DIR.q());

  /*p06.DAWA*/ wire _DAWA_SER_CLK = or(_SER_CLK_MUXn, !XFER_START.q()); // this must stop the clock or something when the transmit's done
  /*p06.EDYL*/ wire _EDYL_SER_CLK = not(_DAWA_SER_CLK);
  /*p06.EPYT*/ wire _EPYT_SER_CLK = not(_EDYL_SER_CLK);
  /*p06.DEHO*/ wire _DEHO_SER_CLK = not(_EPYT_SER_CLK);
  /*p06.DAWE*/ wire _DAWE_SER_CLK = not(_DEHO_SER_CLK);

  /*p06.CARO*/ wire _SER_RST = and (_FF02_WRn_xxxxxFGH, ALUR_RSTn);
  /*p06.CAFA*/ SER_CNT0.set(_DAWA_SER_CLK, _SER_RST, !SER_CNT0.q());
  /*p06.CYLO*/ SER_CNT1.set(!SER_CNT0.q(), _SER_RST, !SER_CNT1.q());
  /*p06.CYDE*/ SER_CNT2.set(!SER_CNT1.q(), _SER_RST, !SER_CNT2.q());
  /*p06.CALY*/ CALY_INT_SERIALp.set(!SER_CNT2.q(), _SER_RST, !CALY_INT_SERIALp.q());

  /*p06.CUFU*/ wire _SER_DATA0_SETn = nand(cpu_bus.TRI_D0, _FF01_WR_xxxxxFGHp);
  /*p06.DOCU*/ wire _SER_DATA1_SETn = nand(cpu_bus.TRI_D1, _FF01_WR_xxxxxFGHp);
  /*p06.DELA*/ wire _SER_DATA2_SETn = nand(cpu_bus.TRI_D2, _FF01_WR_xxxxxFGHp);
  /*p06.DYGE*/ wire _SER_DATA3_SETn = nand(cpu_bus.TRI_D3, _FF01_WR_xxxxxFGHp);
  /*p06.DOLA*/ wire _SER_DATA4_SETn = nand(cpu_bus.TRI_D4, _FF01_WR_xxxxxFGHp);
  /*p06.ELOK*/ wire _SER_DATA5_SETn = nand(cpu_bus.TRI_D5, _FF01_WR_xxxxxFGHp);
  /*p06.EDEL*/ wire _SER_DATA6_SETn = nand(cpu_bus.TRI_D6, _FF01_WR_xxxxxFGHp);
  /*p06.EFEF*/ wire _SER_DATA7_SETn = nand(cpu_bus.TRI_D7, _FF01_WR_xxxxxFGHp);

  // COHY 5-rung
  // DUMO 5-rung
  // DYBO 5-rung
  // DAJU 5-rung
  // DYLY 5-rung
  // EHUJ 5-rung
  // EFAK 5-rung
  // EGUV 5-rung

  /*p06.COHY*/ wire _SER_DATA0_RSTn = or(and(_FF01_WR_xxxxxFGHn, cpu_bus.TRI_D0), ALUR_RSTn);
  /*p06.DUMO*/ wire _SER_DATA1_RSTn = or(and(_FF01_WR_xxxxxFGHn, cpu_bus.TRI_D1), ALUR_RSTn);
  /*p06.DYBO*/ wire _SER_DATA2_RSTn = or(and(_FF01_WR_xxxxxFGHn, cpu_bus.TRI_D2), ALUR_RSTn);
  /*p06.DAJU*/ wire _SER_DATA3_RSTn = or(and(_FF01_WR_xxxxxFGHn, cpu_bus.TRI_D3), ALUR_RSTn);
  /*p06.DYLY*/ wire _SER_DATA4_RSTn = or(and(_FF01_WR_xxxxxFGHn, cpu_bus.TRI_D4), ALUR_RSTn);
  /*p06.EHUJ*/ wire _SER_DATA5_RSTn = or(and(_FF01_WR_xxxxxFGHn, cpu_bus.TRI_D5), ALUR_RSTn);
  /*p06.EFAK*/ wire _SER_DATA6_RSTn = or(and(_FF01_WR_xxxxxFGHn, cpu_bus.TRI_D6), ALUR_RSTn);
  /*p06.EGUV*/ wire _SER_DATA7_RSTn = or(and(_FF01_WR_xxxxxFGHn, cpu_bus.TRI_D7), ALUR_RSTn);

  /*p06.CAGE*/ wire _SIN_Cn = not(SIN_C);
  /*p06.CUBA*/ SER_DATA0.set(_DAWE_SER_CLK, _SER_DATA0_SETn, _SER_DATA0_RSTn, _SIN_Cn);
  /*p06.DEGU*/ SER_DATA1.set(_DAWE_SER_CLK, _SER_DATA1_SETn, _SER_DATA1_RSTn, SER_DATA0.q());
  /*p06.DYRA*/ SER_DATA2.set(_DAWE_SER_CLK, _SER_DATA2_SETn, _SER_DATA2_RSTn, SER_DATA1.q());
  /*p06.DOJO*/ SER_DATA3.set(_DAWE_SER_CLK, _SER_DATA3_SETn, _SER_DATA3_RSTn, SER_DATA2.q());
  /*p06.DOVU*/ SER_DATA4.set(_EPYT_SER_CLK, _SER_DATA4_SETn, _SER_DATA4_RSTn, SER_DATA3.q());
  /*p06.EJAB*/ SER_DATA5.set(_EPYT_SER_CLK, _SER_DATA5_SETn, _SER_DATA5_RSTn, SER_DATA4.q());
  /*p06.EROD*/ SER_DATA6.set(_EPYT_SER_CLK, _SER_DATA6_SETn, _SER_DATA6_RSTn, SER_DATA5.q());
  /*p06.EDER*/ SER_DATA7.set(_EPYT_SER_CLK, _SER_DATA7_SETn, _SER_DATA7_RSTn, SER_DATA6.q());
  /*p06.ELYS*/ SER_OUT.set(_EDYL_SER_CLK, ALUR_RSTn, SER_DATA7.q());

  ///*p05.KENA*/ ser_reg.SOUT  = mux2n(DBG_FF00_D6, ser_reg.SER_OUT, FF60_0);
  /*p05.KENA*/ SOUT.set(SER_OUT.q());
  /*p06.KEXU*/ SCK_A.set(nand(SER_CLK.q(), XFER_DIR.q()));
  SCK_B.set(XFER_DIR.q());
  /*p06.KUJO*/ SCK_D.set(nor(SER_CLK.q(), /*p06.JAGO*/ not(XFER_DIR.q())));

  /*p06.CUGY*/ cpu_bus.TRI_D0.set_tribuf(_FF01_RD, SER_DATA0.q());
  /*p06.DUDE*/ cpu_bus.TRI_D1.set_tribuf(_FF01_RD, SER_DATA1.q());
  /*p06.DETU*/ cpu_bus.TRI_D2.set_tribuf(_FF01_RD, SER_DATA2.q());
  /*p06.DASO*/ cpu_bus.TRI_D3.set_tribuf(_FF01_RD, SER_DATA3.q());
  /*p06.DAME*/ cpu_bus.TRI_D4.set_tribuf(_FF01_RD, SER_DATA4.q());
  /*p06.EVOK*/ cpu_bus.TRI_D5.set_tribuf(_FF01_RD, SER_DATA5.q());
  /*p06.EFAB*/ cpu_bus.TRI_D6.set_tribuf(_FF01_RD, SER_DATA6.q());
  /*p06.ETAK*/ cpu_bus.TRI_D7.set_tribuf(_FF01_RD, SER_DATA7.q());

  /*p06.CORE*/ cpu_bus.TRI_D0.set_tribuf(_FF02_RD, XFER_DIR.q());
  /*p06.ELUV*/ cpu_bus.TRI_D1.set_tribuf(_FF02_RD, XFER_START.q());
}

//------------------------------------------------------------------------------

SignalHash SerialRegisters::commit() {
  SignalHash hash;

  /*p06.ETAF*/ hash << XFER_START.commit_reg();
  /*p06.CULY*/ hash << XFER_DIR.commit_reg();
  /*p06.COTY*/ hash << SER_CLK.commit_reg();
  /*p06.CAFA*/ hash << SER_CNT0.commit_reg();
  /*p06.CYLO*/ hash << SER_CNT1.commit_reg();
  /*p06.CYDE*/ hash << SER_CNT2.commit_reg();
  /*p06.CALY*/ hash << CALY_INT_SERIALp.commit_reg();
  /*p06.CUBA*/ hash << SER_DATA0.commit_reg();
  /*p06.DEGU*/ hash << SER_DATA1.commit_reg();
  /*p06.DYRA*/ hash << SER_DATA2.commit_reg();
  /*p06.DOJO*/ hash << SER_DATA3.commit_reg();
  /*p06.DOVU*/ hash << SER_DATA4.commit_reg();
  /*p06.EJAB*/ hash << SER_DATA5.commit_reg();
  /*p06.EROD*/ hash << SER_DATA6.commit_reg();
  /*p06.EDER*/ hash << SER_DATA7.commit_reg();
  /*p06.ELYS*/ hash << SER_OUT.commit_reg();

  /* PIN_68 */ hash << SCK_A.commit_pinout();   // <- P06.KEXU
  /* PIN_68 */ hash << SCK_B.commit_pinout();   // <- P06.CULY
  /* PIN_68 */ hash << SCK_C.clear_preset();   // -> P06.CAVE
  /* PIN_68 */ hash << SCK_D.commit_pinout();   // <- P06.KUJO
  ///* PIN_69 */ hash << SIN_A.commit();   // nc?
  ///* PIN_69 */ hash << SIN_B.commit();   // nc?
  /* PIN_69 */ hash << SIN_C.clear_preset();   // -> P06.CAGE
  ///* PIN_69 */ hash << SIN_D.commit();   // nc?
  /* PIN_70 */ hash << SOUT.commit_pinout();    // <- P05.KENA
  return hash;
}

//------------------------------------------------------------------------------

void SerialRegisters::dump_regs(TextPainter& text_painter) {
  text_painter.dprintf("----- SER_REG -----\n");
  text_painter.dprintf("SER_CLK    %d\n", SER_CLK.prev().val);
  text_painter.dprintf("XFER_START %d\n", XFER_START.prev().val);
  text_painter.dprintf("XFER_DIR   %d\n", XFER_DIR.prev().val);
  text_painter.dprintf("SER_OUT    %d\n", SER_OUT.prev().val);
  text_painter.dprintf("SER_CNT    %d\n", ser_cnt());
  text_painter.dprintf("SER_DATA   %d\n", ser_data());
  text_painter.newline();
}

void SerialRegisters::dump_pins(TextPainter& text_painter) {
  text_painter.dprintf("----- SER_PINS -----\n");
  text_painter.dprintf("SCK  %d:%d:%d:%d\n", SCK_A.prev().val, SCK_B.prev().val, SCK_C.prev().val, SCK_D.prev().val);
  text_painter.dprintf("SIN  %d:%d:%d:%d\n", SIN_A.prev().val, SIN_B.prev().val, SIN_C.prev().val, SIN_D.prev().val);
  text_painter.dprintf("SOUT %d\n", SOUT.prev().val);
  text_painter.newline();
}

//------------------------------------------------------------------------------