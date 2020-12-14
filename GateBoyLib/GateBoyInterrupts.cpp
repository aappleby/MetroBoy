#include "GateBoyLib/GateBoyInterrupts.h"

#include "CoreLib/Constants.h"

#include "GateBoyLib/GateBoyJoypad.h"
#include "GateBoyLib/GateBoyResetDebug.h"
#include "GateBoyLib/GateBoyPixPipe.h"
#include "GateBoyLib/GateBoyLCD.h"
#include "GateBoyLib/GateBoySerial.h"

void GateBoyInterrupts::tock(
  const GateBoyResetDebug& rstdbg,
  const GateBoyJoypad& joypad,
  const RegStat& reg_stat,
  const RegLYC& reg_lyc,
  const GateBoySerial& serial,

  wire BUS_CPU_A[16],
  wire BUS_CPU_D[8],
  wire TEDO_CPU_RDp,
  wire TAPU_CPU_WRp,

  wire PARU_VBLANKp,
  wire PURE_LINE_ENDn,
  wire MOBA_TIMER_OVERFLOWp,
  wire WODU_HBLANKp,
  BusOut BUS_CPU_D_out[8])
{
  /*#p01.ALUR*/ wire _ALUR_SYS_RSTn_new = not1(rstdbg.AVOR_SYS_RSTp());

  /* p07.ROLO*/ wire _ROLO_FF0F_RDn_ext = nand4(TEDO_CPU_RDp, SYKE_ADDR_HIp_ext(BUS_CPU_A),  SEMY_XX_0000xxxxp_ext(BUS_CPU_A), SAPA_XX_xxxx1111p_ext(BUS_CPU_A)); // schematic wrong, is NAND
  /* p07.REFA*/ wire _REFA_FF0F_WRn_clk = nand4(TAPU_CPU_WRp, SYKE_ADDR_HIp_ext(BUS_CPU_A),  SEMY_XX_0000xxxxp_ext(BUS_CPU_A), SAPA_XX_xxxx1111p_ext(BUS_CPU_A)); // schematic wrong, is NAND

  // Bit 0 : V-Blank  Interrupt Request(INT 40h)  (1=Request)
  // Bit 1 : LCD STAT Interrupt Request(INT 48h)  (1=Request)
  // Bit 2 : Timer    Interrupt Request(INT 50h)  (1=Request)
  // Bit 3 : Serial   Interrupt Request(INT 58h)  (1=Request)
  // Bit 4 : Joypad   Interrupt Request(INT 60h)  (1=Request)

  /* p02.LETY*/ wire _LETY_INT_VBL_ACKn_ext  = not1(PIN_CPU_ACK_VBLANK.qp_new());
  /* p02.LEJA*/ wire _LEJA_INT_STAT_ACKn_ext = not1(PIN_CPU_ACK_STAT.qp_new());
  /* p02.LESA*/ wire _LESA_INT_TIM_ACKn_ext  = not1(PIN_CPU_ACK_TIMER.qp_new());
  /* p02.LUFE*/ wire _LUFE_INT_SER_ACKn_ext  = not1(PIN_CPU_ACK_SERIAL.qp_new());
  /* p02.LAMO*/ wire _LAMO_INT_JOY_ACKn_ext  = not1(PIN_CPU_ACK_JOYPAD.qp_new());

  /* p02.ROTU*/ wire _ROTU_FF0F_WRp_clk   = not1(_REFA_FF0F_WRn_clk);
  /* p02.MYZU*/ wire _MYZU_FF0F_SET0n_new = nand3(_ROTU_FF0F_WRp_clk, _LETY_INT_VBL_ACKn_ext,  BUS_CPU_D[0]);
  /* p02.MODY*/ wire _MODY_FF0F_SET1n_new = nand3(_ROTU_FF0F_WRp_clk, _LEJA_INT_STAT_ACKn_ext, BUS_CPU_D[1]);
  /* p02.PYHU*/ wire _PYHU_FF0F_SET2n_new = nand3(_ROTU_FF0F_WRp_clk, _LESA_INT_TIM_ACKn_ext,  BUS_CPU_D[2]);
  /* p02.TOME*/ wire _TOME_FF0F_SET3n_new = nand3(_ROTU_FF0F_WRp_clk, _LUFE_INT_SER_ACKn_ext,  BUS_CPU_D[3]);
  /* p02.TOGA*/ wire _TOGA_FF0F_SET4n_new = nand3(_ROTU_FF0F_WRp_clk, _LAMO_INT_JOY_ACKn_ext,  BUS_CPU_D[4]);

  /*#p02.MUXE*/ wire _MUXE_INT0_WRn_new = or2(BUS_CPU_D[0], _REFA_FF0F_WRn_clk);
  /* p02.NABE*/ wire _NABE_INT1_WRn_new = or2(BUS_CPU_D[1], _REFA_FF0F_WRn_clk);
  /* p02.RAKE*/ wire _RAKE_INT2_WRn_new = or2(BUS_CPU_D[2], _REFA_FF0F_WRn_clk);
  /* p02.SULO*/ wire _SULO_INT3_WRn_new = or2(BUS_CPU_D[3], _REFA_FF0F_WRn_clk);
  /* p02.SEME*/ wire _SEME_INT4_WRn_new = or2(BUS_CPU_D[4], _REFA_FF0F_WRn_clk);

  /*#p02.LYTA*/ wire _LYTA_FF0F_RST0n_new = and3(_MUXE_INT0_WRn_new, _LETY_INT_VBL_ACKn_ext,  _ALUR_SYS_RSTn_new);
  /* p02.MOVU*/ wire _MOVU_FF0F_RST1n_new = and3(_NABE_INT1_WRn_new, _LEJA_INT_STAT_ACKn_ext, _ALUR_SYS_RSTn_new);
  /* p02.PYGA*/ wire _PYGA_FF0F_RST2n_new = and3(_RAKE_INT2_WRn_new, _LESA_INT_TIM_ACKn_ext,  _ALUR_SYS_RSTn_new);
  /* p02.TUNY*/ wire _TUNY_FF0F_RST3n_new = and3(_SULO_INT3_WRn_new, _LUFE_INT_SER_ACKn_ext,  _ALUR_SYS_RSTn_new);
  /* p02.TYME*/ wire _TYME_FF0F_RST4n_new = and3(_SEME_INT4_WRn_new, _LAMO_INT_JOY_ACKn_ext,  _ALUR_SYS_RSTn_new);

  /*#p21.TOLU*/ wire _TOLU_VBLANKn_new      = not1(PARU_VBLANKp);
  /*#p21.SELA*/ wire _SELA_LINE_ENDp_new    = not1(PURE_LINE_ENDn);
  /*#p21.TAPA*/ wire _TAPA_INT_OAM_t3_new   = and2(_TOLU_VBLANKn_new, _SELA_LINE_ENDp_new);
  /*#p21.TARU*/ wire _TARU_INT_HBL_t3_new   = and2(WODU_HBLANKp, _TOLU_VBLANKn_new);
  /*#p21.SUKO*/ wire _SUKO_INT_STATp_t3_new = amux4(reg_stat.RUGU_STAT_LYI_ENn_h.qn_new(), reg_lyc.ROPO_LY_MATCH_SYNCp_c.qp_new(),
                                                    reg_stat.REFE_STAT_OAI_ENn_h.qn_new(), _TAPA_INT_OAM_t3_new,
                                                    reg_stat.RUFO_STAT_VBI_ENn_h.qn_new(), PARU_VBLANKp, // polarity?
                                                    reg_stat.ROXE_STAT_HBI_ENn_h.qn_new(), _TARU_INT_HBL_t3_new);

  /*#p21.VYPU*/ wire _VYPU_INT_VBLANKp_t3_new = not1(_TOLU_VBLANKn_new);
  /*#p21.TUVA*/ wire _TUVA_INT_STATn_t3_new   = not1(_SUKO_INT_STATp_t3_new);
  /*#p21.VOTY*/ wire _VOTY_INT_STATp_t3_new   = not1(_TUVA_INT_STATn_t3_new);

  wire _PESU_VCC = 1;
  /* p02.LOPE*/ LOPE_FF0F_D0p.dff22(_VYPU_INT_VBLANKp_t3_new,       _MYZU_FF0F_SET0n_new, _LYTA_FF0F_RST0n_new, _PESU_VCC);
  /* p02.LALU*/ LALU_FF0F_D1p.dff22(_VOTY_INT_STATp_t3_new,         _MODY_FF0F_SET1n_new, _MOVU_FF0F_RST1n_new, _PESU_VCC);
  /* p02.NYBO*/ NYBO_FF0F_D2p.dff22(MOBA_TIMER_OVERFLOWp,           _PYHU_FF0F_SET2n_new, _PYGA_FF0F_RST2n_new, _PESU_VCC);
  /* p02.UBUL*/ UBUL_FF0F_D3p.dff22(serial.CALY_SER_CNT3.qp_new(),  _TOME_FF0F_SET3n_new, _TUNY_FF0F_RST3n_new, _PESU_VCC);
  /* p02.ULAK*/ ULAK_FF0F_D4p.dff22(joypad.ASOK_INT_JOYp(),         _TOGA_FF0F_SET4n_new, _TYME_FF0F_RST4n_new, _PESU_VCC);

  // FIXME this inversion fixes a bunch of tests...
  // MATY is one of those big yellow latchy things
  /* p02.MATY*/ MATY_FF0F_L0p.tp_latch(!_ROLO_FF0F_RDn_ext, LOPE_FF0F_D0p.qp_new()); // OUTPUT ON RUNG 10
  /* p02.MOPO*/ MOPO_FF0F_L1p.tp_latch(!_ROLO_FF0F_RDn_ext, LALU_FF0F_D1p.qp_new()); // OUTPUT ON RUNG 10
  /* p02.PAVY*/ PAVY_FF0F_L2p.tp_latch(!_ROLO_FF0F_RDn_ext, NYBO_FF0F_D2p.qp_new()); // OUTPUT ON RUNG 10
  /* p02.NEJY*/ NEJY_FF0F_L3p.tp_latch(!_ROLO_FF0F_RDn_ext, UBUL_FF0F_D3p.qp_new()); // OUTPUT ON RUNG 10
  /* p02.NUTY*/ NUTY_FF0F_L4p.tp_latch(!_ROLO_FF0F_RDn_ext, ULAK_FF0F_D4p.qp_new()); // OUTPUT ON RUNG 10

  PIN_CPU_INT_VBLANK.setp(LOPE_FF0F_D0p.qp_new());
  PIN_CPU_INT_STAT  .setp(LALU_FF0F_D1p.qp_new());
  PIN_CPU_INT_TIMER .setp(NYBO_FF0F_D2p.qp_new());
  PIN_CPU_INT_SERIAL.setp(UBUL_FF0F_D3p.qp_new());
  PIN_CPU_INT_JOYPAD.setp(ULAK_FF0F_D4p.qp_new());

  // FF0F INTF
  /* p02.POLA*/ wire _POLA_FF0F_RDp_ext = not1(_ROLO_FF0F_RDn_ext);
  /*#p02.NELA*/ BUS_CPU_D_out[0].tri6_pn(_POLA_FF0F_RDp_ext, MATY_FF0F_L0p.qn_new());
  /*#p02.NABO*/ BUS_CPU_D_out[1].tri6_pn(_POLA_FF0F_RDp_ext, MOPO_FF0F_L1p.qn_new());
  /*#p02.ROVA*/ BUS_CPU_D_out[2].tri6_pn(_POLA_FF0F_RDp_ext, PAVY_FF0F_L2p.qn_new());
  /*#p02.PADO*/ BUS_CPU_D_out[3].tri6_pn(_POLA_FF0F_RDp_ext, NEJY_FF0F_L3p.qn_new());
  /*#p02.PEGY*/ BUS_CPU_D_out[4].tri6_pn(_POLA_FF0F_RDp_ext, NUTY_FF0F_L4p.qn_new());

  {
    // FFFF - IE
    // This is technically in the CPU, but we're going to implement it here for now.

    uint16_t cpu_addr = pack_u16p(16, BUS_CPU_A);
    wire FFFF_HIT_ext = cpu_addr == 0xFFFF;
    wire FFFF_RDn_ext = nand2(TEDO_CPU_RDp, FFFF_HIT_ext);
    wire FFFF_WRn_ext = nand2(TAPU_CPU_WRp, FFFF_HIT_ext);

    IE_D0.dff(FFFF_WRn_ext, 1, !rstdbg.PIN_SYS_RST.qp_any(), BUS_CPU_D[0]);
    IE_D1.dff(FFFF_WRn_ext, 1, !rstdbg.PIN_SYS_RST.qp_any(), BUS_CPU_D[1]);
    IE_D2.dff(FFFF_WRn_ext, 1, !rstdbg.PIN_SYS_RST.qp_any(), BUS_CPU_D[2]);
    IE_D3.dff(FFFF_WRn_ext, 1, !rstdbg.PIN_SYS_RST.qp_any(), BUS_CPU_D[3]);
    IE_D4.dff(FFFF_WRn_ext, 1, !rstdbg.PIN_SYS_RST.qp_any(), BUS_CPU_D[4]);

    BUS_CPU_D_out[0].tri6_nn(FFFF_RDn_ext, IE_D0.qn_new());
    BUS_CPU_D_out[1].tri6_nn(FFFF_RDn_ext, IE_D1.qn_new());
    BUS_CPU_D_out[2].tri6_nn(FFFF_RDn_ext, IE_D2.qn_new());
    BUS_CPU_D_out[3].tri6_nn(FFFF_RDn_ext, IE_D3.qn_new());
    BUS_CPU_D_out[4].tri6_nn(FFFF_RDn_ext, IE_D4.qn_new());
  }
}