/// plait_noparse

#include "GateBoyLib/Sch_Channel2.h"

#if 0

//-----------------------------------------------------------------------------
// This file should contain the schematics as directly translated to C,
// no modifications or simplifications.

void P14_Ch2Regs_tick(const Gameboy& a,
                      const Gameboy& b,
                      const ClkSignals& dff20,
                      Gameboy& next) {
  /*p15.AZEG*/ next.ch2.AZEG = not1(dff20.AMUK_xBxDxFxH);

  /*p14.HUDE*/ next.ch2.APU_RESETn1 = not1(b.apu.APU_RESET1);

  /*p14.JYBU*/ next.ch2.JYBU = not1(b.apu.APU_RESET1);
  /*p14.KYPU*/ next.ch2.KYPU = not1(b.apu.APU_RESET1);
  /*p14.FAZO*/ next.ch2.FAZO = not1(b.apu.APU_RESET1);
  /*p15.KATY*/ next.ch2.KATY = not1(b.apu.APU_RESET1);
  /*p15.CYWU*/ next.ch2.CYWU = not1(b.apu.APU_RESET1);
  /*p15.CEXE*/ next.ch2.CEXE = not1(b.apu.APU_RESET1);
  /*p15.BUWE*/ next.ch2.BUWE = not1(b.apu.APU_RESET1);

  //----------
  // FF16 NR21

  {
    /*p10.DAZA*/ wire ADDR_0110an = nand2(b.apu.ADDR_0xxx, b.apu.ADDR_x1xx, b.apu.ADDR_xx1x, b.apu.ADDR_xxx0);
    /*p10.COVY*/ wire ADDR_FF16  = nor4(b.apu.ADDR_FF1Xn, ADDR_0110an);

    /*p09.AGUZ*/ wire CPU_RDn = not1(b.ctl.TEDO_CPURD);
    /*p14.BYGO*/ next.ch2.BYGO = not1(CPU_RDn);
    /*p14.CORO*/ next.ch2.CORO = nand2(ADDR_FF16, b.ch2.BYGO);
    /*p14.AGYN*/ next.ch2.FF16_WR = nand2(b.apu.APU_WR, ADDR_FF16); // FF16_WR
    /*p14.ASYP*/ next.ch2.ASYP = not1(b.ch2.FF16_WR);
    /*p14.BENY*/ next.ch2.BENY = nor4(b.ch2.ASYP, b.apu.APU_RESET1, b.ch2.ELOX);
    /*p14.BACU*/ next.ch2.BACU = and2(ADDR_FF16, b.apu.APU_WR);
    /*p14.BUDU*/ next.ch2.BUDU = not1(b.ch2.BACU);

    {
      /*p09.AFAT*/ wire APU_RESETn = not1(b.apu.APU_RESET1);
      /*p14.BERA*/ next.ch2.NR21_DUTY0 = tock_pos(a.ch2.BUDU, b.ch2.BUDU, APU_RESETn, b.ch2.NR21_DUTY0, b.bus.TS_D0());
      /*p14.BAMY*/ next.ch2.NR21_DUTY1 = tock_pos(a.ch2.BUDU, b.ch2.BUDU, APU_RESETn, b.ch2.NR21_DUTY1, b.bus.TS_D1());
    }

    if (b.ch2.CORO) next.bus.set_data(
      /*p14.CEKA*/ b.ch2.NR21_DUTY0,
      /*p14.CECY*/ b.ch2.NR21_DUTY1
    );
  }

  //----------
  // FF17 NR22

  {
    /*p10.DUVU*/ wire ADDR_0111an = nand2(b.apu.ADDR_0xxx, b.apu.ADDR_x1xx, b.apu.ADDR_xx1x, b.apu.ADDR_xxx1);
    /*p10.DUTU*/ wire ADDR_FF17  = nor4(b.apu.ADDR_FF1Xn, ADDR_0111an);

    /*p09.AGUZ*/ wire CPU_RDn = not1(b.ctl.TEDO_CPURD);
    /*p14.ENUF*/ next.ch2.ENUF = and2(ADDR_FF17, b.apu.APU_WR);
    /*p14.FYRY*/ next.ch2.FYRY = not1(ADDR_FF17);
    /*p14.GURU*/ next.ch2.GURU = or2(b.ch2.FYRY, CPU_RDn);
    /*p14.GURE*/ next.ch2.GURE = not1(ADDR_FF17);
    /*p14.GEXA*/ next.ch2.GEXA = or2(b.ch2.GURE, CPU_RDn);
    /*p14.GERE*/ next.ch2.GERE = and2(b.apu.APU_WR, ADDR_FF17);
    /*p14.JEDE*/ next.ch2.JEDE = not1(b.ch2.GERE);

    /*p14.HYFU*/ next.ch2.NR22_ENV_TIMER0 = tock_pos(a.ch2.JEDE, b.ch2.JEDE, b.ch2.JYBU, b.ch2.NR22_ENV_TIMER0, b.bus.TS_D0());
    /*p14.HAVA*/ next.ch2.NR22_ENV_TIMER1 = tock_pos(a.ch2.JEDE, b.ch2.JEDE, b.ch2.JYBU, b.ch2.NR22_ENV_TIMER1, b.bus.TS_D1());
    /*p14.HORE*/ next.ch2.NR22_ENV_TIMER2 = tock_pos(a.ch2.JEDE, b.ch2.JEDE, b.ch2.JYBU, b.ch2.NR22_ENV_TIMER2, b.bus.TS_D2());
    /*p14.FORE*/ next.ch2.FF17_D3 = tock_pos(a.ch2.ENUF, b.ch2.ENUF, b.ch2.JYBU, b.ch2.FF17_D3, b.bus.TS_D3());
    /*p14.GATA*/ next.ch2.FF17_D4 = tock_pos(a.ch2.ENUF, b.ch2.ENUF, b.ch2.JYBU, b.ch2.FF17_D4, b.bus.TS_D4());
    /*p14.GUFE*/ next.ch2.FF17_D5 = tock_pos(a.ch2.ENUF, b.ch2.ENUF, b.ch2.JYBU, b.ch2.FF17_D5, b.bus.TS_D5());
    /*p14.GURA*/ next.ch2.FF17_D6 = tock_pos(a.ch2.ENUF, b.ch2.ENUF, b.ch2.JYBU, b.ch2.FF17_D6, b.bus.TS_D6());
    /*p14.GAGE*/ next.ch2.FF17_D7 = tock_pos(a.ch2.ENUF, b.ch2.ENUF, b.ch2.JYBU, b.ch2.FF17_D7, b.bus.TS_D7());

#if 0
    /*p14.HUVU*/ if (b.ch2.GEXA) next.\1() = b.ch2.NR22_ENV_TIMER0;
    /*p14.HYRE*/ if (b.ch2.GEXA) next.\1() = b.ch2.NR22_ENV_TIMER1;
    /*p14.HAVU*/ if (b.ch2.GEXA) next.\1() = b.ch2.NR22_ENV_TIMER2;
    /*p14.GENE*/ if (b.ch2.GURU) next.\1() = b.ch2.FF17_D3;
    /*p14.HUPE*/ if (b.ch2.GURU) next.\1() = b.ch2.FF17_D4;
    /*p14.HERE*/ if (b.ch2.GURU) next.\1() = b.ch2.FF17_D5;
    /*p14.HORO*/ if (b.ch2.GURU) next.\1() = b.ch2.FF17_D6;
    /*p14.HYRY*/ if (b.ch2.GURU) next.\1() = b.ch2.FF17_D7;
#endif

    if (b.ch2.GEXA || b.ch2.GURU) next.bus.set_data(
      /*p14.HUVU*/ b.ch2.NR22_ENV_TIMER0,
      /*p14.HYRE*/ b.ch2.NR22_ENV_TIMER1,
      /*p14.HAVU*/ b.ch2.NR22_ENV_TIMER2,
      /*p14.GENE*/ b.ch2.FF17_D3,
      /*p14.HUPE*/ b.ch2.FF17_D4,
      /*p14.HERE*/ b.ch2.FF17_D5,
      /*p14.HORO*/ b.ch2.FF17_D6,
      /*p14.HYRY*/ b.ch2.FF17_D7
    );
  }

  //----------
  // FF18 NR23

  {

    /*p10.DAFY*/ wire ADDR_1000an = nand2(b.apu.ADDR_1xxx, b.apu.ADDR_x0xx, b.apu.ADDR_xx0x, b.apu.ADDR_xxx0);
    /*p10.DARA*/ wire ADDR_FF18  = nor4(b.apu.ADDR_FF1Xn, ADDR_1000an);

    /*p10.DEJY*/ wire ADDR_1001an = nand2(b.apu.ADDR_1xxx, b.apu.ADDR_x0xx, b.apu.ADDR_xx0x, b.apu.ADDR_xxx1);
    /*p10.DOZA*/ wire ADDR_FF19  = nor4(b.apu.ADDR_FF1Xn, ADDR_1001an);

    /*p09.AGUZ*/ wire CPU_RDn = not1(b.ctl.TEDO_CPURD);
    /*p14.FOGE*/ next.ch2.FOGE = not1(CPU_RDn);
    /*p14.FAPE*/ next.ch2.FAPE = and2(b.ch2.FOGE, b.apu.NR52_DBG_APU);

    /*p14.GOTE*/ next.ch2.GOTE = not1(ADDR_FF19);
    /*p14.HYPO*/ next.ch2.HYPO = or2(b.ch2.GOTE, b.ch2.FAPE);

    /*p14.DOSA*/ next.ch2.NR23_WR1 = and2(ADDR_FF18, b.apu.APU_WR);
    /*p14.EXUC*/ next.ch2.NR23_WR2 = and2(ADDR_FF18, b.apu.APU_WR);

    /*p14.ESUR*/ next.ch2.NR23_WRn1 = not1(b.ch2.NR23_WR1);
    /*p14.FYXO*/ next.ch2.NR23_WRn2 = not1(b.ch2.NR23_WR2);

    /*p14.FERY*/ next.ch2.FERY = not1(b.apu.ADDR_1xxx);
    /*p14.GUZA*/ next.ch2.GUZA = nor4(b.ch2.FERY, b.ch2.FAPE);
    /*p14.FUTY*/ next.ch2.FUTY = not1(b.ch2.GUZA);

    /*p14.FOFE*/ next.ch2.NR23_FREQ0 = tock_pos(a.ch2.NR23_WRn1, b.ch2.NR23_WRn1, b.ch2.APU_RESETn1, b.ch2.NR23_FREQ0, b.bus.TS_D0());
    /*p14.FOVA*/ next.ch2.NR23_FREQ1 = tock_pos(a.ch2.NR23_WRn1, b.ch2.NR23_WRn1, b.ch2.APU_RESETn1, b.ch2.NR23_FREQ1, b.bus.TS_D1());
    /*p14.FEDY*/ next.ch2.NR23_FREQ2 = tock_pos(a.ch2.NR23_WRn1, b.ch2.NR23_WRn1, b.ch2.APU_RESETn1, b.ch2.NR23_FREQ2, b.bus.TS_D2());
    /*p14.FOME*/ next.ch2.NR23_FREQ3 = tock_pos(a.ch2.NR23_WRn1, b.ch2.NR23_WRn1, b.ch2.APU_RESETn1, b.ch2.NR23_FREQ3, b.bus.TS_D3());
    /*p14.FORA*/ next.ch2.NR23_FREQ4 = tock_pos(a.ch2.NR23_WRn1, b.ch2.NR23_WRn1, b.ch2.APU_RESETn1, b.ch2.NR23_FREQ4, b.bus.TS_D4());
    /*p14.GODA*/ next.ch2.NR23_FREQ5 = tock_pos(a.ch2.NR23_WRn2, b.ch2.NR23_WRn2, b.ch2.APU_RESETn1, b.ch2.NR23_FREQ5, b.bus.TS_D5());
    /*p14.GUMY*/ next.ch2.NR23_FREQ6 = tock_pos(a.ch2.NR23_WRn2, b.ch2.NR23_WRn2, b.ch2.APU_RESETn1, b.ch2.NR23_FREQ6, b.bus.TS_D6());
    /*p14.GUPU*/ next.ch2.NR23_FREQ7 = tock_pos(a.ch2.NR23_WRn2, b.ch2.NR23_WRn2, b.ch2.APU_RESETn1, b.ch2.NR23_FREQ7, b.bus.TS_D7());

    if (b.ch2.FUTY) next.bus.set_data(
      /*p14.FAVA*/ b.ch2.CH2_FREQ_00,
      /*p14.FAJY*/ b.ch2.CH2_FREQ_01,
      /*p14.FEGU*/ b.ch2.CH2_FREQ_02,
      /*p14.FOSE*/ b.ch2.CH2_FREQ_03,
      /*p14.GERO*/ b.ch2.CH2_FREQ_04, // d4 and d5 are switched on the schematic
      /*p14.GAKY*/ b.ch2.CH2_FREQ_05,
      /*p14.GADU*/ b.ch2.CH2_FREQ_06,
      /*p14.GAZO*/ b.ch2.CH2_FREQ_07
    );
  }

  //----------
  // FF19 NR24

  {
    /*p10.DEJY*/ wire ADDR_1001an = nand2(b.apu.ADDR_1xxx, b.apu.ADDR_x0xx, b.apu.ADDR_xx0x, b.apu.ADDR_xxx1);
    /*p10.DOZA*/ wire ADDR_FF19  = nor4(b.apu.ADDR_FF1Xn, ADDR_1001an);

    next.ch2.JENU = and2(ADDR_FF19, b.apu.APU_WR);
    next.ch2.KYSA = not1(b.ch2.JENU);
    next.ch2.NR24_FREQ8  = tock_pos(a.ch2.KYSA, b.ch2.KYSA, b.ch2.KYPU, b.ch2.NR24_FREQ8,  b.bus.TS_D0());
    next.ch2.NR24_FREQ9  = tock_pos(a.ch2.KYSA, b.ch2.KYSA, b.ch2.KYPU, b.ch2.NR24_FREQ9,  b.bus.TS_D1());
    next.ch2.NR24_FREQ10 = tock_pos(a.ch2.KYSA, b.ch2.KYSA, b.ch2.KYPU, b.ch2.NR24_FREQ10, b.bus.TS_D2());
    next.ch2.NR24_STOP   = tock_pos(a.ch2.EVYF, b.ch2.EVYF, b.ch2.FAZO, b.ch2.NR24_STOP,   b.bus.TS_D3());
    next.ch2.NR24_START  = tock_pos(a.ch2.DETA, b.ch2.DETA, b.ch2.DERA, b.ch2.NR24_START,  b.bus.TS_D4());

    next.ch2.DETA = nand2(b.apu.APU_WR, ADDR_FF19);

    if (b.ch2.HYPO) next.bus.set_data(
      /*p14.HUNA*/ b.ch2.CH2_FREQ_08,
      /*p14.JARO*/ b.ch2.CH2_FREQ_09,
      /*p14.JEKE*/ b.ch2.CH2_FREQ_10
    );

    /*
    // wat?
    if (b.ch2.HUMA) next.new_bus.set_data(
      b.ch2.GOJY
    );
    */

    /*p09.AGUZ*/ wire CPU_RDn = not1(b.ctl.TEDO_CPURD);
    next.ch2.GADO = not1(CPU_RDn);
    /*p16.ANUJ*/ wire CPU_WR_WEIRD = and2(b.cpu.SIG_CPU_HOLD_MEM, b.apu.APU_WR);
    next.ch2.EVYF = nand2(CPU_WR_WEIRD, ADDR_FF19);
    next.ch2.HUMA = nor4(ADDR_FF19, b.ch2.GADO);
  }



  //----------
  // reset thing

  /*p15.ELOX*/ next.ch2.ELOX = tock_pos(a.ch2.CEMO, b.ch2.CEMO, b.ch2.DOXA, b.ch2.ELOX, b.ch2.DALA);
  /*p15.DORY*/ next.ch2.DORY = tock_pos(a.ch2.CEMO, b.ch2.CEMO, b.ch2.CEXE, b.ch2.DORY, b.ch2.ELOX);
  /*p15.DOXA*/ next.ch2.DOXA = or2(b.apu.APU_RESET1, b.ch2.DORY);

  //----------

  /*p15.BUFO*/ next.ch2.BUFO = not1(b.ch2.ATEP);
  /*p15.BODO*/ next.ch2.BODO = not1(b.ch2.CAZA);
  /*p15.CEMO*/ next.ch2.CEMO = tock_pos(a.ch2.BUFO, b.ch2.BUFO, b.ch2.BYHO, b.ch2.CEMO, !b.ch2.CEMO);

  // weird latch?
  /*p15.BUTA*/ next.ch2.BUTA = or2(b.ch2.BODO, b.ch2.ARES);
  /*p15.CAMA*/ next.ch2.CAMA = nor4(b.ch2.CEMO, b.ch2.BUTA);
  /*p15.DOCA*/ next.ch2.DOCA = not1(b.ch2.CAMA);

  /*p15.ETUK*/ next.ch2.ETUK = not1(b.ch2.GYKO);
  /*p15.DAVU*/ next.ch2.DAVU = not1(b.ch2.ETUK);

  /*p15.DUJU*/ next.ch2.DUJU = nor4(b.ch2.DAVU, b.ch2.ELOX);
  /*p15.COGU*/ next.ch2.COGU = not1(b.ch2.DUJU);
  /*p15.EROG*/ next.ch2.EROG = not1(b.ch2.DUJU);
  /*p15.GYPA*/ next.ch2.GYPA = not1(b.ch2.DUJU);

  /*p14.DONE*/ next.ch2.CH2_FREQ_00 = count_pos(a.ch2.DOCA,         b.ch2.DOCA,         b.ch2.COGU, b.ch2.CH2_FREQ_00, b.ch2.NR23_FREQ0);
  /*p14.DYNU*/ next.ch2.CH2_FREQ_01 = count_pos(a.ch2.CH2_FREQ_00,  b.ch2.CH2_FREQ_00,  b.ch2.COGU, b.ch2.CH2_FREQ_01, b.ch2.NR23_FREQ1);
  /*p14.EZOF*/ next.ch2.CH2_FREQ_02 = count_pos(a.ch2.CH2_FREQ_01,  b.ch2.CH2_FREQ_01,  b.ch2.COGU, b.ch2.CH2_FREQ_02, b.ch2.NR23_FREQ2);
  /*p14.CYVO*/ next.ch2.CH2_FREQ_03 = count_pos(a.ch2.CH2_FREQ_02,  b.ch2.CH2_FREQ_02,  b.ch2.COGU, b.ch2.CH2_FREQ_03, b.ch2.NR23_FREQ3);
  /*p14.EDEP*/ next.ch2.CH2_FREQ_03b = not1(!b.ch2.CH2_FREQ_03);
  /*p14.FUXO*/ next.ch2.CH2_FREQ_04 = count_pos(a.ch2.CH2_FREQ_03b, b.ch2.CH2_FREQ_03b, b.ch2.EROG, b.ch2.CH2_FREQ_04, b.ch2.NR23_FREQ4);
  /*p14.GANO*/ next.ch2.CH2_FREQ_05 = count_pos(a.ch2.CH2_FREQ_04,  b.ch2.CH2_FREQ_04,  b.ch2.EROG, b.ch2.CH2_FREQ_05, b.ch2.NR23_FREQ5);
  /*p14.GOCA*/ next.ch2.CH2_FREQ_06 = count_pos(a.ch2.CH2_FREQ_05,  b.ch2.CH2_FREQ_05,  b.ch2.EROG, b.ch2.CH2_FREQ_06, b.ch2.NR23_FREQ6);
  /*p14.GANE*/ next.ch2.CH2_FREQ_07 = count_pos(a.ch2.CH2_FREQ_06,  b.ch2.CH2_FREQ_06,  b.ch2.EROG, b.ch2.CH2_FREQ_07, b.ch2.NR23_FREQ7);
  /*p14.GALA*/ next.ch2.CH2_FREQ_07b = not1(!b.ch2.CH2_FREQ_07);
  /*p14.HEVY*/ next.ch2.CH2_FREQ_08 = count_pos(a.ch2.CH2_FREQ_07b, b.ch2.CH2_FREQ_07b, b.ch2.GYPA, next.ch2.CH2_FREQ_08, b.ch2.NR24_FREQ8);
  /*p14.HEPU*/ next.ch2.CH2_FREQ_09 = count_pos(a.ch2.CH2_FREQ_08,  b.ch2.CH2_FREQ_08,  b.ch2.GYPA, next.ch2.CH2_FREQ_09, b.ch2.NR24_FREQ9);
  /*p14.HERO*/ next.ch2.CH2_FREQ_10 = count_pos(a.ch2.CH2_FREQ_09,  b.ch2.CH2_FREQ_09,  b.ch2.GYPA, next.ch2.CH2_FREQ_10, b.ch2.NR24_FREQ10);

  /*p15.FUJY*/ next.ch2.FUJY = and2(b.ch2.GYKO, b.ch2.CEMO);
  /*p15.GALU*/ next.ch2.GALU = not1(b.ch2.CH2_FREQ_10);
  /*p15.GYRE*/ next.ch2.GYRE = nor4(b.apu.APU_RESET1, b.ch2.ELOX, b.ch2.FUJY);
  /*p15.GYKO*/ next.ch2.GYKO = tock_pos(a.ch2.GALU, b.ch2.GALU, b.ch2.GYRE, b.ch2.GYKO, !b.ch2.GYKO);

  //----------

  /*p15.HOTA*/ next.ch2.HOTA = not1(b.apu.CLK_128a);
  /*p15.JYNA*/ next.ch2.JYNA = tock_pos(a.ch2.HOTA, b.ch2.HOTA, b.ch2.KATY, b.ch2.JYNA, !b.ch2.JYNA);
  /*p15.KYLO*/ next.ch2.KYLO = not1(b.ch2.JYNA);
  /*p15.KENE*/ next.ch2.KENE = not1(b.ch2.KYLO);
  /*p15.JORE*/ next.ch2.JORE = count_pos(a.ch2.KENE, b.ch2.KENE, b.ch2.JAKE, b.ch2.JORE, !b.ch2.NR22_ENV_TIMER0);
  /*p15.JONA*/ next.ch2.JONA = count_pos(a.ch2.JORE, b.ch2.JORE, b.ch2.JAKE, b.ch2.JONA, !b.ch2.NR22_ENV_TIMER1);
  /*p15.JEVY*/ next.ch2.JEVY = count_pos(a.ch2.JONA, b.ch2.JONA, b.ch2.JAKE, b.ch2.JEVY, !b.ch2.NR22_ENV_TIMER2);
  /*p15.KYVO*/ next.ch2.KYVO = and2(!b.ch2.JORE, b.ch2.JONA, b.ch2.JEVY);
  /*p15.JOPA*/ next.ch2.JOPA = tock_pos(a.apu.CLK_512a, b.apu.CLK_512a, b.ch2.HAFE, b.ch2.JOPA, b.ch2.KYVO);
  /*p15.HEPO*/ next.ch2.HEPO = tock_pos(a.ch2.JOPA,     b.ch2.JOPA,     b.ch2.HYPA, b.ch2.HEPO, b.ch2.GUFY);
  ///*p15.DOPE*/ next.ch2.DOPE = tock_pos(a.clk.DOVA_ABCDxxxx, b.clk.DOVA_ABCDxxxx, b.ch2.CYWU, b.ch2.DOPE, b.ch2.NR24_START);
  /*p15.DERA*/ next.ch2.DERA = nor4(b.apu.APU_RESET1, b.ch2.DOPE);

  // weird latch?
  /*p15.DALA*/ next.ch2.DALA = or2(b.ch2.CELO, b.ch2.DOPE);
  /*p15.CELO*/ next.ch2.CELO = not1(b.ch2.DOXA);
  /*p15.HYLE*/ next.ch2.HYLE = or2(b.apu.APU_RESET1, b.ch2.ELOX);
  /*p15.HYPA*/ next.ch2.HYPA = nor4(b.ch2.ELOX, b.apu.APU_RESET1);
  /*p15.HYLY*/ next.ch2.HYLY = nor4(b.ch2.ELOX, b.ch2.JOPA);
  /*p15.JAKE*/ next.ch2.JAKE = not1(b.ch2.HYLY);

  // weird latch?
  /*p15.JEME*/ next.ch2.JEME = or2(b.ch2.HEPO, b.ch2.HYLE);
  /*p15.ATEP*/ next.ch2.ATEP = tock_pos(a.ch2.AZEG, b.ch2.AZEG, b.ch2.BUWE, b.ch2.ATEP, !b.ch2.ATEP);
  /*p15.CAZA*/ next.ch2.CAZA = tock_pos(a.ch2.CEMO, b.ch2.CEMO, b.ch2.CEXE, b.ch2.CAZA, b.ch2.DORY);
  /*p15.BYHO*/ next.ch2.BYHO = not1(b.ch2.BUWE);

  /*p15.DEME*/ next.ch2.DEME = or2(b.ch2.CYRE, b.apu.CLK_256a, b.ch2.NR24_STOP);
  /*p15.DORA*/ next.ch2.DORA = and2(b.ch2.CYRE, b.ch2.NR24_STOP);
  /*p15.FUTE*/ next.ch2.CH2_AMP_ENn = nor4(b.ch2.FF17_D3,b.ch2.FF17_D4,b.ch2.FF17_D5,b.ch2.FF17_D6,b.ch2.FF17_D7);
  /*p15.DYRO*/ next.ch2.DYRO = not1(b.ch2.DEME);
  /*p15.ESYK*/ next.ch2.ESYK = or2(b.apu.APU_RESET1, b.ch2.DORA, b.ch2.CH2_AMP_ENn);
  /*p15.ARES*/ next.ch2.ARES = nand2(b.apu.APU_RESET1, b.ch2.CH2_AMP_ENn);

  // weird latch?
  /*p15.DANE*/ next.ch2.DANE = or2(b.ch2.ELOX, b.ch2.ESYK);
  /*p15.DEFU*/ next.ch2.CH2_ACTIVEn = not1(b.ch2.DANE);
  /*p15.BYMO*/ next.ch2.BYMO = not1(b.ch2.FF16_WR);
  /*p15.AGET*/ next.ch2.AGET = not1(b.ch2.FF16_WR);


  /*p15.ERYC*/ next.ch2.ERYC_00 = count_pos(a.ch2.DYRO,     b.ch2.DYRO,     b.ch2.BYMO, b.ch2.ERYC_00, b.bus.TS_D0());
  /*p15.CERA*/ next.ch2.CERA_01 = count_pos(a.ch2.ERYC_00,  b.ch2.ERYC_00,  b.ch2.BYMO, b.ch2.CERA_01, b.bus.TS_D1());
  /*p15.CONU*/ next.ch2.CONU_02 = count_pos(a.ch2.CERA_01,  b.ch2.CERA_01,  b.ch2.BYMO, b.ch2.CONU_02, b.bus.TS_D2());
  /*p15.CAME*/ next.ch2.CAME_03 = count_pos(a.ch2.CONU_02,  b.ch2.CONU_02,  b.ch2.BYMO, b.ch2.CAME_03, b.bus.TS_D3());
  /*p15.BUKO*/ next.ch2.BUKO    = not1(!b.ch2.CAME_03);
  /*p15.BUVA*/ next.ch2.BUVA_04 = count_pos(a.ch2.BUKO,     b.ch2.BUKO,     b.ch2.AGET, b.ch2.BUVA_04, b.bus.TS_D4());
  /*p15.AKYD*/ next.ch2.AKYD_05 = count_pos(a.ch2.BUVA_04,  b.ch2.BUVA_04,  b.ch2.AGET, b.ch2.AKYD_05, b.bus.TS_D5());
  /*p15.CYRE*/ next.ch2.CYRE    = tock_pos(!a.ch2.AKYD_05,  !b.ch2.AKYD_05, b.ch2.BENY, b.ch2.CYRE,    !b.ch2.CYRE);

  /*p15.GADE*/ next.ch2.GADE = not1(b.ch2.JOPA);
  /*p15.HOLY*/ next.ch2.HOLY = nor4(b.apu.CLK_512a, b.ch2.GADE);
  /*p15.JUPU*/ next.ch2.JUPU = nor4(b.ch2.NR22_ENV_TIMER0, b.ch2.NR22_ENV_TIMER1, b.ch2.NR22_ENV_TIMER2);
  /*p15.HOFO*/ next.ch2.HOFO = or2(b.ch2.JOPA, b.ch2.JUPU, b.ch2.JEME);
  /*p15.HAFE*/ next.ch2.HAFE = or2(b.ch2.HOLY, b.ch2.ELOX, b.apu.APU_RESET1);

  {
    /*p15.CULE*/ next.ch2.CULE = not1(b.ch2.DAVU);
    /*p09.AFAT*/ wire APU_RESETn = not1(b.apu.APU_RESET1);
    /*p15.CANO*/ next.ch2.CANO_00 = tock_pos(a.ch2.CULE,     b.ch2.CULE,     APU_RESETn, b.ch2.CANO_00, !b.ch2.CANO_00);
    /*p15.CAGY*/ next.ch2.CAGY_01 = tock_pos(!a.ch2.CANO_00, !b.ch2.CANO_00, APU_RESETn, b.ch2.CAGY_01, !b.ch2.CAGY_01);
    /*p15.DYVE*/ next.ch2.DYVE_02 = tock_pos(!a.ch2.CAGY_01, !b.ch2.CAGY_01, APU_RESETn, b.ch2.DYVE_02, !b.ch2.DYVE_02);
  }

  /*p15.DYMU*/ next.ch2.DYMU = and2(b.ch2.DYVE_02, b.ch2.CAGY_01);
  /*p15.DUGE*/ next.ch2.DUGE = not1(b.ch2.CANO_00);
  /*p15.DARE*/ next.ch2.DARE = not1(b.ch2.DYMU);
  /*p15.EGOG*/ next.ch2.EGOG = and2(b.ch2.DYMU, b.ch2.DUGE);

  /*p15.DOMO*/ next.ch2.DOMO = nor4( b.ch2.NR21_DUTY0,  b.ch2.NR21_DUTY1);
  /*p15.DYTA*/ next.ch2.DYTA = nor4(!b.ch2.NR21_DUTY0,  b.ch2.NR21_DUTY1);
  /*p15.DOJU*/ next.ch2.DOJU = nor4( b.ch2.NR21_DUTY0, !b.ch2.NR21_DUTY1);
  /*p15.DOVE*/ next.ch2.DOVE = nor4(!b.ch2.NR21_DUTY0, !b.ch2.NR21_DUTY1);
  /*p15.EXES*/ next.ch2.EXES = amux4(b.ch2.EGOG,    b.ch2.DOMO,
                                  b.ch2.DYMU,    b.ch2.DYTA,
                                  b.ch2.DYVE_02, b.ch2.DOJU,
                                  b.ch2.DARE,    b.ch2.DOVE);



  {
    /*p09.AFAT*/ wire APU_RESETn = not1(b.apu.APU_RESET1);
    /*p15.DOME*/ next.ch2.DOME = tock_pos(a.ch2.DAVU, b.ch2.DAVU, APU_RESETn, b.ch2.DOME, b.ch2.EXES);
  }
  /*p15.CYSE*/ next.ch2.CYSE = and2(b.ch2.DANE, b.ch2.DOME);
  /*p15.BONU*/ next.ch2.BONU = or2(b.ch2.CYSE, b.apu.NR52_DBG_APU);
  /*p15.FOPY*/ next.ch2.FOPY = amux2(b.ch2.FOMY, b.ch2.FF17_D3, !b.ch2.FOMY, !b.ch2.FF17_D3);
  /*p15.ETUP*/ next.ch2.ETUP = amux2(b.ch2.FETE, b.ch2.FF17_D3, !b.ch2.FETE, !b.ch2.FF17_D3);
  /*p15.FARU*/ next.ch2.FARU = amux2(b.ch2.FENO, b.ch2.FF17_D3, !b.ch2.FENO, !b.ch2.FF17_D3);
  /*p15.GAFA*/ next.ch2.GAFA = amux2(b.ch2.HOFO, b.ch2.FF17_D3, b.ch2.HOFO,  !b.ch2.FF17_D3);
  /*p15.FENA*/ next.ch2.FENA = count_pos(a.ch2.FOPY, b.ch2.FOPY, b.ch2.ELOX, b.ch2.FENA, b.ch2.FF17_D7);
  /*p15.FOMY*/ next.ch2.FOMY = count_pos(a.ch2.ETUP, b.ch2.ETUP, b.ch2.ELOX, b.ch2.FOMY, b.ch2.FF17_D6);
  /*p15.FETE*/ next.ch2.FETE = count_pos(a.ch2.FARU, b.ch2.FARU, b.ch2.ELOX, b.ch2.FETE, b.ch2.FF17_D5);
  /*p15.FENO*/ next.ch2.FENO = count_pos(a.ch2.GAFA, b.ch2.GAFA, b.ch2.ELOX, b.ch2.FENO, b.ch2.FF17_D4);
  /*p15.EMYR*/ next.ch2.EMYR = nor4 (b.ch2.FF17_D3, b.ch2.FENO, b.ch2.FETE, b.ch2.FOMY, b.ch2.FENA);
  /*p15.ERAT*/ next.ch2.ERAT = nand2(b.ch2.FF17_D3, b.ch2.FENO, b.ch2.FETE, b.ch2.FOMY, b.ch2.FENA);
  /*p15.FYRE*/ next.ch2.FYRE = not1(b.ch2.ERAT);
  /*p15.GUFY*/ next.ch2.GUFY = or2(b.ch2.EMYR, b.ch2.FYRE);
  /*p15.ANYV*/ next.ch2.ANYV = and2(b.ch2.FETE, b.ch2.BONU);
  /*p15.ANAN*/ next.ch2.ANAN = and2(b.ch2.FENO, b.ch2.BONU);
  /*p15.AMOV*/ next.ch2.AMOV = and2(b.ch2.FENA, b.ch2.BONU);
  /*p15.ASOG*/ next.ch2.ASOG = and2(b.ch2.FOMY, b.ch2.BONU);
}

#endif

//-----------------------------------------------------------------------------
