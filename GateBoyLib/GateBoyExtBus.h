#pragma once
#include "GateBoyLib/Gates.h"

struct GateBoyDMA;
struct GateBoyResetDebug;

//------------------------------------------------------------------------------------------------------------------------

struct ExtDataLatch {
  void reset_to_cart() {
    SOMA_EXT_DATA_LATCH_D0n.state = 0b00011000;
    RONY_EXT_DATA_LATCH_D1n.state = 0b00011000;
    RAXY_EXT_DATA_LATCH_D2n.state = 0b00011000;
    SELO_EXT_DATA_LATCH_D3n.state = 0b00011000;
    SODY_EXT_DATA_LATCH_D4n.state = 0b00011000;
    SAGO_EXT_DATA_LATCH_D5n.state = 0b00011000;
    RUPA_EXT_DATA_LATCH_D6n.state = 0b00011000;
    SAZY_EXT_DATA_LATCH_D7n.state = 0b00011000;
  }

  /*p08.SOMA*/ TpLatch SOMA_EXT_DATA_LATCH_D0n; // AxCxExxx
  /*p08.RONY*/ TpLatch RONY_EXT_DATA_LATCH_D1n; // AxCxExxx
  /*p08.RAXY*/ TpLatch RAXY_EXT_DATA_LATCH_D2n; // AxCxExxx
  /*p08.SELO*/ TpLatch SELO_EXT_DATA_LATCH_D3n; // AxCxExxx
  /*p08.SODY*/ TpLatch SODY_EXT_DATA_LATCH_D4n; // AxCxExxx
  /*p08.SAGO*/ TpLatch SAGO_EXT_DATA_LATCH_D5n; // AxCxExxx
  /*p08.RUPA*/ TpLatch RUPA_EXT_DATA_LATCH_D6n; // AxCxExxx
  /*p08.SAZY*/ TpLatch SAZY_EXT_DATA_LATCH_D7n; // AxCxExxx
};

//------------------------------------------------------------------------------------------------------------------------

struct ExtAddrLatch {
  void reset_to_cart() {
    ALOR_EXT_ADDR_LATCH_00p.state = 0b00011001;
    APUR_EXT_ADDR_LATCH_01p.state = 0b00011000;
    ALYR_EXT_ADDR_LATCH_02p.state = 0b00011001;
    ARET_EXT_ADDR_LATCH_03p.state = 0b00011001;
    AVYS_EXT_ADDR_LATCH_04p.state = 0b00011000;
    ATEV_EXT_ADDR_LATCH_05p.state = 0b00011000;
    AROS_EXT_ADDR_LATCH_06p.state = 0b00011001;
    ARYM_EXT_ADDR_LATCH_07p.state = 0b00011000;
    LUNO_EXT_ADDR_LATCH_08p.state = 0b00011000;
    LYSA_EXT_ADDR_LATCH_09p.state = 0b00011000;
    PATE_EXT_ADDR_LATCH_10p.state = 0b00011000;
    LUMY_EXT_ADDR_LATCH_11p.state = 0b00011000;
    LOBU_EXT_ADDR_LATCH_12p.state = 0b00011000;
    LONU_EXT_ADDR_LATCH_13p.state = 0b00011000;
    NYRE_EXT_ADDR_LATCH_14p.state = 0b00011000;
  }

  /*p08.ALOR*/ TpLatch ALOR_EXT_ADDR_LATCH_00p; // xBxxxxxx
  /*p08.APUR*/ TpLatch APUR_EXT_ADDR_LATCH_01p; // xBxxxxxx
  /*p08.ALYR*/ TpLatch ALYR_EXT_ADDR_LATCH_02p; // xBxxxxxx
  /*p08.ARET*/ TpLatch ARET_EXT_ADDR_LATCH_03p; // xBxxxxxx
  /*p08.AVYS*/ TpLatch AVYS_EXT_ADDR_LATCH_04p; // xBxxxxxx
  /*p08.ATEV*/ TpLatch ATEV_EXT_ADDR_LATCH_05p; // xBxxxxxx
  /*p08.AROS*/ TpLatch AROS_EXT_ADDR_LATCH_06p; // xBxxxxxx
  /*p08.ARYM*/ TpLatch ARYM_EXT_ADDR_LATCH_07p; // xBxxxxxx
  /*p08.LUNO*/ TpLatch LUNO_EXT_ADDR_LATCH_08p; // ABxxxxxx
  /*p08.LYSA*/ TpLatch LYSA_EXT_ADDR_LATCH_09p; // ABxxxxxx
  /*p08.PATE*/ TpLatch PATE_EXT_ADDR_LATCH_10p; // ABxxxxxx
  /*p08.LUMY*/ TpLatch LUMY_EXT_ADDR_LATCH_11p; // ABxxxxxx
  /*p08.LOBU*/ TpLatch LOBU_EXT_ADDR_LATCH_12p; // ABxxxxxx
  /*p08.LONU*/ TpLatch LONU_EXT_ADDR_LATCH_13p; // ABxxxxxx
  /*p08.NYRE*/ TpLatch NYRE_EXT_ADDR_LATCH_14p; // ABxxxxxx
};

//------------------------------------------------------------------------------------------------------------------------

struct GateBoyExtPins {
  void reset_to_cart() {
    PIN_01_A00.reset(0b00011000);
    PIN_02_A01.reset(0b00011001);
    PIN_03_A02.reset(0b00011000);
    PIN_04_A03.reset(0b00011000);
    PIN_05_A04.reset(0b00011001);
    PIN_06_A05.reset(0b00011001);
    PIN_07_A06.reset(0b00011000);
    PIN_08_A07.reset(0b00011001);
    PIN_09_A08.reset(0b00011001);
    PIN_10_A09.reset(0b00011001);
    PIN_11_A10.reset(0b00011001);
    PIN_12_A11.reset(0b00011001);
    PIN_13_A12.reset(0b00011001);
    PIN_14_A13.reset(0b00011001);
    PIN_15_A14.reset(0b00011001);
    PIN_16_A15.reset(0b00011000);

    PIN_17_D00.reset_int(0b00010100);
    PIN_18_D01.reset_int(0b00010100);
    PIN_19_D02.reset_int(0b00010100);
    PIN_20_D03.reset_int(0b00010100);
    PIN_21_D04.reset_int(0b00010100);
    PIN_22_D05.reset_int(0b00010100);
    PIN_23_D06.reset_int(0b00010100);
    PIN_24_D07.reset_int(0b00010100);

    PIN_80_CSn.reset(0b00011000);
    PIN_79_RDn.reset(0b00011001);
    PIN_78_WRn.reset(0b00011000);
  }

  /*PIN_01*/ PinOut PIN_01_A00;
  /*PIN_02*/ PinOut PIN_02_A01;
  /*PIN_03*/ PinOut PIN_03_A02;
  /*PIN_04*/ PinOut PIN_04_A03;
  /*PIN_05*/ PinOut PIN_05_A04;
  /*PIN_06*/ PinOut PIN_06_A05;
  /*PIN_07*/ PinOut PIN_07_A06;
  /*PIN_08*/ PinOut PIN_08_A07;
  /*PIN_09*/ PinOut PIN_09_A08;
  /*PIN_10*/ PinOut PIN_10_A09;
  /*PIN_11*/ PinOut PIN_11_A10;
  /*PIN_12*/ PinOut PIN_12_A11;
  /*PIN_13*/ PinOut PIN_13_A12;
  /*PIN_14*/ PinOut PIN_14_A13;
  /*PIN_15*/ PinOut PIN_15_A14;
  /*PIN_16*/ PinOut PIN_16_A15;

  /*PIN_17*/ PinIO PIN_17_D00;
  /*PIN_18*/ PinIO PIN_18_D01;
  /*PIN_19*/ PinIO PIN_19_D02;
  /*PIN_20*/ PinIO PIN_20_D03;
  /*PIN_21*/ PinIO PIN_21_D04;
  /*PIN_22*/ PinIO PIN_22_D05;
  /*PIN_23*/ PinIO PIN_23_D06;
  /*PIN_24*/ PinIO PIN_24_D07;

  /*PIN_80*/ PinOut PIN_80_CSn;      // CS changes on phase C if addr in [A000,FDFF]
  /*PIN_79*/ PinOut PIN_79_RDn;      // RDn idles low, goes high on phase B for an external write
  /*PIN_78*/ PinOut PIN_78_WRn;      // WRn idles high, goes low during EFG if there's a write

  Gate MBC1_RAM_EN;
  Gate MBC1_MODE;

  Gate MBC1_BANK0;
  Gate MBC1_BANK1;
  Gate MBC1_BANK2;
  Gate MBC1_BANK3;
  Gate MBC1_BANK4;
  Gate MBC1_BANK5;
  Gate MBC1_BANK6;
};

//------------------------------------------------------------------------------------------------------------------------
