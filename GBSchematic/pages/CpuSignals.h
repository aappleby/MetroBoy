#pragma once

//-----------------------------------------------------------------------------

struct CpuIn {

  //----------
  // top right port

  // ADDR_VALID and one of the other signals might be switched

  bool CPU_RAW_RD;     // PORTA_00: -> P07.UJYV, P08.LAGU, P08.LAVO
  bool CPU_RAW_WR;     // PORTA_01: -> P01.AREV, P08.LAGU. This is almost definitely "raw write"
  bool ADDR_VALID;     // PORTA_06: -> P01.AGUT, P08.TEX0. This is almost definitely "address valid"

  //----------
  // bottom right port

  bool CPU_A00;        // PORTB_00: -> A00
  bool FROM_CPU9;      // PORTB_01: -> P02.LETY, vblank int ack
  bool CPU_A08;        // PORTB_02: -> A08
  bool CPU_A01;        // PORTB_04: -> A01
  bool FROM_CPU8;      // PORTB_05: -> P02.LEJA, stat int ack
  bool CPU_A09;        // PORTB_06: -> A09
  bool CPU_A02;        // PORTB_08: -> A02
  bool FROM_CPU10;     // PORTB_09: -> P02.LESA, timer int ack
  bool CPU_A10;        // PORTB_10: -> A10
  bool CPU_A03;        // PORTB_12: -> A03
  bool FROM_CPU7;      // PORTB_13: -> P02.LUFE, serial int ack
  bool CPU_A11;        // PORTB_14: -> A11
  bool CPU_A04;        // PORTB_16: -> A04
  bool FROM_CPU11;     // PORTB_17: -> P02.LAMO, joypad int ack
  bool CPU_A12;        // PORTB_18: -> A12
  bool CPU_A05;        // PORTB_20: -> A05
  bool CPU_A13;        // PORTB_22: -> A13
  bool CPU_A06;        // PORTB_24: -> A06
  bool CPU_A14;        // PORTB_26: -> A14
  bool CPU_A07;        // PORTB_28: -> A07
  bool CPU_A15;        // PORTB_30: -> A15

  //----------
  // top center port

  bool CPUCLK_REQ;     // PORTC_00: -> ABOL (an inverter) -> BATE. Something about "cpu ready". clock request?

  //----------
  // top left port

  bool FROM_CPU6;      // PORTD_00: -> P07.LEXY, doesn't do anything
  bool FROM_CPU5;      // PORTD_05: -> FROM_CPU5 - controls driving the external data pins onto the internal data bus and other stuff. is this actually a clock, or like OE?

  //----------
  // bottom left port, tristate data bus

  bool CPU_D0;
  bool CPU_D1;
  bool CPU_D2;
  bool CPU_D3;
  bool CPU_D4;
  bool CPU_D5;
  bool CPU_D6;
  bool CPU_D7;
};

//-----------------------------------------------------------------------------

struct CpuOut {

  //----------
  // top right wire by itself

  bool TO_CPU2;        // <- P02.ASOK

  //----------
  // top right port

  bool T1nT2;          // PORTA_02: <- P07.T1nT2
  bool SYRO;           // PORTA_03: <- P25.SYRO
  bool READ_BOOTROM;   // PORTA_04: <- P07.READ_BOOTROM
  bool T1T2n;          // PORTA_05: <- P07.T1T2n

  //----------
  // bottom right port

  bool TO_CPU3;        // PORTB_03: <- P02.LOPE, vblank int
  bool TO_CPU6;        // PORTB_07: <- P02.LALU, stat int
  bool TO_CPU7;        // PORTB_11: <- P02.NYBO, timer int
  bool TO_CPU4;        // PORTB_15: <- P02.UBUL, serial int
  bool TO_CPU5;        // PORTB_19: <- P02.ULAK, joypad int

  //----------
  // top center port

  bool AFER;           // PORTC_01: <- P01.AFER , reset related reg
  bool PIN_RESET;      // PORTC_02: <- PIN_RESET directly connected to the pad
  bool CLKIN_A;        // PORTC_03: <- chip.CLKIN_A top wire on PAD_XI,
  bool CPU_RESET;      // PORTC_04: <- P01.CPU_RESET

  //----------
  // top left port

  bool CPU_CLK1;       // PORTD_01: <- P01.BOWA abcde___
  bool BEDO;           // PORTD_02: <- P01.BEDO _____fgh

  bool BEKO;           // PORTD_03: <- P01.BEKO ____efgh connection not indicated on P01
  bool PHI_OUT;        // PORTD_04: <- P01.BUDE abcd____

  bool BUKE;           // PORTD_06: <- P01.BUKE _____f__
  bool RESET_CLK;      // PORTD_07: <- P01.RESET_CLK _____fgh
  bool BOGA;           // PORTD_08: <- P01.BOGA abcde___

  //----------
  // bottom left port, tristate data bus

  bool CPU_D0;
  bool CPU_D1;
  bool CPU_D2;
  bool CPU_D3;
  bool CPU_D4;
  bool CPU_D5;
  bool CPU_D6;
  bool CPU_D7;
};

//-----------------------------------------------------------------------------