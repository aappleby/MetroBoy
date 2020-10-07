#pragma once
#include "GateBoyLib/Gates.h"

namespace Schematics {

struct SchematicTop;
struct CpuBus;

//-----------------------------------------------------------------------------

struct Joypad {

  void reset() {}

  void reset_bootrom() {
    ASOK_INT_JOYp.reset(TRI_HZNP);
    AWOB_WAKE_CPU.reset(TRI_D0NP);
    PIN_CPU_WAKE.reset(TRI_HZNP);

    BATU_JP_GLITCH0.reset(REG_D0C0);
    ACEF_JP_GLITCH1.reset(REG_D0C0);
    AGEM_JP_GLITCH2.reset(REG_D0C0);
    APUG_JP_GLITCH3.reset(REG_D0C0);

    JUTE_JOYP_RA.reset(REG_D0C0);
    KECY_JOYP_LB.reset(REG_D0C0);
    JALE_JOYP_UC.reset(REG_D0C0);
    KYME_JOYP_DS.reset(REG_D0C0);
    KELY_JOYP_UDLR.reset(REG_D0C0);
    COFY_JOYP_ABCS.reset(REG_D0C0);
    KUKO_DBG_FF00_D6.reset(REG_D0C0);
    KERU_DBG_FF00_D7.reset(REG_D0C0);

    KEVU_JOYP_L0.reset(TRI_D0NP);
    KAPA_JOYP_L1.reset(TRI_D0NP);
    KEJA_JOYP_L2.reset(TRI_D0NP);
    KOLO_JOYP_L3.reset(TRI_D0NP);

    PIN_JOY_P10.reset(TRI_HZPU);
    PIN_JOY_P11.reset(TRI_HZPU);
    PIN_JOY_P12.reset(TRI_HZPU);
    PIN_JOY_P13.reset(TRI_HZPU);
    PIN_JOY_P14.reset(TRI_HZPU);
    PIN_JOY_P15.reset(TRI_HZPU);
  }

  void tick(const SchematicTop& top);
  void tock(const SchematicTop& top, CpuBus& cpu_bus);
  void dump(Dumper& d) const;
  void set_buttons(uint8_t buttons);

  /*p02.ASOK*/ Sig ASOK_INT_JOYp;

//private:
  friend struct SchematicTop;

  // This is driven by what we think is a latch and it goes straight to the CPU - maybe there's a pull-down?
  /*p02.AWOB*/ TpLatch AWOB_WAKE_CPU = ERR_XXXX;
  Pin PIN_CPU_WAKE = ERR_XXXX; // top right wire by itself <- P02.AWOB

  /*p02.BATU*/ DFF17 BATU_JP_GLITCH0 = ERR_XXXX;
  /*p02.ACEF*/ DFF17 ACEF_JP_GLITCH1 = ERR_XXXX;
  /*p02.AGEM*/ DFF17 AGEM_JP_GLITCH2 = ERR_XXXX;
  /*p02.APUG*/ DFF17 APUG_JP_GLITCH3 = ERR_XXXX;

  /*p05.JUTE*/ DFF17 JUTE_JOYP_RA = ERR_XXXX;
  /*p05.KECY*/ DFF17 KECY_JOYP_LB = ERR_XXXX;
  /*p05.JALE*/ DFF17 JALE_JOYP_UC = ERR_XXXX;
  /*p05.KYME*/ DFF17 KYME_JOYP_DS = ERR_XXXX;
  /*p05.KELY*/ DFF17 KELY_JOYP_UDLR = ERR_XXXX;
  /*p05.COFY*/ DFF17 COFY_JOYP_ABCS = ERR_XXXX;
  /*p05.KUKO*/ DFF17 KUKO_DBG_FF00_D6 = ERR_XXXX;
  /*p05.KERU*/ DFF17 KERU_DBG_FF00_D7 = ERR_XXXX;

  /*p05.KEVU*/ TpLatch KEVU_JOYP_L0 = ERR_XXXX; // 10-rung, looks like pass gate or something
  /*p05.KAPA*/ TpLatch KAPA_JOYP_L1 = ERR_XXXX; // 10-rung, looks like pass gate or something
  /*p05.KEJA*/ TpLatch KEJA_JOYP_L2 = ERR_XXXX; // 10-rung, looks like pass gate or something
  /*p05.KOLO*/ TpLatch KOLO_JOYP_L3 = ERR_XXXX; // 10-rung, looks like pass gate or something

  Pin PIN_JOY_P10 = ERR_XXXX; // PIN_67
  Pin PIN_JOY_P11 = ERR_XXXX; // PIN_66
  Pin PIN_JOY_P12 = ERR_XXXX; // PIN_65
  Pin PIN_JOY_P13 = ERR_XXXX; // PIN_64
  Pin PIN_JOY_P14 = ERR_XXXX; // PIN_63
  Pin PIN_JOY_P15 = ERR_XXXX; // PIN_62
};

//-----------------------------------------------------------------------------

}; // namespace Schematics