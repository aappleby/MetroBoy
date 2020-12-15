#pragma once
#include "GateBoyLib/Gates.h"

//-----------------------------------------------------------------------------

struct RegLX {
  uint8_t get() const  { return pack_u8p(7, &SAXO_LX0p); }

  wire PURE_LINE_ENDn() const {
    /*#p21.PURE*/ wire _PURE_LINE_ENDn_new_evn = not1(_RUTU_x113p.qp_new());
    return _PURE_LINE_ENDn_new_evn;
  }

  wire TEGY_STROBE() const;

  wire NYPE_x113p() const { return _NYPE_x113p.qp_new(); }
  wire NYPE_x113n() const { return _NYPE_x113p.qn_new(); }

  wire RUTU_x113p() const { return _RUTU_x113p.qp_new(); }
  wire RUTU_x113n() const { return _RUTU_x113p.qn_new(); }

  /*#p24.LOFU*/ wire LOFU_x113n() const { return not1(RUTU_x113p()); }

  void tock(GateBoyResetDebug& rst, GateBoyClock& clk);

  /*p21.RUTU*/ DFF17 _RUTU_x113p;  // xxxxxxGx
  /*p21.NYPE*/ DFF17 _NYPE_x113p;  // xxCxxxxx

  /*p21.SAXO*/ DFF17 SAXO_LX0p; // xxCxxxGx Ticks on C, reset on G
  /*p21.TYPO*/ DFF17 TYPO_LX1p; // xxCxxxGx Ticks on C, reset on G
  /*p21.VYZO*/ DFF17 VYZO_LX2p; // xxCxxxGx Ticks on C, reset on G
  /*p21.TELU*/ DFF17 TELU_LX3p; // xxCxxxGx Ticks on C, reset on G
  /*p21.SUDE*/ DFF17 SUDE_LX4p; // xxCxxxGx Ticks on C, reset on G
  /*p21.TAHA*/ DFF17 TAHA_LX5p; // xxCxxxGx Ticks on C, reset on G
  /*p21.TYRY*/ DFF17 TYRY_LX6p; // xxCxxxGx Ticks on C, reset on G
};

//-----------------------------------------------------------------------------

struct RegLY {
  uint8_t get() const  { return pack_u8p(8, &MUWY_LY0p); }

  void tock(GateBoyResetDebug& rst, GateBoyCpuBus& cpu_bus, RegLX& reg_lx);

  /*#p24.NERU*/ wire NERU_y000p() const {
    return nor8(LAFO_LY7p.qp_new(), LOVU_LY4p.qp_new(), LYDO_LY3p.qp_new(), MUWY_LY0p.qp_new(),
                MYRO_LY1p.qp_new(), LEXA_LY2p.qp_new(), LEMA_LY5p.qp_new(), MATO_LY6p.qp_new());
  }

  /*p21.MYTA*/ DFF17 MYTA_y153p; // xxCxxxxH

  /*p21.MUWY*/ DFF17 MUWY_LY0p;  // xxCxxxGx Ticks on G, reset on C
  /*p21.MYRO*/ DFF17 MYRO_LY1p;  // xxCxxxGx Ticks on G, reset on C
  /*p21.LEXA*/ DFF17 LEXA_LY2p;  // xxCxxxGx Ticks on G, reset on C
  /*p21.LYDO*/ DFF17 LYDO_LY3p;  // xxCxxxGx Ticks on G, reset on C
  /*p21.LOVU*/ DFF17 LOVU_LY4p;  // xxCxxxGx Ticks on G, reset on C
  /*p21.LEMA*/ DFF17 LEMA_LY5p;  // xxCxxxGx Ticks on G, reset on C
  /*p21.MATO*/ DFF17 MATO_LY6p;  // xxCxxxGx Ticks on G, reset on C
  /*p21.LAFO*/ DFF17 LAFO_LY7p;  // xxCxxxGx Ticks on G, reset on C
};

//-----------------------------------------------------------------------------
// FF40 LCDC

struct RegLCDC {

  void tock(GateBoyResetDebug& rst, GateBoyCpuBus& cpu_bus);

  /*p23.VYXE*/ DFF9 VYXE_LCDC_BGENn;   // xxxxxxxH
  /*p23.XYLO*/ DFF9 XYLO_LCDC_SPENn;   // xxxxxxxH
  /*p23.XYMO*/ DFF9 XYMO_LCDC_SPSIZEn; // xxxxxxxH
  /*p23.XAFO*/ DFF9 XAFO_LCDC_BGMAPn;  // xxxxxxxH
  /*p23.WEXU*/ DFF9 WEXU_LCDC_BGTILEn; // xxxxxxxH
  /*p23.WYMO*/ DFF9 WYMO_LCDC_WINENn;  // xxxxxxxH
  /*p23.WOKY*/ DFF9 WOKY_LCDC_WINMAPn; // xxxxxxxH
  /*p23.WOKY*/ DFF9 XONA_LCDC_LCDENn;  // xxxxxxxH
};

//-----------------------------------------------------------------------------

struct RegLYC {
  uint8_t get() const { return pack_u8n(8, &SYRY_LYC0n); }

  void tock(
    GateBoyResetDebug& rst,
    GateBoyClock& clk,
    GateBoyCpuBus& cpu_bus,
    const RegLY& reg_ly);

  /*p21.ROPO*/ DFF17 ROPO_LY_MATCH_SYNCp;   // xxCxxxxx

  /*p23.SYRY*/ DFF9 SYRY_LYC0n; // xxxxxxxH
  /*p23.VUCE*/ DFF9 VUCE_LYC1n; // xxxxxxxH
  /*p23.SEDY*/ DFF9 SEDY_LYC2n; // xxxxxxxH
  /*p23.SALO*/ DFF9 SALO_LYC3n; // xxxxxxxH
  /*p23.SOTA*/ DFF9 SOTA_LYC4n; // xxxxxxxH
  /*p23.VAFA*/ DFF9 VAFA_LYC5n; // xxxxxxxH
  /*p23.VEVO*/ DFF9 VEVO_LYC6n; // xxxxxxxH
  /*p23.RAHA*/ DFF9 RAHA_LYC7n; // xxxxxxxH
};

//-----------------------------------------------------------------------------

struct GateBoyLCD {

  /* p28.ATEJ*/ wire ATEJ_LINE_RSTp() const;
  /* p27.XAHY*/ wire XAHY_LINE_RSTn() const { return not1(ATEJ_LINE_RSTp()); }

  /*#p28.ANOM*/ wire ANOM_LINE_RSTn() const { return nor2(ATEJ_LINE_RSTp(), ATAR_VID_RSTp(_XODO_VID_RSTp)); }
  /* p28.ABAK*/ wire ABAK_LINE_RSTp() const { return  or2(ATEJ_LINE_RSTp(), AMYG_VID_RSTp(_XODO_VID_RSTp)); }
  /*#p29.BALU*/ wire BALU_LINE_RSTp() const { return not1(ANOM_LINE_RSTn()); }
  /* p28.BYVA*/ wire BYVA_LINE_RSTn() const { return not1(ABAK_LINE_RSTp()); }
  /* p29.DYBA*/ wire DYBA_LINE_RSTp() const { return not1(BYVA_LINE_RSTn()); }
  /*#p29.BAGY*/ wire BAGY_LINE_RSTn() const { return not1(BALU_LINE_RSTp()); }
  /* p21.TADY*/ wire TADY_LINE_RSTn() const { return nor2(ATEJ_LINE_RSTp(), TOFU_VID_RSTp(_XODO_VID_RSTp)); }

  /*#p21.PARU*/ wire PARU_VBLANKp() const { return not1(POPU_VBLANKp.qn_new()); }

  void tock(
    GateBoyResetDebug& rst,
    GateBoyClock& clk,
    const RegLX& reg_lx,
    const RegLY& reg_ly);

  void set_pins(
    GateBoyResetDebug& rst,
    GateBoyClock& clk,
    wire TYFA_CLKPIPE_odd,
    wire XONA_LCDC_LCDENp,
    wire XYMU_RENDERINGp,
    wire AVAP_SCAN_DONE_TRIGp,
    wire XYDO_PX3p,
    wire TULU_DIV07p,
    wire REMY_LD0n,
    wire RAVO_LD1n,
    const RegLX& reg_lx,
    const RegLY& reg_ly);

  Signal _XODO_VID_RSTp;

  // H deltas are due to reg writes
  /*p29.CATU*/ DFF17 CATU_LINE_P000p;  // Axxxxxxx
  /*p28.ANEL*/ DFF17 ANEL_LINE_P002p;  // xxCxxxxx
  /*p21.POPU*/ DFF17 POPU_VBLANKp;     // xxCxxxxH

  /*p21.SYGU*/ DFF17 SYGU_LINE_STROBE; // xxxxxxGH
  /*p24.MEDA*/ DFF17 MEDA_VSYNC_OUTn;  // xxCxxxxH
  /*p24.LUCA*/ DFF17 LUCA_LINE_EVENp;  // xxxxxxGH
  /*p21.NAPO*/ DFF17 NAPO_FRAME_EVENp; // xxCxxxxx

  // RUJU+POFY+POME form a nor latch
  /*p24.RUJU*/ Gate RUJU;                    // AxxxxFxx
  /*p24.POFY*/ Gate POFY;                    // AxxxxFxx
  /*p24.POME*/ Gate POME;                    // AxxxxFxx
  /*p24.PAHO*/ DFF17 PAHO_X_8_SYNC;          // xBxDxFxH
  /*p21.WUSA*/ NorLatch WUSA_LCD_CLOCK_GATE; // xBxDxFGH High on G at beginning of line, low on H at end of line. Not sure what's up with the others. Scroll/sprite count?

  /*PIN_50*/ PinOut PIN_LCD_DATA1;
  /*PIN_51*/ PinOut PIN_LCD_DATA0;
  /*PIN_54*/ PinOut PIN_LCD_HSYNC;
  /*PIN_56*/ PinOut PIN_LCD_FLIPS;
  /*PIN_52*/ PinOut PIN_LCD_CNTRL;
  /*PIN_55*/ PinOut PIN_LCD_LATCH;
  /*PIN_53*/ PinOut PIN_LCD_CLOCK;
  /*PIN_57*/ PinOut PIN_LCD_VSYNC;

  NorLatch lcd_pix_lo;
  NorLatch lcd_pix_hi;

  DFF lcd_pipe_lo[160];
  DFF lcd_pipe_hi[160];
};

//-----------------------------------------------------------------------------
