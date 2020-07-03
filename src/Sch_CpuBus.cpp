#include "Sch_CpuBus.h"

#include "Sch_Top.h"

using namespace Schematics;

// TEVY box color wrong on die trace, but schematic correct.

// Die trace:
// SORE = not(A15)
// TEVY = or(A13, A13, SORE) // A13 line not fully drawn
// TEXO = and(ADDR_VALIDx?, TEVY)

// UJYV01 << UNOR04
// UJYV02 << (mux 1) RD_C
// UJYV03 << (mux 0) CPU_RAW_RD
// UJYV04 nc
// UJYV05 >> TEDO01

// UBAL1 << UNOR04
// UBAL2 << (mux1) << WR_C
// UBAL3 << (mux0) << APOV04
// UBAL4 nc
// UBAL5 >>

//------------------------------------------------------------------------------

CpuBusSignals CpuBus::sig(const SchematicTop& gb) const {
  return sig(gb.clk_reg, gb.dbg_reg.sig(gb));
}

CpuBusSignals CpuBus::sig(const ClockRegisters& clk_reg, const DebugSignals& dbg_sig) const {
  auto& cpu_bus = *this;
  CpuBusSignals cpu_sig;

  {
    /*p07.UJYV*/ cpu_sig.UJYV_CPU_RD = mux2_n(EXT_PIN_RD_C, cpu_bus.CPU_PIN_RD, dbg_sig.UNOR_MODE_DBG2p);
  }

  {
    /*p01.ATYP*/ wire ATYP_xBCDExxx = not(!clk_reg.AFUR_xBCDExxx);
    /*p01.ADAR*/ wire ADAR_ABCDxxxx = not(clk_reg.ADYK_xxxxEFGH);
    /*p01.AFAS*/ wire AFAS_xxxxxFGH = nor(ADAR_ABCDxxxx, ATYP_xBCDExxx);
    /*p01.AREV*/ wire AREV_CPU_WRn_ABCDExxx = nand(cpu_bus.CPU_PIN_WR, AFAS_xxxxxFGH);
    /*p01.APOV*/ wire APOV_CPU_WRp_xxxxxFGH    = not(AREV_CPU_WRn_ABCDExxx);
    /*p07.UBAL*/ cpu_sig.UBAL_CPU_WRp_ABCDExxx = mux2_n(EXT_PIN_WRn_C, APOV_CPU_WRp_xxxxxFGH, dbg_sig.UNOR_MODE_DBG2p);
  }

  return cpu_sig;
}

//------------------------------------------------------------------------------

void CpuBus::tick(SchematicTop& gb) {
  {
    auto& cpu_bus = gb.cpu_bus;
    auto cpu_sig = gb.cpu_bus.sig(gb);
    auto dbg_sig = gb.dbg_reg.sig(gb);

    // So does this mean that if the CPU writes to the external bus during dma, that data
    // will actually end up in oam?


    /*p08.SORE*/ wire SORE_0000_7FFFp = not(cpu_bus.CPU_PIN_A15);
    /*p08.TEVY*/ wire TEVY_8000_9FFFn = or(cpu_bus.CPU_PIN_A13, cpu_bus.CPU_PIN_A14, SORE_0000_7FFFp);
    /*p08.TEXO*/ wire TEXO_8000_9FFFn = and (cpu_bus.CPU_PIN_ADDR_VALID, TEVY_8000_9FFFn);
    /*p08.LEVO*/ wire LEVO_8000_9FFFp = not(TEXO_8000_9FFFn);
    /*p08.LAGU*/ wire LAGU = or(and(cpu_bus.CPU_PIN_RD, LEVO_8000_9FFFp), cpu_bus.CPU_PIN_WR);
    /*p08.LYWE*/ wire LYWE = not(LAGU);
    /*p08.MOCA*/ wire MOCA_DBG_EXT_RD = nor(TEXO_8000_9FFFn, dbg_sig.UMUT_MODE_DBG1p);
    /*p08.MOTY*/ wire MOTY_CPU_EXT_RD = or(MOCA_DBG_EXT_RD, LYWE);

    /*p07.TEDO*/ wire TEDO_CPU_RD = not(cpu_sig.UJYV_CPU_RD);
    /*p08.REDU*/ wire REDU_CPU_RD = not(TEDO_CPU_RD);
    /*p08.RORU*/ wire RORU_IBUS_TO_EBUSn = mux2_p(REDU_CPU_RD, MOTY_CPU_EXT_RD, dbg_sig.UNOR_MODE_DBG2p);

    /*p08.LULA*/ wire LULA_IBUS_TO_EBUSp = not(RORU_IBUS_TO_EBUSn);
    EXT_PIN_D0_B.set(LULA_IBUS_TO_EBUSp);
    EXT_PIN_D1_B.set(LULA_IBUS_TO_EBUSp);
    EXT_PIN_D2_B.set(LULA_IBUS_TO_EBUSp);
    EXT_PIN_D3_B.set(LULA_IBUS_TO_EBUSp);
    EXT_PIN_D4_B.set(LULA_IBUS_TO_EBUSp);
    EXT_PIN_D5_B.set(LULA_IBUS_TO_EBUSp);
    EXT_PIN_D6_B.set(LULA_IBUS_TO_EBUSp);
    EXT_PIN_D7_B.set(LULA_IBUS_TO_EBUSp);

    /*p25.RUXA*/ EXT_PIN_D0_A.set(nand(CPU_TRI_D0, LULA_IBUS_TO_EBUSp));
    /*p25.RUJA*/ EXT_PIN_D1_A.set(nand(CPU_TRI_D1, LULA_IBUS_TO_EBUSp));
    /*p25.RABY*/ EXT_PIN_D2_A.set(nand(CPU_TRI_D2, LULA_IBUS_TO_EBUSp));
    /*p25.RERA*/ EXT_PIN_D3_A.set(nand(CPU_TRI_D3, LULA_IBUS_TO_EBUSp));
    /*p25.RORY*/ EXT_PIN_D4_A.set(nand(CPU_TRI_D4, LULA_IBUS_TO_EBUSp));
    /*p25.RYVO*/ EXT_PIN_D5_A.set(nand(CPU_TRI_D5, LULA_IBUS_TO_EBUSp));
    /*p25.RAFY*/ EXT_PIN_D6_A.set(nand(CPU_TRI_D6, LULA_IBUS_TO_EBUSp));
    /*p25.RAVU*/ EXT_PIN_D7_A.set(nand(CPU_TRI_D7, LULA_IBUS_TO_EBUSp));

    /*p08.RUNE*/ EXT_PIN_D0_D.set(nor(CPU_TRI_D0, RORU_IBUS_TO_EBUSn));
    /*p08.RYPU*/ EXT_PIN_D1_D.set(nor(CPU_TRI_D1, RORU_IBUS_TO_EBUSn));
    /*p08.SULY*/ EXT_PIN_D2_D.set(nor(CPU_TRI_D2, RORU_IBUS_TO_EBUSn));
    /*p08.SEZE*/ EXT_PIN_D3_D.set(nor(CPU_TRI_D3, RORU_IBUS_TO_EBUSn));
    /*p08.RESY*/ EXT_PIN_D4_D.set(nor(CPU_TRI_D4, RORU_IBUS_TO_EBUSn));
    /*p08.TAMU*/ EXT_PIN_D5_D.set(nor(CPU_TRI_D5, RORU_IBUS_TO_EBUSn));
    /*p08.ROGY*/ EXT_PIN_D6_D.set(nor(CPU_TRI_D6, RORU_IBUS_TO_EBUSn));
    /*p08.RYDA*/ EXT_PIN_D7_D.set(nor(CPU_TRI_D7, RORU_IBUS_TO_EBUSn));
  }

  {
    auto& cpu_bus = gb.cpu_bus;

    // External data bus to internal data bus
    // SOMA01 << LAVO04
    // SOMA02 nc
    // SOMA03 << D0_C
    // SOMA04 nc
    // SOMA05 nc
    // SOMA06 nc
    // SOMA07 >> RYMA04
    // SOMA08 nc
    // SOMA09 == nc

    // Is this actually like a pass gate? We already know the latch cells, and this is bigger than those.

    /*p08.SORE*/ wire SORE_0000_7FFFp = not(cpu_bus.CPU_PIN_A15);
    /*p08.TEVY*/ wire TEVY_8000_9FFFn = or(cpu_bus.CPU_PIN_A13, cpu_bus.CPU_PIN_A14, SORE_0000_7FFFp);
    /*p08.TEXO*/ wire TEXO_8000_9FFFn = and (cpu_bus.CPU_PIN_ADDR_VALID, TEVY_8000_9FFFn);
    /*p08.LAVO*/ wire LAVO_LATCH_CPU_DATAp = nand(cpu_bus.CPU_PIN_RD, TEXO_8000_9FFFn, cpu_bus.CPU_PIN5);

    /*p08.SOMA*/ SOMA_EXT_DATA_LATCH_00.tp_latch(LAVO_LATCH_CPU_DATAp, gb.EXT_PIN_D0_C);
    /*p08.RONY*/ RONY_EXT_DATA_LATCH_01.tp_latch(LAVO_LATCH_CPU_DATAp, gb.EXT_PIN_D1_C);
    /*p08.RAXY*/ RAXY_EXT_DATA_LATCH_02.tp_latch(LAVO_LATCH_CPU_DATAp, gb.EXT_PIN_D2_C);
    /*p08.SELO*/ SELO_EXT_DATA_LATCH_03.tp_latch(LAVO_LATCH_CPU_DATAp, gb.EXT_PIN_D3_C);
    /*p08.SODY*/ SODY_EXT_DATA_LATCH_04.tp_latch(LAVO_LATCH_CPU_DATAp, gb.EXT_PIN_D4_C);
    /*p08.SAGO*/ SAGO_EXT_DATA_LATCH_05.tp_latch(LAVO_LATCH_CPU_DATAp, gb.EXT_PIN_D5_C);
    /*p08.RUPA*/ RUPA_EXT_DATA_LATCH_06.tp_latch(LAVO_LATCH_CPU_DATAp, gb.EXT_PIN_D6_C);
    /*p08.SAZY*/ SAZY_EXT_DATA_LATCH_07.tp_latch(LAVO_LATCH_CPU_DATAp, gb.EXT_PIN_D7_C);

    // RYMA 6-rung green tribuf

    /*p08.RYMA*/ CPU_TRI_D0.set_tribuf(LAVO_LATCH_CPU_DATAp, SOMA_EXT_DATA_LATCH_00);
    /*p08.RUVO*/ CPU_TRI_D1.set_tribuf(LAVO_LATCH_CPU_DATAp, RONY_EXT_DATA_LATCH_01);
    /*p08.RYKO*/ CPU_TRI_D2.set_tribuf(LAVO_LATCH_CPU_DATAp, RAXY_EXT_DATA_LATCH_02);
    /*p08.TAVO*/ CPU_TRI_D3.set_tribuf(LAVO_LATCH_CPU_DATAp, SELO_EXT_DATA_LATCH_03);
    /*p08.TEPE*/ CPU_TRI_D4.set_tribuf(LAVO_LATCH_CPU_DATAp, SODY_EXT_DATA_LATCH_04);
    /*p08.SAFO*/ CPU_TRI_D5.set_tribuf(LAVO_LATCH_CPU_DATAp, SAGO_EXT_DATA_LATCH_05);
    /*p08.SEVU*/ CPU_TRI_D6.set_tribuf(LAVO_LATCH_CPU_DATAp, RUPA_EXT_DATA_LATCH_06);
    /*p08.TAJU*/ CPU_TRI_D7.set_tribuf(LAVO_LATCH_CPU_DATAp, SAZY_EXT_DATA_LATCH_07);
  }}

//------------------------------------------------------------------------------

SignalHash CpuBus::commit() {
  SignalHash hash;
  /* PIN_78 */ hash << EXT_PIN_WRn_C.clear_preset();     // -> UBAL
  /* PIN_79 */ hash << EXT_PIN_RD_C.clear_preset();     // -> UJYV

  hash << SOMA_EXT_DATA_LATCH_00.commit_latch();
  hash << RONY_EXT_DATA_LATCH_01.commit_latch();
  hash << RAXY_EXT_DATA_LATCH_02.commit_latch();
  hash << SELO_EXT_DATA_LATCH_03.commit_latch();
  hash << SODY_EXT_DATA_LATCH_04.commit_latch();
  hash << SAGO_EXT_DATA_LATCH_05.commit_latch();
  hash << RUPA_EXT_DATA_LATCH_06.commit_latch();
  hash << SAZY_EXT_DATA_LATCH_07.commit_latch();

  hash << CPU_TRI_D0.commit_tribuf();
  hash << CPU_TRI_D1.commit_tribuf();
  hash << CPU_TRI_D2.commit_tribuf();
  hash << CPU_TRI_D3.commit_tribuf();
  hash << CPU_TRI_D4.commit_tribuf();
  hash << CPU_TRI_D5.commit_tribuf();
  hash << CPU_TRI_D6.commit_tribuf();
  hash << CPU_TRI_D7.commit_tribuf();

  hash << CPU_PIN_RD.clear_preset();     // PORTA_00: -> UJYV, LAGU, LAVO
  hash << CPU_PIN_WR.clear_preset();     // PORTA_01: -> AREV, LAGU.
  hash << CPU_PIN_ADDR_VALID.clear_preset();     // PORTA_06: -> APAP, TEXO
  hash << CPU_PIN5.clear_preset();     // PORTD_05: -> FROM_CPU5
  hash << CPU_PIN6.clear_preset();      // PORTD_00: -> LEXY, doesn't do anything
  hash << CPU_PIN_CLKREQ.clear_preset();         // PORTC_00: -> ABOL

  hash << CPU_PIN_A00.clear_preset();
  hash << CPU_PIN_A01.clear_preset();
  hash << CPU_PIN_A02.clear_preset();
  hash << CPU_PIN_A03.clear_preset();
  hash << CPU_PIN_A04.clear_preset();
  hash << CPU_PIN_A05.clear_preset();
  hash << CPU_PIN_A06.clear_preset();
  hash << CPU_PIN_A07.clear_preset();
  hash << CPU_PIN_A08.clear_preset();
  hash << CPU_PIN_A09.clear_preset();
  hash << CPU_PIN_A10.clear_preset();
  hash << CPU_PIN_A11.clear_preset();
  hash << CPU_PIN_A12.clear_preset();
  hash << CPU_PIN_A13.clear_preset();
  hash << CPU_PIN_A14.clear_preset();
  hash << CPU_PIN_A15.clear_preset();

  /* PIN_01 */ hash << EXT_PIN_A00_C.clear_preset();      // -> KOVA
  /* PIN_02 */ hash << EXT_PIN_A01_C.clear_preset();      // -> CAMU
  /* PIN_03 */ hash << EXT_PIN_A02_C.clear_preset();      // -> BUXU
  /* PIN_04 */ hash << EXT_PIN_A03_C.clear_preset();      // -> BASE
  /* PIN_05 */ hash << EXT_PIN_A04_C.clear_preset();      // -> AFEC
  /* PIN_06 */ hash << EXT_PIN_A05_C.clear_preset();      // -> ABUP
  /* PIN_07 */ hash << EXT_PIN_A06_C.clear_preset();      // -> CYGU
  /* PIN_08 */ hash << EXT_PIN_A07_C.clear_preset();      // -> COGO
  /* PIN_09 */ hash << EXT_PIN_A08_C.clear_preset();      // -> MUJY
  /* PIN_10 */ hash << EXT_PIN_A09_C.clear_preset();      // -> NENA
  /* PIN_11 */ hash << EXT_PIN_A10_C.clear_preset();      // -> SURA
  /* PIN_12 */ hash << EXT_PIN_A11_C.clear_preset();      // -> MADY
  /* PIN_13 */ hash << EXT_PIN_A12_C.clear_preset();      // -> LAHE
  /* PIN_14 */ hash << EXT_PIN_A13_C.clear_preset();      // -> LURA
  /* PIN_15 */ hash << EXT_PIN_A14_C.clear_preset();      // -> PEVO
  /* PIN_16 */ hash << EXT_PIN_A15_C.clear_preset();      // -> RAZA


  /* PIN_17 */ hash << EXT_PIN_D0_A.commit_pinout();      // <- RUXA
  /* PIN_17 */ hash << EXT_PIN_D0_B.commit_pinout();      // <- LULA
  /* PIN_17 */ hash << EXT_PIN_D0_D.commit_pinout();      // <- RUNE
  /* PIN_18 */ hash << EXT_PIN_D1_A.commit_pinout();      // <- RUJA
  /* PIN_18 */ hash << EXT_PIN_D1_B.commit_pinout();      // <- LULA
  /* PIN_18 */ hash << EXT_PIN_D1_D.commit_pinout();      // <- RYPU
  /* PIN_19 */ hash << EXT_PIN_D2_A.commit_pinout();      // <- RABY
  /* PIN_19 */ hash << EXT_PIN_D2_B.commit_pinout();      // <- LULA
  /* PIN_19 */ hash << EXT_PIN_D2_D.commit_pinout();      // <- SULY
  /* PIN_20 */ hash << EXT_PIN_D3_A.commit_pinout();      // <- RERA
  /* PIN_20 */ hash << EXT_PIN_D3_B.commit_pinout();      // <- LULA
  /* PIN_20 */ hash << EXT_PIN_D3_D.commit_pinout();      // <- SEZE
  /* PIN_21 */ hash << EXT_PIN_D4_A.commit_pinout();      // <- RORY
  /* PIN_21 */ hash << EXT_PIN_D4_B.commit_pinout();      // <- LULA
  /* PIN_21 */ hash << EXT_PIN_D4_D.commit_pinout();      // <- RESY
  /* PIN_22 */ hash << EXT_PIN_D5_A.commit_pinout();      // <- RYVO
  /* PIN_22 */ hash << EXT_PIN_D5_B.commit_pinout();      // <- LULA
  /* PIN_22 */ hash << EXT_PIN_D5_D.commit_pinout();      // <- TAMU
  /* PIN_23 */ hash << EXT_PIN_D6_A.commit_pinout();      // <- RAFY
  /* PIN_23 */ hash << EXT_PIN_D6_B.commit_pinout();      // <- LULA
  /* PIN_23 */ hash << EXT_PIN_D6_D.commit_pinout();      // <- ROGY
  /* PIN_24 */ hash << EXT_PIN_D7_A.commit_pinout();      // <- RAVU
  /* PIN_24 */ hash << EXT_PIN_D7_B.commit_pinout();      // <- LULA
  /* PIN_24 */ hash << EXT_PIN_D7_D.commit_pinout();      // <- RYDA

  return hash;
}

//------------------------------------------------------------------------------

void CpuPinsOut::tick(SchematicTop& gb) {
  auto& cpu_bus = gb.cpu_bus;

  auto rst_sig = gb.rst_reg.sig(gb);
  auto cpu_sig = gb.cpu_bus.sig(gb);
  auto boot_sig = gb.bootrom.sig(gb);
  auto& clk_reg = gb.clk_reg;

#if 0
  // Debug stuff I disabled

  /*p07.APET*/ wire APET_MODE_DBG = or(dbg_sig.UMUT_MODE_DBG1p, UNOR_MODE_DBG2p); // suggests UMUTp
  /*p07.APER*/ wire FF60_WRn = nand(APET_MODE_DBG, CPU_PIN_A05, CPU_PIN_A06, TAPU_CPUWR, ADDR_111111110xx00000);

  //----------
  // weird debug things, probably not right

  /*p05.AXYN*/ wire AXYN_xBCDEFGH = not(clk_reg.BEDO_xBxxxxxx);
  /*p05.ADYR*/ wire ADYR_Axxxxxxx = not(AXYN_xBCDEFGH);
  /*p05.APYS*/ wire APYS_xBCDEFGH = nor(sys_sig.MODE_DBG2, ADYR_Axxxxxxx);
  /*p05.AFOP*/ wire AFOP_Axxxxxxx = not(APYS_xBCDEFGH);
  /*p07.LECO*/ wire LECO_xBCDEFGH = nor(clk_reg.BEDO_xBxxxxxx, sys_sig.MODE_DBG2);

  if (AFOP_Axxxxxxx) set_data(
    /*p05.ANOC*/ not(sys_sig.PIN_P10_B),
    /*p05.ATAJ*/ not(sys_sig.PIN_P10_B),
    /*p05.AJEC*/ not(sys_sig.PIN_P10_B),
    /*p05.ASUZ*/ not(sys_sig.PIN_P10_B),
    /*p05.BENU*/ not(sys_sig.PIN_P10_B),
    /*p05.AKAJ*/ not(sys_sig.PIN_P10_B),
    /*p05.ARAR*/ not(sys_sig.PIN_P10_B),
    /*p05.BEDA*/ not(sys_sig.PIN_P10_B)
  );

  if (LECO_xBCDEFGH) set_data(
    /*p07.ROMY*/ sys_sig.PIN_P10_B,
    /*p07.RYNE*/ sys_sig.PIN_P10_B,
    /*p07.REJY*/ sys_sig.PIN_P10_B,
    /*p07.RASE*/ sys_sig.PIN_P10_B,
    /*p07.REKA*/ sys_sig.PIN_P10_B,
    /*p07.ROWE*/ sys_sig.PIN_P10_B,
    /*p07.RYKE*/ sys_sig.PIN_P10_B,
    /*p07.RARU*/ sys_sig.PIN_P10_B
  );



  //----------
  // more debug stuff

  /*p25.TUSO*/ wire TUSO = nor(MODE_DBG2, clk.PIN_BOGA_AxCDEFGH);
  /*p25.SOLE*/ wire SOLE = not(TUSO);

  if (dbg_sig.VYPO_P10_Bn) bus_out.set_data(
    /*p25.TOVU*/ SOLE,
    /*p25.SOSA*/ SOLE,
    /*p25.SEDU*/ SOLE,
    /*p25.TAXO*/ SOLE,
    /*p25.TAHY*/ SOLE,
    /*p25.TESU*/ SOLE,
    /*p25.TAZU*/ SOLE,
    /*p25.TEWA*/ SOLE
  );

  // FIXME
  ///*p05.KORE*/ wire P05_NC0 = nand(DBG_FF00_D7, FF60_0);
  ///*p05.KYWE*/ wire P05_NC1 = nor (DBG_FF00_D7, FF60_0o);

  /*p08.LYRA*/ wire DBG_D_RDn = nand(sys_sig.MODE_DBG2, bus_sig.CBUS_TO_CEXTn);
  /*p08.TUTY*/ if (!DBG_D_RDn) CPU_TRI_D0 = not(/*p08.TOVO*/ not(pins.PIN_D0_C));
  /*p08.SYWA*/ if (!DBG_D_RDn) CPU_TRI_D1 = not(/*p08.RUZY*/ not(pins.PIN_D1_C));
  /*p08.SUGU*/ if (!DBG_D_RDn) CPU_TRI_D2 = not(/*p08.ROME*/ not(pins.PIN_D2_C));
  /*p08.TAWO*/ if (!DBG_D_RDn) CPU_TRI_D3 = not(/*p08.SAZA*/ not(pins.PIN_D3_C));
  /*p08.TUTE*/ if (!DBG_D_RDn) CPU_TRI_D4 = not(/*p08.TEHE*/ not(pins.PIN_D4_C));
  /*p08.SAJO*/ if (!DBG_D_RDn) CPU_TRI_D5 = not(/*p08.RATU*/ not(pins.PIN_D5_C));
  /*p08.TEMY*/ if (!DBG_D_RDn) CPU_TRI_D6 = not(/*p08.SOCA*/ not(pins.PIN_D6_C));
  /*p08.ROPA*/ if (!DBG_D_RDn) CPU_TRI_D7 = not(/*p08.RYBA*/ not(pins.PIN_D7_C));
#endif

  // hack, not correct
#if 0
  {
    // FF60 debug reg
    /*p07.APET*/ wire APET_MODE_DBG = or(sys_sig.MODE_DBG1, sys_sig.MODE_DBG2);
    /*p07.APER*/ wire FF60_WRn = nand(APET_MODE_DBG, CPU_PIN_A05, CPU_PIN_A06, bus_sig.TAPU_CPUWR, dec_sig.ADDR_111111110xx00000);

    /*p05.KURA*/ wire FF60_0n = not(BURO_FF60_0);
    /*p05.JEVA*/ wire FF60_0o = not(BURO_FF60_0);
    /*p07.BURO*/ BURO_FF60_0.set(FF60_WRn, rst_sig.SYS_RESETn, CPU_TRI_D0);
    /*p07.AMUT*/ AMUT_FF60_1.set(FF60_WRn, rst_sig.SYS_RESETn, CPU_TRI_D1);

    ///*p05.KURA*/ wire FF60_0n = not(FF60);
    ///*p05.JEVA*/ wire FF60_0o = not(FF60);
    /*p07.BURO*/ dbg_reg.BURO_FF60_0.set(1, rst_sig.ALUR_RSTn, CPU_TRI_D0);
    /*p07.AMUT*/ dbg_reg.AMUT_FF60_1.set(1, rst_sig.ALUR_RSTn, CPU_TRI_D1);
  }
#endif


  /*p01.ATEZ*/ wire ATEZ_CLKBAD   = not(gb.EXT_PIN_CLK_GOOD);
  /*p01.ABOL*/ wire ABOL_CLKREQn  = not(cpu_bus.CPU_PIN_CLKREQ);
  /*p01.BUTY*/ wire BUTY_CLKREQ   = not(ABOL_CLKREQn);

  /*p01.ATYP*/ wire ATYP_xBCDExxx = not(!clk_reg.AFUR_xBCDExxx);
  /*p01.NULE*/ wire NULE_AxxxxFGH = nor(ABOL_CLKREQn,  ATYP_xBCDExxx);
  /*p01.AROV*/ wire AROV_xxxDEFGx = not(!clk_reg.APUK_xxxDEFGx);
  /*p01.BAPY*/ wire BAPY_AxxxxxxH = nor(ABOL_CLKREQn,  AROV_xxxDEFGx, ATYP_xBCDExxx);
  /*p01.AFEP*/ wire AFEP_ABxxxxGH = not(clk_reg.ALEF_xxCDEFxx);
  /*p01.BYRY*/ wire BYRY_xBCDExxx = not(NULE_AxxxxFGH);
  /*p01.BUDE*/ wire BUDE_AxxxxFGH = not(BYRY_xBCDExxx);
  /*p01.BERU*/ wire BERU_xBCDEFGx = not(BAPY_AxxxxxxH);
  /*p01.BUFA*/ wire BUFA_AxxxxxxH = not(BERU_xBCDEFGx);
  /*p01.BOLO*/ wire BOLO_xBCDEFGx = not(BUFA_AxxxxxxH);
  /*p01.BEKO*/ wire BEKO_xBCDExxx = not(BUDE_AxxxxFGH);
  /*p01.BEJA*/ wire BEJA_AxxxxFGH = nand(BOLO_xBCDEFGx, BEKO_xBCDExxx);
  /*p01.BANE*/ wire BANE_xBCDExxx = not(BEJA_AxxxxFGH);
  /*p01.BELO*/ wire BELO_AxxxxFGH = not(BANE_xBCDExxx);
  /*p01.BAZE*/ wire BAZE_xBCDExxx = not(BELO_AxxxxFGH);
  /*p01.BUTO*/ wire BUTO_AxCDEFGH = nand(AFEP_ABxxxxGH, ATYP_xBCDExxx, BAZE_xBCDExxx);
  /*p01.BELE*/ wire BELE_xBxxxxxx = not(BUTO_AxCDEFGH);
  /*p01.BYJU*/ wire BYJU_AxCDEFGH = nor(BELE_xBxxxxxx, ATEZ_CLKBAD);
  /*p01.BALY*/ wire BALY_xBxxxxxx = not(BYJU_AxCDEFGH);
  /*p01.BUVU*/ wire BUVU_xBxxxxxx = and(BUTY_CLKREQ, BALY_xBxxxxxx);
  /*p01.BYXO*/ wire BYXO_AxCDEFGH = not(BUVU_xBxxxxxx);
  /*p01.BEDO*/ wire BEDO_xBxxxxxx = not(BYXO_AxCDEFGH);
  /*p01.BOWA*/ wire BOWA_AxCDEFGH = not(BEDO_xBxxxxxx);
  /*p01.BOGA*/ wire BOGA_AxCDEFGH = not(BALY_xBxxxxxx);
  /*p01.BOMA*/ wire BOMA_xBxxxxxx = not(BOGA_AxCDEFGH);
  /*p01.BUGO*/ wire BUGO_xxCDEFxx = not(AFEP_ABxxxxGH);
  /*p01.BATE*/ wire BATE_ABxxxxxH = nor(ABOL_CLKREQn,  BUGO_xxCDEFxx, AROV_xxxDEFGx);
  /*p01.BASU*/ wire BASU_xxCDEFGx = not(BATE_ABxxxxxH);
  /*p01.BUKE*/ wire BUKE_ABxxxxxH = not(BASU_xxCDEFGx);

  PIN_BOWA_AxCDEFGH.set(BOWA_AxCDEFGH);
  PIN_BEDO_xBxxxxxx.set(BEDO_xBxxxxxx);
  PIN_BEKO_xBCDExxx.set(BEKO_xBCDExxx);
  PIN_BUDE_AxxxxFGH.set(BUDE_AxxxxFGH);
  PIN_BOLO_xBCDEFGx.set(BOLO_xBCDEFGx);
  PIN_BUKE_ABxxxxxH.set(BUKE_ABxxxxxH);
  PIN_BOMA_xBxxxxxx.set(BOMA_xBxxxxxx);
  PIN_BOGA_AxCDEFGH.set(BOGA_AxCDEFGH);
  
  PIN_AFER_RSTp.set(rst_sig.AFER_RSTp);
  PIN_EXT_RESET.set(gb.EXT_PIN_RST);
  PIN_EXT_CLKGOOD.set(gb.EXT_PIN_CLK_GOOD);
  PIN_TABA_RSTp.set(rst_sig.TABA_RSTp);

  /*p07.TUNA*/ wire TUNA_0000_FDFFp = nand(cpu_bus.CPU_PIN_A15, cpu_bus.CPU_PIN_A14, cpu_bus.CPU_PIN_A13, cpu_bus.CPU_PIN_A12, cpu_bus.CPU_PIN_A11, cpu_bus.CPU_PIN_A10, cpu_bus.CPU_PIN_A09);
  /*p25.SYRO*/ wire SYRO_FE00_FFFFp = not(TUNA_0000_FDFFp);
  PIN_SYRO.set(SYRO_FE00_FFFFp);
  PIN_TUTU_BOOTp.set(boot_sig.TUTU_BOOTp);
}

//------------------------------------------------------------------------------

void CpuPinsOut::commit(SignalHash& hash) {
  hash << PIN_AWOB.commit_pinout();       // <- P02.AWOB
  hash << PIN_TUTU_BOOTp.commit_pinout();  // PORTA_04: <- TUTU
  hash << PIN_AFER_RSTp.commit_pinout();          // PORTC_01: <- AFER
  hash << PIN_EXT_RESET.commit_pinout();         // PORTC_02: <- PIN_RESET directly connected to the pad
  hash << PIN_EXT_CLKGOOD.commit_pinout();      // PORTC_03: <- CLKIN_A
  hash << PIN_TABA_RSTp.commit_pinout();     // PORTC_04: <- TABA
  hash << PIN_BOWA_AxCDEFGH.commit_pinout(); // PORTD_01: <- BOWA
  hash << PIN_BEDO_xBxxxxxx.commit_pinout(); // PORTD_02: <- BEDO _____fgh
  hash << PIN_BEKO_xBCDExxx.commit_pinout(); // PORTD_03: <- BEKO ____efgh connection not indicated on P01
  hash << PIN_BUDE_AxxxxFGH.commit_pinout(); // PORTD_04: <- BUDE abcd____
  hash << PIN_BOLO_xBCDEFGx.commit_pinout(); // PORTD_05: <- BOLO
  hash << PIN_BUKE_ABxxxxxH.commit_pinout(); // PORTD_06: <- BUKE _____f__
  hash << PIN_BOMA_xBxxxxxx.commit_pinout(); // PORTD_07: <- BOMA _____fgh
  hash << PIN_BOGA_AxCDEFGH.commit_pinout(); // PORTD_08: <- BOGA abcde___
}

//------------------------------------------------------------------------------

#if 0
void dump_pins(TextPainter& text_painter) {
  text_painter.dprintf("----- CPU DBG/PIN_RST -----\n");
  text_painter.dprintf("PIN_EXT_RESET     %d\n", PIN_EXT_RESET.a.val);
  text_painter.dprintf("PIN_TABA_RSTp     %d\n", PIN_TABA_RSTp.a.val);
  text_painter.dprintf("PIN_EXT_CLKGOOD   %d\n", PIN_EXT_CLKGOOD.a.val);

  text_painter.dprintf("----- CPU CLOCKS -----\n");

  text_painter.dprintf("PIN_BOWA_AxCDEFGH %d\n", PIN_BOWA_AxCDEFGH.a.val);
  text_painter.dprintf("PIN_BEDO_xBxxxxxx %d\n", PIN_BEDO_xBxxxxxx.a.val);
  text_painter.dprintf("PIN_BEKO_xBCDExxx %d\n", PIN_BEKO_xBCDExxx.a.val);
  text_painter.dprintf("PIN_BUDE_AxxxxFGH %d\n", PIN_BUDE_AxxxxFGH.a.val);
  text_painter.dprintf("PIN_BOLO_xBCDEFGx %d\n", PIN_BOLO_xBCDEFGx.a.val);
  text_painter.dprintf("PIN_BUKE_ABxxxxxH %d\n", PIN_BUKE_ABxxxxxH.a.val);
  text_painter.dprintf("PIN_BOMA_xBxxxxxx %d\n", PIN_BOMA_xBxxxxxx.a.val);
  text_painter.dprintf("PIN_BOGA_AxCDEFGH %d\n", PIN_BOGA_AxCDEFGH.a.val);

  text_painter.dprintf("----- FROM CPU -----\n");
  //text_painter.dprintf("CPU_PIN_CLKREQ        %d\n", CPU_PIN_CLKREQ.a.val);
  //text_painter.dprintf("CPU_PIN5     %d\n", CPU_PIN5.a.val);
  //text_painter.dprintf("CPU_PIN6     %d\n", CPU_PIN6.a.val);

  text_painter.dprintf("----- TO CPU -----\n");
  text_painter.dprintf("PIN_AFER_RSTp     %d\n", PIN_AFER_RSTp.a.val);
  text_painter.dprintf("PIN_AWOB          %d\n", PIN_AWOB.a.val);
  text_painter.dprintf("PIN_SYRO          %d\n", PIN_SYRO.a.val);
  text_painter.dprintf("PIN_TUTU_BOOTp    %d\n", PIN_TUTU_BOOTp.a.val);

  /*
  text_painter.dprintf("----- CPU INT -----\n");
  text_painter.dprintf("PIN_INT_VBLANK    %d\n", PIN_INT_VBLANK.a.val);
  text_painter.dprintf("PIN_INT_STAT      %d\n", PIN_INT_STAT.a.val);
  text_painter.dprintf("PIN_INT_TIMER     %d\n", PIN_INT_TIMER.a.val);
  text_painter.dprintf("PIN_INT_SERIAL    %d\n", PIN_INT_SERIAL.a.val);
  text_painter.dprintf("PIN_INT_JOYPAD    %d\n", PIN_INT_JOYPAD.a.val);
  text_painter.dprintf("PIN_ACK_VBLANK    %d\n", PIN_ACK_VBLANK.a.val);
  text_painter.dprintf("PIN_ACK_STAT      %d\n", PIN_ACK_STAT.a.val);
  text_painter.dprintf("PIN_ACK_TIMER     %d\n", PIN_ACK_TIMER.a.val);
  text_painter.dprintf("PIN_ACK_SERIAL    %d\n", PIN_ACK_SERIAL.a.val);
  text_painter.dprintf("PIN_ACK_JOYPAD    %d\n", PIN_ACK_JOYPAD.a.val);
  */

  /*
  text_painter.dprintf("----- CPU BUS -----\n");
  text_painter.dprintf("CPU_PIN_RD    %d\n", CPU_PIN_RD.a.val);
  text_painter.dprintf("CPU_PIN_WR    %d\n", CPU_PIN_WR.a.val);
  text_painter.dprintf("CPU_PIN_ADDR_VALID    %d\n", CPU_PIN_ADDR_VALID.a.val);
  */

  /*
  text_painter.add_text("Axx ");
  dump2(text_painter, CPU_PIN_A15.a);
  dump2(text_painter, CPU_PIN_A14.a);
  dump2(text_painter, CPU_PIN_A13.a);
  dump2(text_painter, CPU_PIN_A12.a);
  text_painter.add_char(':');
  dump2(text_painter, CPU_PIN_A11.a);
  dump2(text_painter, CPU_PIN_A10.a);
  dump2(text_painter, CPU_PIN_A09.a);
  dump2(text_painter, CPU_PIN_A08.a);
  text_painter.add_char(':');
  dump2(text_painter, CPU_PIN_A07.a);
  dump2(text_painter, CPU_PIN_A06.a);
  dump2(text_painter, CPU_PIN_A05.a);
  dump2(text_painter, CPU_PIN_A04.a);
  text_painter.add_char(':');
  dump2(text_painter, CPU_PIN_A03.a);
  dump2(text_painter, CPU_PIN_A02.a);
  dump2(text_painter, CPU_PIN_A01.a);
  dump2(text_painter, CPU_PIN_A00.a);
  text_painter.newline();
  */

  /*
  text_painter.add_text("Dxx ");
  dump2(text_painter, BUS_CPU_D7.a);
  dump2(text_painter, BUS_CPU_D6.a);
  dump2(text_painter, BUS_CPU_D5.a);
  dump2(text_painter, BUS_CPU_D4.a);
  text_painter.add_char(':');
  dump2(text_painter, BUS_CPU_D3.a);
  dump2(text_painter, BUS_CPU_D2.a);
  dump2(text_painter, BUS_CPU_D1.a);
  dump2(text_painter, BUS_CPU_D0.a);
  text_painter.newline();
  */

  text_painter.newline();
}

/*
void preset_data(bool oe, uint8_t data) {
D0.preset(oe, data & 0x01);
D1.preset(oe, data & 0x02);
D2.preset(oe, data & 0x04);
D3.preset(oe, data & 0x08);
D4.preset(oe, data & 0x10);
D5.preset(oe, data & 0x20);
D6.preset(oe, data & 0x40);
D7.preset(oe, data & 0x80);
}
*/

#endif


#if 0
// so the address bus is technically a tribuf, but we're going to ignore
// this debug circuit for now.
{
// If we're in debug mode 2, drive external address bus onto internal address
// these may be backwards, probably don't want to drive external address onto bus normally...

/*p08.KOVA*/ wire A00_Cn = not(EXT_PIN_A00_C);
/*p08.CAMU*/ wire A01_Cn = not(EXT_PIN_A01_C);
/*p08.BUXU*/ wire A02_Cn = not(EXT_PIN_A02_C);
/*p08.BASE*/ wire A03_Cn = not(EXT_PIN_A03_C);
/*p08.AFEC*/ wire A04_Cn = not(EXT_PIN_A04_C);
/*p08.ABUP*/ wire A05_Cn = not(EXT_PIN_A05_C);
/*p08.CYGU*/ wire A06_Cn = not(EXT_PIN_A06_C);
/*p08.COGO*/ wire A07_Cn = not(EXT_PIN_A07_C);
/*p08.MUJY*/ wire A08_Cn = not(EXT_PIN_A08_C);
/*p08.NENA*/ wire A09_Cn = not(EXT_PIN_A09_C);
/*p08.SURA*/ wire A10_Cn = not(EXT_PIN_A10_C);
/*p08.MADY*/ wire A11_Cn = not(EXT_PIN_A11_C);
/*p08.LAHE*/ wire A12_Cn = not(EXT_PIN_A12_C);
/*p08.LURA*/ wire A13_Cn = not(EXT_PIN_A13_C);
/*p08.PEVO*/ wire A14_Cn = not(EXT_PIN_A14_C);
/*p08.RAZA*/ wire A15_Cn = not(EXT_PIN_A15_C);

/*p08.KEJO*/ cpu_bus.CPU_PIN_A00.set(!TOVA_MODE_DBG2n, not(A00_Cn));
/*p08.BYXE*/ cpu_bus.CPU_PIN_A01.set(!TOVA_MODE_DBG2n, not(A01_Cn));
/*p08.AKAN*/ cpu_bus.CPU_PIN_A02.set(!TOVA_MODE_DBG2n, not(A02_Cn));
/*p08.ANAR*/ cpu_bus.CPU_PIN_A03.set(!TOVA_MODE_DBG2n, not(A03_Cn));
/*p08.AZUV*/ cpu_bus.CPU_PIN_A04.set(!TOVA_MODE_DBG2n, not(A04_Cn));
/*p08.AJOV*/ cpu_bus.CPU_PIN_A05.set(!TOVA_MODE_DBG2n, not(A05_Cn));
/*p08.BYNE*/ cpu_bus.CPU_PIN_A06.set(!TOVA_MODE_DBG2n, not(A06_Cn));
/*p08.BYNA*/ cpu_bus.CPU_PIN_A07.set(!TOVA_MODE_DBG2n, not(A07_Cn));
/*p08.LOFA*/ cpu_bus.CPU_PIN_A08.set(!TOVA_MODE_DBG2n, not(A08_Cn));
/*p08.MAPU*/ cpu_bus.CPU_PIN_A09.set(!TOVA_MODE_DBG2n, not(A09_Cn));
/*p08.RALA*/ cpu_bus.CPU_PIN_A10.set(!TOVA_MODE_DBG2n, not(A10_Cn));
/*p08.LORA*/ cpu_bus.CPU_PIN_A11.set(!TOVA_MODE_DBG2n, not(A11_Cn));
/*p08.LYNA*/ cpu_bus.CPU_PIN_A12.set(!TOVA_MODE_DBG2n, not(A12_Cn));
/*p08.LEFY*/ cpu_bus.CPU_PIN_A13.set(!TOVA_MODE_DBG2n, not(A13_Cn));
/*p08.NEFE*/ cpu_bus.CPU_PIN_A14.set(!TOVA_MODE_DBG2n, not(A14_Cn));
/*p08.SYZU*/ cpu_bus.CPU_PIN_A15.set(!TOVA_MODE_DBG2n, not(A15_Cn));
}
#endif
