#pragma once
#include "GateBoyLib/Gates.h"

namespace Schematics {

struct SchematicTop;
struct CpuBus;

//-----------------------------------------------------------------------------

struct InterruptRegisters {
  void reset() {}
  void reset_bootrom() {
    LOPE_FF0F_D0p.reset(REG_D0C0);
    UBUL_FF0F_D3p.reset(REG_D0C0);
    ULAK_FF0F_D4p.reset(REG_D0C0);
    LALU_FF0F_D1p.reset(REG_D0C0);
    NYBO_FF0F_D2p.reset(REG_D0C0);

    MATY_FF0F_L0p.reset(TRI_D0NP);
    NEJY_FF0F_L3p.reset(TRI_D0NP);
    NUTY_FF0F_L4p.reset(TRI_D0NP);
    MOPO_FF0F_L1p.reset(TRI_D0NP);
    PAVY_FF0F_L2p.reset(TRI_D0NP);

    // Interrupts
    PIN_CPU_INT_VBLANK.reset(TRI_HZNP);
    PIN_CPU_INT_STAT  .reset(TRI_HZNP);
    PIN_CPU_INT_TIMER .reset(TRI_HZNP);
    PIN_CPU_INT_SERIAL.reset(TRI_HZNP);
    PIN_CPU_INT_JOYPAD.reset(TRI_HZNP);

    PIN_CPU_ACK_VBLANK.reset(TRI_D0NP);
    PIN_CPU_ACK_STAT  .reset(TRI_D0NP);
    PIN_CPU_ACK_TIMER .reset(TRI_D0NP);
    PIN_CPU_ACK_SERIAL.reset(TRI_D0NP);
    PIN_CPU_ACK_JOYPAD.reset(TRI_D0NP);
  }
  void dump(Dumper& d, const SchematicTop& top) const;
  void tock(const SchematicTop& gb, CpuBus& cpu_bus);

//private:
  friend SchematicTop;

  /*p02.LOPE*/ DFF22 LOPE_FF0F_D0p = ERR_XXXX;
  /*p02.UBUL*/ DFF22 UBUL_FF0F_D3p = ERR_XXXX;
  /*p02.ULAK*/ DFF22 ULAK_FF0F_D4p = ERR_XXXX;
  /*p02.LALU*/ DFF22 LALU_FF0F_D1p = ERR_XXXX;
  /*p02.NYBO*/ DFF22 NYBO_FF0F_D2p = ERR_XXXX;

  /*p02.MATY*/ TpLatch MATY_FF0F_L0p = ERR_XXXX;
  /*p02.NEJY*/ TpLatch NEJY_FF0F_L3p = ERR_XXXX;
  /*p02.NUTY*/ TpLatch NUTY_FF0F_L4p = ERR_XXXX;
  /*p02.MOPO*/ TpLatch MOPO_FF0F_L1p = ERR_XXXX;
  /*p02.PAVY*/ TpLatch PAVY_FF0F_L2p = ERR_XXXX;

  // Interrupts
  Pin PIN_CPU_INT_VBLANK = ERR_XXXX; // bottom right port PORTB_03: <- P02.LOPE, vblank int
  Pin PIN_CPU_INT_STAT   = ERR_XXXX; // bottom right port PORTB_07: <- P02.LALU, stat int
  Pin PIN_CPU_INT_TIMER  = ERR_XXXX; // bottom right port PORTB_11: <- P02.NYBO, timer int
  Pin PIN_CPU_INT_SERIAL = ERR_XXXX; // bottom right port PORTB_15: <- P02.UBUL, serial int
  Pin PIN_CPU_INT_JOYPAD = ERR_XXXX; // bottom right port PORTB_19: <- P02.ULAK, joypad int

  // Interrupt acks
  Pin PIN_CPU_ACK_VBLANK = ERR_XXXX; // bottom right port PORTB_01: -> P02.LETY, vblank int ack
  Pin PIN_CPU_ACK_STAT   = ERR_XXXX; // bottom right port PORTB_05: -> P02.LEJA, stat int ack
  Pin PIN_CPU_ACK_TIMER  = ERR_XXXX; // bottom right port PORTB_09: -> P02.LESA, timer int ack
  Pin PIN_CPU_ACK_SERIAL = ERR_XXXX; // bottom right port PORTB_13: -> P02.LUFE, serial int ack
  Pin PIN_CPU_ACK_JOYPAD = ERR_XXXX; // bottom right port PORTB_17: -> P02.LAMO, joypad int ack
};

//-----------------------------------------------------------------------------

}; // namespace Schematics