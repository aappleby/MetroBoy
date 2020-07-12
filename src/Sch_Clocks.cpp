#include "Sch_Clocks.h"
#include "Sch_Top.h"

using namespace Schematics;

//-----------------------------------------------------------------------------
void ClockRegisters::tick(SchematicTop& /*top*/) {
}

void ClockRegisters::tock(SchematicTop& top) {
  // ignoring the deglitcher here

  {
    wire AFUR_xBCDExxx_ = _AFUR_ABCDxxxx.q();
    wire ALEF_xxCDEFxx_ = _ALEF_xBCDExxx.q();
    wire APUK_xxxDEFGx_ = _APUK_xxCDEFxx.q();
    wire ADYK_xxxxEFGH_ = _ADYK_xxxDEFGx.q();

    // the comp clock is unmarked on the die trace but it's directly to the left of ATAL

    /*p01.AFUR*/ _AFUR_ABCDxxxx.set(!ATAL_xBxDxFxH(),  ATAL_xBxDxFxH(), top.UPOJ_MODE_PRODn(), !ADYK_xxxxEFGH_);
    /*p01.ALEF*/ _ALEF_xBCDExxx.set( ATAL_xBxDxFxH(), !ATAL_xBxDxFxH(), top.UPOJ_MODE_PRODn(),  AFUR_xBCDExxx_);
    /*p01.APUK*/ _APUK_xxCDEFxx.set(!ATAL_xBxDxFxH(),  ATAL_xBxDxFxH(), top.UPOJ_MODE_PRODn(),  ALEF_xxCDEFxx_);
    /*p01.ADYK*/ _ADYK_xxxDEFGx.set( ATAL_xBxDxFxH(), !ATAL_xBxDxFxH(), top.UPOJ_MODE_PRODn(),  APUK_xxxDEFGx_);
  }

  {
    /*p29.XYVA*/ wire _XYVA_xBxDxFxH = not(ZEME_AxCxExGx());
    /*p29.XOTA*/ wire _XOTA_AxCxExGx = not(_XYVA_xBxDxFxH);
    /*p29.XYFY*/ wire _XYFY_xBxDxFxH = not(_XOTA_AxCxExGx);

    wire WUVU_xxCDxxGH_ = _WUVU_xxCDxxGH.q();
    wire VENA_xxxxEFGH_ = _VENA_xxxxEFGH.q();

    /*p29.WUVU*/ _WUVU_xxCDxxGH.set( _XOTA_AxCxExGx, top.XAPO_VID_RSTn(), !WUVU_xxCDxxGH_);
    /*p21.VENA*/ _VENA_xxxxEFGH.set(!WUVU_xxCDxxGH_, top.XAPO_VID_RSTn(), !VENA_xxxxEFGH_);
    /*p29.WOSU*/ _WOSU_xBCxxFGx.set( _XYFY_xBxDxFxH, top.XAPO_VID_RSTn(), !WUVU_xxCDxxGH_);
  }

  {
    /*p01.ABOL*/ wire ABOL_CLKREQn  = not(CPU_PIN_READYp);
    /*p01.BUTY*/ wire BUTY_CLKREQ   = not(ABOL_CLKREQn);
    /*p01.BUVU*/ wire BUVU_Axxxxxxx = and(BUTY_CLKREQ, BALY_Axxxxxxx());
    /*p01.BYXO*/ wire BYXO_xBCDEFGH = not(BUVU_Axxxxxxx);
    /*p01.BEDO*/ wire BEDO_Axxxxxxx = not(BYXO_xBCDEFGH);
    /*p01.BOWA*/ wire BOWA_xBCDEFGH = not(BEDO_Axxxxxxx);
    CPU_PIN_BOWA_AxCDEFGH.set(BOWA_xBCDEFGH);
    CPU_PIN_BEDO_xBxxxxxx.set(BEDO_Axxxxxxx);
  }
    
  CPU_PIN_BEKO_xBCDExxx.set(BEKO_ABCDxxxx());
  CPU_PIN_BUDE_AxxxxFGH.set(BUDE_xxxxEFGH());
  CPU_PIN_BOLO_xBCDEFGx.set(BOLO_ABCDEFxx());
    
  {
    /*p01.ABOL*/ wire ABOL_CLKREQn  = not(CPU_PIN_READYp);
    /*p01.AROV*/ wire AROV_xxCDEFxx = not(!APUK_xxCDEFxx());
    /*p01.AFEP*/ wire AFEP_AxxxxFGH = not(ALEF_xBCDExxx());
    /*p01.BUGO*/ wire BUGO_xBCDExxx = not(AFEP_AxxxxFGH);
    /*p01.BATE*/ wire BATE_AxxxxxGH = nor(ABOL_CLKREQn,
                                          BUGO_xBCDExxx,
                                          AROV_xxCDEFxx);
    /*p01.BASU*/ wire BASU_xBCDEFxx = not(BATE_AxxxxxGH);
    /*p01.BUKE*/ wire BUKE_AxxxxxGH = not(BASU_xBCDEFxx);
    CPU_PIN_BUKE_ABxxxxxH.set(BUKE_AxxxxxGH);
  }

  CPU_PIN_BOMA_xBxxxxxx.set(BOMA_Axxxxxxx());
  CPU_PIN_BOGA_AxCDEFGH.set(BOGA_xBCDEFGH());

  {
    wire ABOL_CLKREQn  = not(CPU_PIN_READYp);
    /*p01.ATYP*/ wire ATYP_xBCDExxx = not(AFUR_ABCDxxxx());
    /*p01.NULE*/ wire NULE_AxxxxFGH = nor(ABOL_CLKREQn, ATYP_xBCDExxx);
    /*p01.BYRY*/ wire BYRY_xBCDExxx = not(NULE_AxxxxFGH);
    /*p01.BUDE*/ wire BUDE_AxxxxFGH = not(BYRY_xBCDExxx);
    /* PIN_75 */ EXT_PIN_CLK.set(BUDE_AxxxxFGH);
  }

  CPU_PIN_EXT_CLKGOOD.set(SYS_PIN_CLK_A);
}

//-----------------------------------------------------------------------------

SignalHash ClockRegisters::commit() {
  SignalHash hash;

  hash << SYS_PIN_CLK_B.commit();
  hash << SYS_PIN_CLK_A.commit();
  hash << CPU_PIN_READYp.commit();

  hash << _AFUR_ABCDxxxx.commit();
  hash << _ALEF_xBCDExxx.commit();
  hash << _APUK_xxCDEFxx.commit();
  hash << _ADYK_xxxDEFGx.commit();

  hash << _WUVU_xxCDxxGH.commit();
  hash << _VENA_xxxxEFGH.commit();
  hash << _WOSU_xBCxxFGx.commit();

  hash << CPU_PIN_EXT_CLKGOOD.commit();   // PORTC_03: <- CLKIN_A
  hash << CPU_PIN_BOWA_AxCDEFGH.commit(); // PORTD_01: <- BOWA
  hash << CPU_PIN_BEDO_xBxxxxxx.commit(); // PORTD_02: <- BEDO _____fgh
  hash << CPU_PIN_BEKO_xBCDExxx.commit(); // PORTD_03: <- BEKO ____efgh connection not indicated on P01
  hash << CPU_PIN_BUDE_AxxxxFGH.commit(); // PORTD_04: <- BUDE abcd____
  hash << CPU_PIN_BOLO_xBCDEFGx.commit(); // PORTD_05: <- BOLO
  hash << CPU_PIN_BUKE_ABxxxxxH.commit(); // PORTD_07: <- BUKE _____f__
  hash << CPU_PIN_BOMA_xBxxxxxx.commit(); // PORTD_08: <- BOMA _____fgh
  hash << CPU_PIN_BOGA_AxCDEFGH.commit(); // PORTD_09: <- BOGA abcde___

  hash << EXT_PIN_CLK.commit();      // PIN_75 <- BUDE/BEVA

  return hash;
}

//-----------------------------------------------------------------------------

void ClockRegisters::dump(Dumper& d) {
  d("----------ClockRegisters----------\n");
  d("AFUR_ABCDxxxx %d\n", _AFUR_ABCDxxxx);
  d("ALEF_xBCDExxx %d\n", _ALEF_xBCDExxx);
  d("APUK_xxCDEFxx %d\n", _APUK_xxCDEFxx);
  d("ADYK_xxxDEFGx %d\n", _ADYK_xxxDEFGx);
}

//-----------------------------------------------------------------------------