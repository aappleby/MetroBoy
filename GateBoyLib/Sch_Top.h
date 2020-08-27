#pragma once

#include "GateBoyLib/Sch_Timer.h"
#include "GateBoyLib/Sch_LCD.h"
#include "GateBoyLib/Sch_DMA.h"
#include "GateBoyLib/Sch_SpriteStore.h"
#include "GateBoyLib/Sch_Clocks.h"
#include "GateBoyLib/Sch_PixPipe.h"
#include "GateBoyLib/Sch_Joypad.h"
#include "GateBoyLib/Sch_Serial.h"
#include "GateBoyLib/Sch_Interrupts.h"
#include "GateBoyLib/Sch_Bootrom.h"
#include "GateBoyLib/Sch_SpriteFetcher.h"
#include "GateBoyLib/Sch_TileFetcher.h"
#include "GateBoyLib/Sch_SpriteScanner.h"

#include "GateBoyLib/Sch_CpuBus.h"
#include "GateBoyLib/Sch_ExtBus.h"
#include "GateBoyLib/Sch_OamBus.h"
#include "GateBoyLib/Sch_VramBus.h"

namespace Schematics {

//-----------------------------------------------------------------------------

#pragma pack(push, 1)
struct SchematicTop {

  SchematicTop();

  void tick_slow(wire RST, wire CLK, wire CLKGOOD, wire T1n, wire T2n);

  /*p04.DECY*/ wire DECY_LATCH_EXTn() {
    /*p04.DECY*/ wire DECY_LATCH_EXTn = not1(cpu_bus.CPU_PIN_LATCH_EXT.tp());
    return DECY_LATCH_EXTn;
  }

  /*p04.CATY*/ wire CATY_LATCH_EXTp() {
    /*p04.CATY*/ wire CATY_LATCH_EXTp = not1(DECY_LATCH_EXTn());
    return CATY_LATCH_EXTp;
  }

  /*p29.AVAP*/ wire AVAP_RENDER_START_TRIGp() const {
    /*p01.ATAR*/ wire ATAR_VID_RSTp = not1(clk_reg.XAPO_VID_RSTn);
    /*#p28.ANOM*/ wire ANOM_LINE_RSTn = nor2(lcd_reg.ATEJ_VID_LINE_END_TRIGp(), ATAR_VID_RSTp);
    /*#p29.BALU*/ wire BALU_LINE_RSTp = not1(ANOM_LINE_RSTn);
    /*#p29.BEBU*/ wire BEBU_SCAN_DONE_TRIGn = or3(sprite_scanner.DOBA_SCAN_DONE_B.qp(), BALU_LINE_RSTp, sprite_scanner.BYBA_SCAN_DONE_A.qn());
    /*#p29.AVAP*/ wire AVAP_RENDER_START_TRIGp = not1(BEBU_SCAN_DONE_TRIGn);
    return AVAP_RENDER_START_TRIGp;
  }

  // top.BETE, top.AJUJ
  /*p28.AJON*/ wire AJON_OAM_BUSY() const {
    /*p28.BOGE*/ wire BOGE_DMA_RUNNINGn = not1(dma_reg.MATU_DMA_RUNNINGp());
    /*p28.AJON*/ wire AJON_OAM_BUSY = and2(BOGE_DMA_RUNNINGn, pix_pipe.XYMU_RENDERINGp()); // def AND. ppu can read oam when there's rendering but no dma
    return AJON_OAM_BUSY;
  }

  // -> top.AMAB, top.APAG
  /*p28.AJUJ*/ wire AJUJ_OAM_BUSYn() const { 
     /*p28.AJUJ*/ wire AJUJ_OAM_BUSYn = nor3(dma_reg.MATU_DMA_RUNNINGp(), ACYL_SCANNINGp(), AJON_OAM_BUSY()); // def nor4
     return AJUJ_OAM_BUSYn;
  }

  // -> oam.WYJA, oam.WUKU, oam.GUKO, top.APAG
  /*p28.AMAB*/ wire AMAB_CPU_READ_OAMp() const { 
      /*p28.AMAB*/ wire AMAB_CPU_READ_OAMp = and2(cpu_bus.SARO_FE00_FEFFp(), AJUJ_OAM_BUSYn()); // def and
      return AMAB_CPU_READ_OAMp;
  }

  //-----------------------------------------------------------------------------

  // boot.YULA, ext.SOBY, CPU_PIN_BOOTp
  /*p07.TUTU*/ wire TUTU_ADDR_BOOTp() const {
    /*p07.TERA*/ wire TERA_BOOT_BITp  = not1(bootrom.BOOT_BITn.qp());
    return and2(TERA_BOOT_BITp, cpu_bus.TULO_ADDR_00XXp());
  }

  //-----------------------------------------------------------------------------

  // -> buncha stuff
  /*p07.TEDO*/ wire TEDO_CPU_RDp() const {
    /*p07.UJYV*/ wire UJYV_CPU_RDn = mux2_n(!ext_bus.EXT_PIN_RDn.qp(), cpu_bus.CPU_PIN_RDp.tp(), UNOR_MODE_DBG2p);
    /*p07.TEDO*/ wire TEDO_CPU_RDp = not1(UJYV_CPU_RDn);
    return TEDO_CPU_RDp;
  }

  // -> buncha stuff
  /*p07.ASOT*/ wire ASOT_CPU_RDp() const {
    /*p07.AJAS*/ wire _AJAS_CPU_RDn = not1(TEDO_CPU_RDp());
    /*p07.ASOT*/ wire ASOT_CPU_RDp = not1(_AJAS_CPU_RDn);
    return ASOT_CPU_RDp;
  }

  // vram.TUJA, top.UBAL/MEXO
  /*p01.APOV*/ wire APOV_CPU_WRp_xxxxEFGx() const {
    /*p01.AFAS*/ wire AFAS_xxxxEFGx = nor2(clk_reg.ADAR_ABCxxxxH, clk_reg.ATYP_ABCDxxxx);
    /*p01.AREV*/ wire AREV_CPU_WRn_ABCDxxxH = nand2(cpu_bus.CPU_PIN_WRp.tp(), AFAS_xxxxEFGx);
    /*p01.APOV*/ wire APOV_CPU_WRp_xxxxEFGx = not1(AREV_CPU_WRn_ABCDxxxH);
    return APOV_CPU_WRp_xxxxEFGx;
  }

  // dma.lavy, lcd.xufa, oam.wyja, pxp.vely/xoma/myxe, ppu.waru/sepa, tile.bedy/arur, top.xuto, win.weko/wuza (most all the FF4X regs)
  /*p07.CUPA*/ wire CUPA_CPU_WRp_xxxxEFGx() const {
    /*p07.DYKY*/ wire DYKY_CPU_WRn_ABCDxxxH = not1(TAPU_CPU_WRp_xxxxEFGx);
    /*p07.CUPA*/ wire CUPA_CPU_WRp_xxxxEFGx = not1(DYKY_CPU_WRn_ABCDxxxH);
    return CUPA_CPU_WRp_xxxxEFGx;
  }

  // ext.NEVY
  /*p08.MEXO*/ wire MEXO_CPU_WRn_ABCDxxxH() const {
    /*p08.MEXO*/ wire MEXO_CPU_WRn_ABCDxxxH = not1(APOV_CPU_WRp_xxxxEFGx());
    return MEXO_CPU_WRn_ABCDxxxH;
  }

  //-----------------------------------------------------------------------------
  // PPU signals

  // int.asam, oam.aver/ajep, ppu.xaty, top.apar/.ajuj
  // so dma stops oam scan?
  /*p28.ACYL*/ wire ACYL_SCANNINGp() const {
    /*p28.ACYL*/ wire ACYL_SCANNINGp = and2(dma_reg.BOGE_DMA_RUNNINGn(), sprite_scanner.BESU_SCANNINGp.tp());
    return ACYL_SCANNINGp;
  }

  // -> sprite fetcher, top.TEVO
  /*p27.TAVE*/ wire TAVE_PRELOAD_DONE_TRIGp() const {
    /*p27.ROMO*/ wire _ROMO_PRELOAD_DONEn = not1(tile_fetcher._POKY_PRELOAD_LATCHp.qp());
    /*p27.SUVU*/ wire _SUVU_PRELOAD_DONE_TRIGn = nand4(pix_pipe.XYMU_RENDERINGp(), _ROMO_PRELOAD_DONEn, tile_fetcher._NYKA_FETCH_DONE_P11.qp(), tile_fetcher._PORY_FETCH_DONE_P12.qp());
    return  not1(_SUVU_PRELOAD_DONE_TRIGn);
  }

  //-----------------------------------------------------------------------------

  /*p07.UBET*/ Sig UBET_T1p;
  /*p07.UVAR*/ Sig UVAR_T2p;
  /*p07.UMUT*/ Sig UMUT_MODE_DBG1p;
  /*p07.UNOR*/ Sig UNOR_MODE_DBG2p;
  /*p07.UPOJ*/ Sig UPOJ_MODE_PRODn;
  /*p08.TOVA*/ Sig TOVA_MODE_DBG2n;

  /*p25.TUTO*/ Sig TUTO_DBG_VRAMp;
  /*p25.RACO*/ Sig RACO_DBG_VRAMn;

  /*p01.ABUZ*/ Sig ABUZ_AVn;

  /*p28.LEKO*/ Sig LEKO_CPU_RDp;
  /*p07.TAPU*/ Sig TAPU_CPU_WRp_xxxxEFGx;

  /*p27.NYXU*/ Sig NYXU_FETCH_TRIGn;

  /*p27.TEVO*/ Sig TEVO_FETCH_TRIGp;

  //-----------------------------------------------------------------------------

  OamBus oam_bus;
  ExtBus ext_bus;
  CpuBus cpu_bus;
  VramBus vram_bus;

  ClockRegisters  clk_reg;

  DmaRegisters dma_reg;
  InterruptRegisters int_reg;
  Joypad joypad;
  LcdRegisters lcd_reg;
  PixelPipe pix_pipe;
  SerialRegisters ser_reg;
  SpriteStore sprite_store;
  Timer tim_reg;
  TileFetcher tile_fetcher;
  SpriteFetcher sprite_fetcher;
  SpriteScanner sprite_scanner;
  Bootrom bootrom;

  /*p25.SOTO*/ RegQN SOTO_DBG_VRAM  = REG_D0C0;

  Tri LCD_PIN_DATA1n = TRI_HZNP; // PIN_50 
  Tri LCD_PIN_DATA0n = TRI_HZNP; // PIN_51 
  Tri LCD_PIN_CNTRL = TRI_HZNP; // PIN_52 
  Tri LCD_PIN_CLOCK = TRI_HZNP; // PIN_53 
  Tri LCD_PIN_HSYNC = TRI_HZNP; // PIN_54 
  Tri LCD_PIN_LATCH = TRI_HZNP; // PIN_55 
  Tri LCD_PIN_ALTSG = TRI_HZNP; // PIN_56 
  Tri LCD_PIN_VSYNC = TRI_HZNP; // PIN_57 
};
#pragma pack(pop)

//-----------------------------------------------------------------------------

};