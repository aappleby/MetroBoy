#include "GateBoyLib/GateBoyClocks.h"

#include "GateBoyLib/GateBoy.h"

//------------------------------------------------------------------------------------------------------------------------

void GateBoy::tock_clocks() {
  /*PIN_73*/ clk.PIN_73_CLK_DRIVE.set_pin_int(clk.PIN_74_CLK.clock());

  /* p01.ARYS*/ wire ARYS_CLKIN = not1(clk.PIN_74_CLK.clock());
  /* p01.AVET*/ clk.AVET_DEGLITCH = nand2(clk.ANOS_DEGLITCH.qp_mid(), ARYS_CLKIN);
  /* p01.ANOS*/ clk.ANOS_DEGLITCH = nand2(clk.PIN_74_CLK.clock(), clk.AVET_DEGLITCH.qp_mid());
  /* p01.AVET*/ clk.AVET_DEGLITCH = nand2(clk.ANOS_DEGLITCH.qp_mid(), ARYS_CLKIN);
  /* p01.ANOS*/ clk.ANOS_DEGLITCH = nand2(clk.PIN_74_CLK.clock(), clk.AVET_DEGLITCH.qp_mid());

  /* p01.ATAL*/ wire _ATAL_xBxDxFxH = not1(clk.AVET_DEGLITCH.qp_new());
  /* p01.ATAN*/ wire _ATAN_AxCxExGx = not1(_ATAL_xBxDxFxH); // cell not marked on die but it's next to ATAL

  DFF9 _ADYK_ABCxxxxH_old = clk.ADYK_ABCxxxxH;
  DFF9 _AFUR_xxxxEFGH_old = clk.AFUR_xxxxEFGH;
  DFF9 _ALEF_AxxxxFGH_old = clk.ALEF_AxxxxFGH;
  DFF9 _APUK_ABxxxxGH_old = clk.APUK_ABxxxxGH;

  /* p01.AFUR*/ clk.AFUR_xxxxEFGH.dff9(_ATAN_AxCxExGx, UPOJ_MODE_PRODn(), _ADYK_ABCxxxxH_old.qp_old());
  /* p01.ALEF*/ clk.ALEF_AxxxxFGH.dff9(_ATAL_xBxDxFxH, UPOJ_MODE_PRODn(), _AFUR_xxxxEFGH_old.qn_old());
  /* p01.APUK*/ clk.APUK_ABxxxxGH.dff9(_ATAN_AxCxExGx, UPOJ_MODE_PRODn(), _ALEF_AxxxxFGH_old.qn_old());
  /* p01.ADYK*/ clk.ADYK_ABCxxxxH.dff9(_ATAL_xBxDxFxH, UPOJ_MODE_PRODn(), _APUK_ABxxxxGH_old.qn_old());

  /*PIN_75*/ clk.PIN_75_CLK_OUT.set_pin_int(BUDE_xxxxEFGH());
  /*SIG_CPU_BOWA_Axxxxxxx*/ clk.SIG_CPU_BOWA_Axxxxxxx.sig_out(BOWA_xBCDEFGH());
  /*SIG_CPU_BEDO_xBCDEFGH*/ clk.SIG_CPU_BEDO_xBCDEFGH.sig_out(BEDO_Axxxxxxx());
  /*SIG_CPU_BEKO_ABCDxxxx*/ clk.SIG_CPU_BEKO_ABCDxxxx.sig_out(BEKO_ABCDxxxx());
  /*SIG_CPU_BUDE_xxxxEFGH*/ clk.SIG_CPU_BUDE_xxxxEFGH.sig_out(BUDE_xxxxEFGH());
  /*SIG_CPU_BOLO_ABCDEFxx*/ clk.SIG_CPU_BOLO_ABCDEFxx.sig_out(BOLO_ABCDEFxx());
  /*SIG_CPU_BUKE_AxxxxxGH*/ clk.SIG_CPU_BUKE_AxxxxxGH.sig_out(BUKE_AxxxxxGH());
  /*SIG_CPU_BOMA_xBCDEFGH*/ clk.SIG_CPU_BOMA_xBCDEFGH.sig_out(BOMA_xBCDEFGH());
  /*SIG_CPU_BOGA_Axxxxxxx*/ clk.SIG_CPU_BOGA_Axxxxxxx.sig_out(BOGA_Axxxxxxx());
}

//------------------------------------------------------------------------------------------------------------------------

void GateBoy::tock_vid_clocks() {
  /* p29.WOSU*/ clk.WOSU_AxxDExxH.dff17(XYFY_xBxDxFxH(),            XAPO_VID_RSTn(), clk.WUVU_ABxxEFxx.qn_old());
  /* p29.WUVU*/ clk.WUVU_ABxxEFxx.dff17(XOTA_AxCxExGx(),            XAPO_VID_RSTn(), clk.WUVU_ABxxEFxx.qn_old());
  /* p21.VENA*/ clk.VENA_xxCDEFxx.dff17(clk.WUVU_ABxxEFxx.qn_new(), XAPO_VID_RSTn(), clk.VENA_xxCDEFxx.qn_old()); // inverting the clock to VENA doesn't seem to break anything, which is really weird
}

//------------------------------------------------------------------------------------------------------------------------
