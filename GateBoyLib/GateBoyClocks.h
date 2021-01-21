#pragma once
#include "GateBoyLib/Gates.h"

struct GateBoyResetDebug;

//-----------------------------------------------------------------------------

struct GateBoyClock {

  void reset_to_cart() {
    ANOS.state = 0b00011000;
    AVET.state = 0b00011001;
    AFUR_xxxxEFGHp.state = 0b00011010;
    ALEF_AxxxxFGHp.state = 0b00011001;
    APUK_ABxxxxGHp.state = 0b00011011;
    ADYK_ABCxxxxHp.state = 0b00011001;
    WUVU_ABxxEFxx.state = 0b00011011;
    VENA_xxCDEFxx.state = 0b00011000;
    WOSU_AxxDExxH.state = 0b00011001;
    SIG_CPU_CLKREQ.state = 0b00011001;
    SIG_CPU_BOWA_Axxxxxxx.state = 0b00011001;
    SIG_CPU_BEDO_xBCDEFGH.state = 0b00011000;
    SIG_CPU_BEKO_ABCDxxxx.state = 0b00011001;
    SIG_CPU_BUDE_xxxxEFGH.state = 0b00011000;
    SIG_CPU_BOLO_ABCDEFxx.state = 0b00011001;
    SIG_CPU_BUKE_AxxxxxGH.state = 0b00011001;
    SIG_CPU_BOMA_xBCDEFGH.state = 0b00011000;
    SIG_CPU_BOGA_Axxxxxxx.state = 0b00011001;
  }

  void dump(Dumper& d) {
    d.dump_bitp("AFUR_xxxxEFGHp : ", AFUR_xxxxEFGHp.state);
    d.dump_bitp("ALEF_AxxxxFGHp : ", ALEF_AxxxxFGHp.state);
    d.dump_bitp("APUK_ABxxxxGHp : ", APUK_ABxxxxGHp.state);
    d.dump_bitp("ADYK_ABCxxxxHp : ", ADYK_ABCxxxxHp.state);
    d("\n");
    d.dump_bitp("WUVU_ABxxEFxxp : ", WUVU_ABxxEFxx.state);
    d.dump_bitp("VENA_xxCDEFxxp : ", VENA_xxCDEFxx.state);
    d.dump_bitp("WOSU_AxxDExxHp : ", WOSU_AxxDExxH.state);
  }

  /* p01.ANOS*/ Gate ANOS; // NAND latch deglitcher
  /* p01.AVET*/ Gate AVET; // NAND latch deglitcher

  /* p01.AFUR*/ DFF9 AFUR_xxxxEFGHp;
  /* p01.ALEF*/ DFF9 ALEF_AxxxxFGHp;
  /* p01.APUK*/ DFF9 APUK_ABxxxxGHp;
  /* p01.ADYK*/ DFF9 ADYK_ABCxxxxHp;

  /* p29.WUVU*/ DFF17 WUVU_ABxxEFxx;
  /* p21.VENA*/ DFF17 VENA_xxCDEFxx;
  /* p29.WOSU*/ DFF17 WOSU_AxxDExxH;

  /*SIG_CPU_CLKREQ       */ SigIn  SIG_CPU_CLKREQ;
  /*SIG_CPU_BOWA_Axxxxxxx*/ SigOut SIG_CPU_BOWA_Axxxxxxx; // top left port PORTD_01: <- this is the "put address on bus" clock
  /*SIG_CPU_BEDO_xBCDEFGH*/ SigOut SIG_CPU_BEDO_xBCDEFGH; // top left port PORTD_02: <-
  /*SIG_CPU_BEKO_ABCDxxxx*/ SigOut SIG_CPU_BEKO_ABCDxxxx; // top left port PORTD_03: <- this is the "reset for next cycle" clock
  /*SIG_CPU_BUDE_xxxxEFGH*/ SigOut SIG_CPU_BUDE_xxxxEFGH; // top left port PORTD_04: <- this is the "put write data on bus" clock
  /*SIG_CPU_BOLO_ABCDEFxx*/ SigOut SIG_CPU_BOLO_ABCDEFxx; // top left port PORTD_05: <-
  /*SIG_CPU_BUKE_AxxxxxGH*/ SigOut SIG_CPU_BUKE_AxxxxxGH; // top left port PORTD_07: <- this is probably the "latch bus data" clock
  /*SIG_CPU_BOMA_xBCDEFGH*/ SigOut SIG_CPU_BOMA_xBCDEFGH; // top left port PORTD_08: <- (RESET_CLK) // These two clocks are the only ones that run before SIG_CPU_READYp is asserted.
  /*SIG_CPU_BOGA_Axxxxxxx*/ SigOut SIG_CPU_BOGA_Axxxxxxx; // top left port PORTD_09: <- test pad 3
};

//-----------------------------------------------------------------------------
