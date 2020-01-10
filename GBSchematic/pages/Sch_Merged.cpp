#include "Sch_Merged.h"

#include "Sch_Common.h"
#include "Sch_Pins.h"

using namespace Schematics;

/*
0b000xxxxx_xxxxxxxx - 0x0000 to 0x1FFF (cart rom)
0b001xxxxx_xxxxxxxx - 0x2000 to 0x3FFF (cart rom)
0b010xxxxx_xxxxxxxx - 0x4000 to 0x5FFF (cart rom banked)
0b011xxxxx_xxxxxxxx - 0x6000 to 0x7FFF (cart rom banked)
0b100xxxxx_xxxxxxxx - 0x8000 to 0x9FFF (vram)
0b101xxxxx_xxxxxxxx - 0xA000 to 0xBFFF (cart ram)
0b110xxxxx_xxxxxxxx - 0xC000 to 0xDFFF (internal ram)
0b111xxxxx_xxxxxxxx - 0xE000 to 0xFFFF (echo ram, oam, high ram, io)

0b11111110_00000000 - 0xFE00 - OAM begin
0b11111110_10011111 - 0xFE9F - OAM end

0b11111111_00000000 - 0xFF00 - IO begin
0b11111111_01111111 - 0xFF7F - IO end

0b11111111_10000000 - 0xFF80 - Zeropage begin
0b11111111_11111110 - 0xFFFE - Zeropage end
0b11111111_11111111 - 0xFFFF - Interrupt enable
*/

static ClkRegisters clk_reg;
static ConfigRegisters cfg_reg;
static DebugRegisters dbg_reg;
static DmaRegisters dma_reg;
static InterruptRegisters int_reg;
static JoypadRegisters joy_reg;
static LcdRegisters lcd_reg;
static OamRegisters oam_reg;
static PixelPipeRegisters pxp_reg;
static RstRegisters rst_reg;
static SerialRegisters ser_reg;
static SpriteRegisters spr_reg;
static SpriteStoreRegisters sst_reg;
static SysRegisters sys_reg;
static TimerRegisters tim_reg;
static VclkRegisters vclk_reg;
static VidRegisters vid_reg;

static Bootrom bootrom;
static Ram8K ram;
static Ram8K vram;

static ExtPins ext_pins;
static VramPins vram_pins;
static SerialPins ser_pins;
static JoypadPins joy_pins;
static CpuPins cpu_pins;
static CartPins cart_pins;
static OamPins oam_pins;

static VramBus vram_bus;

static BusTristates bus_tri;
static SpriteTristate sil_tri;

static const uint8_t DMG_ROM_bin[] = {
  0x31, 0xfe, 0xff, 0xaf, 0x21, 0xff, 0x9f, 0x32,
  0xcb, 0x7c, 0x20, 0xfb, 0x21, 0x26, 0xff, 0x0e,
  0x11, 0x3e, 0x80, 0x32, 0xe2, 0x0c, 0x3e, 0xf3,
  0xe2, 0x32, 0x3e, 0x77, 0x77, 0x3e, 0xfc, 0xe0,
  0x47, 0x11, 0x04, 0x01, 0x21, 0x10, 0x80, 0x1a,
  0xcd, 0x95, 0x00, 0xcd, 0x96, 0x00, 0x13, 0x7b,
  0xfe, 0x34, 0x20, 0xf3, 0x11, 0xd8, 0x00, 0x06,
  0x08, 0x1a, 0x13, 0x22, 0x23, 0x05, 0x20, 0xf9,
  0x3e, 0x19, 0xea, 0x10, 0x99, 0x21, 0x2f, 0x99,
  0x0e, 0x0c, 0x3d, 0x28, 0x08, 0x32, 0x0d, 0x20,
  0xf9, 0x2e, 0x0f, 0x18, 0xf3, 0x67, 0x3e, 0x64,
  0x57, 0xe0, 0x42, 0x3e, 0x91, 0xe0, 0x40, 0x04,
  0x1e, 0x02, 0x0e, 0x0c, 0xf0, 0x44, 0xfe, 0x90,
  0x20, 0xfa, 0x0d, 0x20, 0xf7, 0x1d, 0x20, 0xf2,
  0x0e, 0x13, 0x24, 0x7c, 0x1e, 0x83, 0xfe, 0x62,
  0x28, 0x06, 0x1e, 0xc1, 0xfe, 0x64, 0x20, 0x06,
  0x7b, 0xe2, 0x0c, 0x3e, 0x87, 0xe2, 0xf0, 0x42,
  0x90, 0xe0, 0x42, 0x15, 0x20, 0xd2, 0x05, 0x20,
  0x4f, 0x16, 0x20, 0x18, 0xcb, 0x4f, 0x06, 0x04,
  0xc5, 0xcb, 0x11, 0x17, 0xc1, 0xcb, 0x11, 0x17,
  0x05, 0x20, 0xf5, 0x22, 0x23, 0x22, 0x23, 0xc9,

  // the logo @ 0x00A8
  0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b,
  0x03, 0x73, 0x00, 0x83, 0x00, 0x0c, 0x00, 0x0d,
  0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e,
  0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99,
  0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc,
  0xdd, 0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e,

  0x3c, 0x42, 0xb9, 0xa5, 0xb9, 0xa5, 0x42, 0x3c,
  0x21, 0x04, 0x01, 0x11, 0xa8, 0x00, 0x1a, 0x13,
  0xbe, 0x20, 0xfe, 0x23, 0x7d, 0xfe, 0x34, 0x20,
  0xf5, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20,
  0xfb, 0x86, 0x20, 0xfe, 0x3e, 0x01, 0xe0, 0x50
};

//-----------------------------------------------------------------------------

void tick_everything() {
  //----------------------------------------
  // sch_system

  /*p01.ARYS*/ wire ARYS_xBxDxFxH = not(ext_pins.CLK); // ignoring the deglitcher here
  /*p01.AVET*/ wire AVET_AxCxExGx = ext_pins.CLK;
  /*p01.ANOS*/ wire ANOS_xBxDxFxH = not(ext_pins.CLK);
  /*p01.ATAL*/ wire ATAL_AxCxExGx = not(ANOS_xBxDxFxH);
  /*p01.AZOF*/ wire AZOF_xBxDxFxH = not(ATAL_AxCxExGx);
  /*p01.ABOL*/ wire ABOL_CLKREQn  = not(cpu_pins.CLKREQ);
  /*p01.BUTY*/ wire BUTY_CLKREQ   = not(ABOL_CLKREQn);
  /*p01.UCOB*/ wire UCOB_CLKBAD   = not(ext_pins.CLK_GOOD);
  /*p01.ATEZ*/ wire ATEZ_CLKBAD   = not(ext_pins.CLK_GOOD);

  /*p07.UBET*/ wire UBET_T1n = not(ext_pins.T1);
  /*p07.UVAR*/ wire UVAR_T2n = not(ext_pins.T2);
  /*p07.UPOJ*/ wire UPOJ_MODE_PROD = nand(UBET_T1n, UVAR_T2n, ext_pins.RST);
  /*p07.UMUT*/ wire UMUT_MODE_DBG1 = and(ext_pins.T1, UVAR_T2n);
  /*p07.UNOR*/ wire UNOR_MODE_DBG2 = and(ext_pins.T2, UBET_T1n);
  /*p28.WEFE*/ wire WEFE_P10_Bn = not(joy_pins.P10_B);

  //----------------------------------------
  // sch_clocks

  /*p01.AFUR*/ clk_reg.PHAZ_ABCDxxxx.set(ATAL_AxCxExGx, UPOJ_MODE_PROD, !clk_reg.PHAZ_xxxDEFGx);
  /*p01.ALEF*/ clk_reg.PHAZ_xBCDExxx.set(ATAL_AxCxExGx, UPOJ_MODE_PROD,  clk_reg.PHAZ_ABCDxxxx);
  /*p01.APUK*/ clk_reg.PHAZ_xxCDEFxx.set(ATAL_AxCxExGx, UPOJ_MODE_PROD,  clk_reg.PHAZ_xBCDExxx);
  /*p01.ADYK*/ clk_reg.PHAZ_xxxDEFGx.set(ATAL_AxCxExGx, UPOJ_MODE_PROD,  clk_reg.PHAZ_xxCDEFxx);

  /*p01.ZAXY*/ wire ZAXY_xBxDxFxH = not(AZOF_xBxDxFxH);
  /*p01.ZEME*/ wire ZEME_xBxDxFxH = not(ZAXY_xBxDxFxH); // dma, sprite store
  /*p01.ALET*/ wire ALET_AxCxExGx = not(ZEME_xBxDxFxH); // video, sprites

  /*p01.ATAG*/ wire ATAG_xBxDxFxH = not(AZOF_xBxDxFxH);
  /*p01.AMUK*/ wire AMUK_xBxDxFxH = not(ATAG_xBxDxFxH); // apu master 4m clock

  /*p01.AFEP*/ wire AFEP_AxxxxFGH = not( clk_reg.PHAZ_xBCDExxx);
  /*p01.ATYP*/ wire ATYP_ABCDxxxx = not(!clk_reg.PHAZ_ABCDxxxx);
  /*p01.AROV*/ wire AROV_xxCDEFxx = not(!clk_reg.PHAZ_xxCDEFxx);
  /*p01.AJAX*/ wire AJAX_xxxxEFGH = not(ATYP_ABCDxxxx);

  /*p01.ADAR*/ wire ADAR_ABCxxxxH = not(clk_reg.PHAZ_xxxDEFGx);
  /*p01.AFAS*/ wire AFAS_xxxxEFGx = nor(ADAR_ABCxxxxH, ATYP_ABCDxxxx);

  // joypad, reset, timer, debug clock
  /*p01.BAPY*/ wire BAPY_xxxxxxGH = nor(ABOL_CLKREQn, AROV_xxCDEFxx, ATYP_ABCDxxxx);
  /*p01.BERU*/ wire BERU_ABCDEFxx = not(BAPY_xxxxxxGH);
  /*p01.BUFA*/ wire BUFA_xxxxxxGH = not(BERU_ABCDEFxx);
  /*p01.BOLO*/ wire BOLO_ABCDEFxx = not(BUFA_xxxxxxGH);

  // gated on BUTY_CLKREQ
  /*p01.NULE*/ wire NULE_xxxxEFGH = nor(ABOL_CLKREQn, ATYP_ABCDxxxx);
  /*p01.BYRY*/ wire BYRY_ABCDxxxx = not(NULE_xxxxEFGH);
  /*p01.BUDE*/ wire BUDE_xxxxEFGH = not(BYRY_ABCDxxxx);
  /*p01.BEKO*/ wire BEKO_ABCDxxxx = not(BUDE_xxxxEFGH);
  /*p01.BEJA*/ wire BEJA_xxxxEFGH = nand(BOLO_ABCDEFxx, BEKO_ABCDxxxx);
  /*p01.BANE*/ wire BANE_ABCDxxxx = not(BEJA_xxxxEFGH);
  /*p01.BELO*/ wire BELO_xxxxEFGH = not(BANE_ABCDxxxx);
  /*p01.BAZE*/ wire BAZE_ABCDxxxx = not(BELO_xxxxEFGH);
  /*p01.DOVA*/ wire DOVA_ABCDxxxx = not(BUDE_xxxxEFGH);
  /*p01.UVYT*/ wire UVYT_ABCDxxxx = not(BUDE_xxxxEFGH);
  /*p04.MOPA*/ wire MOPA_xxxxEFGH = not(UVYT_ABCDxxxx);

  // BAZE here seems incongruous
  /*p01.BUTO*/ wire BUTO_xBCDEFGH = nand(AFEP_AxxxxFGH, ATYP_ABCDxxxx, BAZE_ABCDxxxx);
  /*p01.BELE*/ wire BELE_Axxxxxxx = not(BUTO_xBCDEFGH);
  /*p01.BYJU*/ wire BYJU_xBCDEFGH = nor(BELE_Axxxxxxx, ATEZ_CLKBAD);
  /*p01.BALY*/ wire BALY_Axxxxxxx = not(BYJU_xBCDEFGH);
  /*p01.BOGA*/ wire BOGA_xBCDEFGH = not(BALY_Axxxxxxx);

  /*p01.BUVU*/ wire BUVU_Axxxxxxx = and(BUTY_CLKREQ, BALY_Axxxxxxx);
  /*p01.BYXO*/ wire BYXO_xBCDEFGH = not(BUVU_Axxxxxxx);
  /*p01.BEDO*/ wire BEDO_Axxxxxxx = not(BYXO_xBCDEFGH);
  /*p01.BOWA*/ wire BOWA_xBCDEFGH = not(BEDO_Axxxxxxx);

  // wave ram write clock
  /*p01.BUGO*/ wire BUGO_xBCDExxx = not(AFEP_AxxxxFGH);
  /*p01.BATE*/ wire BATE_AxxxxxGH = nor(ABOL_CLKREQn, BUGO_xBCDExxx, AROV_xxCDEFxx);
  /*p01.BASU*/ wire BASU_xBCDEFxx = not(BATE_AxxxxxGH);
  /*p01.BUKE*/ wire BUKE_AxxxxxGH = not(BASU_xBCDEFxx);
  /*p17.ABUR*/ wire ABUR_xBCDEFxx = not(BUKE_AxxxxxGH);
  /*p17.BORY*/ wire BORY_AxxxxxGH = not(ABUR_xBCDEFxx);

  //----------------------------------------
  // sch_resets

  /*p01.UPYF*/ wire UPYF        = or(ext_pins.RST, UCOB_CLKBAD);
  /*p01.BOMA*/ wire RESET_CLK   = not(BOGA_xBCDEFGH);
  /*p01.UNUT*/ wire TIMEOUT     = and(rst_reg.WAITING_FOR_CLKREQ, tim_reg.DIV_15);
  /*p01.TABA*/ wire CPU_RESET   = or(UNOR_MODE_DBG2, UMUT_MODE_DBG1, TIMEOUT);
  /*p01.ALYP*/ wire CPU_RESETn  = not(CPU_RESET);

  /*p01.AFAR*/ wire AFAR        = nor(CPU_RESETn, ext_pins.RST);
  /*p01.ASOL*/ wire RESET_IN    = or (AFAR, ext_pins.RST);
  /*p01.AVOR*/ wire AVOR_RESET  = or(rst_reg.RESET_REG, RESET_IN);
  /*p01.ALUR*/ wire SYS_RESETn  = not(AVOR_RESET);   // this goes all over the place
  /*p01.DULA*/ wire SYS_RESET   = not(SYS_RESETn);

  /*p01.CUNU*/ wire CUNU_RESETn = not(SYS_RESET);
  /*p01.XORE*/ wire XORE_RESET  = not(CUNU_RESETn);
  /*p01.XEBE*/ wire XEBE_RESET  = not(XORE_RESET);
  /*p01.XODO*/ wire VID_RESET   = nand(XEBE_RESET, cfg_reg.LCDC_EN);
  /*p01.XAPO*/ wire VID_RESETn  = not(VID_RESET);

  /*p01.TUBO*/ rst_reg.WAITING_FOR_CLKREQ.sr_latch(!UPYF, !ABOL_CLKREQn);
  /*p01.AFER*/ rst_reg.RESET_REG.set(RESET_CLK, UPOJ_MODE_PROD, RESET_IN);

  //----------------------------------------
  // sch_clocks

  /*p29.XYVA*/ wire XYVA_AxCxExGx = not(ZEME_xBxDxFxH);
  /*p29.XOTA*/ wire XOTA_xBxDxFxH = not(XYVA_AxCxExGx);
  /*p29.XYFY*/ wire XYFY_AxCxExGx = not(XOTA_xBxDxFxH);

  /*p29.WUVU*/ vclk_reg.WUVU_AxxDExxH.set(          XOTA_xBxDxFxH, VID_RESETn, !vclk_reg.WUVU_AxxDExxH);
  /*p21.VENA*/ vclk_reg.VENA_xBCDExxx.set(!vclk_reg.WUVU_AxxDExxH, VID_RESETn, !vclk_reg.VENA_xBCDExxx);
  /*p29.WOSU*/ vclk_reg.WOSU_xxCDxxGH.set(          XYFY_AxCxExGx, VID_RESETn, !vclk_reg.WUVU_AxxDExxH);

  /*p29.WOJO*/ wire WOJO_xxxDxxxH = nor(!vclk_reg.WUVU_AxxDExxH, !vclk_reg.WOSU_xxCDxxGH);
  /*p29.XUPY*/ wire XUPY_xBCxxFGx = not(vclk_reg.WUVU_AxxDExxH);  // lcd, sprites
  /*p28.AWOH*/ wire AWOH_AxxDExxH = not(XUPY_xBCxxFGx);  // lcd
  /*p21.TALU*/ wire TALU_xBCDExxx = not(!vclk_reg.VENA_xBCDExxx); // this drives the LCD xy counter
  /*p21.SONO*/ wire SONO_AxxxxFGH = not(TALU_xBCDExxx);  // lcd
  /*p29.XOCE*/ wire XOCE_ABxxEFxx = not(vclk_reg.WOSU_xxCDxxGH);  // oam, sprites
  /*p29.XYSO*/ wire XYSO_ABCxDEFx = not(WOJO_xxxDxxxH);  // oam

  //----------------------------------------
  // sch_decoder

  /*p10.AMUS*/ wire ADDR_0xx00000 = nor(bus_tri.A00(), bus_tri.A01(), bus_tri.A02(), bus_tri.A03(), bus_tri.A04(), bus_tri.A07());
  /*p07.TUNA*/ wire ADDR_0000_FE00 = nand(bus_tri.A15(), bus_tri.A14(), bus_tri.A13(), bus_tri.A12(), bus_tri.A11(), bus_tri.A10(), bus_tri.A09());
  /*p07.TONA*/ wire ADDR_08n = not(bus_tri.A08());
  /*p07.SYKE*/ wire ADDR_FFXX = nor(ADDR_0000_FE00, ADDR_08n);
  /*p10.ANAP*/ wire ADDR_111111110xx00000 = and(ADDR_0xx00000, ADDR_FFXX);

  /*p22.XOLA*/ wire A00n = not(bus_tri.A00());
  /*p22.XENO*/ wire A01n = not(bus_tri.A01());
  /*p22.XUSY*/ wire A02n = not(bus_tri.A02());
  /*p22.XERA*/ wire A03n = not(bus_tri.A03());
  /*p22.XALY*/ wire ADDR_0x00xxxx  = nor(bus_tri.A07(), bus_tri.A05(), bus_tri.A04());
  /*p22.WUTU*/ wire FF4Xn          = nand(ADDR_FFXX, bus_tri.A06(), ADDR_0x00xxxx);
  /*p22.WERO*/ wire FF4X           = not(FF4Xn);

  /*p07.SEMY*/ wire ADDR_XX0X = nor(bus_tri.A07(), bus_tri.A06(), bus_tri.A05(), bus_tri.A04());
  /*p07.SAPA*/ wire ADDR_XXXF = and(bus_tri.A00(), bus_tri.A01(), bus_tri.A02(), bus_tri.A03());

  /*p22.WADO*/ wire WADO_A00 = not(A00n);
  /*p22.WESA*/ wire WESA_A01 = not(A01n);
  /*p22.WALO*/ wire WALO_A02 = not(A02n);
  /*p22.WEPO*/ wire WEPO_A03 = not(A03n);

  /*p22.WORU*/ wire FF40n = nand(FF4X, A00n, A01n, A02n, A03n);
  /*p22.WOFA*/ wire FF41n = nand(FF4X, WADO_A00, A01n, A02n, A03n);
  /*p22.WEBU*/ wire FF42n = nand(FF4X, A00n, bus_tri.A01(), A02n, A03n);
  /*p22.WAVU*/ wire FF43n = nand(FF4X, bus_tri.A00(), bus_tri.A01(), A02n, A03n);
  /*p22.WYLE*/ wire FF44n = nand(FF4X, A00n, A01n, WALO_A02,  A03n);
  /*p22.WETY*/ wire FF45n = nand(FF4X, bus_tri.A00(), A01n, bus_tri.A02(), A03n);
  /*p22.WATE*/ wire FF46n = nand(FF4X, A00n, WESA_A01, WALO_A02, A03n);
  /*p22.WYBO*/ wire FF47n = nand(FF4X, bus_tri.A00(), bus_tri.A01(), bus_tri.A02(), A03n);
  /*p22.WETA*/ wire FF48n = nand(FF4X, A00n, A01n, A02n, bus_tri.A03());
  /*p22.VAMA*/ wire FF49n = nand(FF4X, bus_tri.A00(), A01n, A02n, bus_tri.A03());
  /*p22.WYVO*/ wire FF4An = nand(FF4X, A00n, bus_tri.A01(), A02n, bus_tri.A03());
  /*p22.WAGE*/ wire FF4Bn = nand(FF4X, bus_tri.A00(), bus_tri.A01(), A02n, bus_tri.A03());

  /*p22.VOCA*/ wire FF40 = not(FF40n);
  /*p22.VARY*/ wire FF41 = not(FF41n);
  /*p22.XARO*/ wire FF42 = not(FF42n);
  /*p22.XAVY*/ wire FF43 = not(FF43n);
  /*p22.XOGY*/ wire FF44 = not(FF44n);
  /*p22.XAYU*/ wire FF45 = not(FF45n);
  /*p22.XEDA*/ wire FF46 = not(FF46n);
  /*p22.WERA*/ wire FF47 = not(FF47n);
  /*p22.XAYO*/ wire FF48 = not(FF48n);
  /*p22.TEGO*/ wire FF49 = not(FF49n);
  /*p22.VYGA*/ wire FF4A = not(FF4An);
  /*p22.VUMY*/ wire FF4B = not(FF4Bn);

  /*p07.RYCU*/ wire ADDR_FE00_FFFF = not(ADDR_0000_FE00);
  /*p07.SOHA*/ wire ADDR_FFXXn2 = not(ADDR_FFXX);
  /*p07.ROPE*/ wire ADDR_FEXXn = nand(ADDR_FE00_FFFF, ADDR_FFXXn2);
  /*p07.SARO*/ wire ADDR_OAM = not(ADDR_FEXXn);

  /*p01.AGUT*/ wire AGUT_xxCDEFGH = and(or(AJAX_xxxxEFGH, AROV_xxCDEFxx), cpu_pins.ADDR_VALID);
  /*p01.AWOD*/ wire AWOD = or(UNOR_MODE_DBG2, AGUT_xxCDEFGH);
  /*p01.ABUZ*/ wire ADDR_VALID_xBCxxxxx = not(AWOD);

  /*p03.TOVY*/ wire TOVY_A00n = not(bus_tri.A00());
  /*p08.TOLA*/ wire TOLA_A01n = not(bus_tri.A01());
  /*p06.SEFY*/ wire SEFY_A02n = not(bus_tri.A02());

  /*p06.SARE*/ wire ADDR_XX00_XX07 = nor(bus_tri.A07(), bus_tri.A06(), bus_tri.A05(), bus_tri.A04(), bus_tri.A03());
  /*p06.SANO*/ wire ADDR_FF00_FF03 = and(ADDR_XX00_XX07, SEFY_A02n, ADDR_FFXX);

  /*p03.RYFO*/ wire FF04_FF07 = and(bus_tri.A02(), ADDR_XX00_XX07, ADDR_FFXX);

  //----------------------------------------
  // sch_buscontrol

  /*p01.AREV*/ wire AREV = nand(cpu_pins.CPU_RAW_WR, AFAS_xxxxEFGx);
  /*p01.APOV*/ wire CPU_WR_xxxxEFGx = not(AREV);

  /*p07.UJYV*/ wire CPU_RD_MUX = mux2n(cart_pins.RDn_C, cpu_pins.CPU_RAW_RD, UNOR_MODE_DBG2);
  /*p07.UBAL*/ wire CPU_WR_MUX = mux2n(cart_pins.WRn_C, CPU_WR_xxxxEFGx, UNOR_MODE_DBG2);

  /*p07.TEDO*/ wire TEDO_CPURD  = not(CPU_RD_MUX);
  /*p07.TAPU*/ wire TAPU_CPUWR  = not(CPU_WR_MUX);
    
  /*p07.AJAS*/ wire CPU_RDn = not(TEDO_CPURD);
  /*p07.DYKY*/ wire CPU_WRn = not(TAPU_CPUWR);

  /*p07.ASOT*/ wire ASOT_CPURD = not(CPU_RDn);
  /*p07.CUPA*/ wire CUPA_CPUWR = not(CPU_WRn);

  /*p08.SORE*/ wire ADDR_0000_7FFF = not(bus_tri.A15());
  /*p08.TEVY*/ wire ADDR_NOT_VRAM = or(bus_tri.A13(), bus_tri.A14(), ADDR_0000_7FFF);
  /*p08.TEXO*/ wire ADDR_VALID_AND_NOT_VRAM = and(cpu_pins.ADDR_VALID, ADDR_NOT_VRAM);
  /*p08.LEVO*/ wire ADDR_VALID_AND_NOT_VRAMn = not(ADDR_VALID_AND_NOT_VRAM);

  /*p08.MOCA*/ wire DBG_EXT_RDn = nor(ADDR_VALID_AND_NOT_VRAM, UMUT_MODE_DBG1);
  /*p08.LAGU*/ wire LAGU = or(and(cpu_pins.CPU_RAW_RD, ADDR_VALID_AND_NOT_VRAMn), cpu_pins.CPU_RAW_WR);
  /*p08.LYWE*/ wire LYWE = not(LAGU);

  /*p08.REDU*/ wire CPU_RDo = not(TEDO_CPURD);
  /*p08.MOTY*/ wire CPU_EXT_RD = or(DBG_EXT_RDn, LYWE);
  /*p08.RORU*/ wire CBUS_TO_CEXTn = mux2(CPU_RDo, CPU_EXT_RD, UNOR_MODE_DBG2);
  /*p08.LULA*/ wire CBUS_TO_CEXT  = not(CBUS_TO_CEXTn);

  //----------------------------------------
  // sch_lcd

  /*p01.LYHA*/ wire VID_RESET2 = not(VID_RESETn);
  /*p21.MUDE*/ wire X_RSTn = nor(lcd_reg.NEW_LINE_d0a, VID_RESET2);
  /*p21.SAXO*/ lcd_reg.X0.set(TALU_xBCDExxx, X_RSTn, !lcd_reg.X0);
  /*p21.TYPO*/ lcd_reg.X1.set(!lcd_reg.X0,   X_RSTn, !lcd_reg.X1);
  /*p21.VYZO*/ lcd_reg.X2.set(!lcd_reg.X1,   X_RSTn, !lcd_reg.X2);
  /*p21.TELU*/ lcd_reg.X3.set(!lcd_reg.X2,   X_RSTn, !lcd_reg.X3);
  /*p21.SUDE*/ lcd_reg.X4.set(!lcd_reg.X3,   X_RSTn, !lcd_reg.X4);
  /*p21.TAHA*/ lcd_reg.X5.set(!lcd_reg.X4,   X_RSTn, !lcd_reg.X5);
  /*p21.TYRY*/ lcd_reg.X6.set(!lcd_reg.X5,   X_RSTn, !lcd_reg.X6);

  /*p21.SANU*/ wire LINE_END     = and(lcd_reg.X6, lcd_reg.X5, lcd_reg.X4, lcd_reg.X0); // 113 = 64 + 32 + 16 + 1, schematic is wrong
  /*p01.LYFE*/ wire VID_RESETn2 = not(VID_RESET2);
  /*p21.RUTU*/ lcd_reg.NEW_LINE_d0a.set(SONO_AxxxxFGH, VID_RESETn, LINE_END);
  /*p21.NYPE*/ lcd_reg.NEW_LINE_d4a.set(TALU_xBCDExxx, VID_RESETn2, lcd_reg.NEW_LINE_d0a);
  
  /*p21.NOKO*/ wire LINE_153_d0  = and(lcd_reg.Y7, lcd_reg.Y4, lcd_reg.Y3, lcd_reg.Y0); // Schematic wrong: NOKO = and(V7, V4, V3, V0) = 128 + 16 + 8 + 1 = 153
  /*p21.MYTA*/ lcd_reg.LINE_153_d4.set(lcd_reg.NEW_LINE_d4a, VID_RESETn2, LINE_153_d0);

  /*p21.LAMA*/ wire FRAME_RSTn   = nor(lcd_reg.LINE_153_d4, VID_RESET2);
  /*p21.MUWY*/ lcd_reg.Y0.set(lcd_reg.NEW_LINE_d0a, FRAME_RSTn, !lcd_reg.Y0);
  /*p21.MYRO*/ lcd_reg.Y1.set(!lcd_reg.Y0,          FRAME_RSTn, !lcd_reg.Y1);
  /*p21.LEXA*/ lcd_reg.Y2.set(!lcd_reg.Y1,          FRAME_RSTn, !lcd_reg.Y2);
  /*p21.LYDO*/ lcd_reg.Y3.set(!lcd_reg.Y2,          FRAME_RSTn, !lcd_reg.Y3);
  /*p21.LOVU*/ lcd_reg.Y4.set(!lcd_reg.Y3,          FRAME_RSTn, !lcd_reg.Y4);
  /*p21.LEMA*/ lcd_reg.Y5.set(!lcd_reg.Y4,          FRAME_RSTn, !lcd_reg.Y5);
  /*p21.MATO*/ lcd_reg.Y6.set(!lcd_reg.Y5,          FRAME_RSTn, !lcd_reg.Y6);
  /*p21.LAFO*/ lcd_reg.Y7.set(!lcd_reg.Y6,          FRAME_RSTn, !lcd_reg.Y7);

  /*p21.XYVO*/ wire LINE_144_d0  = and(lcd_reg.Y4, lcd_reg.Y7); // 128 + 16 = 144
  /*p21.POPU*/ lcd_reg.VBLANK_d4.set(lcd_reg.NEW_LINE_d4a, VID_RESETn2, LINE_144_d0);

  /*p21.PURE*/ wire NEW_LINE_d0n = not(lcd_reg.NEW_LINE_d0a);
  /*p21.SELA*/ wire NEW_LINE_d0b = not(NEW_LINE_d0n);
  /*p29.ALES*/ wire LINE_144_d0n = not(LINE_144_d0);
  /*p29.ABOV*/ wire VID_LINE_d0  = and(NEW_LINE_d0b, LINE_144_d0n);
  /*p01.ATAR*/ wire VID_RESET6  = not(VID_RESETn);
  /*p01.ABEZ*/ wire VID_RESETn3 = not(VID_RESET6);
  /*p29.CATU*/ lcd_reg.VID_LINE_d4.set(XUPY_xBCxxFGx, VID_RESETn3, VID_LINE_d0);
  /*p28.ANEL*/ lcd_reg.VID_LINE_d6.set(AWOH_AxxDExxH, VID_RESETn3, lcd_reg.VID_LINE_d4);

  /*p01.AMYG*/ wire VID_RESET7  = not(VID_RESETn);

  /*p28.ABAF*/ wire VID_LINE_d4n = not(lcd_reg.VID_LINE_d4);

  /*p21.PARU*/ wire VBLANK_d4b = not(!lcd_reg.VBLANK_d4);
  /*p21.TOLU*/ wire INT_VBLn = not(VBLANK_d4b);

  /*p28.BYHA*/ wire BYHA_VID_LINE_TRIG_d4n = and(or(lcd_reg.VID_LINE_d6, VID_LINE_d4n), VID_RESETn3);
  /*p28.ATEJ*/ wire ATEJ_VID_LINE_TRIG_d4  = not(BYHA_VID_LINE_TRIG_d4n);
  /*p28.ABAK*/ wire ABAK_VID_LINE_TRIG_d4  = or (ATEJ_VID_LINE_TRIG_d4, VID_RESET7);
  /*p27.XAHY*/ wire XAHY_VID_LINE_TRIG_d4n = not(ATEJ_VID_LINE_TRIG_d4);
  /*p28.BYVA*/ wire BYVA_VID_LINE_TRIG_d4n = not(ABAK_VID_LINE_TRIG_d4);
  /*p29.DYBA*/ wire DYBA_VID_LINE_TRIG_d4  = not(BYVA_VID_LINE_TRIG_d4n);

  //----------------------------------------
  // sch_debug

  /*p27.VYPO*/ wire VYPO_P10Bn = not(joy_pins.P10_B);

#if 0

  //----------
  // weird debug things, probably not right

  /*p05.AXYN*/ wire AXYN_xBCDEFGH = not(clk_sig.BEDO_Axxxxxxx);
  /*p05.ADYR*/ wire ADYR_Axxxxxxx = not(AXYN_xBCDEFGH);
  /*p05.APYS*/ wire APYS_xBCDEFGH = nor(sys_sig.MODE_DBG2, ADYR_Axxxxxxx);
  /*p05.AFOP*/ wire AFOP_Axxxxxxx = not(APYS_xBCDEFGH);
  /*p07.LECO*/ wire LECO_xBCDEFGH = nor(clk_sig.BEDO_Axxxxxxx, sys_sig.MODE_DBG2);

  if (AFOP_Axxxxxxx) bus_tri.set_data(
    /*p05.ANOC*/ not(sys_sig.PIN_P10_B),
    /*p05.ATAJ*/ not(sys_sig.PIN_P10_B),
    /*p05.AJEC*/ not(sys_sig.PIN_P10_B),
    /*p05.ASUZ*/ not(sys_sig.PIN_P10_B),
    /*p05.BENU*/ not(sys_sig.PIN_P10_B),
    /*p05.AKAJ*/ not(sys_sig.PIN_P10_B),
    /*p05.ARAR*/ not(sys_sig.PIN_P10_B),
    /*p05.BEDA*/ not(sys_sig.PIN_P10_B)
  );

  if (LECO_xBCDEFGH) bus_tri.set_data(
    /*p07.ROMY*/ sys_sig.PIN_P10_B,
    /*p07.RYNE*/ sys_sig.PIN_P10_B,
    /*p07.REJY*/ sys_sig.PIN_P10_B,
    /*p07.RASE*/ sys_sig.PIN_P10_B,
    /*p07.REKA*/ sys_sig.PIN_P10_B,
    /*p07.ROWE*/ sys_sig.PIN_P10_B,
    /*p07.RYKE*/ sys_sig.PIN_P10_B,
    /*p07.RARU*/ sys_sig.PIN_P10_B
  );

  // FF60 debug reg
  /*p07.APET*/ wire APET_MODE_DBG = or(sys_sig.MODE_DBG1, sys_sig.MODE_DBG2);
  /*p07.APER*/ wire FF60_WRn = nand(APET_MODE_DBG, bus_tri.A05(), bus_tri.A06(), bus_sig.TAPU_CPUWR, dec_sig.ADDR_111111110xx00000);

  /*p05.KURA*/ wire FF60_0n = not(FF60_0);
  /*p05.JEVA*/ wire FF60_0o = not(FF60_0);


  /*p07.BURO*/ FF60_0.set(FF60_WRn, rst_sig.SYS_RESETn, bus_tri.D0());
  /*p07.AMUT*/ FF60_1.set(FF60_WRn, rst_sig.SYS_RESETn, bus_tri.D1());


  // FIXME
  ///*p05.KORE*/ wire P05_NC0 = nand(DBG_FF00_D7, FF60_0);
  ///*p05.KYWE*/ wire P05_NC1 = nor (DBG_FF00_D7, FF60_0o);

  /*p08.LYRA*/ wire DBG_D_RDn = nand(sys_sig.MODE_DBG2, bus_sig.CBUS_TO_CEXTn);
  /*p08.TUTY*/ if (!DBG_D_RDn) bus_tri.D0 = not(/*p08.TOVO*/ not(pins.D0_C));
  /*p08.SYWA*/ if (!DBG_D_RDn) bus_tri.D1 = not(/*p08.RUZY*/ not(pins.D1_C));
  /*p08.SUGU*/ if (!DBG_D_RDn) bus_tri.D2 = not(/*p08.ROME*/ not(pins.D2_C));
  /*p08.TAWO*/ if (!DBG_D_RDn) bus_tri.D3 = not(/*p08.SAZA*/ not(pins.D3_C));
  /*p08.TUTE*/ if (!DBG_D_RDn) bus_tri.D4 = not(/*p08.TEHE*/ not(pins.D4_C));
  /*p08.SAJO*/ if (!DBG_D_RDn) bus_tri.D5 = not(/*p08.RATU*/ not(pins.D5_C));
  /*p08.TEMY*/ if (!DBG_D_RDn) bus_tri.D6 = not(/*p08.SOCA*/ not(pins.D6_C));
  /*p08.ROPA*/ if (!DBG_D_RDn) bus_tri.D7 = not(/*p08.RYBA*/ not(pins.D7_C));


  //----------
  // more debug stuff

  /*p27.VYPO*/ wire VYPO = not(joy_pins.P10_B);
  /*p25.TUSO*/ wire TUSO = nor(MODE_DBG2, clk.BOGA_xBCDEFGH);
  /*p25.SOLE*/ wire SOLE = not(TUSO);

  if (VYPO) bus_out.set_data(
    /*p25.TOVU*/ SOLE,
    /*p25.SOSA*/ SOLE,
    /*p25.SEDU*/ SOLE,
    /*p25.TAXO*/ SOLE,
    /*p25.TAHY*/ SOLE,
    /*p25.TESU*/ SOLE,
    /*p25.TAZU*/ SOLE,
    /*p25.TEWA*/ SOLE
  );

}
#endif

  //----------------------------------------
  // sch_dma

  // schematic incorrect.
  // lyxe - weird gate - lavy, loko
  // lupa - nor - lavy, lyxe

  /*p04.LAVY*/ wire FF46_WRn  = nand(FF46, CUPA_CPUWR);
  /*p04.LORU*/ wire FF46_WR = not(FF46_WRn);
 
  /*p04.LEBU*/ wire DMA_A15n  = not(dma_reg.DMA_A15);
  /*p04.MUDA*/ wire DMA_VRAM  = nor(dma_reg.DMA_A13, dma_reg.DMA_A14, DMA_A15n);
  /*p04.LOGO*/ wire DMA_VRAMn = not(DMA_VRAM);
  /*p04.MORY*/ wire DMA_READ_CARTn = nand(dma_reg.REG_DMA_RW_EN, DMA_VRAMn);
  /*p04.MUHO*/ wire DMA_READ_VRAMn = nand(dma_reg.REG_DMA_RW_EN, DMA_VRAM);
  /*p04.LUMA*/ wire DMA_READ_CART = not(DMA_READ_CARTn);
  /*p04.LUFA*/ wire DMA_READ_VRAM = not(DMA_READ_VRAMn);

  /*p04.MAKA*/ dma_reg.FROM_CPU5_SYNC.set(ZEME_xBxDxFxH, CUNU_RESETn, cpu_pins.FROM_CPU5);
  /*p04.NAXY*/ wire NAXY = nor(dma_reg.FROM_CPU5_SYNC, dma_reg.REG_DMA_EN_d0);
  /*p04.POWU*/ wire DMA_WRITE_OAM = and(dma_reg.REG_DMA_RW_EN, NAXY);

  /*p04.LOKO*/ wire DMA_RST   = nand(CUNU_RESETn, !dma_reg.REG_DMA_EN_d4);
  /*p04.LAPA*/ wire DMA_RSTn  = not(DMA_RST);
  /*p04.NAVO*/ wire DMA_DONEn = nand(dma_reg.DMA_A00, dma_reg.DMA_A01, dma_reg.DMA_A02, dma_reg.DMA_A03, dma_reg.DMA_A04, dma_reg.DMA_A07); // 128+16+8+4+2+1 = 159, this must be "dma done"
  /*p04.NOLO*/ wire DMA_DONE  = not(DMA_DONEn);
  /*p04.MYTE*/ dma_reg.DMA_DONE_SYNC.set(MOPA_xxxxEFGH, DMA_RSTn, DMA_DONE);

  /*p04.LYXE*/ dma_reg.LATCH_DMA_ENn_d0.sr_latch(DMA_RST, FF46_WRn);
  /*p04.LUPA*/ wire DMA_EN = nor(FF46_WRn, dma_reg.LATCH_DMA_ENn_d0); // this seems redundant

  /*p04.LUVY*/ dma_reg.REG_DMA_EN_d0.set(UVYT_ABCDxxxx, CUNU_RESETn, DMA_EN);
  /*p04.LENE*/ dma_reg.REG_DMA_EN_d4.set(MOPA_xxxxEFGH, CUNU_RESETn, dma_reg.REG_DMA_EN_d0);

  /*p04.LOKY*/ dma_reg.LATCH_DMA_EN_d4.tp_latch(dma_reg.REG_DMA_EN_d4, dma_reg.DMA_DONE_SYNC || !CUNU_RESETn); // lara/loky nand sr latch
  /*p04.MATU*/ dma_reg.REG_DMA_RW_EN.set(UVYT_ABCDxxxx, CUNU_RESETn, dma_reg.LATCH_DMA_EN_d4);

  /*p04.META*/ wire CLK_DMA_LO = and(UVYT_ABCDxxxx, dma_reg.LATCH_DMA_EN_d4);
  /*p04.NAKY*/ dma_reg.DMA_A00.set(CLK_DMA_LO, DMA_RSTn, !dma_reg.DMA_A00);
  /*p04.PYRO*/ dma_reg.DMA_A01.set(!dma_reg.DMA_A00, DMA_RSTn, !dma_reg.DMA_A01);
  /*p04.NEFY*/ dma_reg.DMA_A02.set(!dma_reg.DMA_A01, DMA_RSTn, !dma_reg.DMA_A02);
  /*p04.MUTY*/ dma_reg.DMA_A03.set(!dma_reg.DMA_A02, DMA_RSTn, !dma_reg.DMA_A03);
  /*p04.NYKO*/ dma_reg.DMA_A04.set(!dma_reg.DMA_A03, DMA_RSTn, !dma_reg.DMA_A04);
  /*p04.PYLO*/ dma_reg.DMA_A05.set(!dma_reg.DMA_A04, DMA_RSTn, !dma_reg.DMA_A05);
  /*p04.NUTO*/ dma_reg.DMA_A06.set(!dma_reg.DMA_A05, DMA_RSTn, !dma_reg.DMA_A06);
  /*p04.MUGU*/ dma_reg.DMA_A07.set(!dma_reg.DMA_A06, DMA_RSTn, !dma_reg.DMA_A07);

  /*p04.NAFA*/ dma_reg.DMA_A08.set(FF46_WR, bus_tri.D0());
  /*p04.PYNE*/ dma_reg.DMA_A09.set(FF46_WR, bus_tri.D1());
  /*p04.PARA*/ dma_reg.DMA_A10.set(FF46_WR, bus_tri.D2());
  /*p04.NYDO*/ dma_reg.DMA_A11.set(FF46_WR, bus_tri.D3());
  /*p04.NYGY*/ dma_reg.DMA_A12.set(FF46_WR, bus_tri.D4());
  /*p04.PULA*/ dma_reg.DMA_A13.set(FF46_WR, bus_tri.D5());
  /*p04.POKU*/ dma_reg.DMA_A14.set(FF46_WR, bus_tri.D6());
  /*p04.MARU*/ dma_reg.DMA_A15.set(FF46_WR, bus_tri.D7());

  /*p04.MOLU*/ wire FF46_RDn1 = nand(FF46, ASOT_CPURD);
  /*p04.NYGO*/ wire FF46_RD   = not(FF46_RDn1);
  /*p04.PUSY*/ wire FF46_RDn2 = not(FF46_RD);

  if (FF46_RDn2) bus_tri.set_data(
    /*p04.POLY*/ dma_reg.DMA_A08,
    /*p04.ROFO*/ dma_reg.DMA_A09,
    /*p04.REMA*/ dma_reg.DMA_A10,
    /*p04.PANE*/ dma_reg.DMA_A11,
    /*p04.PARE*/ dma_reg.DMA_A12,
    /*p04.RALY*/ dma_reg.DMA_A13,
    /*p04.RESU*/ dma_reg.DMA_A14,
    /*p04.NUVY*/ dma_reg.DMA_A15
  );

  //----------------------------------------
  // sch_timer.tick_div

  /*p01.TAPE*/ wire FF04_WR = and(TAPU_CPUWR, FF04_FF07, TOLA_A01n, TOVY_A00n);
  /*p01.TAGY*/ wire FF04_RD = and(TEDO_CPURD, FF04_FF07, TOLA_A01n, TOVY_A00n);

  /*p01.UFOL*/ wire DIV_RSTn = nor(UCOB_CLKBAD, ext_pins.RST, FF04_WR);
  
  ///*p01.ULUR*/ wire DIV_06_CLK = mux2n(clk_sig.BOGA_xBCDEFGH, !DIV_05, sys_sig.FF60_1);
  wire DIV_06_CLK = !tim_reg.DIV_05;

  /*p01.UKUP*/ tim_reg.DIV_00.set(BOGA_xBCDEFGH,   DIV_RSTn, !tim_reg.DIV_00);
  /*p01.UFOR*/ tim_reg.DIV_01.set(!tim_reg.DIV_00, DIV_RSTn, !tim_reg.DIV_01);
  /*p01.UNER*/ tim_reg.DIV_02.set(!tim_reg.DIV_01, DIV_RSTn, !tim_reg.DIV_02);
  /*p01.TERO*/ tim_reg.DIV_03.set(!tim_reg.DIV_02, DIV_RSTn, !tim_reg.DIV_03);
  /*p01.UNYK*/ tim_reg.DIV_04.set(!tim_reg.DIV_03, DIV_RSTn, !tim_reg.DIV_04);
  /*p01.TAMA*/ tim_reg.DIV_05.set(!tim_reg.DIV_04, DIV_RSTn, !tim_reg.DIV_05);

  // schematic says DIV_06_CLK is not inverted, but the counter acts funny if it isn't.
  /*p01.UGOT*/ tim_reg.DIV_06.set(!DIV_06_CLK,     DIV_RSTn, !tim_reg.DIV_06);
  /*p01.TULU*/ tim_reg.DIV_07.set(!tim_reg.DIV_06, DIV_RSTn, !tim_reg.DIV_07);
  /*p01.TUGO*/ tim_reg.DIV_08.set(!tim_reg.DIV_07, DIV_RSTn, !tim_reg.DIV_08);
  /*p01.TOFE*/ tim_reg.DIV_09.set(!tim_reg.DIV_08, DIV_RSTn, !tim_reg.DIV_09);
  /*p01.TERU*/ tim_reg.DIV_10.set(!tim_reg.DIV_09, DIV_RSTn, !tim_reg.DIV_10);
  /*p01.SOLA*/ tim_reg.DIV_11.set(!tim_reg.DIV_10, DIV_RSTn, !tim_reg.DIV_11);
  /*p01.SUBU*/ tim_reg.DIV_12.set(!tim_reg.DIV_11, DIV_RSTn, !tim_reg.DIV_12);
  /*p01.TEKA*/ tim_reg.DIV_13.set(!tim_reg.DIV_12, DIV_RSTn, !tim_reg.DIV_13);
  /*p01.UKET*/ tim_reg.DIV_14.set(!tim_reg.DIV_13, DIV_RSTn, !tim_reg.DIV_14);
  /*p01.UPOF*/ tim_reg.DIV_15.set(!tim_reg.DIV_14, DIV_RSTn, !tim_reg.DIV_15);

  /*p01.UMEK*/ wire DIV_06n   = not(tim_reg.DIV_06);
  /*p01.UREK*/ wire DIV_07n   = not(tim_reg.DIV_07);
  /*p01.UTOK*/ wire DIV_08n   = not(tim_reg.DIV_08);
  /*p01.SAPY*/ wire DIV_09n   = not(tim_reg.DIV_09);
  /*p01.UMER*/ wire DIV_10n   = not(tim_reg.DIV_10);
  /*p01.RAVE*/ wire DIV_11n   = not(tim_reg.DIV_11);
  /*p01.RYSO*/ wire DIV_12n   = not(tim_reg.DIV_12);
  /*p01.UDOR*/ wire DIV_13n   = not(tim_reg.DIV_13);
  
  if (FF04_RD) bus_tri.set_data(
    /*p01.TAWU*/ not(DIV_06n),
    /*p01.TAKU*/ not(DIV_07n),
    /*p01.TEMU*/ not(DIV_08n),
    /*p01.TUSE*/ not(DIV_09n),
    /*p01.UPUG*/ not(DIV_10n),
    /*p01.SEPU*/ not(DIV_11n),
    /*p01.SAWA*/ not(DIV_12n),
    /*p01.TATU*/ not(DIV_13n)
  );

  //----------------------------------------
  // sch_timer.tick_tima

  /*p03.TOPE*/ wire FF05_WRn = nand(TAPU_CPUWR, FF04_FF07, TOLA_A01n, bus_tri.A00());
  /*p03.TEDA*/ wire FF05_RD = and(TEDO_CPURD, FF04_FF07, TOLA_A01n,     bus_tri.A00());

  /*p03.ROKE*/ wire TIMA_MUX_0 = mux2n(tim_reg.TMA_0, bus_tri.D0(), FF05_WRn);
  /*p03.PETU*/ wire TIMA_MUX_1 = mux2n(tim_reg.TMA_1, bus_tri.D1(), FF05_WRn);
  /*p03.NYKU*/ wire TIMA_MUX_2 = mux2n(tim_reg.TMA_2, bus_tri.D2(), FF05_WRn);
  /*p03.SOCE*/ wire TIMA_MUX_3 = mux2n(tim_reg.TMA_3, bus_tri.D3(), FF05_WRn);
  /*p03.SALA*/ wire TIMA_MUX_4 = mux2n(tim_reg.TMA_4, bus_tri.D4(), FF05_WRn);
  /*p03.SYRU*/ wire TIMA_MUX_5 = mux2n(tim_reg.TMA_5, bus_tri.D5(), FF05_WRn);
  /*p03.REFU*/ wire TIMA_MUX_6 = mux2n(tim_reg.TMA_6, bus_tri.D6(), FF05_WRn);
  /*p03.RATO*/ wire TIMA_MUX_7 = mux2n(tim_reg.TMA_7, bus_tri.D7(), FF05_WRn);

  /*p03.MULO*/ wire TIMA_RST = not(SYS_RESETn);

  /*p03.PUXY*/ wire TIMA_LD_0 = nor(TIMA_RST, TIMA_MUX_0);
  /*p03.NERO*/ wire TIMA_LD_1 = nor(TIMA_RST, TIMA_MUX_1);
  /*p03.NADA*/ wire TIMA_LD_2 = nor(TIMA_RST, TIMA_MUX_2);
  /*p03.REPA*/ wire TIMA_LD_3 = nor(TIMA_RST, TIMA_MUX_3);
  /*p03.ROLU*/ wire TIMA_LD_4 = nor(TIMA_RST, TIMA_MUX_4);
  /*p03.RUGY*/ wire TIMA_LD_5 = nor(TIMA_RST, TIMA_MUX_5);
  /*p03.PYMA*/ wire TIMA_LD_6 = nor(TIMA_RST, TIMA_MUX_6);
  /*p03.PAGU*/ wire TIMA_LD_7 = nor(TIMA_RST, TIMA_MUX_7);

  // Clock mux
  /*p03.UBOT*/ wire CLK_256Kn = not(tim_reg.DIV_01);
  /*p03.UVYR*/ wire CLK_64Kn  = not(tim_reg.DIV_03);
  /*p01.UVYN*/ wire CLK_16Kn  = not(tim_reg.DIV_05);
  /*p01.UREK*/ wire CLK_4Kn   = not(tim_reg.DIV_07);

  /*p03.UKAP*/ wire CLK_MUXa = mux2n(CLK_16Kn,  CLK_64Kn, tim_reg.TAC_0);
  /*p03.TEKO*/ wire CLK_MUXb = mux2n(CLK_256Kn, CLK_4Kn,  tim_reg.TAC_0);
  /*p03.TECY*/ wire CLK_MUXc = mux2n(CLK_MUXa,  CLK_MUXb, tim_reg.TAC_1);

  /*p03.MUZU*/ wire MUZU       = or(cpu_pins.FROM_CPU5, FF05_WRn);
  /*p03.MEKE*/ wire INT_TIMERn = not(tim_reg.INT_TIMER);
  /*p03.MEXU*/ wire TIMA_LOAD  = nand(MUZU, SYS_RESETn, INT_TIMERn);
  /*p03.MUGY*/ wire TIMA_LOADn = not(TIMA_LOAD);

  /*p03.SOGU*/ wire TIMA_CLK = nor(CLK_MUXc, !tim_reg.TAC_2);

  /*p03.REGA*/ tim_reg.TIMA_0.set(TIMA_CLK,   TIMA_LOAD, TIMA_LD_0);
  /*p03.POVY*/ tim_reg.TIMA_1.set(tim_reg.TIMA_0.c(), TIMA_LOAD, TIMA_LD_1);
  /*p03.PERU*/ tim_reg.TIMA_2.set(tim_reg.TIMA_1.c(), TIMA_LOAD, TIMA_LD_2);
  /*p03.RATE*/ tim_reg.TIMA_3.set(tim_reg.TIMA_2.c(), TIMA_LOAD, TIMA_LD_3);
  /*p03.RUBY*/ tim_reg.TIMA_4.set(tim_reg.TIMA_3.c(), TIMA_LOAD, TIMA_LD_4);
  /*p03.RAGE*/ tim_reg.TIMA_5.set(tim_reg.TIMA_4.c(), TIMA_LOAD, TIMA_LD_5);
  /*p03.PEDA*/ tim_reg.TIMA_6.set(tim_reg.TIMA_5.c(), TIMA_LOAD, TIMA_LD_6);
  /*p03.NUGA*/ tim_reg.TIMA_7.set(tim_reg.TIMA_6.c(), TIMA_LOAD, TIMA_LD_7);

  /*p03.MERY*/ wire INT_TIMER_IN = nor(!tim_reg.TIMA_MAX, tim_reg.TIMA_7.c());

  /*p03.NYDU*/ tim_reg.TIMA_MAX.set(BOGA_xBCDEFGH, TIMA_LOADn, tim_reg.TIMA_7.c());
  /*p03.MOBA*/ tim_reg.INT_TIMER.set(BOGA_xBCDEFGH, SYS_RESETn, INT_TIMER_IN);

  if (FF05_RD) bus_tri.set_data(
    /*p03.SOKU*/ tim_reg.TIMA_0.v(),
    /*p03.RACY*/ tim_reg.TIMA_1.v(),
    /*p03.RAVY*/ tim_reg.TIMA_2.v(),
    /*p03.SOSY*/ tim_reg.TIMA_3.v(),
    /*p03.SOMU*/ tim_reg.TIMA_4.v(),
    /*p03.SURO*/ tim_reg.TIMA_5.v(),
    /*p03.ROWU*/ tim_reg.TIMA_6.v(),
    /*p03.PUSO*/ tim_reg.TIMA_7.v()
  );

  //----------------------------------------
  // sch_timer.tick_tma

  /*p03.TYJU*/ wire FF06_WRn = nand(TAPU_CPUWR, FF04_FF07, TOVY_A00n, bus_tri.A01());
  /*p03.TUBY*/ wire FF06_RD = and(TEDO_CPURD, FF04_FF07, bus_tri.A01(), TOVY_A00n);

  /*p03.SABU*/ tim_reg.TMA_0.set(FF06_WRn, SYS_RESETn, bus_tri.D0());
  /*p03.NYKE*/ tim_reg.TMA_1.set(FF06_WRn, SYS_RESETn, bus_tri.D1());
  /*p03.MURU*/ tim_reg.TMA_2.set(FF06_WRn, SYS_RESETn, bus_tri.D2());
  /*p03.TYVA*/ tim_reg.TMA_3.set(FF06_WRn, SYS_RESETn, bus_tri.D3());
  /*p03.TYRU*/ tim_reg.TMA_4.set(FF06_WRn, SYS_RESETn, bus_tri.D4());
  /*p03.SUFY*/ tim_reg.TMA_5.set(FF06_WRn, SYS_RESETn, bus_tri.D5());
  /*p03.PETO*/ tim_reg.TMA_6.set(FF06_WRn, SYS_RESETn, bus_tri.D6());
  /*p03.SETA*/ tim_reg.TMA_7.set(FF06_WRn, SYS_RESETn, bus_tri.D7());

  if (FF06_RD) bus_tri.set_data(
    /*p03.SETE*/ tim_reg.TMA_0,
    /*p03.PYRE*/ tim_reg.TMA_1,
    /*p03.NOLA*/ tim_reg.TMA_2,
    /*p03.SALU*/ tim_reg.TMA_3,
    /*p03.SUPO*/ tim_reg.TMA_4,
    /*p03.SOTU*/ tim_reg.TMA_5,
    /*p03.REVA*/ tim_reg.TMA_6,
    /*p03.SAPU*/ tim_reg.TMA_7
  );

  //----------------------------------------
  // sch_timer.tick_tac

  /*p03.SARA*/ wire FF07_WRn = nand(TAPU_CPUWR, FF04_FF07, bus_tri.A00(), bus_tri.A01());
  /*p03.SORA*/ wire FF07_RD = and(TEDO_CPURD, FF04_FF07, bus_tri.A00(), bus_tri.A01());

  /*p03.SOPU*/ tim_reg.TAC_0.set(FF07_WRn, SYS_RESETn, bus_tri.D0());
  /*p03.SAMY*/ tim_reg.TAC_1.set(FF07_WRn, SYS_RESETn, bus_tri.D1());
  /*p03.SABO*/ tim_reg.TAC_2.set(FF07_WRn, SYS_RESETn, bus_tri.D2());

  if (FF07_RD) bus_tri.set_data(
    /*p03.RYLA*/ tim_reg.TAC_0,
    /*p03.ROTE*/ tim_reg.TAC_1,
    /*p03.SUPE*/ tim_reg.TAC_2
  );

  //----------------------------------------
  // sch_serial

  /*p06.COBA*/ wire SER_CNT3n = not(ser_reg.SER_CNT3);

  /*p06.UWAM*/ wire FF02_WRn = nand(TAPU_CPUWR, ADDR_FF00_FF03, bus_tri.A01(), TOVY_A00n);
  /*p06.CABY*/ wire XFER_RESET = and(SER_CNT3n, SYS_RESETn);
  /*p06.ETAF*/ ser_reg.XFER_START.set(FF02_WRn, XFER_RESET,         bus_tri.D0());
  /*p06.CULY*/ ser_reg.XFER_DIR  .set(FF02_WRn, SYS_RESETn, bus_tri.D1());

  /*p01.UVYN*/ wire CLK_16K = not(tim_reg.DIV_05);
  /*p06.COTY*/ ser_reg.SER_CLK.set(CLK_16K, FF02_WRn, !ser_reg.SER_CLK);

  /*p06.CAVE*/ wire SER_CLK_MUXn = mux2n(ser_reg.SER_CLK, ser_pins.SCK_C, ser_reg.XFER_DIR);

  /*p06.CARO*/ wire SER_RST  = and(FF02_WRn, SYS_RESETn);
  /*p06.DAWA*/ wire SER_CLK1 = or(SER_CLK_MUXn, !ser_reg.XFER_START); // this must stop the clock or something when the transmit's done
  /*p06.CAFA*/ ser_reg.SER_CNT0.set(SER_CLK1,          SER_RST, !ser_reg.SER_CNT0);
  /*p06.CYLO*/ ser_reg.SER_CNT1.set(!ser_reg.SER_CNT0, SER_RST, !ser_reg.SER_CNT1);
  /*p06.CYDE*/ ser_reg.SER_CNT2.set(!ser_reg.SER_CNT1, SER_RST, !ser_reg.SER_CNT2);
  /*p06.CALY*/ ser_reg.SER_CNT3.set(!ser_reg.SER_CNT2, SER_RST, !ser_reg.SER_CNT3);
  
  /*p06.URYS*/ wire FF01_WRn = nand(TAPU_CPUWR, ADDR_FF00_FF03, bus_tri.A00(), TOLA_A01n);
  /*p06.DAKU*/ wire FF01_WR  = not (FF01_WRn);

  /*p06.CUFU*/ wire SER_DATA0_SETn = nand(bus_tri.D0(), FF01_WR);
  /*p06.DOCU*/ wire SER_DATA1_SETn = nand(bus_tri.D1(), FF01_WR);
  /*p06.DELA*/ wire SER_DATA2_SETn = nand(bus_tri.D2(), FF01_WR);
  /*p06.DYGE*/ wire SER_DATA3_SETn = nand(bus_tri.D3(), FF01_WR);
  /*p06.DOLA*/ wire SER_DATA4_SETn = nand(bus_tri.D4(), FF01_WR);
  /*p06.ELOK*/ wire SER_DATA5_SETn = nand(bus_tri.D5(), FF01_WR);
  /*p06.EDEL*/ wire SER_DATA6_SETn = nand(bus_tri.D6(), FF01_WR);
  /*p06.EFEF*/ wire SER_DATA7_SETn = nand(bus_tri.D7(), FF01_WR);

  /*p06.COHY*/ wire SER_DATA0_RSTn = or(and(FF01_WRn, bus_tri.D0()), SYS_RESETn);
  /*p06.DUMO*/ wire SER_DATA1_RSTn = or(and(FF01_WRn, bus_tri.D1()), SYS_RESETn);
  /*p06.DYBO*/ wire SER_DATA2_RSTn = or(and(FF01_WRn, bus_tri.D2()), SYS_RESETn);
  /*p06.DAJU*/ wire SER_DATA3_RSTn = or(and(FF01_WRn, bus_tri.D3()), SYS_RESETn);
  /*p06.DYLY*/ wire SER_DATA4_RSTn = or(and(FF01_WRn, bus_tri.D4()), SYS_RESETn);
  /*p06.EHUJ*/ wire SER_DATA5_RSTn = or(and(FF01_WRn, bus_tri.D5()), SYS_RESETn);
  /*p06.EFAK*/ wire SER_DATA6_RSTn = or(and(FF01_WRn, bus_tri.D6()), SYS_RESETn);
  /*p06.EGUV*/ wire SER_DATA7_RSTn = or(and(FF01_WRn, bus_tri.D7()), SYS_RESETn);

  /*p06.CAGE*/ wire SIN_Cn = not(ser_pins.SIN_C);
  /*p06.EDYL*/ wire SER_CLKn = not(SER_CLK1);
  /*p06.EPYT*/ wire SER_CLK2 = not(SER_CLKn);
  /*p06.DEHO*/ wire DEHO = not(SER_CLK2);
  /*p06.DAWE*/ wire SER_CLK3 = not(DEHO);
  /*p06.CUBA*/ ser_reg.SER_DATA0.set(SER_CLK3, SER_DATA0_SETn, SER_DATA0_RSTn, SIN_Cn);
  /*p06.DEGU*/ ser_reg.SER_DATA1.set(SER_CLK3, SER_DATA1_SETn, SER_DATA1_RSTn, ser_reg.SER_DATA0);
  /*p06.DYRA*/ ser_reg.SER_DATA2.set(SER_CLK3, SER_DATA2_SETn, SER_DATA2_RSTn, ser_reg.SER_DATA1);
  /*p06.DOJO*/ ser_reg.SER_DATA3.set(SER_CLK3, SER_DATA3_SETn, SER_DATA3_RSTn, ser_reg.SER_DATA2);
  /*p06.DOVU*/ ser_reg.SER_DATA4.set(SER_CLK2, SER_DATA4_SETn, SER_DATA4_RSTn, ser_reg.SER_DATA3);
  /*p06.EJAB*/ ser_reg.SER_DATA5.set(SER_CLK2, SER_DATA5_SETn, SER_DATA5_RSTn, ser_reg.SER_DATA4);
  /*p06.EROD*/ ser_reg.SER_DATA6.set(SER_CLK2, SER_DATA6_SETn, SER_DATA6_RSTn, ser_reg.SER_DATA5);
  /*p06.EDER*/ ser_reg.SER_DATA7.set(SER_CLK2, SER_DATA7_SETn, SER_DATA7_RSTn, ser_reg.SER_DATA6);
  /*p06.ELYS*/ ser_reg.SER_OUT.set(SER_CLKn, SYS_RESETn, ser_reg.SER_DATA7);

  ///*p05.KENA*/ ser_pins.SOUT  = mux2n(DBG_FF00_D6, ser_reg.SER_OUT, FF60_0);
  ser_pins.SOUT = ser_reg.SER_OUT;
  /*p06.KEXU*/ ser_pins.SCK_A = nand(ser_reg.SER_CLK, ser_reg.XFER_DIR);
  /*p06.CULY*/ ser_pins.SCK_B = ser_reg.XFER_DIR;
  /*p06.KUJO*/ ser_pins.SCK_D = nor (ser_reg.SER_CLK, /*p06.JAGO*/ not(ser_reg.XFER_DIR));

  /*p06.UFEG*/ wire FF01_RD = and(TEDO_CPURD, ADDR_FF00_FF03, bus_tri.A00(), TOLA_A01n);

  if (FF01_RD) bus_tri.set_data(
    /*p06.CUGY*/ ser_reg.SER_DATA0,
    /*p06.DUDE*/ ser_reg.SER_DATA1,
    /*p06.DETU*/ ser_reg.SER_DATA2,
    /*p06.DASO*/ ser_reg.SER_DATA3,
    /*p06.DAME*/ ser_reg.SER_DATA4,
    /*p06.EVOK*/ ser_reg.SER_DATA5,
    /*p06.EFAB*/ ser_reg.SER_DATA6,
    /*p06.ETAK*/ ser_reg.SER_DATA7
  );

  /*p06.UCOM*/ wire FF02_RD = and (TEDO_CPURD, ADDR_FF00_FF03, bus_tri.A01(), TOVY_A00n);

  if (FF02_RD) bus_tri.set_data(
    /*p06.CORE*/ ser_reg.XFER_DIR,
    /*p06.ELUV*/ ser_reg.XFER_START
  );

  //----------------------------------------
  // sch_joypad

  // FIXME
  wire FF60_0 = 0;
  wire FF60_0n = 1;

  /*p10.AKUG*/ wire A06n = not(bus_tri.A06());
  /*p10.BYKO*/ wire A05n = not(bus_tri.A05());

  /*p05.KOLE*/ joy_pins.P10_A = nand(joy_reg.JOYP_RA,  FF60_0);
  /*p05.KYBU*/ joy_pins.P10_D = nor (joy_reg.JOYP_RA,  FF60_0n);
  /*p05.KYTO*/ joy_pins.P11_A = nand(joy_reg.JOYP_LB,  FF60_0);
  /*p05.KABU*/ joy_pins.P11_D = nor (joy_reg.JOYP_LB,  FF60_0n);
  /*p05.KYHU*/ joy_pins.P12_A = nand(joy_reg.JOYP_UC,  FF60_0);
  /*p05.KASY*/ joy_pins.P12_D = nor (joy_reg.JOYP_UC,  FF60_0n); // schematic wrong
  /*p05.KORY*/ joy_pins.P13_A = nand(joy_reg.JOYP_DS,  FF60_0);
  /*p05.KALE*/ joy_pins.P13_D = nor (joy_reg.JOYP_DS,  FF60_0n);
  /*p05.KARU*/ joy_pins.P14_A = or(!joy_reg.JOYP_UDLR, FF60_0n);
  /*p05.KARU*/ joy_pins.P14_D = joy_reg.JOYP_UDLR;
  /*p05.CELA*/ joy_pins.P15_A = or(!joy_reg.JOYP_ABCS, FF60_0n);
  /*p05.KARU*/ joy_pins.P15_D = !joy_reg.JOYP_ABCS;

  /*p02.KERY*/ wire ANY_BUTTON = or(joy_pins.P13_C, joy_pins.P12_C, joy_pins.P11_C, joy_pins.P10_C);

  /*p02.AWOB*/ joy_reg.WAKE_CPU.set(ANY_BUTTON, BOGA_xBCDEFGH);

  /*p02.BATU*/ joy_reg.JP_GLITCH0.set(BOGA_xBCDEFGH, SYS_RESETn, ANY_BUTTON);
  /*p02.ACEF*/ joy_reg.JP_GLITCH1.set(BOGA_xBCDEFGH, SYS_RESETn, joy_reg.JP_GLITCH0);
  /*p02.AGEM*/ joy_reg.JP_GLITCH2.set(BOGA_xBCDEFGH, SYS_RESETn, joy_reg.JP_GLITCH1);
  /*p02.APUG*/ joy_reg.JP_GLITCH3.set(BOGA_xBCDEFGH, SYS_RESETn, joy_reg.JP_GLITCH2);
  /*p02.ASOK*/ wire INT_JP = and(joy_reg.JP_GLITCH3, joy_reg.JP_GLITCH0);

  /*p10.ATOZ*/ wire FF00_WRn   = nand(TAPU_CPUWR, ADDR_111111110xx00000, A06n, A05n);
  /*p05.JUTE*/ joy_reg.JOYP_RA    .set(FF00_WRn, SYS_RESETn, bus_tri.D0());
  /*p05.KECY*/ joy_reg.JOYP_LB    .set(FF00_WRn, SYS_RESETn, bus_tri.D1());
  /*p05.JALE*/ joy_reg.JOYP_UC    .set(FF00_WRn, SYS_RESETn, bus_tri.D2());
  /*p05.KYME*/ joy_reg.JOYP_DS    .set(FF00_WRn, SYS_RESETn, bus_tri.D3());
  /*p05.KELY*/ joy_reg.JOYP_UDLR  .set(FF00_WRn, SYS_RESETn, bus_tri.D4());
  /*p05.COFY*/ joy_reg.JOYP_ABCS  .set(FF00_WRn, SYS_RESETn, bus_tri.D5());
  /*p05.KUKO*/ joy_reg.DBG_FF00_D6.set(FF00_WRn, SYS_RESETn, bus_tri.D6());
  /*p05.KERU*/ joy_reg.DBG_FF00_D7.set(FF00_WRn, SYS_RESETn, bus_tri.D7());

  /*p10.ACAT*/ wire FF00_RD = and (TEDO_CPURD, ADDR_111111110xx00000, A06n, A05n);
  /*p05.BYZO*/ wire FF00_RDn = not(FF00_RD);
  /*p05.KEVU*/ joy_reg.JOYP_L0.set(joy_pins.P10_C, FF00_RDn);
  /*p05.KAPA*/ joy_reg.JOYP_L1.set(joy_pins.P11_C, FF00_RDn);
  /*p05.KEJA*/ joy_reg.JOYP_L2.set(joy_pins.P12_C, FF00_RDn);
  /*p05.KOLO*/ joy_reg.JOYP_L3.set(joy_pins.P13_C, FF00_RDn);

  if (!FF00_RDn) bus_tri.set_data(
    /*p05.KEMA*/ joy_reg.JOYP_L0,
    /*p05.KURO*/ joy_reg.JOYP_L1,
    /*p05.KUVE*/ joy_reg.JOYP_L2,
    /*p05.JEKU*/ joy_reg.JOYP_L3,
    /*p05.KOCE*/ joy_reg.JOYP_UDLR,
    /*p05.CUDY*/ joy_reg.JOYP_ABCS,
    /*p05.KUKO*/ joy_reg.DBG_FF00_D6,
    /*p05.KERU*/ joy_reg.DBG_FF00_D7
  );

  //----------------------------------------
  // video config

  /*p01.WESY*/ wire WESY_RESET  = not(XORE_RESET);
  /*p01.WALU*/ wire WALU_RESET  = not(XORE_RESET);
  /*p01.XARE*/ wire XARE_RESET  = not(XORE_RESET);

  //----------

  /*p23.VYRE*/ wire FF40_RD = and(FF40, ASOT_CPURD);
  /*p23.WYCE*/ wire FF40_RDn = not(FF40_RD);
  /*p23.WARU*/ wire FF40_WR = and(FF40, CUPA_CPUWR);
  /*p23.XUBO*/ wire FF40_WRn = not(FF40_WR);

  if (!FF40_RDn) bus_tri.set_data(
    /*p23.WYPO*/ cfg_reg.LCDC_BGEN,
    /*p23.XERO*/ cfg_reg.LCDC_SPEN,
    /*p23.WYJU*/ cfg_reg.LCDC_SPSIZE,
    /*p23.WUKA*/ cfg_reg.LCDC_BGMAP,
    /*p23.VOKE*/ cfg_reg.LCDC_BGTILE,
    /*p23.VATO*/ cfg_reg.LCDC_WINEN,
    /*p23.VAHA*/ cfg_reg.LCDC_WINMAP,
    /*p23.XEBU*/ cfg_reg.LCDC_EN
  );

  /*p23.VYXE*/ cfg_reg.LCDC_BGEN    .set(FF40_WRn, XARE_RESET, bus_tri.D0());
  /*p23.XYLO*/ cfg_reg.LCDC_SPEN    .set(FF40_WRn, XARE_RESET, bus_tri.D1());
  /*p23.XYMO*/ cfg_reg.LCDC_SPSIZE  .set(FF40_WRn, XARE_RESET, bus_tri.D2());
  /*p23.XAFO*/ cfg_reg.LCDC_BGMAP   .set(FF40_WRn, XARE_RESET, bus_tri.D3());
  /*p23.WEXU*/ cfg_reg.LCDC_BGTILE  .set(FF40_WRn, XARE_RESET, bus_tri.D4());
  /*p23.WYMO*/ cfg_reg.LCDC_WINEN   .set(FF40_WRn, XARE_RESET, bus_tri.D5());
  /*p23.WOKY*/ cfg_reg.LCDC_WINMAP  .set(FF40_WRn, XARE_RESET, bus_tri.D6());
  /*p23.XONA*/ cfg_reg.LCDC_EN      .set(FF40_WRn, XARE_RESET, bus_tri.D7());

  //----------

  /*p23.ANYP*/ wire FF42_RD = and(FF42, ASOT_CPURD);
  /*p23.BUWY*/ wire FF42_RDn = not(FF42_RD);
  /*p23.BEDY*/ wire FF42_WR = and(FF42, CUPA_CPUWR);
  /*p23.CAVO*/ wire FF42_WRn = not(FF42_WR);

  if (!FF42_RDn) bus_tri.set_data(
    /*p23.WARE*/ cfg_reg.SCY0,
    /*p23.GOBA*/ cfg_reg.SCY1,
    /*p23.GONU*/ cfg_reg.SCY2,
    /*p23.GODO*/ cfg_reg.SCY3,
    /*p23.CUSA*/ cfg_reg.SCY4,
    /*p23.GYZO*/ cfg_reg.SCY5,
    /*p23.GUNE*/ cfg_reg.SCY6,
    /*p23.GYZA*/ cfg_reg.SCY7
  );

  /*p23.GAVE*/ cfg_reg.SCY0.set(FF42_WRn, CUNU_RESETn, bus_tri.D0());
  /*p23.FYMO*/ cfg_reg.SCY1.set(FF42_WRn, CUNU_RESETn, bus_tri.D1());
  /*p23.FEZU*/ cfg_reg.SCY2.set(FF42_WRn, CUNU_RESETn, bus_tri.D2());
  /*p23.FUJO*/ cfg_reg.SCY3.set(FF42_WRn, CUNU_RESETn, bus_tri.D3());
  /*p23.DEDE*/ cfg_reg.SCY4.set(FF42_WRn, CUNU_RESETn, bus_tri.D4());
  /*p23.FOTY*/ cfg_reg.SCY5.set(FF42_WRn, CUNU_RESETn, bus_tri.D5());
  /*p23.FOHA*/ cfg_reg.SCY6.set(FF42_WRn, CUNU_RESETn, bus_tri.D6());
  /*p23.FUNY*/ cfg_reg.SCY7.set(FF42_WRn, CUNU_RESETn, bus_tri.D7());

  //----------

  /*p23.AVOG*/ wire FF43_RD = and(FF43, ASOT_CPURD);
  /*p23.BEBA*/ wire FF43_RDn = not(FF43_RD);
  /*p23.ARUR*/ wire FF43_WR = and(FF43, CUPA_CPUWR);
  /*p23.AMUN*/ wire FF43_WRn = not(FF43_WR);

  if (!FF43_RDn) bus_tri.set_data(
    /*p23.EDOS*/ cfg_reg.SCX0,
    /*p23.EKOB*/ cfg_reg.SCX1,
    /*p23.CUGA*/ cfg_reg.SCX2,
    /*p23.WONY*/ cfg_reg.SCX3,
    /*p23.CEDU*/ cfg_reg.SCX4,
    /*p23.CATA*/ cfg_reg.SCX5,
    /*p23.DOXE*/ cfg_reg.SCX6,
    /*p23.CASY*/ cfg_reg.SCX7
  );

  /*p23.DATY*/ cfg_reg.SCX0.set(FF43_WRn, CUNU_RESETn, bus_tri.D0());
  /*p23.DUZU*/ cfg_reg.SCX1.set(FF43_WRn, CUNU_RESETn, bus_tri.D1());
  /*p23.CYXU*/ cfg_reg.SCX2.set(FF43_WRn, CUNU_RESETn, bus_tri.D2());
  /*p23.GUBO*/ cfg_reg.SCX3.set(FF43_WRn, CUNU_RESETn, bus_tri.D3());
  /*p23.BEMY*/ cfg_reg.SCX4.set(FF43_WRn, CUNU_RESETn, bus_tri.D4());
  /*p23.CUZY*/ cfg_reg.SCX5.set(FF43_WRn, CUNU_RESETn, bus_tri.D5());
  /*p23.CABU*/ cfg_reg.SCX6.set(FF43_WRn, CUNU_RESETn, bus_tri.D6());
  /*p23.BAKE*/ cfg_reg.SCX7.set(FF43_WRn, CUNU_RESETn, bus_tri.D7());

  //----------

  /*p23.XYLY*/ wire FF45_RD = and(ASOT_CPURD, FF45);
  /*p23.WEKU*/ wire FF45_RDn = not(FF45_RD);
  /*p23.XUFA*/ wire FF45_WR = and(CUPA_CPUWR, FF45);
  /*p23.WANE*/ wire FF45_WRn = not(FF45_WR);

  if (!FF45_RDn) bus_tri.set_data(
    /*p23.RETU*/ cfg_reg.LYC0,
    /*p23.VOJO*/ cfg_reg.LYC1,
    /*p23.RAZU*/ cfg_reg.LYC2,
    /*p23.REDY*/ cfg_reg.LYC3,
    /*p23.RACE*/ cfg_reg.LYC4,
    /*p23.VAZU*/ cfg_reg.LYC5,
    /*p23.VAFE*/ cfg_reg.LYC6,
    /*p23.PUFY*/ cfg_reg.LYC7
  );

  /*p23.SYRY*/ cfg_reg.LYC0.set(FF45_WRn, WESY_RESET, bus_tri.D0());
  /*p23.VUCE*/ cfg_reg.LYC1.set(FF45_WRn, WESY_RESET, bus_tri.D1());
  /*p23.SEDY*/ cfg_reg.LYC2.set(FF45_WRn, WESY_RESET, bus_tri.D2());
  /*p23.SALO*/ cfg_reg.LYC3.set(FF45_WRn, WESY_RESET, bus_tri.D3());
  /*p23.SOTA*/ cfg_reg.LYC4.set(FF45_WRn, WESY_RESET, bus_tri.D4());
  /*p23.VAFA*/ cfg_reg.LYC5.set(FF45_WRn, WESY_RESET, bus_tri.D5());
  /*p23.VEVO*/ cfg_reg.LYC6.set(FF45_WRn, WESY_RESET, bus_tri.D6());
  /*p23.RAHA*/ cfg_reg.LYC7.set(FF45_WRn, WESY_RESET, bus_tri.D7());

  //----------

  /*p36.VUSO*/ wire FF47_RD = and(ASOT_CPURD, FF47);
  /*p36.TEPY*/ wire FF47_RDn = not(FF47_RD);
  /*p36.VELY*/ wire FF47_WR = and(CUPA_CPUWR, FF47);
  /*p36.TEPO*/ wire FF47_WRn = not(FF47_WR);

  if (!FF47_RDn) bus_tri.set_data(
    /*p36.RARO*/ cfg_reg.BGP0,
    /*p36.PABA*/ cfg_reg.BGP1,
    /*p36.REDO*/ cfg_reg.BGP2,
    /*p36.LOBE*/ cfg_reg.BGP3,
    /*p36.LACE*/ cfg_reg.BGP4,
    /*p36.LYKA*/ cfg_reg.BGP5,
    /*p36.LODY*/ cfg_reg.BGP6,
    /*p36.LARY*/ cfg_reg.BGP7
  );

  /*p36.PAVO*/ cfg_reg.BGP0.set(FF47_WRn, 1, bus_tri.D0());
  /*p36.NUSY*/ cfg_reg.BGP1.set(FF47_WRn, 1, bus_tri.D1());
  /*p36.PYLU*/ cfg_reg.BGP2.set(FF47_WRn, 1, bus_tri.D2());
  /*p36.MAXY*/ cfg_reg.BGP3.set(FF47_WRn, 1, bus_tri.D3());
  /*p36.MUKE*/ cfg_reg.BGP4.set(FF47_WRn, 1, bus_tri.D4());
  /*p36.MORU*/ cfg_reg.BGP5.set(FF47_WRn, 1, bus_tri.D5());
  /*p36.MOGY*/ cfg_reg.BGP6.set(FF47_WRn, 1, bus_tri.D6());
  /*p36.MENA*/ cfg_reg.BGP7.set(FF47_WRn, 1, bus_tri.D7());

  //----------

  /*p36.XUFY*/ wire FF48_RD  = and(ASOT_CPURD, FF48);
  /*p36.XOZY*/ wire FF48_RDn = not(FF48_RD);
  /*p36.XOMA*/ wire FF48_WR  = and(CUPA_CPUWR, FF48);
  /*p36.XELO*/ wire FF48_WRn = not(FF48_WR);

  if (!FF48_RDn) bus_tri.set_data(
    /*p36.XARY*/ cfg_reg.OBP00,
    /*p36.XOKE*/ cfg_reg.OBP01,
    /*p36.XUNO*/ cfg_reg.OBP02,
    /*p36.XUBY*/ cfg_reg.OBP03,
    /*p36.XAJU*/ cfg_reg.OBP04,
    /*p36.XOBO*/ cfg_reg.OBP05,
    /*p36.XAXA*/ cfg_reg.OBP06,
    /*p36.XAWO*/ cfg_reg.OBP07
  );

  /*p36.XUFU*/ cfg_reg.OBP00.set(FF48_WRn, 1, bus_tri.D0());
  /*p36.XUKY*/ cfg_reg.OBP01.set(FF48_WRn, 1, bus_tri.D1());
  /*p36.XOVA*/ cfg_reg.OBP02.set(FF48_WRn, 1, bus_tri.D2());
  /*p36.XALO*/ cfg_reg.OBP03.set(FF48_WRn, 1, bus_tri.D3());
  /*p36.XERU*/ cfg_reg.OBP04.set(FF48_WRn, 1, bus_tri.D4());
  /*p36.XYZE*/ cfg_reg.OBP05.set(FF48_WRn, 1, bus_tri.D5());
  /*p36.XUPO*/ cfg_reg.OBP06.set(FF48_WRn, 1, bus_tri.D6());
  /*p36.XANA*/ cfg_reg.OBP07.set(FF48_WRn, 1, bus_tri.D7());

  //----------

  /*p36.MUMY*/ wire FF49_RD  = and(ASOT_CPURD, FF49);
  /*p36.LOTE*/ wire FF49_RDn = not(FF49_RD);
  /*p36.MYXE*/ wire FF49_WR  = and(CUPA_CPUWR, FF49);
  /*p36.LEHO*/ wire FF49_WRn = not(FF49_WR);

  if (!FF48_RDn) bus_tri.set_data(
    /*p36.LAJU*/ cfg_reg.OBP10,
    /*p36.LEPA*/ cfg_reg.OBP11,
    /*p36.LODE*/ cfg_reg.OBP12,
    /*p36.LYZA*/ cfg_reg.OBP13,
    /*p36.LUKY*/ cfg_reg.OBP14,
    /*p36.LUGA*/ cfg_reg.OBP15,
    /*p36.LEBA*/ cfg_reg.OBP16,
    /*p36.LELU*/ cfg_reg.OBP17
  );

  /*p36.MOXY*/ cfg_reg.OBP10.set(FF49_WRn, 1, bus_tri.D0());
  /*p36.LAWO*/ cfg_reg.OBP11.set(FF49_WRn, 1, bus_tri.D1());
  /*p36.MOSA*/ cfg_reg.OBP12.set(FF49_WRn, 1, bus_tri.D2());
  /*p36.LOSE*/ cfg_reg.OBP13.set(FF49_WRn, 1, bus_tri.D3());
  /*p36.LUNE*/ cfg_reg.OBP14.set(FF49_WRn, 1, bus_tri.D4());
  /*p36.LUGU*/ cfg_reg.OBP15.set(FF49_WRn, 1, bus_tri.D5());
  /*p36.LEPU*/ cfg_reg.OBP16.set(FF49_WRn, 1, bus_tri.D6());
  /*p36.LUXO*/ cfg_reg.OBP17.set(FF49_WRn, 1, bus_tri.D7());

  //----------

  /*p23.WAXU*/ wire FF4A_RD = and(ASOT_CPURD, FF4A);
  /*p23.VOMY*/ wire FF4A_RDn = not(FF4A_RD);
  /*p23.WEKO*/ wire FF4A_WR = and(CUPA_CPUWR, FF4A);
  /*p23.VEFU*/ wire FF4A_WRn = not(FF4A_WR);

  if (!FF4A_RDn) bus_tri.set_data(
    /*p23.PUNU*/ cfg_reg.WY0,
    /*p23.PODA*/ cfg_reg.WY1,
    /*p23.PYGU*/ cfg_reg.WY2,
    /*p23.LOKA*/ cfg_reg.WY3,
    /*p23.MEGA*/ cfg_reg.WY4,
    /*p23.PELA*/ cfg_reg.WY5,
    /*p23.POLO*/ cfg_reg.WY6,
    /*p23.MERA*/ cfg_reg.WY7
  );

  /*p23.NESO*/ cfg_reg.WY0.set(FF4A_WRn, WALU_RESET, bus_tri.D0());
  /*p23.NYRO*/ cfg_reg.WY1.set(FF4A_WRn, WALU_RESET, bus_tri.D1());
  /*p23.NAGA*/ cfg_reg.WY2.set(FF4A_WRn, WALU_RESET, bus_tri.D2());
  /*p23.MELA*/ cfg_reg.WY3.set(FF4A_WRn, WALU_RESET, bus_tri.D3());
  /*p23.NULO*/ cfg_reg.WY4.set(FF4A_WRn, WALU_RESET, bus_tri.D4());
  /*p23.NENE*/ cfg_reg.WY5.set(FF4A_WRn, WALU_RESET, bus_tri.D5());
  /*p23.NUKA*/ cfg_reg.WY6.set(FF4A_WRn, WALU_RESET, bus_tri.D6());
  /*p23.NAFU*/ cfg_reg.WY7.set(FF4A_WRn, WALU_RESET, bus_tri.D7());

  //----------

  /*p23.WYZE*/ wire FF4B_RD = and(ASOT_CPURD, FF4B);
  /*p23.VYCU*/ wire FF4B_RDn = not(FF4B_RD);
  /*p23.WUZA*/ wire FF4B_WR = and(CUPA_CPUWR, FF4B);
  /*p23.VOXU*/ wire FF4B_WRn = not(FF4B_WR);

  if (!FF4B_RDn) bus_tri.set_data(
    /*p23.LOVA*/ cfg_reg.WX0,
    /*p23.MUKA*/ cfg_reg.WX1,
    /*p23.MOKO*/ cfg_reg.WX2,
    /*p23.LOLE*/ cfg_reg.WX3,
    /*p23.MELE*/ cfg_reg.WX4,
    /*p23.MUFE*/ cfg_reg.WX5,
    /*p23.MULY*/ cfg_reg.WX6,
    /*p23.MARA*/ cfg_reg.WX7
  );

  /*p23.MYPA*/ cfg_reg.WX0.set(FF4B_WRn, WALU_RESET, bus_tri.D0());
  /*p23.NOFE*/ cfg_reg.WX1.set(FF4B_WRn, WALU_RESET, bus_tri.D1());
  /*p23.NOKE*/ cfg_reg.WX2.set(FF4B_WRn, WALU_RESET, bus_tri.D2());
  /*p23.MEBY*/ cfg_reg.WX3.set(FF4B_WRn, WALU_RESET, bus_tri.D3());
  /*p23.MYPU*/ cfg_reg.WX4.set(FF4B_WRn, WALU_RESET, bus_tri.D4());
  /*p23.MYCE*/ cfg_reg.WX5.set(FF4B_WRn, WALU_RESET, bus_tri.D5()); 
  /*p23.MUVO*/ cfg_reg.WX6.set(FF4B_WRn, WALU_RESET, bus_tri.D6());
  /*p23.NUKU*/ cfg_reg.WX7.set(FF4B_WRn, WALU_RESET, bus_tri.D7());

  //----------------------------------------
  // sch_bootrom

  /*p07.TULO*/ wire ADDR_00XX  = nor(bus_tri.A15(), bus_tri.A14(), bus_tri.A13(), bus_tri.A12(), bus_tri.A11(), bus_tri.A10(), bus_tri.A09(), bus_tri.A08());
  /*p07.TERA*/ wire BOOT_BITn  = not(bootrom.BOOT_BIT);
  /*p07.TUTU*/ wire ADDR_BOOT  = and(BOOT_BITn, ADDR_00XX);

  /*p07.ZORO*/ wire ADDR_0XXX = nor(bus_tri.A15(), bus_tri.A14(), bus_tri.A13(), bus_tri.A12());
  /*p07.ZADU*/ wire ADDR_X0XX = nor(bus_tri.A11(), bus_tri.A10(), bus_tri.A09(), bus_tri.A08());
  /*p07.ZUFA*/ wire ADDR_00XX2 = and(ADDR_0XXX, ADDR_X0XX);
  /*p07.YAZA*/ wire MODE_DBG1n = not(UMUT_MODE_DBG1);
  /*p07.YULA*/ wire BOOT_RD    = and(TEDO_CPURD, MODE_DBG1n, ADDR_BOOT);
  /*p07.ZADO*/ wire BOOT_CSn   = nand(BOOT_RD, ADDR_00XX2);
  /*p07.ZERY*/ wire BOOT_CS    = not(BOOT_CSn);

  /*p07.ZYBA*/ wire ADDR_00n = not(bus_tri.A00());
  /*p07.ZUVY*/ wire ADDR_01n = not(bus_tri.A01());
  /*p07.ZUFY*/ wire ADDR_04n = not(bus_tri.A04());
  /*p07.ZERA*/ wire ADDR_05n = not(bus_tri.A05());
  /*p07.ZOLE*/ wire ADDR_00  = and(ADDR_01n, ADDR_00n);
  /*p07.ZAJE*/ wire ADDR_01  = and(ADDR_01n, bus_tri.A00());
  /*p07.ZUBU*/ wire ADDR_10  = and(bus_tri.A01(), ADDR_00n);
  /*p07.ZAPY*/ wire ADDR_11  = and(bus_tri.A01(), bus_tri.A00());

  /*p07.ZETE*/ wire BOOTROM_A1nA0n = not(ADDR_00);
  /*p07.ZEFU*/ wire BOOTROM_A1nA0  = not(ADDR_01);
  /*p07.ZYRO*/ wire BOOTROM_A1A0n  = not(ADDR_10);
  /*p07.ZAPA*/ wire BOOTROM_A1A0   = not(ADDR_11);
  /*p07.ZYGA*/ wire BOOTROM_A2n    = not(bus_tri.A02());
  /*p07.ZYKY*/ wire BOOTROM_A3n    = not(bus_tri.A03());
  /*p07.ZYKY*/ wire BOOTROM_A5nA4n = and(ADDR_05n, ADDR_04n);
  /*p07.ZYGA*/ wire BOOTROM_A5nA4  = and(ADDR_05n, bus_tri.A04());
  /*p07.ZOVY*/ wire BOOTROM_A5A4n  = and(bus_tri.A05(), ADDR_04n);
  /*p07.ZUKO*/ wire BOOTROM_A5A4   = and(bus_tri.A05(), bus_tri.A04());
  /*p07.ZAGE*/ wire BOOTROM_A6n    = not(bus_tri.A06());
  /*p07.ZYRA*/ wire BOOTROM_A7n    = not(bus_tri.A07());

  /*p07.TYRO*/ wire ADDR_0x0x0000 = nor(bus_tri.A07(), bus_tri.A05(), bus_tri.A03(), bus_tri.A02(), bus_tri.A01(), bus_tri.A00());
  /*p07.TUFA*/ wire ADDR_x1x1xxxx = and(bus_tri.A04(), bus_tri.A06());
  /*p07.TEXE*/ wire FF50_RD  = and (TEDO_CPURD, ADDR_FFXX, ADDR_0x0x0000, ADDR_x1x1xxxx);
  /*p07.TUGE*/ wire FF50_WRn = nand(TAPU_CPUWR, ADDR_FFXX, ADDR_0x0x0000, ADDR_x1x1xxxx);
  /*p07.SATO*/ wire BOOT_BIT_IN  = or(bus_tri.D0(), bootrom.BOOT_BIT);
  
  /*p07.TEPU*/ bootrom.BOOT_BIT.set(FF50_WRn, SYS_RESETn, BOOT_BIT_IN);

  /*p07.SYPU*/ if (FF50_RD) bus_tri.set_data(bootrom.BOOT_BIT);

  // how does this not collide with cart reads?
  if (BOOT_CS && BOOT_RD) {
    uint16_t addr = (uint16_t)bus_tri.get_addr();
    bus_tri.set_data(DMG_ROM_bin[addr & 0xFF]);
  }

  //----------------------------------------
  // sch_sprites

  /*p28.ANOM*/ wire SCAN_RSTn = nor(ATEJ_VID_LINE_TRIG_d4, VID_RESET6);
  /*p29.BALU*/ wire SCAN_RSTa = not(SCAN_RSTn);
  /*p29.BAGY*/ wire SCAN_RSTo = not(SCAN_RSTa);

  /*p28.FETO*/ wire SCAN_DONE_d0 = and(spr_reg.SCAN0, spr_reg.SCAN1, spr_reg.SCAN2, spr_reg.SCAN5); // 32 + 4 + 2 + 1 = 39
  /*p28.GAVA*/ wire SCAN_CLK = or(SCAN_DONE_d0, XUPY_xBCxxFGx);
  /*p28.YFEL*/ spr_reg.SCAN0.set(SCAN_CLK, SCAN_RSTn, !spr_reg.SCAN0);
  /*p28.WEWY*/ spr_reg.SCAN1.set(!spr_reg.SCAN0,   SCAN_RSTn, !spr_reg.SCAN1);
  /*p28.GOSO*/ spr_reg.SCAN2.set(!spr_reg.SCAN1,   SCAN_RSTn, !spr_reg.SCAN2);
  /*p28.ELYN*/ spr_reg.SCAN3.set(!spr_reg.SCAN2,   SCAN_RSTn, !spr_reg.SCAN3);
  /*p28.FAHA*/ spr_reg.SCAN4.set(!spr_reg.SCAN3,   SCAN_RSTn, !spr_reg.SCAN4);
  /*p28.FONY*/ spr_reg.SCAN5.set(!spr_reg.SCAN4,   SCAN_RSTn, !spr_reg.SCAN5);

  // the second clock here doesn't match the usual synchronizer pattern, but it matches the die.
  /*p29.BYBA*/ spr_reg.SCAN_DONE_d4.set(XUPY_xBCxxFGx, SCAN_RSTo, SCAN_DONE_d0);
  /*p29.DOBA*/ spr_reg.SCAN_DONE_d5.set(ALET_AxCxExGx, SCAN_RSTo, spr_reg.SCAN_DONE_d4);

  // which means this trigger is exactly 1 phase long
  /*p29.BEBU*/ wire SCAN_DONE_d0_TRIGn = or(SCAN_RSTa, spr_reg.SCAN_DONE_d5, !spr_reg.SCAN_DONE_d4);
  /*p29.AVAP*/ wire AVAP_SCAN_DONE_d0_TRIG = not(SCAN_DONE_d0_TRIGn);

  /*p01.ROSY*/ wire VID_RESET5 = not(VID_RESETn);

  /*p28.ASEN*/ wire ASEN = or(VID_RESET6, AVAP_SCAN_DONE_d0_TRIG);
  /*p28.BESU*/ wire BESU = or(lcd_reg.VID_LINE_d4, ASEN);


  /*p28.BOGE*/ wire DMA_RW_ENn = not(dma_reg.REG_DMA_RW_EN);
  /*p28.ACYL*/ wire ACYL_OAM_ADDR_PARSE = and(DMA_RW_ENn, BESU);

  /*p29.CEHA*/ wire CEHA = not(!spr_reg.STORE_SPRITE_IDXn);
  /*p29.BYJO*/ wire BYJO = not(CEHA);
  /*p29.AZEM*/ wire AZEM = and(BYJO, vid_reg.XYMU_RENDERING_LATCH);
  /*p29.AROR*/ wire MATCH_EN = and(AZEM, cfg_reg.LCDC_SPEN);


  /*p29.TYTU*/ wire SPR_SEQ0n = not(spr_reg.TOXE_SPR_SEQ0);
  /*p24.LOBY*/ wire LOBY_RENDERINGn = not(vid_reg.XYMU_RENDERING_LATCH);
  /*p29.SYCU*/ wire SYCU = nor(SPR_SEQ0n, LOBY_RENDERINGn, spr_reg.TYFO_SEQ_B0d);

  /*p29.TOPU*/ wire SPRITE_PIX_LATCH_Ao = and(spr_reg.TULY_SPR_SEQ1, SYCU);
  /*p29.VYWA*/ wire SPRITE_PIX_LATCH_Ab = not(SPRITE_PIX_LATCH_Ao);
  /*p29.WENY*/ wire SPRITE_PIX_LATCH_An = not(SPRITE_PIX_LATCH_Ab);
  /*p29.XADO*/ wire SPRITE_PIX_LATCH_A  = not(SPRITE_PIX_LATCH_An);

  /*p29.RACA*/ wire SPRITE_PIX_LATCH_Bo = and(spr_reg.VONU_SEQ_xxx34xn, SYCU);
  /*p29.PEBY*/ wire SPRITE_PIX_LATCH_Bb = not(SPRITE_PIX_LATCH_Bo);
  /*p29.NYBE*/ wire SPRITE_PIX_LATCH_Bn = not(SPRITE_PIX_LATCH_Bb);
  /*p29.PUCO*/ wire SPRITE_PIX_LATCH_B  = not(SPRITE_PIX_LATCH_Bn);

  /*p29.TYNO*/ wire TYNO = nand(spr_reg.TOXE_SPR_SEQ0, spr_reg.SEBA_SEQ_xxxx45n, spr_reg.VONU_SEQ_xxx34xn);
  /*p29.VUSA*/ wire VUSA = or(!spr_reg.TYFO_SEQ_B0d, TYNO);
  /*p29.WUTY*/ wire WUTY_SPRITE_DONE = not(VUSA);

  /*p29.SAKY*/ wire SAKY = nor(spr_reg.TULY_SPR_SEQ1, spr_reg.VONU_SEQ_xxx34xn);
  /*p29.TEPA*/ wire RENDERINGn = not(vid_reg.XYMU_RENDERING_LATCH);
  /*p29.TYSO*/ wire SPRITE_READq = or(SAKY, RENDERINGn); // seems wrong
  /*p29.TEXY*/ wire SPRITE_READ = not(SPRITE_READq);
    
  //----------------------------------------
  // Sprite y comparator

  /*p29.EBOS*/ wire V0n = not(lcd_reg.Y0);
  /*p29.DASA*/ wire V1n = not(lcd_reg.Y1);
  /*p29.FUKY*/ wire V2n = not(lcd_reg.Y2);
  /*p29.FUVE*/ wire V3n = not(lcd_reg.Y3);
  /*p29.FEPU*/ wire V4n = not(lcd_reg.Y4);
  /*p29.FOFA*/ wire V5n = not(lcd_reg.Y5);
  /*p29.FEMO*/ wire V6n = not(lcd_reg.Y6);
  /*p29.GUSU*/ wire V7n = not(lcd_reg.Y7);

  /*p29.ERUC*/ wire YDIFF0   = add_c(V0n, !oam_reg.REG_OAM_B0, joy_pins.P10_B);
  /*p29.ERUC*/ wire YDIFF_C0 = add_s(V0n, !oam_reg.REG_OAM_B0, joy_pins.P10_B);
  /*p29.ENEF*/ wire YDIFF1   = add_s(V1n, !oam_reg.REG_OAM_B1, YDIFF_C0);
  /*p29.ENEF*/ wire YDIFF_C1 = add_c(V1n, !oam_reg.REG_OAM_B1, YDIFF_C0);
  /*p29.FECO*/ wire YDIFF2   = add_s(V2n, !oam_reg.REG_OAM_B2, YDIFF_C1);
  /*p29.FECO*/ wire YDIFF_C2 = add_c(V2n, !oam_reg.REG_OAM_B2, YDIFF_C1);
  /*p29.GYKY*/ wire YDIFF3   = add_s(V3n, !oam_reg.REG_OAM_B3, YDIFF_C2);
  /*p29.GYKY*/ wire YDIFF_C3 = add_c(V3n, !oam_reg.REG_OAM_B3, YDIFF_C2);
  /*p29.GOPU*/ wire YDIFF4   = add_s(V4n, !oam_reg.REG_OAM_B4, YDIFF_C3);
  /*p29.GOPU*/ wire YDIFF_C4 = add_c(V4n, !oam_reg.REG_OAM_B4, YDIFF_C3);
  /*p29.FUWA*/ wire YDIFF5   = add_s(V5n, !oam_reg.REG_OAM_B5, YDIFF_C4);
  /*p29.FUWA*/ wire YDIFF_C5 = add_c(V5n, !oam_reg.REG_OAM_B5, YDIFF_C4);
  /*p29.GOJU*/ wire YDIFF6   = add_s(V6n, !oam_reg.REG_OAM_B6, YDIFF_C5);
  /*p29.GOJU*/ wire YDIFF_C6 = add_c(V6n, !oam_reg.REG_OAM_B6, YDIFF_C5);
  /*p29.WUHU*/ wire YDIFF7   = add_s(V7n, !oam_reg.REG_OAM_B7, YDIFF_C6);
  /*p29.WUHU*/ wire YDIFF_C7 = add_c(V7n, !oam_reg.REG_OAM_B7, YDIFF_C6);

  /*p29.DEGE*/ wire SPRITE_DELTA0 = not(YDIFF0);
  /*p29.DABY*/ wire SPRITE_DELTA1 = not(YDIFF1);
  /*p29.DABU*/ wire SPRITE_DELTA2 = not(YDIFF2);
  /*p29.GYSA*/ wire SPRITE_DELTA3 = not(YDIFF3);
  /*p29.GACE*/ wire SPRITE_DELTA4 = not(YDIFF4);
  /*p29.GUVU*/ wire SPRITE_DELTA5 = not(YDIFF5);
  /*p29.GYDA*/ wire SPRITE_DELTA6 = not(YDIFF6);
  /*p29.GEWY*/ wire SPRITE_DELTA7 = not(YDIFF7);

  /*p29.GOVU*/ wire GOVU = or(YDIFF3, cfg_reg.LCDC_SPSIZE);
  /*p29.WOTA*/ wire SPR_MATCH_Yn = nand(SPRITE_DELTA4, SPRITE_DELTA5, SPRITE_DELTA6, SPRITE_DELTA7, YDIFF_C7, GOVU);
  /*p29.GESE*/ wire SPR_MATCH_Y = not(SPR_MATCH_Yn);


  /*p29.CARE*/ wire STORE_ENn = or(XOCE_ABxxEFxx, CEHA, SPR_MATCH_Y);
  /*p29.DYTY*/ wire STORE_EN = not(STORE_ENn);

  /*p28.WEFE*/ wire WEFE = not(joy_pins.P10_B);
  /*p30.CYKE*/ wire CYKE = not(XUPY_xBCxxFGx);
  /*p30.WUDA*/ wire WUDA = not(CYKE);
  /*p30.XADU*/ spr_reg.SPRITE_IDX0.set(WUDA, WEFE, oam_pins.A2);
  /*p30.XEDY*/ spr_reg.SPRITE_IDX1.set(WUDA, WEFE, oam_pins.A3);
  /*p30.ZUZE*/ spr_reg.SPRITE_IDX2.set(WUDA, WEFE, oam_pins.A4);
  /*p30.XOBE*/ spr_reg.SPRITE_IDX3.set(WUDA, WEFE, oam_pins.A5);
  /*p30.YDUF*/ spr_reg.SPRITE_IDX4.set(WUDA, WEFE, oam_pins.A6);
  /*p30.XECU*/ spr_reg.SPRITE_IDX5.set(WUDA, WEFE, oam_pins.A7);

  /*p29.CENO*/ spr_reg.STORE_SPRITE_IDXn.set(XUPY_xBCxxFGx, VID_RESETn3, BESU);
  /*p29.BUZA*/ wire STORE_SPRITE_IDX = and(!spr_reg.STORE_SPRITE_IDXn, vid_reg.XYMU_RENDERING_LATCH);

#if 0
  /*p30.WEZA*/ if (BUZA_STORE_SPRITE_IDX) sil_tri.TS_IDX_0  = spr_reg.SPRITE_IDX5; // order wrong
  /*p30.WUCO*/ if (BUZA_STORE_SPRITE_IDX) sil_tri.TS_IDX_1  = spr_reg.SPRITE_IDX4;
  /*p30.WYDA*/ if (BUZA_STORE_SPRITE_IDX) sil_tri.TS_IDX_2  = spr_reg.SPRITE_IDX3;
  /*p30.ZYSU*/ if (BUZA_STORE_SPRITE_IDX) sil_tri.TS_IDX_3  = spr_reg.SPRITE_IDX2;
  /*p30.WYSE*/ if (BUZA_STORE_SPRITE_IDX) sil_tri.TS_IDX_4  = spr_reg.SPRITE_IDX1;
  /*p30.WUZY*/ if (BUZA_STORE_SPRITE_IDX) sil_tri.TS_IDX_5  = spr_reg.SPRITE_IDX0;
#endif

  /*p01.LAPE*/ wire LAPE_xBxDxFxH = not(ALET_AxCxExGx);
  /*p27.TAVA*/ wire TAVA_AxCxExGx = not(LAPE_xBxDxFxH);

  /*p29.TAME*/ wire SEQ_5n = nand(spr_reg.TESE_SPR_SEQ2, spr_reg.TOXE_SPR_SEQ0);

  ///*p29.TOMA*/ SEQ_CLK = or(AxCxExGx, SEQ_5);

  /*p29.TOMA*/ wire TOMA_AxCxExGx = nand(LAPE_xBxDxFxH, SEQ_5n);
#if 0
  /*p27.SECA*/ wire SPR_SEQ_RST = nor(RYCE_SPRITE_FETCH_TRIG, VID_RESET5, BYHA_VID_LINE_TRIG_d4n);
  /*p29.TOXE*/ spr_reg.TOXE_SPR_SEQ0.set(TOMA_AxCxExGx,          SPR_SEQ_RST, !spr_reg.TOXE_SPR_SEQ0);
  /*p29.TULY*/ spr_reg.TULY_SPR_SEQ1.set(!spr_reg.TOXE_SPR_SEQ0, SPR_SEQ_RST, !spr_reg.TULY_SPR_SEQ1);
  /*p29.TESE*/ spr_reg.TESE_SPR_SEQ2.set(!spr_reg.TULY_SPR_SEQ1, SPR_SEQ_RST, !spr_reg.TESE_SPR_SEQ2);
#endif

  /*p29.TYFO*/ spr_reg.TYFO_SEQ_B0d    .set(LAPE_xBxDxFxH, VYPO_P10Bn,                   spr_reg.TOXE_SPR_SEQ0);
  /*p29.TOBU*/ spr_reg.TOBU_SEQ_xx23xx .set(TAVA_AxCxExGx, vid_reg.XYMU_RENDERING_LATCH, spr_reg.TULY_SPR_SEQ1);    // note input is seq 1 not 2
  /*p29.VONU*/ spr_reg.VONU_SEQ_xxx34xn.set(TAVA_AxCxExGx, vid_reg.XYMU_RENDERING_LATCH, spr_reg.TOBU_SEQ_xx23xx);
  /*p29.SEBA*/ spr_reg.SEBA_SEQ_xxxx45n.set(LAPE_xBxDxFxH, vid_reg.XYMU_RENDERING_LATCH, spr_reg.VONU_SEQ_xxx34xn); // is this clock wrong?

  //----------------------------------------
  // sprite store

  /*p21.ACAM*/ wire X0n = not(vid_reg.X0);
  /*p21.AZUB*/ wire X1n = not(vid_reg.X1);
  /*p21.AMEL*/ wire X2n = not(vid_reg.X2);
  /*p21.AHAL*/ wire X3n = not(vid_reg.X3);
  /*p21.APUX*/ wire X4n = not(vid_reg.X4);
  /*p21.ABEF*/ wire X5n = not(vid_reg.X5);
  /*p21.ADAZ*/ wire X6n = not(vid_reg.X6);
  /*p21.ASAH*/ wire X7n = not(vid_reg.X7);

  /*p31.ZAGO*/ wire SPRITE_X0 = not(!oam_reg.REG_OAM_A0);
  /*p31.ZOCY*/ wire SPRITE_X1 = not(!oam_reg.REG_OAM_A1);
  /*p31.YPUR*/ wire SPRITE_X2 = not(!oam_reg.REG_OAM_A2);
  /*p31.YVOK*/ wire SPRITE_X3 = not(!oam_reg.REG_OAM_A3);
  /*p31.COSE*/ wire SPRITE_X4 = not(!oam_reg.REG_OAM_A4);
  /*p31.AROP*/ wire SPRITE_X5 = not(!oam_reg.REG_OAM_A5);
  /*p31.XATU*/ wire SPRITE_X6 = not(!oam_reg.REG_OAM_A6);
  /*p31.BADY*/ wire SPRITE_X7 = not(!oam_reg.REG_OAM_A7);

  /*p29.EDEN*/ wire SPRITE_COUNT0n = not(sst_reg.SPRITE_COUNT0);
  /*p29.CYPY*/ wire SPRITE_COUNT1n = not(sst_reg.SPRITE_COUNT1);
  /*p29.CAPE*/ wire SPRITE_COUNT2n = not(sst_reg.SPRITE_COUNT2);
  /*p29.CAXU*/ wire SPRITE_COUNT3n = not(sst_reg.SPRITE_COUNT3);

  /*p29.FYCU*/ wire SPRITE_COUNT0b = not(SPRITE_COUNT0n);
  /*p29.FONE*/ wire SPRITE_COUNT1b = not(SPRITE_COUNT1n);
  /*p29.EKUD*/ wire SPRITE_COUNT2b = not(SPRITE_COUNT2n);
  /*p29.ELYG*/ wire SPRITE_COUNT3b = not(SPRITE_COUNT3n);

  /*p29.DEZO*/ wire STORE0_SEL = nand(SPRITE_COUNT0n, SPRITE_COUNT1n, SPRITE_COUNT2n, SPRITE_COUNT3n);
  /*p29.CUVA*/ wire STORE1_SEL = nand(SPRITE_COUNT0b, SPRITE_COUNT1n, SPRITE_COUNT2n, SPRITE_COUNT3n);
  /*p29.GEBU*/ wire STORE2_SEL = nand(SPRITE_COUNT0n, SPRITE_COUNT1b, SPRITE_COUNT2n, SPRITE_COUNT3n);
  /*p29.FOCO*/ wire STORE3_SEL = nand(SPRITE_COUNT0b, SPRITE_COUNT1b, SPRITE_COUNT2n, SPRITE_COUNT3n);
  /*p29.CUPE*/ wire STORE4_SEL = nand(SPRITE_COUNT0n, SPRITE_COUNT1n, SPRITE_COUNT2b, SPRITE_COUNT3n);
  /*p29.CUGU*/ wire STORE5_SEL = nand(SPRITE_COUNT0b, SPRITE_COUNT1n, SPRITE_COUNT2b, SPRITE_COUNT3n);
  /*p29.WOMU*/ wire STORE6_SEL = nand(SPRITE_COUNT0n, SPRITE_COUNT1b, SPRITE_COUNT2b, SPRITE_COUNT3n);
  /*p29.GUNA*/ wire STORE7_SEL = nand(SPRITE_COUNT0b, SPRITE_COUNT1b, SPRITE_COUNT2b, SPRITE_COUNT3n);
  /*p29.DEWY*/ wire STORE8_SEL = nand(SPRITE_COUNT0n, SPRITE_COUNT1n, SPRITE_COUNT2n, SPRITE_COUNT3b);
  /*p29.DOGU*/ wire STORE9_SEL = nand(SPRITE_COUNT0b, SPRITE_COUNT1n, SPRITE_COUNT2n, SPRITE_COUNT3b);

  //----------------------------------------
  // sprite store 0

  /*p31.ZOGY*/ wire STORE0_MATCH0n = xor(sst_reg.STORE0_X0, X0n);
  /*p31.ZEBA*/ wire STORE0_MATCH1n = xor(sst_reg.STORE0_X1, X1n);
  /*p31.ZAHA*/ wire STORE0_MATCH2n = xor(sst_reg.STORE0_X2, X2n);
  /*p31.ZOKY*/ wire STORE0_MATCH3n = xor(sst_reg.STORE0_X3, X3n);
  /*p31.WOJU*/ wire STORE0_MATCH4n = xor(sst_reg.STORE0_X4, X4n);
  /*p31.YFUN*/ wire STORE0_MATCH5n = xor(sst_reg.STORE0_X5, X5n);
  /*p31.WYZA*/ wire STORE0_MATCH6n = xor(sst_reg.STORE0_X6, X6n);
  /*p31.YPUK*/ wire STORE0_MATCH7n = xor(sst_reg.STORE0_X7, X7n);
  /*p31.ZAKO*/ wire STORE0_MATCHA = nor(STORE0_MATCH0n, STORE0_MATCH1n, STORE0_MATCH2n, STORE0_MATCH3n);
  /*p31.XEBA*/ wire STORE0_MATCHB = nor(STORE0_MATCH4n, STORE0_MATCH5n, STORE0_MATCH6n, STORE0_MATCH7n);

  /*p29.YDUG*/ wire STORE0_MATCHn = nand(MATCH_EN, STORE0_MATCHB, STORE0_MATCHA);
  /*p29.WEFU*/ wire STORE0_MATCH = not(STORE0_MATCHn);
  /*p29.GEZE*/ wire STORE0_MATCH_OUT = or (STORE0_MATCH, joy_pins.P10_B);

  /*p29.GUVA*/ wire SPRITE0_GET = nor(STORE0_MATCHn,  joy_pins.P10_B);
  /*p29.EBOJ*/ sst_reg.SPRITE0_GET_SYNCn.set(WUTY_SPRITE_DONE, BYVA_VID_LINE_TRIG_d4n, SPRITE0_GET);

  /*p29.CEMY*/ wire STORE0_CLK    = or(STORE_EN, STORE0_SEL);
  /*p29.DYHU*/ wire STORE0_CLKn   = not(STORE0_CLK);
  /*p29.DYWE*/ wire STORE0_RST    = or(DYBA_VID_LINE_TRIG_d4, sst_reg.SPRITE0_GET_SYNCn);
  /*p29.DYNA*/ wire STORE0_RSTn   = not(STORE0_RST);

  /*p29.GENY*/ wire STORE0_CLKa  = not(STORE0_CLKn);
  /*p30.YGUS*/ sst_reg.STORE0_IDX0.set(STORE0_CLK, sil_tri.TS_IDX_0);
  /*p30.YSOK*/ sst_reg.STORE0_IDX1.set(STORE0_CLK, sil_tri.TS_IDX_1);
  /*p30.YZEP*/ sst_reg.STORE0_IDX2.set(STORE0_CLK, sil_tri.TS_IDX_2);
  /*p30.WYTE*/ sst_reg.STORE0_IDX3.set(STORE0_CLK, sil_tri.TS_IDX_3);
  /*p30.ZONY*/ sst_reg.STORE0_IDX4.set(STORE0_CLK, sil_tri.TS_IDX_4);
  /*p30.YWAK*/ sst_reg.STORE0_IDX5.set(STORE0_CLK, sil_tri.TS_IDX_5);
    
  /*p29.ENOB*/ wire STORE0_CLKb  = not(STORE0_CLKn);
  /*p30.FYHY*/ sst_reg.STORE0_LINE0.set(STORE0_CLKb, sil_tri.TS_LINE_0);
  /*p30.GYHO*/ sst_reg.STORE0_LINE1.set(STORE0_CLKb, sil_tri.TS_LINE_1);
  /*p30.BOZU*/ sst_reg.STORE0_LINE2.set(STORE0_CLKb, sil_tri.TS_LINE_2);
  /*p30.CUFO*/ sst_reg.STORE0_LINE3.set(STORE0_CLKb, sil_tri.TS_LINE_3);

  /*p29.FUXU*/ wire STORE0_CLKc  = not(STORE0_CLKn);
  /*p31.XEPE*/ sst_reg.STORE0_X0.set(STORE0_CLKc, STORE0_RSTn, SPRITE_X0);
  /*p31.YLAH*/ sst_reg.STORE0_X1.set(STORE0_CLKc, STORE0_RSTn, SPRITE_X1);
  /*p31.ZOLA*/ sst_reg.STORE0_X2.set(STORE0_CLKc, STORE0_RSTn, SPRITE_X2);
  /*p31.ZULU*/ sst_reg.STORE0_X3.set(STORE0_CLKc, STORE0_RSTn, SPRITE_X3);
  /*p31.WELO*/ sst_reg.STORE0_X4.set(STORE0_CLKc, STORE0_RSTn, SPRITE_X4);
  /*p31.XUNY*/ sst_reg.STORE0_X5.set(STORE0_CLKc, STORE0_RSTn, SPRITE_X5);
  /*p31.WOTE*/ sst_reg.STORE0_X6.set(STORE0_CLKc, STORE0_RSTn, SPRITE_X6);
  /*p31.XAKO*/ sst_reg.STORE0_X7.set(STORE0_CLKc, STORE0_RSTn, SPRITE_X7);

  /*p29.FURO*/ wire SPRITE0_GETn = not(SPRITE0_GET);
  /*p30.ZETU*/ if (!SPRITE0_GETn) sil_tri.TS_IDX_0  = sst_reg.STORE0_IDX0;
  /*p30.ZECE*/ if (!SPRITE0_GETn) sil_tri.TS_IDX_1  = sst_reg.STORE0_IDX1;
  /*p30.ZAVE*/ if (!SPRITE0_GETn) sil_tri.TS_IDX_2  = sst_reg.STORE0_IDX2;
  /*p30.WOKO*/ if (!SPRITE0_GETn) sil_tri.TS_IDX_3  = sst_reg.STORE0_IDX3;
  /*p30.ZUMU*/ if (!SPRITE0_GETn) sil_tri.TS_IDX_4  = sst_reg.STORE0_IDX4;
  /*p30.ZEDY*/ if (!SPRITE0_GETn) sil_tri.TS_IDX_5  = sst_reg.STORE0_IDX5;
  /*p30.GOFO*/ if (!SPRITE0_GETn) sil_tri.TS_LINE_0 = sst_reg.STORE0_LINE0;
  /*p30.WEHE*/ if (!SPRITE0_GETn) sil_tri.TS_LINE_1 = sst_reg.STORE0_LINE1;
  /*p30.AJAL*/ if (!SPRITE0_GETn) sil_tri.TS_LINE_2 = sst_reg.STORE0_LINE2;
  /*p30.BUKY*/ if (!SPRITE0_GETn) sil_tri.TS_LINE_3 = sst_reg.STORE0_LINE3;

  //----------------------------------------
  // sprite store 1

  /*p31.EDYM*/ wire STORE1_MATCH0 = xor(sst_reg.STORE1_X0, X0n);
  /*p31.EMYB*/ wire STORE1_MATCH1 = xor(sst_reg.STORE1_X1, X1n);
  /*p31.EBEF*/ wire STORE1_MATCH2 = xor(sst_reg.STORE1_X2, X2n);
  /*p31.EWOK*/ wire STORE1_MATCH3 = xor(sst_reg.STORE1_X3, X3n);
  /*p31.COLA*/ wire STORE1_MATCH4 = xor(sst_reg.STORE1_X4, X4n);
  /*p31.BOBA*/ wire STORE1_MATCH5 = xor(sst_reg.STORE1_X5, X5n);
  /*p31.COLU*/ wire STORE1_MATCH6 = xor(sst_reg.STORE1_X6, X6n);
  /*p31.BAHU*/ wire STORE1_MATCH7 = xor(sst_reg.STORE1_X7, X7n);
  /*p31.CYVY*/ wire STORE1_MATCHB = nor(STORE1_MATCH4, STORE1_MATCH5, STORE1_MATCH6, STORE1_MATCH7);
  /*p31.EWAM*/ wire STORE1_MATCHA = nor(STORE1_MATCH0, STORE1_MATCH1, STORE1_MATCH2, STORE1_MATCH3);

  /*p29.DYDU*/ wire STORE1_MATCHn = nand(MATCH_EN, STORE1_MATCHA, STORE1_MATCHB);
  /*p29.GAJA*/ wire STORE1_MATCH = not(STORE1_MATCHn);
  /*p29.FUMA*/ wire STORE1_MATCH_OUT = or(STORE1_MATCH, STORE0_MATCH_OUT);

  /*p29.ENUT*/ wire SPRITE1_GETn = nor(STORE1_MATCHn, STORE0_MATCH_OUT);

  /*p29.BYBY*/ wire STORE1_CLK  = or(STORE_EN, STORE1_SEL);
  /*p29.BUCO*/ wire STORE1_CLKn = not(STORE1_CLK);

  /*p29.CEDY*/ sst_reg.SPRITE1_GET_SYNCn.set(WUTY_SPRITE_DONE, BYVA_VID_LINE_TRIG_d4n, SPRITE1_GETn);
  /*p29.EFEV*/ wire STORE1_RST = or(DYBA_VID_LINE_TRIG_d4, sst_reg.SPRITE1_GET_SYNCn);
  /*p29.DOKU*/ wire STORE1_RSTn = not(STORE1_RST);

  /*p29.BYVY*/ wire STORE1_CLKc = not(STORE1_CLKn);
  /*p30.CAJY*/ sst_reg.STORE1_IDX0.set(STORE1_CLKc, sil_tri.TS_IDX_0);
  /*p30.CUZA*/ sst_reg.STORE1_IDX1.set(STORE1_CLKc, sil_tri.TS_IDX_1);
  /*p30.COMA*/ sst_reg.STORE1_IDX2.set(STORE1_CLKc, sil_tri.TS_IDX_2);
  /*p30.CUFA*/ sst_reg.STORE1_IDX3.set(STORE1_CLKc, sil_tri.TS_IDX_3);
  /*p30.CEBO*/ sst_reg.STORE1_IDX4.set(STORE1_CLKc, sil_tri.TS_IDX_4);
  /*p30.CADU*/ sst_reg.STORE1_IDX5.set(STORE1_CLKc, sil_tri.TS_IDX_5);
                                                                                           
  /*p29.AHOF*/ wire STORE1_CLKb = not(STORE1_CLKn);
  /*p30.ABUG*/ sst_reg.STORE1_LINE0.set(STORE1_CLKb, sil_tri.TS_LINE_0);
  /*p30.AMES*/ sst_reg.STORE1_LINE1.set(STORE1_CLKb, sil_tri.TS_LINE_1);
  /*p30.ABOP*/ sst_reg.STORE1_LINE2.set(STORE1_CLKb, sil_tri.TS_LINE_2);
  /*p30.AROF*/ sst_reg.STORE1_LINE3.set(STORE1_CLKb, sil_tri.TS_LINE_3);

  /*p29.ASYS*/ wire STORE1_CLKa = not(STORE1_CLKn);
  /*p31.DANY*/ sst_reg.STORE1_X0.set(STORE1_CLKa, STORE1_RSTn, SPRITE_X0);
  /*p31.DUKO*/ sst_reg.STORE1_X1.set(STORE1_CLKa, STORE1_RSTn, SPRITE_X1);
  /*p31.DESU*/ sst_reg.STORE1_X2.set(STORE1_CLKa, STORE1_RSTn, SPRITE_X2);
  /*p31.DAZO*/ sst_reg.STORE1_X3.set(STORE1_CLKa, STORE1_RSTn, SPRITE_X3);
  /*p31.DAKE*/ sst_reg.STORE1_X4.set(STORE1_CLKa, STORE1_RSTn, SPRITE_X4);
  /*p31.CESO*/ sst_reg.STORE1_X5.set(STORE1_CLKa, STORE1_RSTn, SPRITE_X5);
  /*p31.DYFU*/ sst_reg.STORE1_X6.set(STORE1_CLKa, STORE1_RSTn, SPRITE_X6);
  /*p31.CUSY*/ sst_reg.STORE1_X7.set(STORE1_CLKa, STORE1_RSTn, SPRITE_X7);

  /*p29.DYDO*/ wire SPRITE1_GET = not(SPRITE1_GETn);
  /*p30.BEMO*/ if (SPRITE1_GET) sil_tri.TS_IDX_0  = not(!sst_reg.STORE1_IDX0);
  /*p30.CYBY*/ if (SPRITE1_GET) sil_tri.TS_IDX_1  = not(!sst_reg.STORE1_IDX1);
  /*p30.BETY*/ if (SPRITE1_GET) sil_tri.TS_IDX_2  = not(!sst_reg.STORE1_IDX2);
  /*p30.CEGY*/ if (SPRITE1_GET) sil_tri.TS_IDX_3  = not(!sst_reg.STORE1_IDX3);
  /*p30.CELU*/ if (SPRITE1_GET) sil_tri.TS_IDX_4  = not(!sst_reg.STORE1_IDX4);
  /*p30.CUBO*/ if (SPRITE1_GET) sil_tri.TS_IDX_5  = not(!sst_reg.STORE1_IDX5);
  /*p30.BEFE*/ if (SPRITE1_GET) sil_tri.TS_LINE_0 = not(!sst_reg.STORE1_LINE0);
  /*p30.BYRO*/ if (SPRITE1_GET) sil_tri.TS_LINE_1 = not(!sst_reg.STORE1_LINE1);
  /*p30.BACO*/ if (SPRITE1_GET) sil_tri.TS_LINE_2 = not(!sst_reg.STORE1_LINE2);
  /*p30.AHUM*/ if (SPRITE1_GET) sil_tri.TS_LINE_3 = not(!sst_reg.STORE1_LINE3);

  //----------------------------------------
  // sprite store 2

  /*p31.FUZU*/ wire STORE2_MATCH0 = xor(sst_reg.STORE2_X0, X0n);
  /*p31.FESO*/ wire STORE2_MATCH1 = xor(sst_reg.STORE2_X1, X1n);
  /*p31.FOKY*/ wire STORE2_MATCH2 = xor(sst_reg.STORE2_X2, X2n);
  /*p31.FYVA*/ wire STORE2_MATCH3 = xor(sst_reg.STORE2_X3, X3n);
  /*p31.CEKO*/ wire STORE2_MATCH4 = xor(sst_reg.STORE2_X4, X4n);
  /*p31.DETY*/ wire STORE2_MATCH5 = xor(sst_reg.STORE2_X5, X5n);
  /*p31.DOZO*/ wire STORE2_MATCH6 = xor(sst_reg.STORE2_X6, X6n);
  /*p31.CONY*/ wire STORE2_MATCH7 = xor(sst_reg.STORE2_X7, X7n);
  /*p31.CEHU*/ wire STORE2_MATCHA = nor(STORE2_MATCH4, STORE2_MATCH5, STORE2_MATCH6, STORE2_MATCH7);
  /*p31.EKES*/ wire STORE2_MATCHB = nor(STORE2_MATCH0, STORE2_MATCH1, STORE2_MATCH2, STORE2_MATCH3);

  /*p29.DEGO*/ wire STORE2_MATCHn = nand(MATCH_EN, STORE2_MATCHB, STORE2_MATCHA);
  /*p29.GUPO*/ wire STORE2_MATCH = not(STORE2_MATCHn);
  /*p29.GEDE*/ wire STORE2_MATCH_OUT = or(STORE2_MATCH, STORE1_MATCH_OUT);

  /*p29.EMOL*/ wire SPRITE2_GETn = nor(STORE2_MATCHn, STORE1_MATCH_OUT);
  /*p29.EGAV*/ sst_reg.SPRITE2_GET_SYNCn.set(WUTY_SPRITE_DONE, BYVA_VID_LINE_TRIG_d4n, SPRITE2_GETn);

  /*p29.WYXO*/ wire STORE2_CLK = or(STORE_EN, STORE2_SEL); // 0010
  /*p29.GYFO*/ wire STORE2_CLKn = not(STORE2_CLK); // 0010
  /*p29.FOKO*/ wire STORE2_RST  = or(DYBA_VID_LINE_TRIG_d4, sst_reg.SPRITE2_GET_SYNCn);
  /*p29.GAMY*/ wire STORE2_RSTn = not(STORE2_RST);

  /*p29.BUZY*/ wire STORE2_CLKb = not(STORE2_CLKn);
  /*p30.BOXA*/ sst_reg.STORE2_IDX0.set(STORE2_CLKb, sil_tri.TS_IDX_0);
  /*p30.BUNA*/ sst_reg.STORE2_IDX1.set(STORE2_CLKb, sil_tri.TS_IDX_1);
  /*p30.BULU*/ sst_reg.STORE2_IDX2.set(STORE2_CLKb, sil_tri.TS_IDX_2);
  /*p30.BECA*/ sst_reg.STORE2_IDX3.set(STORE2_CLKb, sil_tri.TS_IDX_3);
  /*p30.BYHU*/ sst_reg.STORE2_IDX4.set(STORE2_CLKb, sil_tri.TS_IDX_4);
  /*p30.BUHE*/ sst_reg.STORE2_IDX5.set(STORE2_CLKb, sil_tri.TS_IDX_5);

  /*p29.FUKE*/ wire STORE2_CLKc = not(STORE2_CLKn);
  /*p30.YKUK*/ sst_reg.STORE2_LINE0.set(STORE2_CLKc, sil_tri.TS_LINE_0);
  /*p30.YLOV*/ sst_reg.STORE2_LINE1.set(STORE2_CLKc, sil_tri.TS_LINE_1);
  /*p30.XAZY*/ sst_reg.STORE2_LINE2.set(STORE2_CLKc, sil_tri.TS_LINE_2);
  /*p30.XOSY*/ sst_reg.STORE2_LINE3.set(STORE2_CLKc, sil_tri.TS_LINE_3);

  /*p29.CACU*/ wire STORE2_CLKa = not(STORE2_CLKn);
  /*p31.FOKA*/ sst_reg.STORE2_X0.set(STORE2_CLKa, STORE2_RSTn, SPRITE_X0);
  /*p31.FYTY*/ sst_reg.STORE2_X1.set(STORE2_CLKa, STORE2_RSTn, SPRITE_X1);
  /*p31.FUBY*/ sst_reg.STORE2_X2.set(STORE2_CLKa, STORE2_RSTn, SPRITE_X2);
  /*p31.GOXU*/ sst_reg.STORE2_X3.set(STORE2_CLKa, STORE2_RSTn, SPRITE_X3);
  /*p31.DUHY*/ sst_reg.STORE2_X4.set(STORE2_CLKa, STORE2_RSTn, SPRITE_X4);
  /*p31.EJUF*/ sst_reg.STORE2_X5.set(STORE2_CLKa, STORE2_RSTn, SPRITE_X5);
  /*p31.ENOR*/ sst_reg.STORE2_X6.set(STORE2_CLKa, STORE2_RSTn, SPRITE_X6);
  /*p31.DEPY*/ sst_reg.STORE2_X7.set(STORE2_CLKa, STORE2_RSTn, SPRITE_X7);


  /*p29.FAME*/ wire SPRITE2_GET = not(SPRITE2_GETn);
  /*p30.AXEC*/ if (SPRITE2_GET) sil_tri.TS_IDX_0  = not(!sst_reg.STORE2_IDX0);
  /*p30.CYRO*/ if (SPRITE2_GET) sil_tri.TS_IDX_1  = not(!sst_reg.STORE2_IDX1);
  /*p30.CUVU*/ if (SPRITE2_GET) sil_tri.TS_IDX_2  = not(!sst_reg.STORE2_IDX2);
  /*p30.APON*/ if (SPRITE2_GET) sil_tri.TS_IDX_3  = not(!sst_reg.STORE2_IDX3);
  /*p30.AFOZ*/ if (SPRITE2_GET) sil_tri.TS_IDX_4  = not(!sst_reg.STORE2_IDX4);
  /*p30.CUBE*/ if (SPRITE2_GET) sil_tri.TS_IDX_5  = not(!sst_reg.STORE2_IDX5);
  /*p30.ZABY*/ if (SPRITE2_GET) sil_tri.TS_LINE_0 = not(!sst_reg.STORE2_LINE0);
  /*p30.ZUKE*/ if (SPRITE2_GET) sil_tri.TS_LINE_1 = not(!sst_reg.STORE2_LINE1);
  /*p30.WUXE*/ if (SPRITE2_GET) sil_tri.TS_LINE_2 = not(!sst_reg.STORE2_LINE2);
  /*p30.WERE*/ if (SPRITE2_GET) sil_tri.TS_LINE_3 = not(!sst_reg.STORE2_LINE3);

  //----------------------------------------
  // sprite store 3

  /*p31.YHOK*/ wire STORE3_MATCH0 = xor(sst_reg.XOLY, X0n);
  /*p31.YCAH*/ wire STORE3_MATCH1 = xor(sst_reg.XYBA, X1n);
  /*p31.YDAJ*/ wire STORE3_MATCH2 = xor(sst_reg.XABE, X2n);
  /*p31.YVUZ*/ wire STORE3_MATCH3 = xor(sst_reg.XEKA, X3n);
  /*p31.YVAP*/ wire STORE3_MATCH4 = xor(sst_reg.XOMY, X4n);
  /*p31.XENY*/ wire STORE3_MATCH5 = xor(sst_reg.WUHA, X5n);
  /*p31.XAVU*/ wire STORE3_MATCH6 = xor(sst_reg.WYNA, X6n);
  /*p31.XEVA*/ wire STORE3_MATCH7 = xor(sst_reg.WECO, X7n);
  /*p31.ZURE*/ wire STORE3_MATCHA = nor(STORE3_MATCH0, STORE3_MATCH1, STORE3_MATCH2, STORE3_MATCH3);
  /*p31.YWOS*/ wire STORE3_MATCHB = nor(STORE3_MATCH4, STORE3_MATCH5, STORE3_MATCH6, STORE3_MATCH7);

  /*p29.YLOZ*/ wire STORE3_MATCHn = nand(MATCH_EN, STORE3_MATCHA, STORE3_MATCHB);
  /*p29.WEBO*/ wire STORE3_MATCH = not(STORE3_MATCHn);
  /*p29.WUTO*/ wire STORE3_MATCH_OUT = or(STORE3_MATCH, STORE2_MATCH_OUT);

  /*p29.GYFY*/ wire SPRITE3_GETn = nor(STORE3_MATCHn, STORE2_MATCH_OUT);
  /*p29.GOTA*/ sst_reg.SPRITE3_GET_SYNCn.set(WUTY_SPRITE_DONE, BYVA_VID_LINE_TRIG_d4n, SPRITE3_GETn);

  /*p29.GAKE*/ wire STORE3_RST = or(DYBA_VID_LINE_TRIG_d4, sst_reg.SPRITE3_GET_SYNCn);
  /*p29.WUPA*/ wire STORE3_RSTn = not(STORE3_RST);

  /*p29.GUVE*/ wire STORE3_CLK = or(STORE_EN, STORE3_SEL); // 0011
  /*p29.GUSA*/ wire STORE3_CLKn = not(STORE3_CLK);
    
  /*p29.FEKA*/ wire FEKA = not(STORE3_CLKn);
  /*p30.DAFU*/ sst_reg.DAFU.set(FEKA, sil_tri.TS_IDX_0);
  /*p30.DEBA*/ sst_reg.DEBA.set(FEKA, sil_tri.TS_IDX_1);
  /*p30.DUHA*/ sst_reg.DUHA.set(FEKA, sil_tri.TS_IDX_2);
  /*p30.DUNY*/ sst_reg.DUNY.set(FEKA, sil_tri.TS_IDX_3);
  /*p30.DESE*/ sst_reg.DESE.set(FEKA, sil_tri.TS_IDX_4);
  /*p30.DEVY*/ sst_reg.DEVY.set(FEKA, sil_tri.TS_IDX_5);

  /*p29.XYHA*/ wire XYHA = not(STORE3_CLKn);
  /*p30.ZURY*/ sst_reg.ZURY.set(XYHA, sil_tri.TS_LINE_0);
  /*p30.ZURO*/ sst_reg.ZURO.set(XYHA, sil_tri.TS_LINE_1);
  /*p30.ZENE*/ sst_reg.ZENE.set(XYHA, sil_tri.TS_LINE_2);
  /*p30.ZYLU*/ sst_reg.ZYLU.set(XYHA, sil_tri.TS_LINE_3);

  /*p29.YFAG*/ wire YFAG = not(STORE3_CLKn);
  /*p31.XOLY*/ sst_reg.XOLY.set(YFAG, STORE3_RSTn, SPRITE_X0);
  /*p31.XYBA*/ sst_reg.XYBA.set(YFAG, STORE3_RSTn, SPRITE_X1);
  /*p31.XABE*/ sst_reg.XABE.set(YFAG, STORE3_RSTn, SPRITE_X2);
  /*p31.XEKA*/ sst_reg.XEKA.set(YFAG, STORE3_RSTn, SPRITE_X3);
  /*p31.XOMY*/ sst_reg.XOMY.set(YFAG, STORE3_RSTn, SPRITE_X4);
  /*p31.WUHA*/ sst_reg.WUHA.set(YFAG, STORE3_RSTn, SPRITE_X5);
  /*p31.WYNA*/ sst_reg.WYNA.set(YFAG, STORE3_RSTn, SPRITE_X6);
  /*p31.WECO*/ sst_reg.WECO.set(YFAG, STORE3_RSTn, SPRITE_X7);

  /*p29.GYMA*/ wire SPRITE3_GET = not(SPRITE3_GETn);
  /*p30.DEZU*/ if (SPRITE3_GET) sil_tri.TS_IDX_0  = not(!sst_reg.DAFU);
  /*p30.EFUD*/ if (SPRITE3_GET) sil_tri.TS_IDX_1  = not(!sst_reg.DEBA);
  /*p30.DONY*/ if (SPRITE3_GET) sil_tri.TS_IDX_2  = not(!sst_reg.DUHA);
  /*p30.DOWA*/ if (SPRITE3_GET) sil_tri.TS_IDX_3  = not(!sst_reg.DUNY);
  /*p30.DYGO*/ if (SPRITE3_GET) sil_tri.TS_IDX_4  = not(!sst_reg.DESE);
  /*p30.ENAP*/ if (SPRITE3_GET) sil_tri.TS_IDX_5  = not(!sst_reg.DEVY);
  /*p30.ZYPO*/ if (SPRITE3_GET) sil_tri.TS_LINE_0 = not(!sst_reg.ZURY);
  /*p30.ZEXE*/ if (SPRITE3_GET) sil_tri.TS_LINE_1 = not(!sst_reg.ZURO);
  /*p30.YJEM*/ if (SPRITE3_GET) sil_tri.TS_LINE_2 = not(!sst_reg.ZENE);
  /*p30.YWAV*/ if (SPRITE3_GET) sil_tri.TS_LINE_3 = not(!sst_reg.ZYLU);

  //----------------------------------------
  // sprite store 4

  /*p31.ZYKU*/ wire ZYKU = xor(sst_reg.YBED, X4n);
  /*p31.ZYPU*/ wire ZYPU = xor(sst_reg.ZALA, X5n);
  /*p31.XAHA*/ wire XAHA = xor(sst_reg.WYDE, X6n);
  /*p31.ZEFE*/ wire ZEFE = xor(sst_reg.XEPA, X7n);
  /*p31.XEJU*/ wire XEJU = xor(sst_reg.WEDU, X0n);
  /*p31.ZATE*/ wire ZATE = xor(sst_reg.YGAJ, X1n);
  /*p31.ZAKU*/ wire ZAKU = xor(sst_reg.ZYJO, X2n);
  /*p31.YBOX*/ wire YBOX = xor(sst_reg.XURY, X3n);
  /*p31.YKOK*/ wire YKOK = nor(ZYKU, ZYPU, XAHA, ZEFE);
  /*p31.YNAZ*/ wire YNAZ = nor(XEJU, ZATE, ZAKU, YBOX);
  /*p29.XAGE*/ wire STORE4_MATCHn = nand(MATCH_EN, YNAZ, YKOK);
  /*p29.WUNA*/ wire STORE4_MATCH = not(STORE4_MATCHn);

  /*p29.GONO*/ wire SPRITE4_GETn = nor(STORE4_MATCHn, STORE3_MATCH_OUT);
  /*p29.XUDY*/ sst_reg.SPRITE4_GET_SYNCn.set(WUTY_SPRITE_DONE, BYVA_VID_LINE_TRIG_d4n, SPRITE4_GETn);
  /*p29.WOHU*/ wire WOHU = or(DYBA_VID_LINE_TRIG_d4, sst_reg.SPRITE4_GET_SYNCn);
  /*p29.XYLA*/ wire STORE4_MATCH_OUT = or(STORE4_MATCH, STORE3_MATCH_OUT);

  /*p29.CECU*/ wire STORE4_CLK = or(STORE_EN, STORE4_SEL); // 0100
  /*p29.DUKE*/ wire DUKE = not(STORE4_CLK);
  
  /*p29.WUNU*/ wire WUNU = not(WOHU); // i think these are swapped
  /*p29.WOFO*/ wire WOFO = not(DUKE);
  /*p31.WEDU*/ sst_reg.WEDU.set(WUNU, WOFO, SPRITE_X0);
  /*p31.YGAJ*/ sst_reg.YGAJ.set(WUNU, WOFO, SPRITE_X1);
  /*p31.ZYJO*/ sst_reg.ZYJO.set(WUNU, WOFO, SPRITE_X2);
  /*p31.XURY*/ sst_reg.XURY.set(WUNU, WOFO, SPRITE_X3);
  /*p31.YBED*/ sst_reg.YBED.set(WUNU, WOFO, SPRITE_X4);
  /*p31.ZALA*/ sst_reg.ZALA.set(WUNU, WOFO, SPRITE_X5);
  /*p31.WYDE*/ sst_reg.WYDE.set(WUNU, WOFO, SPRITE_X6);
  /*p31.XEPA*/ sst_reg.XEPA.set(WUNU, WOFO, SPRITE_X7);

  /*p29.WYLU*/ wire WYLU = not(DUKE);
  /*p30.XYNU*/ sst_reg.XYNU.set(WYLU, sil_tri.TS_IDX_0);
  /*p30.XEGE*/ sst_reg.XEGE.set(WYLU, sil_tri.TS_IDX_1);
  /*p30.XABO*/ sst_reg.XABO.set(WYLU, sil_tri.TS_IDX_2);
  /*p30.WANU*/ sst_reg.WANU.set(WYLU, sil_tri.TS_IDX_3);
  /*p30.XEFE*/ sst_reg.XEFE.set(WYLU, sil_tri.TS_IDX_4);
  /*p30.XAVE*/ sst_reg.XAVE.set(WYLU, sil_tri.TS_IDX_5);

  /*p29.EWOT*/ wire EWOT = not(DUKE);
  /*p30.CUMU*/ sst_reg.CUMU.set(EWOT, sil_tri.TS_LINE_0);
  /*p30.CAPO*/ sst_reg.CAPO.set(EWOT, sil_tri.TS_LINE_1);
  /*p30.CONO*/ sst_reg.CONO.set(EWOT, sil_tri.TS_LINE_2);
  /*p30.CAJU*/ sst_reg.CAJU.set(EWOT, sil_tri.TS_LINE_3);

  /*p29.GOWO*/ wire SPRITE4_GET = not(SPRITE4_GETn);
  /*p30.WAJA*/ if (SPRITE4_GET) sil_tri.TS_IDX_0  = not(!sst_reg.XYNU);
  /*p30.WOXY*/ if (SPRITE4_GET) sil_tri.TS_IDX_1  = not(!sst_reg.XEGE);
  /*p30.XYRE*/ if (SPRITE4_GET) sil_tri.TS_IDX_2  = not(!sst_reg.XABO);
  /*p30.WERU*/ if (SPRITE4_GET) sil_tri.TS_IDX_3  = not(!sst_reg.WANU);
  /*p30.WEPY*/ if (SPRITE4_GET) sil_tri.TS_IDX_4  = not(!sst_reg.XEFE);
  /*p30.WUXU*/ if (SPRITE4_GET) sil_tri.TS_IDX_5  = not(!sst_reg.XAVE);
  /*p30.BYDO*/ if (SPRITE4_GET) sil_tri.TS_LINE_0 = not(!sst_reg.CUMU);
  /*p30.BUCE*/ if (SPRITE4_GET) sil_tri.TS_LINE_1 = not(!sst_reg.CAPO);
  /*p30.BOVE*/ if (SPRITE4_GET) sil_tri.TS_LINE_2 = not(!sst_reg.CONO);
  /*p30.BEVY*/ if (SPRITE4_GET) sil_tri.TS_LINE_3 = not(!sst_reg.CAJU);

  //----------------------------------------
  // sprite store 5

  /*p31.BAZY*/ wire BAZY = xor(sst_reg.CYWE, X4n);
  /*p31.CYLE*/ wire CYLE = xor(sst_reg.DYBY, X5n);
  /*p31.CEVA*/ wire CEVA = xor(sst_reg.DURY, X6n);
  /*p31.BUMY*/ wire BUMY = xor(sst_reg.CUVY, X7n);
  /*p31.GUZO*/ wire GUZO = xor(sst_reg.FUSA, X0n);
  /*p31.GOLA*/ wire GOLA = xor(sst_reg.FAXA, X1n);
  /*p31.GEVE*/ wire GEVE = xor(sst_reg.FOZY, X2n);
  /*p31.GUDE*/ wire GUDE = xor(sst_reg.FESY, X3n);
  /*p31.COGY*/ wire COGY = nor(BAZY, CYLE, CEVA, BUMY);
  /*p31.FYMA*/ wire FYMA = nor(GUZO, GOLA, GEVE, GUDE);
  /*p29.EGOM*/ wire STORE5_MATCHn = nand(MATCH_EN, FYMA, COGY);

  /*p29.GEGA*/ wire GEGA = nor(STORE5_MATCHn, STORE4_MATCH_OUT);
  /*p29.WAFY*/ sst_reg.SPRITE5_GET_SYNCn.set(WUTY_SPRITE_DONE, BYVA_VID_LINE_TRIG_d4n, GEGA);
  /*p29.FEVE*/ wire FEVE = or(DYBA_VID_LINE_TRIG_d4, sst_reg.SPRITE5_GET_SYNCn);
  /*p29.EJAD*/ wire EJAD = not(FEVE);
  /*p29.GABA*/ wire STORE5_MATCH = not(STORE5_MATCHn);
    
  /*p29.WEJA*/ wire STORE5_MATCH_OUT = or(STORE5_MATCH, STORE4_MATCH_OUT);

  /*p29.CADO*/ wire STORE5_CLK = or(STORE_EN, STORE5_SEL); // 0101
  /*p29.BEDE*/ wire BEDE = not(STORE5_CLK);

  /*p29.CYLA*/ wire CYLA = not(BEDE);
  /*p31.FUSA*/ sst_reg.FUSA.set(CYLA, EJAD, SPRITE_X0);
  /*p31.FAXA*/ sst_reg.FAXA.set(CYLA, EJAD, SPRITE_X1);
  /*p31.FOZY*/ sst_reg.FOZY.set(CYLA, EJAD, SPRITE_X2);
  /*p31.FESY*/ sst_reg.FESY.set(CYLA, EJAD, SPRITE_X3);
  /*p31.CYWE*/ sst_reg.CYWE.set(CYLA, EJAD, SPRITE_X4);
  /*p31.DYBY*/ sst_reg.DYBY.set(CYLA, EJAD, SPRITE_X5);
  /*p31.DURY*/ sst_reg.DURY.set(CYLA, EJAD, SPRITE_X6);
  /*p31.CUVY*/ sst_reg.CUVY.set(CYLA, EJAD, SPRITE_X7);

  /*p29.DYMO*/ wire DYMO = not(BEDE);
  /*p30.EKAP*/ sst_reg.EKAP.set(DYMO, sil_tri.TS_IDX_0);
  /*p30.ETAV*/ sst_reg.ETAV.set(DYMO, sil_tri.TS_IDX_1);
  /*p30.EBEX*/ sst_reg.EBEX.set(DYMO, sil_tri.TS_IDX_2);
  /*p30.GORU*/ sst_reg.GORU.set(DYMO, sil_tri.TS_IDX_3);
  /*p30.ETYM*/ sst_reg.ETYM.set(DYMO, sil_tri.TS_IDX_4);
  /*p30.EKOP*/ sst_reg.EKOP.set(DYMO, sil_tri.TS_IDX_5);

  /*p29.BUCY*/ wire BUCY = not(BEDE);
  /*p30.ANED*/ sst_reg.ANED.set(BUCY, sil_tri.TS_LINE_0);
  /*p30.ACEP*/ sst_reg.ACEP.set(BUCY, sil_tri.TS_LINE_1);
  /*p30.ABUX*/ sst_reg.ABUX.set(BUCY, sil_tri.TS_LINE_2);
  /*p30.ABEG*/ sst_reg.ABEG.set(BUCY, sil_tri.TS_LINE_3);

  /*p29.GYGY*/ wire GYGY = not(GEGA);
  /*p30.DALO*/ if (GYGY) sil_tri.TS_IDX_0  = not(!sst_reg.EKAP);
  /*p30.DALY*/ if (GYGY) sil_tri.TS_IDX_1  = not(!sst_reg.ETAV);
  /*p30.DUZA*/ if (GYGY) sil_tri.TS_IDX_2  = not(!sst_reg.EBEX);
  /*p30.WAGA*/ if (GYGY) sil_tri.TS_IDX_3  = not(!sst_reg.GORU);
  /*p30.DYNY*/ if (GYGY) sil_tri.TS_IDX_4  = not(!sst_reg.ETYM);
  /*p30.DOBO*/ if (GYGY) sil_tri.TS_IDX_5  = not(!sst_reg.EKOP);
  /*p30.AWAT*/ if (GYGY) sil_tri.TS_LINE_0 = not(!sst_reg.ANED);
  /*p30.BACE*/ if (GYGY) sil_tri.TS_LINE_1 = not(!sst_reg.ACEP);
  /*p30.BODU*/ if (GYGY) sil_tri.TS_LINE_2 = not(!sst_reg.ABUX);
  /*p30.BUJA*/ if (GYGY) sil_tri.TS_LINE_3 = not(!sst_reg.ABEG);

  //----------------------------------------
  // sprite store 6

  /*p31.ZARE*/ wire ZARE = xor(sst_reg.ZOLY, X4n);
  /*p31.ZEMU*/ wire ZEMU = xor(sst_reg.ZOGO, X5n);
  /*p31.ZYGO*/ wire ZYGO = xor(sst_reg.ZECU, X6n);
  /*p31.ZUZY*/ wire ZUZY = xor(sst_reg.ZESA, X7n);
  /*p31.XOSU*/ wire XOSU = xor(sst_reg.YCOL, X0n);
  /*p31.ZUVU*/ wire ZUVU = xor(sst_reg.YRAC, X1n);
  /*p31.XUCO*/ wire XUCO = xor(sst_reg.YMEM, X2n);
  /*p31.ZULO*/ wire ZULO = xor(sst_reg.YVAG, X3n);
  /*p31.YWAP*/ wire YWAP = nor(ZARE, ZEMU, ZYGO, ZUZY);
  /*p31.YDOT*/ wire YDOT = nor(XOSU, ZUVU, XUCO, ZULO);
  /*p29.YBEZ*/ wire STORE6_MATCHn = nand(MATCH_EN, YDOT, YWAP);

  /*p29.XOJA*/ wire XOJA = nor(STORE6_MATCHn, STORE5_MATCH_OUT);
  /*p29.WOMY*/ sst_reg.SPRITE6_GET_SYNCn.set(WUTY_SPRITE_DONE, BYVA_VID_LINE_TRIG_d4n, XOJA);
  /*p29.WACY*/ wire WACY = or(DYBA_VID_LINE_TRIG_d4, sst_reg.SPRITE6_GET_SYNCn);
  /*p29.XAHO*/ wire XAHO = not(WACY);
  /*p29.WASE*/ wire STORE6_MATCH = not(STORE6_MATCHn);
  /*p29.WYLA*/ wire STORE6_MATCH_OUT = or(STORE6_MATCH, STORE5_MATCH_OUT);

  /*p29.XUJO*/ wire STORE6_CLK = or(STORE_EN, STORE6_SEL); // 0110
  /*p29.WEKA*/ wire WEKA = not(STORE6_CLK);

  /*p29.ZAPE*/ wire ZAPE = not(WEKA);
  /*p31.YCOL*/ sst_reg.YCOL.set(ZAPE, XAHO, SPRITE_X0);
  /*p31.YRAC*/ sst_reg.YRAC.set(ZAPE, XAHO, SPRITE_X1);
  /*p31.YMEM*/ sst_reg.YMEM.set(ZAPE, XAHO, SPRITE_X2);
  /*p31.YVAG*/ sst_reg.YVAG.set(ZAPE, XAHO, SPRITE_X3);
  /*p31.ZOLY*/ sst_reg.ZOLY.set(ZAPE, XAHO, SPRITE_X4);
  /*p31.ZOGO*/ sst_reg.ZOGO.set(ZAPE, XAHO, SPRITE_X5);
  /*p31.ZECU*/ sst_reg.ZECU.set(ZAPE, XAHO, SPRITE_X6);
  /*p31.ZESA*/ sst_reg.ZESA.set(ZAPE, XAHO, SPRITE_X7);

  /*p29.WUSE*/ wire WUSE = not(WEKA);
  /*p30.GECU*/ sst_reg.GECU.set(WUSE, sil_tri.TS_IDX_0);
  /*p30.FOXY*/ sst_reg.FOXY.set(WUSE, sil_tri.TS_IDX_1);
  /*p30.GOHU*/ sst_reg.GOHU.set(WUSE, sil_tri.TS_IDX_2);
  /*p30.FOGO*/ sst_reg.FOGO.set(WUSE, sil_tri.TS_IDX_3);
  /*p30.GACY*/ sst_reg.GACY.set(WUSE, sil_tri.TS_IDX_4);
  /*p30.GABO*/ sst_reg.GABO.set(WUSE, sil_tri.TS_IDX_5);
    
  /*p29.ZURU*/ wire ZURU = not(WEKA);
  /*p30.ZUBE*/ sst_reg.ZUBE.set(ZURU, sil_tri.TS_LINE_0);
  /*p30.ZUMY*/ sst_reg.ZUMY.set(ZURU, sil_tri.TS_LINE_1);
  /*p30.ZEXO*/ sst_reg.ZEXO.set(ZURU, sil_tri.TS_LINE_2);
  /*p30.ZAFU*/ sst_reg.ZAFU.set(ZURU, sil_tri.TS_LINE_3);

  /*p29.XYME*/ wire XYME = not(XOJA);
  /*p30.WOCY*/ if (XYME) sil_tri.TS_IDX_0  = not(!sst_reg.GECU);
  /*p30.ELYC*/ if (XYME) sil_tri.TS_IDX_1  = not(!sst_reg.FOXY);
  /*p30.WABO*/ if (XYME) sil_tri.TS_IDX_2  = not(!sst_reg.GOHU);
  /*p30.EZOC*/ if (XYME) sil_tri.TS_IDX_3  = not(!sst_reg.FOGO);
  /*p30.WYWY*/ if (XYME) sil_tri.TS_IDX_4  = not(!sst_reg.GACY);
  /*p30.WATO*/ if (XYME) sil_tri.TS_IDX_5  = not(!sst_reg.GABO);
  /*p30.ZUDO*/ if (XYME) sil_tri.TS_LINE_0 = not(!sst_reg.ZUBE);
  /*p30.YBUK*/ if (XYME) sil_tri.TS_LINE_1 = not(!sst_reg.ZUMY);
  /*p30.ZYTO*/ if (XYME) sil_tri.TS_LINE_2 = not(!sst_reg.ZEXO);
  /*p30.YKOZ*/ if (XYME) sil_tri.TS_LINE_3 = not(!sst_reg.ZAFU);

  //----------------------------------------
  // sprite store 7

  /*p31.DUSE*/ wire STORE7_MATCH0 = xor(sst_reg.STORE7_X0, X0n);
  /*p31.DAGU*/ wire STORE7_MATCH1 = xor(sst_reg.STORE7_X1, X1n);
  /*p31.DYZE*/ wire STORE7_MATCH2 = xor(sst_reg.STORE7_X2, X2n);
  /*p31.DESO*/ wire STORE7_MATCH3 = xor(sst_reg.STORE7_X3, X3n);
  /*p31.EJOT*/ wire STORE7_MATCH4 = xor(sst_reg.STORE7_X4, X4n);
  /*p31.ESAJ*/ wire STORE7_MATCH5 = xor(sst_reg.STORE7_X5, X5n);
  /*p31.DUCU*/ wire STORE7_MATCH6 = xor(sst_reg.STORE7_X6, X6n);
  /*p31.EWUD*/ wire STORE7_MATCH7 = xor(sst_reg.STORE7_X7, X7n);
  /*p31.CYCO*/ wire STORE7_MATCHA = nor(STORE7_MATCH0, STORE7_MATCH1, STORE7_MATCH2, STORE7_MATCH3);
  /*p31.DAJE*/ wire STORE7_MATCHB = nor(STORE7_MATCH4, STORE7_MATCH5, STORE7_MATCH6, STORE7_MATCH7);
  /*p29.DYKA*/ wire STORE7_MATCHn = nand(MATCH_EN, STORE7_MATCHA, STORE7_MATCHB);
  /*p29.GYTE*/ wire STORE7_MATCH = not(STORE7_MATCHn);
  /*p29.FAVO*/ wire STORE7_MATCH_OUT = or(STORE7_MATCH, STORE6_MATCH_OUT);

  /*p29.GUTU*/ wire STORE7_RD = nor(STORE7_MATCHn, STORE6_MATCH_OUT);
  /*p29.WAPO*/ sst_reg.SPRITE7_GET_SYNCn.set(WUTY_SPRITE_DONE, BYVA_VID_LINE_TRIG_d4n, STORE7_RD);
  /*p29.GUKY*/ wire STORE7_RST = or(DYBA_VID_LINE_TRIG_d4, sst_reg.SPRITE7_GET_SYNCn);
  /*p29.GAFY*/ wire STORE7_RSTn = not(STORE7_RST);

  /*p29.GAPE*/ wire STORE7_CLK = or(STORE_EN, STORE7_SEL); // 0111
  /*p29.GYVO*/ wire STORE7_CLKn = not(STORE7_CLK);

  /*p29.GECY*/ wire GECY = not(STORE7_CLKn);
  /*p31.ERAZ*/ sst_reg.STORE7_X0.set(GECY, STORE7_RSTn, SPRITE_X0);
  /*p31.EPUM*/ sst_reg.STORE7_X1.set(GECY, STORE7_RSTn, SPRITE_X1);
  /*p31.EROL*/ sst_reg.STORE7_X2.set(GECY, STORE7_RSTn, SPRITE_X2);
  /*p31.EHYN*/ sst_reg.STORE7_X3.set(GECY, STORE7_RSTn, SPRITE_X3);
  /*p31.FAZU*/ sst_reg.STORE7_X4.set(GECY, STORE7_RSTn, SPRITE_X4);
  /*p31.FAXE*/ sst_reg.STORE7_X5.set(GECY, STORE7_RSTn, SPRITE_X5);
  /*p31.EXUK*/ sst_reg.STORE7_X6.set(GECY, STORE7_RSTn, SPRITE_X6);
  /*p31.FEDE*/ sst_reg.STORE7_X7.set(GECY, STORE7_RSTn, SPRITE_X7);

  /*p29.FEFO*/ wire FEFO = not(STORE7_CLKn);
  /*p30.FUZO*/ sst_reg.FUZO.set(FEFO, sil_tri.TS_IDX_0);
  /*p30.GESY*/ sst_reg.GESY.set(FEFO, sil_tri.TS_IDX_1);
  /*p30.FYSU*/ sst_reg.FYSU.set(FEFO, sil_tri.TS_IDX_2);
  /*p30.FEFA*/ sst_reg.FEFA.set(FEFO, sil_tri.TS_IDX_3);
  /*p30.GYNO*/ sst_reg.GYNO.set(FEFO, sil_tri.TS_IDX_4);
  /*p30.GULE*/ sst_reg.GULE.set(FEFO, sil_tri.TS_IDX_5);

  /*p29.WABE*/ wire WABE = not(STORE7_CLKn);
  /*p30.XYGO*/ sst_reg.XYGO.set(WABE, sil_tri.TS_LINE_0);
  /*p30.XYNA*/ sst_reg.XYNA.set(WABE, sil_tri.TS_LINE_1);
  /*p30.XAKU*/ sst_reg.XAKU.set(WABE, sil_tri.TS_LINE_2);
  /*p30.YGUM*/ sst_reg.YGUM.set(WABE, sil_tri.TS_LINE_3);

  /*p29.GUGY*/ wire STORE7_RDn = not(STORE7_RD);
  /*p30.EVYT*/ if (!STORE7_RDn) sil_tri.TS_IDX_0  = not(!sst_reg.FUZO);
  /*p30.WABA*/ if (!STORE7_RDn) sil_tri.TS_IDX_1  = not(!sst_reg.GESY);
  /*p30.ETAD*/ if (!STORE7_RDn) sil_tri.TS_IDX_2  = not(!sst_reg.FYSU);
  /*p30.ELEP*/ if (!STORE7_RDn) sil_tri.TS_IDX_3  = not(!sst_reg.FEFA);
  /*p30.WYGO*/ if (!STORE7_RDn) sil_tri.TS_IDX_4  = not(!sst_reg.GYNO);
  /*p30.WAKO*/ if (!STORE7_RDn) sil_tri.TS_IDX_5  = not(!sst_reg.GULE);
  /*p30.WANA*/ if (!STORE7_RDn) sil_tri.TS_LINE_0 = not(!sst_reg.XYGO);
  /*p30.WAXE*/ if (!STORE7_RDn) sil_tri.TS_LINE_1 = not(!sst_reg.XYNA);
  /*p30.WABU*/ if (!STORE7_RDn) sil_tri.TS_LINE_2 = not(!sst_reg.XAKU);
  /*p30.YPOZ*/ if (!STORE7_RDn) sil_tri.TS_LINE_3 = not(!sst_reg.YGUM);

  //----------------------------------------
  // sprite store 8

  /*p31.DUZE*/ wire DUZE = xor(sst_reg.EZUF, X4n);
  /*p31.DAGA*/ wire DAGA = xor(sst_reg.ENAD, X5n);
  /*p31.DAWU*/ wire DAWU = xor(sst_reg.EBOW, X6n);
  /*p31.EJAW*/ wire EJAW = xor(sst_reg.FYCA, X7n);
  /*p31.GOHO*/ wire GOHO = xor(sst_reg.GAVY, X0n);
  /*p31.GASU*/ wire GASU = xor(sst_reg.GYPU, X1n);
  /*p31.GABU*/ wire GABU = xor(sst_reg.GADY, X2n);
  /*p31.GAFE*/ wire GAFE = xor(sst_reg.GAZA, X3n);
  /*p31.DAMA*/ wire DAMA = nor(DUZE, DAGA, DAWU, EJAW);
  /*p31.FEHA*/ wire FEHA = nor(GOHO, GASU, GABU, GAFE);
  /*p29.EFYL*/ wire STORE8_MATCHn = nand(MATCH_EN, FEHA, DAMA);
  /*p29.GEKE*/ wire STORE8_MATCH = not(STORE8_MATCHn);
  /*p29.GYGA*/ wire STORE8_MATCH_OUT = or(STORE8_MATCH, STORE7_MATCH_OUT);
  /*p29.FOXA*/ wire FOXA = nor(STORE8_MATCHn, STORE7_MATCH_OUT);

  /*p29.EXUQ*/ sst_reg.SPRITE8_GET_SYNCn.set(WUTY_SPRITE_DONE, BYVA_VID_LINE_TRIG_d4n, FOXA);
  /*p29.GORO*/ wire GORO = or(DYBA_VID_LINE_TRIG_d4, sst_reg.SPRITE8_GET_SYNCn);
  /*p29.WUZO*/ wire WUZO = not(GORO);

  /*p29.CAHO*/ wire STORE8_CLK = or(STORE_EN, STORE8_SEL); // 1000
  /*p29.BUKA*/ wire BUKA = not(STORE8_CLK);
  /*p29.CEXU*/ wire CEXU = not(BUKA);
  /*p29.AKOL*/ wire AKOL = not(BUKA);
  /*p29.BYMY*/ wire BYMY = not(BUKA);
  /*p31.GAVY*/ sst_reg.GAVY.set(CEXU, WUZO, SPRITE_X0);
  /*p31.GYPU*/ sst_reg.GYPU.set(CEXU, WUZO, SPRITE_X1);
  /*p31.GADY*/ sst_reg.GADY.set(CEXU, WUZO, SPRITE_X2);
  /*p31.GAZA*/ sst_reg.GAZA.set(CEXU, WUZO, SPRITE_X3);
  /*p31.EZUF*/ sst_reg.EZUF.set(CEXU, WUZO, SPRITE_X4);
  /*p31.ENAD*/ sst_reg.ENAD.set(CEXU, WUZO, SPRITE_X5);
  /*p31.EBOW*/ sst_reg.EBOW.set(CEXU, WUZO, SPRITE_X6);
  /*p31.FYCA*/ sst_reg.FYCA.set(CEXU, WUZO, SPRITE_X7);

  /*p30.AXUV*/ sst_reg.AXUV.set(AKOL, sil_tri.TS_IDX_0 );
  /*p30.BADA*/ sst_reg.BADA.set(AKOL, sil_tri.TS_IDX_1 );
  /*p30.APEV*/ sst_reg.APEV.set(AKOL, sil_tri.TS_IDX_2 );
  /*p30.BADO*/ sst_reg.BADO.set(AKOL, sil_tri.TS_IDX_3 );
  /*p30.BEXY*/ sst_reg.BEXY.set(AKOL, sil_tri.TS_IDX_4 );
  /*p30.BYHE*/ sst_reg.BYHE.set(AKOL, sil_tri.TS_IDX_5 );
  /*p30.AFYM*/ sst_reg.AFYM.set(BYMY, sil_tri.TS_LINE_0);
  /*p30.AZAP*/ sst_reg.AZAP.set(BYMY, sil_tri.TS_LINE_1);
  /*p30.AFUT*/ sst_reg.AFUT.set(BYMY, sil_tri.TS_LINE_2);
  /*p30.AFYX*/ sst_reg.AFYX.set(BYMY, sil_tri.TS_LINE_3);

  /*p29.DENY*/ wire DENY = not(FOXA);
  /*p30.ADYB*/ if (DENY) sil_tri.TS_IDX_0  = sst_reg.AXUV;
  /*p30.APOB*/ if (DENY) sil_tri.TS_IDX_1  = sst_reg.BADA;
  /*p30.APYV*/ if (DENY) sil_tri.TS_IDX_2  = sst_reg.APEV;
  /*p30.AFEN*/ if (DENY) sil_tri.TS_IDX_3  = sst_reg.BADO;
  /*p30.AKYH*/ if (DENY) sil_tri.TS_IDX_4  = sst_reg.BEXY;
  /*p30.APOC*/ if (DENY) sil_tri.TS_IDX_5  = sst_reg.BYHE;
  /*p30.BUJY*/ if (DENY) sil_tri.TS_LINE_0 = sst_reg.AFYM;
  /*p30.BOSO*/ if (DENY) sil_tri.TS_LINE_1 = sst_reg.AZAP;
  /*p30.AHAC*/ if (DENY) sil_tri.TS_LINE_2 = sst_reg.AFUT;
  /*p30.BAZU*/ if (DENY) sil_tri.TS_LINE_3 = sst_reg.AFYX;

  //----------------------------------------
  // sprite store 9

  /*p31.YMAM*/ wire STORE9_MATCH0 = xor(sst_reg.STORE9_X0, X0n);
  /*p31.YTYP*/ wire STORE9_MATCH1 = xor(sst_reg.STORE9_X1, X1n);
  /*p31.YFOP*/ wire STORE9_MATCH2 = xor(sst_reg.STORE9_X2, X2n);
  /*p31.YVAC*/ wire STORE9_MATCH3 = xor(sst_reg.STORE9_X3, X3n);
  /*p31.ZYWU*/ wire STORE9_MATCH4 = xor(sst_reg.STORE9_X4, X4n);
  /*p31.ZUZA*/ wire STORE9_MATCH5 = xor(sst_reg.STORE9_X5, X5n);
  /*p31.ZEJO*/ wire STORE9_MATCH6 = xor(sst_reg.STORE9_X6, X6n);
  /*p31.ZEDA*/ wire STORE9_MATCH7 = xor(sst_reg.STORE9_X7, X7n);
  /*p31.YLEV*/ wire STORE9_MATCHA = nor(STORE9_MATCH0, STORE9_MATCH1, STORE9_MATCH2, STORE9_MATCH3);
  /*p31.YTUB*/ wire STORE9_MATCHB = nor(STORE9_MATCH4, STORE9_MATCH5, STORE9_MATCH6, STORE9_MATCH7);
  /*p29.YGEM*/ wire STORE9_MATCHn = nand(MATCH_EN, STORE9_MATCHA, STORE9_MATCHB);

  /*p29.CATO*/ wire STORE9_CLK = or(STORE_EN, STORE9_SEL); // 1001
  /*p29.DECU*/ wire STORE9_CLKn = not(STORE9_CLK);

  /*p29.GUZE*/ wire SPRITE9_GETn = nor(STORE9_MATCHn, STORE8_MATCH_OUT);
  /*p29.FONO*/ sst_reg.SPRITE9_GET_SYNCn.set(WUTY_SPRITE_DONE, BYVA_VID_LINE_TRIG_d4n, SPRITE9_GETn);

  /*p29.DUBU*/ wire STORE9_RST  = or(DYBA_VID_LINE_TRIG_d4, sst_reg.SPRITE9_GET_SYNCn);
  /*p29.DOSY*/ wire STORE9_RSTn = not(STORE9_RST);
    
  /*p29.WEME*/ wire STORE9_CLKa = not(STORE9_CLKn);
  /*p31.XUVY*/ sst_reg.STORE9_X0.set(STORE9_CLKa, STORE9_RSTn, SPRITE_X0);
  /*p31.XERE*/ sst_reg.STORE9_X1.set(STORE9_CLKa, STORE9_RSTn, SPRITE_X1);
  /*p31.XUZO*/ sst_reg.STORE9_X2.set(STORE9_CLKa, STORE9_RSTn, SPRITE_X2);
  /*p31.XEXA*/ sst_reg.STORE9_X3.set(STORE9_CLKa, STORE9_RSTn, SPRITE_X3);
  /*p31.YPOD*/ sst_reg.STORE9_X4.set(STORE9_CLKa, STORE9_RSTn, SPRITE_X4);
  /*p31.YROP*/ sst_reg.STORE9_X5.set(STORE9_CLKa, STORE9_RSTn, SPRITE_X5);
  /*p31.YNEP*/ sst_reg.STORE9_X6.set(STORE9_CLKa, STORE9_RSTn, SPRITE_X6);
  /*p31.YZOF*/ sst_reg.STORE9_X7.set(STORE9_CLKa, STORE9_RSTn, SPRITE_X7);

  /*p29.WUFA*/ wire STORE9_CLKb = not(STORE9_CLKn);
  /*p30.XUFO*/ sst_reg.STORE9_IDX0.set(STORE9_CLKb, sil_tri.TS_IDX_0);
  /*p30.XUTE*/ sst_reg.STORE9_IDX1.set(STORE9_CLKb, sil_tri.TS_IDX_1);
  /*p30.XOTU*/ sst_reg.STORE9_IDX2.set(STORE9_CLKb, sil_tri.TS_IDX_2);
  /*p30.XYFE*/ sst_reg.STORE9_IDX3.set(STORE9_CLKb, sil_tri.TS_IDX_3);
  /*p30.YZOR*/ sst_reg.STORE9_IDX4.set(STORE9_CLKb, sil_tri.TS_IDX_4);
  /*p30.YBER*/ sst_reg.STORE9_IDX5.set(STORE9_CLKb, sil_tri.TS_IDX_5);

  /*p29.FAKA*/ wire STORE9_CLKc = not(STORE9_CLKn);
  /*p30.DEWU*/ sst_reg.STORE9_LINE0.set(STORE9_CLKc, sil_tri.TS_LINE_0);
  /*p30.CANA*/ sst_reg.STORE9_LINE1.set(STORE9_CLKc, sil_tri.TS_LINE_1);
  /*p30.DYSY*/ sst_reg.STORE9_LINE2.set(STORE9_CLKc, sil_tri.TS_LINE_2);
  /*p30.FOFO*/ sst_reg.STORE9_LINE3.set(STORE9_CLKc, sil_tri.TS_LINE_3);

  /*p29.FADO*/ wire SPRITE9_GET = not(SPRITE9_GETn);
  /*p30.YHAL*/ if (SPRITE9_GET) sil_tri.TS_IDX_0  = not(!sst_reg.STORE9_IDX0);
  /*p30.YRAD*/ if (SPRITE9_GET) sil_tri.TS_IDX_1  = not(!sst_reg.STORE9_IDX1);
  /*p30.XYRA*/ if (SPRITE9_GET) sil_tri.TS_IDX_2  = not(!sst_reg.STORE9_IDX2);
  /*p30.YNEV*/ if (SPRITE9_GET) sil_tri.TS_IDX_3  = not(!sst_reg.STORE9_IDX3);
  /*p30.ZOJY*/ if (SPRITE9_GET) sil_tri.TS_IDX_4  = not(!sst_reg.STORE9_IDX4);
  /*p30.ZARO*/ if (SPRITE9_GET) sil_tri.TS_IDX_5  = not(!sst_reg.STORE9_IDX5);
  /*p30.CAWO*/ if (SPRITE9_GET) sil_tri.TS_LINE_0 = not(!sst_reg.STORE9_LINE0);
  /*p30.BYME*/ if (SPRITE9_GET) sil_tri.TS_LINE_1 = not(!sst_reg.STORE9_LINE1);
  /*p30.COHO*/ if (SPRITE9_GET) sil_tri.TS_LINE_2 = not(!sst_reg.STORE9_LINE2);
  /*p30.GATE*/ if (SPRITE9_GET) sil_tri.TS_LINE_3 = not(!sst_reg.STORE9_LINE3);

  //----------------------------------------
  // sprite store

  /*p29.DEZY*/ sst_reg.STORE_EN_SYNC.set(ZEME_xBxDxFxH, VID_RESETn, STORE_EN);

  /*p29.BAKY*/ wire SPRITES_FULL = and(sst_reg.SPRITE_COUNT1, sst_reg.SPRITE_COUNT3);
  /*p29.CAKE*/ wire SPRITE_COUNT_CLK = or(SPRITES_FULL, sst_reg.STORE_EN_SYNC);
  /*p28.AZYB*/ wire SPRITE_COUNT_RSTn = not(BYHA_VID_LINE_TRIG_d4n);
  /*p29.BESE*/ sst_reg.SPRITE_COUNT0.set(SPRITE_COUNT_CLK,  SPRITE_COUNT_RSTn, !sst_reg.SPRITE_COUNT0);
  /*p29.CUXY*/ sst_reg.SPRITE_COUNT1.set(sst_reg.SPRITE_COUNT0,     SPRITE_COUNT_RSTn, !sst_reg.SPRITE_COUNT1);
  /*p29.BEGO*/ sst_reg.SPRITE_COUNT2.set(sst_reg.SPRITE_COUNT1,     SPRITE_COUNT_RSTn, !sst_reg.SPRITE_COUNT2);
  /*p29.DYBE*/ sst_reg.SPRITE_COUNT3.set(sst_reg.SPRITE_COUNT2,     SPRITE_COUNT_RSTn, !sst_reg.SPRITE_COUNT3);

  /*p29.FEFY*/ wire FEFY = nand(STORE4_MATCHn, STORE3_MATCHn, STORE2_MATCHn, STORE1_MATCHn, STORE0_MATCHn);
  /*p29.FOVE*/ wire FOVE = nand(STORE9_MATCHn, STORE8_MATCHn, STORE7_MATCHn, STORE6_MATCHn, STORE5_MATCHn);
  /*p29.FEPO*/ wire FEPO_STORE_MATCH = or(FEFY, FOVE);

  /*p30.WENU*/ if (FEPO_STORE_MATCH) sil_tri.TS_LINE_0 = SPRITE_DELTA1; // order wrong
  /*p30.CUCU*/ if (FEPO_STORE_MATCH) sil_tri.TS_LINE_1 = SPRITE_DELTA2;
  /*p30.CUCA*/ if (FEPO_STORE_MATCH) sil_tri.TS_LINE_2 = SPRITE_DELTA0;
  /*p30.CEGA*/ if (FEPO_STORE_MATCH) sil_tri.TS_LINE_3 = SPRITE_DELTA3;

  //----------------------------------------

}


//-----------------------------------------------------------------------------
































#if 0

VidSignals VidRegisters::tick(const SysSignals& sys_sig,
                              const ClkSignals& clk_sig,
                              const RstSignals& rst_sig,
                              const VclkSignals& /*vid_clk*/,
                              const BusSignals& bus_sig,
                              const DecoderSignals& dec_sig,
                              const LcdSignals& lcd_sig,
                              const LcdRegisters& lcd_reg,
                              const SpriteSignals& spr_sig,
                              const SpriteStoreSignals& /*sst_sig*/,
                              BusTristates& bus_tri,
                              bool STORE_MATCH,
                              bool AVAP_SCAN_DONE_d0_TRIG,
                              bool WUTY_SPRITE_DONE) {

  VidSignals sig = {};

  /*p27.VYPO*/ wire VYPO = not(sys_sig.PIN_P10_B);

  /*p01.LAPE*/ wire LAPE_xBxDxFxH = not(clk_sig.ALET_AxCxExGx);
  /*p27.MYVO*/ wire MYVO_xBxDxFxH = not(clk_sig.ALET_AxCxExGx);
  /*p27.MOXE*/ wire MOXE_xBxDxFxH = not(clk_sig.ALET_AxCxExGx);
  /*p27.MEHE*/ wire MEHE_xBxDxFxH = not(clk_sig.ALET_AxCxExGx);
  /*p27.TAVA*/ wire TAVA_AxCxExGx = not(LAPE_xBxDxFxH);

  /*p24.LOBY*/ wire LOBY_RENDERINGn = not(XYMU_RENDERING_LATCH);
  /*p21.XUGU*/ wire X_167n = nand(X0, X1, X2, X5, X7); // 128 + 32 + 4 + 2 + 1 = 167
  /*p21.XANO*/ wire X_167 = not(X_167n);
  /*p21.XENA*/ wire STORE_MATCHn = not(STORE_MATCH);
  /*p21.WODU*/ wire WODU_RENDER_DONE = and(STORE_MATCHn, X_167);

  /*p01.CUNU*/ bool CUNU_RESETn = not(rst_sig.SYS_RESET);
  /*p01.XORE*/ bool XORE_RESET  = not(CUNU_RESETn);
  /*p01.WESY*/ bool WESY_RESET  = not(XORE_RESET);
  
  //----------

  bool TAVE;
  bool TEVO_CLK_STOPn;

  /*p27.SEKO*/ wire SEKO_WIN_TRIGGER = nor(WIN_MATCH_ONSCREEN_SYNC2, !WIN_MATCH_ONSCREEN_SYNC1);
  /*p27.SYLO*/ wire SYLO_WIN_MODEn = not(RYDY_WIN_MODE_LATCH);
  /*p27.TUXY*/ wire TUXY = nand(SOVY_WIN_MODE_SYNC, SYLO_WIN_MODEn);
  /*p27.SUZU*/ wire SUZU = not(TUXY);
  /*p27.ROMO*/ wire FRONT_PORCH = not(POKY_FRONT_PORCH_LATCHn);
  /*p27.SUVU*/ wire SUVU = nand(XYMU_RENDERING_LATCH, FRONT_PORCH, BG_SEQ_6, BG_SEQ_7);
  /*p27.TAVE*/ TAVE = not(SUVU);
  /*p27.TEVO*/ TEVO_CLK_STOPn = nor(SEKO_WIN_TRIGGER, SUZU, TAVE);

  /*p24.TOMU*/ wire TOMU_WIN_MODE  = not(SYLO_WIN_MODEn);
  /*p27.TUKU*/ wire TUKU_WIN_MODEn = not(TOMU_WIN_MODE);
  /*p27.SOWO*/ wire SOWO_SPRITE_FETCH_LATCHn = not(_SPRITE_FETCH_LATCH);

  /*p27.NUNY*/ wire NUNY_WIN_MODE_TRIG  = and(WIN_MODE_LATCH1, !NOPA_WIN_MODE_SYNC);
  /*p27.NYFO*/ wire NYFO_WIN_MODE_TRIGn = not(NUNY_WIN_MODE_TRIG);
  /*p27.MOSU*/ wire MOSU_WIN_MODE_TRIG  = not(NYFO_WIN_MODE_TRIGn);

  /*p27.NYXU*/ wire NYXU_BG_SEQ_RSTn = nor(AVAP_SCAN_DONE_d0_TRIG, MOSU_WIN_MODE_TRIG, TEVO_CLK_STOPn);
  /*p27.MOCE*/ wire MOCE_BG_SEQ_5n = nand(BG_SEQ_x1x3x5x7, BG_SEQ_xxxx4567, NYXU_BG_SEQ_RSTn);
  /*p27.LYRY*/ wire LYRY_BG_SEQ_5 = not(MOCE_BG_SEQ_5n);
  /*p27.TEKY*/ wire SPRITE_FETCH = and(STORE_MATCH, TUKU_WIN_MODEn, LYRY_BG_SEQ_5, SOWO_SPRITE_FETCH_LATCHn);
  /*p27.SOBU*/ _SPRITE_FETCH_SYNC1.set(TAVA_AxCxExGx, sys_sig.P10_Bn, SPRITE_FETCH);
  /*p27.SUDA*/ _SPRITE_FETCH_SYNC2.set(LAPE_xBxDxFxH, sys_sig.P10_Bn, _SPRITE_FETCH_SYNC1);
  /*p27.RYCE*/ wire RYCE_SPRITE_FETCH_TRIG = and(_SPRITE_FETCH_SYNC1, !_SPRITE_FETCH_SYNC2);
  /*p01.ROSY*/ wire ROSY_VID_RESET = not(rst_sig.VID_RESETn);
  /*p27.SECA*/ wire SPRITE_FETCH_BEGINn = nor(RYCE_SPRITE_FETCH_TRIG, ROSY_VID_RESET, lcd_sig.BYHA_VID_LINE_TRIG_d4n);
  /*p27.VEKU*/ wire SPRITE_FETCH_ENDn = nor(WUTY_SPRITE_DONE, TAVE);
  /*p27.TAKA*/ _SPRITE_FETCH_LATCH.sr_latch(!SPRITE_FETCH_BEGINn, !SPRITE_FETCH_ENDn); // polarity?

  /*p01.TOFU*/ wire VID_RESET3  = not(rst_sig.VID_RESETn);
  /*p21.WEGO*/ wire WEGO = or(VID_RESET3, RENDER_DONE_SYNC);
  /*p21.XYMU*/ XYMU_RENDERING_LATCH.sr_latch(AVAP_SCAN_DONE_d0_TRIG, WEGO);

  /*p21.TADY*/ wire TADY_X_RST = nor(lcd_sig.BYHA_VID_LINE_TRIG_d4n, VID_RESET3);
  /*p21.VOGA*/ RENDER_DONE_SYNC.set(clk_sig.ALET_AxCxExGx, TADY_X_RST, WODU_RENDER_DONE);


  /*p27.NENY*/ wire BG_SEQ_01xxxxxx = not(BG_SEQ_xx234567);

  /*p27.LUSU*/ wire BG_READ_VRAMn = not(LONY_LATCH);
  /*p27.LENA*/ wire BG_READ_VRAM  = not(BG_READ_VRAMn);
  /*p27.POTU*/ wire POTU          = and(BG_READ_VRAM, BG_SEQ_01xxxxxx);
  /*p25.XEZE*/ wire XEZE_WIN_MAP_READ  = nand(POTU, WIN_MODE_PORE);
  /*p26.ACEN*/ wire ACEN_BG_MAP_READ   = and (POTU, WIN_MODE_AXADn);


  /*p27.NOFU*/ wire BG_SEQ_0123xxxx = not(BG_SEQ_xxxx4567);
  /*p27.XUHA*/ wire XUHA_TILE_READ_AB = not(BG_SEQ_0123xxxx);

  /*p27.NETA*/ wire NETA_TILE_READ     = and(BG_READ_VRAM, BG_SEQ_xx234567);

  /*p25.XUCY*/ wire XUCY_WIN_TILE_READ = nand(TILE_READ, WIN_MODE_PORE);
  /*p26.ASUL*/ wire BG_TILE_READn = and (TILE_READ, WIN_MODE_AXADn);
  /*p26.BEJE*/ wire BEJE_BG_TILE_READ  = not(BG_TILE_READn);

  // counts to 5? polarity?
  /*p27.MOCE*/ wire BG_SEQ_5n = nand(BG_SEQ_x1x3x5x7, BG_SEQ_xxxx4567, NYXU_BG_SEQ_RSTn);
  /*p27.LYRY*/ wire BG_SEQ_5 = not(BG_SEQ_5n);

  /*p27.LEBO*/ wire BG_SEQ_CLK = nand(clk_sig.ALET_AxCxExGx, BG_SEQ_5n);
  /*p27.NYXU*/ wire BG_SEQ_RSTn = nor(AVAP_SCAN_DONE_d0_TRIG, MOSU_WIN_MODE_TRIG, TEVO_CLK_STOPn);
  /*p27.LAXU*/ BG_SEQ_x1x3x5x7.set( BG_SEQ_CLK,      BG_SEQ_RSTn, !BG_SEQ_x1x3x5x7);
  /*p27.MESU*/ BG_SEQ_xx23xx67.set(!BG_SEQ_x1x3x5x7, BG_SEQ_RSTn, !BG_SEQ_xx23xx67);
  /*p27.NYVA*/ BG_SEQ_xxxx4567.set(!BG_SEQ_xx23xx67, BG_SEQ_RSTn, !BG_SEQ_xxxx4567);

  /*p27.NAKO*/ wire BG_SEQ_01xx45xx = not(BG_SEQ_xx23xx67);
  /*p27.NOFU*/ wire BG_SEQ_0123xxxx = not(BG_SEQ_xxxx4567);
  /*p27.NOGU*/ wire BG_SEQ_xx234567 = nand(BG_SEQ_01xx45xx, BG_SEQ_0123xxxx);

  /*p24.NAFY*/ wire RENDERING_AND_NOT_WIN_TRIG = nor(MOSU_WIN_MODE_TRIG, LOBY_RENDERINGn);
  /*p24.NYKA*/ BG_SEQ_6.set(clk_sig.ALET_AxCxExGx, RENDERING_AND_NOT_WIN_TRIG, BG_SEQ_5);
  /*p24.PORY*/ BG_SEQ_7.set(MYVO_xBxDxFxH, RENDERING_AND_NOT_WIN_TRIG, BG_SEQ_6);

  /*p27.LYZU*/ BG_SEQ_x1x3x5x7_DELAY.set(clk_sig.ALET_AxCxExGx, XYMU_RENDERING_LATCH, BG_SEQ_x1x3x5x7);

  /*p27.LOVY*/ BG_SEQ5_SYNC.set(MYVO_xBxDxFxH, BG_SEQ_RSTn, BG_SEQ_5);
  /*p27.LURY*/ wire LURY = and(!BG_SEQ5_SYNC, XYMU_RENDERING_LATCH);
    
  /*p27.LONY*/ LONY_LATCH.sr_latch(LURY, NYXU_BG_SEQ_RSTn); // polarity? wait, are we sure this was a latch?
    
  // The first tile generated is thrown away. I'm calling that section of rendering the front porch.
  /*p24.PYGO*/ PYGO_TILE_DONE.set(clk_sig.ALET_AxCxExGx, XYMU_RENDERING_LATCH, BG_SEQ_7);
  /*p24.POKY*/ POKY_FRONT_PORCH_LATCHn.sr_latch(PYGO_TILE_DONE, LOBY_RENDERINGn);

  /*p27.LAXE*/ wire BG_SEQ0n = not(BG_SEQ_x1x3x5x7);
  /*p27.MYSO*/ wire BG_SEQ_TRIG_1357 = nor(LOBY_RENDERINGn, BG_SEQ0n, BG_SEQ_x1x3x5x7_DELAY);

  /*p27.NYDY*/ wire BG_SEQ_TRIG_3n = nand(BG_SEQ_TRIG_1357, BG_SEQ_xx23xx67, BG_SEQ_0123xxxx);
  /*p32.METE*/ wire BG_SEQ_TRIG_3 = not(BG_SEQ_TRIG_3n);
  /*p32.LOMA*/ wire BG_LATCH = not(BG_SEQ_TRIG_3);

  /*p27.MOFU*/ wire MOFU = and(BG_SEQ_TRIG_1357, BG_SEQ_01xx45xx);
  /*p32.LESO*/ wire LESO = not(MOFU);
  /*p32.AJAR*/ wire AJAR = not(LESO);
  /*p32.LABU*/ wire VRAM_TEMP_CLK = not(AJAR);

  //----------
  // FF44 LY

  /*p23.WAFU*/ wire FF44_RD = and(bus_sig.ASOT_CPURD, dec_sig.FF44);
  /*p23.VARO*/ wire FF44_RDn = not(FF44_RD);

  /*p23.WURY*/ wire LY0n = not(lcd_reg.Y0);
  /*p23.XEPO*/ wire LY1n = not(lcd_reg.Y1);
  /*p23.MYFA*/ wire LY2n = not(lcd_reg.Y2);
  /*p23.XUHY*/ wire LY3n = not(lcd_reg.Y3);
  /*p23.WATA*/ wire LY4n = not(lcd_reg.Y4);
  /*p23.XAGA*/ wire LY5n = not(lcd_reg.Y5);
  /*p23.XUCE*/ wire LY6n = not(lcd_reg.Y6);
  /*p23.XOWO*/ wire LY7n = not(lcd_reg.Y7);

  if (!FF44_RDn) bus_tri.set_data(
    /*p23.VEGA*/ not(LY0n),
    /*p23.WUVA*/ not(LY1n),
    /*p23.LYCO*/ not(LY2n),
    /*p23.WOJY*/ not(LY3n),
    /*p23.VYNE*/ not(LY4n),
    /*p23.WAMA*/ not(LY5n),
    /*p23.WAVO*/ not(LY6n),
    /*p23.WEZE*/ not(LY7n)
  );

  //----------
  // FF41 STAT stuff and interrupts

  /*p21.SEPA*/ wire FF41_WR = and(bus_sig.CUPA_CPUWR, dec_sig.FF41);
  /*p21.RYVE*/ wire RYVE_FF41_WRn = not(FF41_WR);

  /*p21.ROXE*/ INT_HBL_EN.set(RYVE_FF41_WRn, WESY_RESET, bus_tri.D0());
  /*p21.RUFO*/ INT_VBL_EN.set(RYVE_FF41_WRn, WESY_RESET, bus_tri.D1());
  /*p21.REFE*/ INT_OAM_EN.set(RYVE_FF41_WRn, WESY_RESET, bus_tri.D2());
  /*p21.RUGU*/ INT_LYC_EN.set(RYVE_FF41_WRn, WESY_RESET, bus_tri.D3());

  // 11: hblank   - rendering 0, vbl 0, oam 0
  // 10: vblank   - rendering 0, vbl 1, oam 0
  // 01: oam scan - rendering 0, vbl 0, oam 1
  // 00: render   - rendering 1, vbl 0, oam 0
  // so one of these has the wrong polarity

  /*p21.SADU*/ wire STAT_MODE0n = nor(XYMU_RENDERING_LATCH, lcd_sig.PARU_INT_VBL);
  /*p21.XATY*/ wire STAT_MODE1n = nor(XYMU_RENDERING_LATCH, spr_sig.ACYL_OAM_ADDR_PARSE);

  /*p21.TOBE*/ wire FF41_RDa = and(bus_sig.ASOT_CPURD, dec_sig.FF41);
  /*p21.VAVE*/ wire FF41_RDb = FF41_RDa; // buffer, not inverter?

  if (FF41_RDa) bus_tri.set_data(
    /*p21.TEBY*/ not(STAT_MODE0n),
    /*p21.WUGA*/ not(STAT_MODE1n),
    /*p21.SEGO*/ not(RUPO_LATCH_LYC_MATCH),
    /*p21.PUZO*/ INT_HBL_EN,
    /*p21.POFO*/ INT_VBL_EN,
    /*p21.SASY*/ INT_OAM_EN,
    /*p21.POTE*/ INT_LYC_EN
  );

  /*p21.PURE*/ wire LINE_DONEa = not(lcd_reg.NEW_LINE_d0a);
  /*p21.SELA*/ wire LINE_DONEo = not(LINE_DONEa);
  /*p21.TOLU*/ wire INT_VBLn = not(lcd_sig.PARU_INT_VBL);
  /*p21.TAPA*/ wire TAPA_INT_OAM = and(INT_VBLn, LINE_DONEo);
  /*p21.TARU*/ wire TARU_INT_HBL = and(INT_VBLn, WODU_RENDER_DONE);

  /*p21.SUKO*/ wire INT_STATb = amux4(INT_LYC_EN, ROPO_INT_LYC,
                                      INT_OAM_EN, TAPA_INT_OAM,
                                      INT_VBL_EN, lcd_sig.PARU_INT_VBL, // polarity?
                                      INT_HBL_EN, TARU_INT_HBL);

  /*p21.TUVA*/ wire INT_STATn = not(INT_STATb);
  /*p21.VOTY*/ wire INT_STAT  = not(INT_STATn);

  return sig;
}

//-----------------------------------------------------------------------------
// Fine x matcher for x scroll
// We stop the pipe as soon as there's a store match or we switch to window mode

void VidRegisters::fineMatch(const ClkSignals& clk_sig,
                             const ConfigRegisters& vid_reg2,
                             bool TEVO_CLK_STOPn,
                             bool STORE_MATCH) {

  /*p21.XUGU*/ wire XUGU_X_167n = nand(X0, X1, X2, X5, X7); // 128 + 32 + 4 + 2 + 1 = 167
  /*p21.XANO*/ wire XANO_X_167 = not(XUGU_X_167n);
  /*p21.XENA*/ wire XENA_STORE_MATCHn = not(STORE_MATCH);
  /*p21.WODU*/ wire WODU_RENDER_DONE = and(XENA_STORE_MATCHn, XANO_X_167);

  /*p27.MYVO*/ wire MYVO_xBxDxFxH = not(clk_sig.ALET_AxCxExGx);
  /*p27.MOXE*/ wire MOXE_xBxDxFxH = not(clk_sig.ALET_AxCxExGx);

  /*p24.VYBO*/ wire VYBO_AxCxExGx = nor(STORE_MATCH, WODU_RENDER_DONE, MYVO_xBxDxFxH);

  /*p27.SYLO*/ wire SYLO_WIN_MODEn = not(RYDY_WIN_MODE_LATCH);
  /*p24.TOMU*/ wire TOMU_WIN_MODE  = not(SYLO_WIN_MODEn);
  /*p24.SOCY*/ wire SOCY_WIN_MODEn = not(TOMU_WIN_MODE);
  /*p24.TYFA*/ wire TYFA_AxCxExGx  = and(SOCY_WIN_MODEn, POKY_FRONT_PORCH_LATCHn, VYBO_AxCxExGx);

  /*p24.SEGU*/ wire SEGU_xBxDxFxH = not(TYFA_AxCxExGx);
  /*p24.ROXO*/ wire ROXO_AxCxExGx = not(SEGU_xBxDxFxH);
  /*p27.ROCO*/ wire ROCO_AxCxExGx = not(SEGU_xBxDxFxH);
  /*p24.SACU*/ wire CLKPIPE_AxCxExGx = nor(SEGU_xBxDxFxH, FINE_MATCH_DUMP);

  /*p27.SUHA*/ wire FINE_MATCH0n = xor(vid_reg2.SCX0, FINE_CNT0);
  /*p27.SYBY*/ wire FINE_MATCH1n = xor(vid_reg2.SCX1, FINE_CNT1);
  /*p27.SOZU*/ wire FINE_MATCH2n = xor(vid_reg2.SCX2, FINE_CNT2);
  /*p27.RONE*/ wire FINE_MATCHn  = nand(FINE_MATCH_DUMP, FINE_MATCH0n, FINE_MATCH1n, FINE_MATCH2n);
  /*p27.POHU*/ wire FINE_MATCH   = not(FINE_MATCHn);

  /*p27.PUXA*/ FINE_MATCH_SYNC1.set(ROXO_AxCxExGx, XYMU_RENDERING_LATCH, FINE_MATCH);
  /*p27.NYZE*/ FINE_MATCH_SYNC2.set(MOXE_xBxDxFxH, XYMU_RENDERING_LATCH, FINE_MATCH_SYNC1);

  /*p27.PAHA*/ wire RENDERINGn = not(XYMU_RENDERING_LATCH);
  /*p27.POVA*/ wire FINE_MATCH_TRIG = and(FINE_MATCH_SYNC1, !FINE_MATCH_SYNC2);
  /*p??.ROXY*/ FINE_MATCH_DUMP.sr_latch(RENDERINGn, FINE_MATCH_TRIG);

  /*p25.ROPY*/ wire ROPY_RENDERINGn = not(XYMU_RENDERING_LATCH);
  /*p27.ROZE*/ wire FINE_COUNT_STOPn = nand(FINE_CNT0, FINE_CNT1, FINE_CNT2);
  /*p27.PASO*/ wire FINE_RST = nor(TEVO_CLK_STOPn, ROPY_RENDERINGn);
  /*p27.PECU*/ wire FINE_CLK = nand(ROXO_AxCxExGx, FINE_COUNT_STOPn);
  /*p27.RYKU*/ FINE_CNT0.set( FINE_CLK,  FINE_RST, !FINE_CNT0);
  /*p27.ROGA*/ FINE_CNT1.set(!FINE_CNT0, FINE_RST, !FINE_CNT1);
  /*p27.RUBU*/ FINE_CNT2.set(!FINE_CNT1, FINE_RST, !FINE_CNT2);
}

//-----------------------------------------------------------------------------
// Map coord counters for window mode

void VidRegisters::mapCounter(const RstSignals& rst_sig,
                              const LcdSignals& lcd_sig,
                              const ConfigRegisters& vid_reg2,
                              bool TEVO_CLK_STOPn) {
  /*p01.PYRY*/ wire PYRY_VID_RST = not(rst_sig.VID_RESETn);
  /*p27.XOFO*/ wire XOFO_WIN_RST = nand(vid_reg2.LCDC_WINEN, lcd_sig.XAHY_VID_LINE_TRIG_d4n, rst_sig.VID_RESETn);

  /*p27.NOCU*/ wire NOCU_WIN_MODEn = not(WIN_MODE_LATCH1);
  /*p27.PORE*/ wire PORE_WIN_MODE  = not(NOCU_WIN_MODEn);

  // something weird here, this doesn't look like a clock
  /*p27.VETU*/ wire VETU_X_CLK = and(TEVO_CLK_STOPn, PORE_WIN_MODE);
  /*p27.XACO*/ wire XACO_X_RSTn = not(XOFO_WIN_RST);

  /*p27.WYKA*/ MAP_X0.set(VETU_X_CLK, XACO_X_RSTn, !MAP_X0);
  /*p27.WODY*/ MAP_X1.set(!MAP_X0,    XACO_X_RSTn, !MAP_X1);
  /*p27.WOBO*/ MAP_X2.set(!MAP_X1,    XACO_X_RSTn, !MAP_X2);
  /*p27.WYKO*/ MAP_X3.set(!MAP_X2,    XACO_X_RSTn, !MAP_X3);
  /*p27.XOLO*/ MAP_X4.set(!MAP_X3,    XACO_X_RSTn, !MAP_X4);

  /*p27.WAZY*/ wire WAZY_Y_CLK = not(PORE_WIN_MODE);
  /*p27.REPU*/ wire REPU_RST  = nor(lcd_sig.PARU_INT_VBL, PYRY_VID_RST);   // schematic wrong, this is NOR
  /*p27.SYNY*/ wire SYNY_RSTn = not(REPU_RST);

  /*p27.VYNO*/ TILE_Y0.set(WAZY_Y_CLK, SYNY_RSTn, !TILE_Y0);
  /*p27.VUJO*/ TILE_Y1.set(!TILE_Y0,   SYNY_RSTn, !TILE_Y1);
  /*p27.VYMU*/ TILE_Y2.set(!TILE_Y1,   SYNY_RSTn, !TILE_Y2);

  /*p27.TUFU*/ MAP_Y0.set(!TILE_Y2,    SYNY_RSTn, !MAP_Y0);
  /*p27.TAXA*/ MAP_Y1.set(!MAP_Y0,     SYNY_RSTn, !MAP_Y1);
  /*p27.TOZO*/ MAP_Y2.set(!MAP_Y1,     SYNY_RSTn, !MAP_Y2);
  /*p27.TATE*/ MAP_Y3.set(!MAP_Y2,     SYNY_RSTn, !MAP_Y3);
  /*p27.TEKE*/ MAP_Y4.set(!MAP_Y3,     SYNY_RSTn, !MAP_Y4);
}

//-----------------------------------------------------------------------------
// Pixel counter. This is a little weird, presumably because it can tick at 4 mhz but not always?

void VidRegisters::pixelCounter(const RstSignals& rst_sig, const LcdSignals& lcd_sig, bool CLKPIPE) {

  /*p01.TOFU*/ wire VID_RESET3  = not(rst_sig.VID_RESETn);
  /*p21.TADY*/ wire TADY_X_RST = nor(lcd_sig.BYHA_VID_LINE_TRIG_d4n, VID_RESET3);

  /*p21.RYBO*/ wire RYBO = xor(X0, X1);
  /*p21.XUKE*/ wire XUKE = and(X0, X1);

  /*p21.XYLE*/ wire XYLE = and(X2, XUKE);
  /*p21.XEGY*/ wire XEGY = xor(X2, XUKE);
  /*p21.XORA*/ wire XORA = xor(X3, XYLE);

  /*p21.SAKE*/ wire SAKE = xor(X4, X5);
  /*p21.TYBA*/ wire TYBA = and(X4, X5);
  /*p21.SURY*/ wire SURY = and(X6, TYBA);
  /*p21.TYGE*/ wire TYGE = xor(X6, TYBA);
  /*p21.ROKU*/ wire ROKU = xor(X7, SURY);
  /*p21.TOCA*/ wire TOCA = not(X3);

  /*p21.XEHO*/ X0.set(CLKPIPE, TADY_X_RST, !X0);
  /*p21.SAVY*/ X1.set(CLKPIPE, TADY_X_RST, RYBO);
  /*p21.XODU*/ X2.set(CLKPIPE, TADY_X_RST, XEGY);
  /*p21.XYDO*/ X3.set(CLKPIPE, TADY_X_RST, XORA);
  /*p21.TUHU*/ X4.set(TOCA,    TADY_X_RST, !X4);
  /*p21.TUKY*/ X5.set(TOCA,    TADY_X_RST, SAKE);
  /*p21.TAKO*/ X6.set(TOCA,    TADY_X_RST, TYGE);
  /*p21.SYBE*/ X7.set(TOCA,    TADY_X_RST, ROKU);

  /*p21.XUGU*/ wire X_167n = nand(X0, X1, X2, X5, X7); // 128 + 32 + 4 + 2 + 1 = 167
  /*p21.XANO*/ wire X_167 = not(X_167n);
}

//-----------------------------------------------------------------------------
// LY compare

void VidRegisters::lyMatch(const BusSignals& bus_sig,
                           const DecoderSignals& dec_sig,
                           const VclkSignals& vid_clk,
                           const RstSignals& rst_sig,
                           const LcdRegisters& lcd_reg,
                           const ConfigRegisters& vid_reg2) {
  /*p21.SYFU*/ wire LY_MATCH7 = xor(lcd_reg.Y7, vid_reg2.LYC7);
  /*p21.TERY*/ wire LY_MATCH6 = xor(lcd_reg.Y6, vid_reg2.LYC6);
  /*p21.TUCY*/ wire LY_MATCH5 = xor(lcd_reg.Y5, vid_reg2.LYC5);
  /*p21.TYKU*/ wire LY_MATCH4 = xor(lcd_reg.Y4, vid_reg2.LYC4);
  /*p21.RASY*/ wire LY_MATCH3 = xor(lcd_reg.Y3, vid_reg2.LYC3);
  /*p21.REDA*/ wire LY_MATCH2 = xor(lcd_reg.Y2, vid_reg2.LYC2);
  /*p21.TYDE*/ wire LY_MATCH1 = xor(lcd_reg.Y1, vid_reg2.LYC1);
  /*p21.RYME*/ wire LY_MATCH0 = xor(lcd_reg.Y0, vid_reg2.LYC0);

  /*p21.SOVU*/ wire LY_MATCHA = nor(LY_MATCH7, LY_MATCH6, LY_MATCH5, LY_MATCH4);
  /*p21.SUBO*/ wire LY_MATCHB = nor(LY_MATCH3, LY_MATCH2, LY_MATCH1, LY_MATCH0);

  /*p21.RAPE*/ wire LY_MATCHn = nand(LY_MATCHA, LY_MATCHB);
  /*p21.PALY*/ wire LY_MATCHa = not(LY_MATCHn);

  /*p01.CUNU*/ bool CUNU_RESETn = not(rst_sig.SYS_RESET);
  /*p01.XORE*/ bool XORE_RESET  = not(CUNU_RESETn);
  /*p01.WESY*/ bool WESY_RESET  = not(XORE_RESET);

  /*p21.SEPA*/ wire FF41_WR = and(bus_sig.CUPA_CPUWR, dec_sig.FF41);
  /*p21.RYJU*/ wire FF41_WRn = not(FF41_WR);

  /*p21.ROPO*/ ROPO_INT_LYC.set(vid_clk.TALU_xBCDExxx, WESY_RESET, LY_MATCHa); // this is the lyc interrupt?
  /*p21.PAGO*/ wire PAGO_LYC_MATCH_RST = nor(WESY_RESET, FF41_WRn);  // schematic wrong, this is NOR
  /*p21.RUPO*/ RUPO_LATCH_LYC_MATCH.sr_latch(ROPO_INT_LYC, PAGO_LYC_MATCH_RST);
}

//-----------------------------------------------------------------------------

bool VidRegisters::winMatch(const ClkSignals& clk_sig,
                            const VclkSignals& vid_clk,
                            const RstSignals& rst_sig,
                            const LcdSignals& lcd_sig,
                            const LcdRegisters& lcd_reg,
                            const ConfigRegisters& vid_reg2,
                            bool SEGU_4M,
                            bool ROCO_4M) {
  
  //----------
  // Window Y match


  /*p27.NAZE*/ wire WY_MATCH0n = xor(lcd_reg.Y0, vid_reg2.WY0);
  /*p27.PEBO*/ wire WY_MATCH1n = xor(lcd_reg.Y1, vid_reg2.WY1);
  /*p27.POMO*/ wire WY_MATCH2n = xor(lcd_reg.Y2, vid_reg2.WY2);
  /*p27.NEVU*/ wire WY_MATCH3n = xor(lcd_reg.Y3, vid_reg2.WY3);
  /*p27.NOJO*/ wire WY_MATCH4n = xor(lcd_reg.Y4, vid_reg2.WY4);
  /*p27.PAGA*/ wire WY_MATCH5n = xor(lcd_reg.Y5, vid_reg2.WY5);
  /*p27.PEZO*/ wire WY_MATCH6n = xor(lcd_reg.Y6, vid_reg2.WY6);
  /*p27.NUPA*/ wire WY_MATCH7n = xor(lcd_reg.Y7, vid_reg2.WY7);

  // huh, the y matcher runs at 1 mhz but the x matcher runs at 4 mhz
  /*p27.PALO*/ wire WY_MATCH_HIn   = nand(vid_reg2.LCDC_WINEN, WY_MATCH4n, WY_MATCH5n, WY_MATCH6n, WY_MATCH7n);
  /*p27.NELE*/ wire WY_MATCH_HI    = not(WY_MATCH_HIn);
  /*p27.PAFU*/ wire WY_MATCHn      = nand(WY_MATCH_HI, WY_MATCH0n, WY_MATCH1n, WY_MATCH2n, WY_MATCH3n);
  /*p27.ROGE*/ wire WY_MATCH       = not(WY_MATCHn);
  /*p27.SARY*/ WY_MATCH_SYNC.set(vid_clk.TALU_xBCDExxx, rst_sig.VID_RESETn, WY_MATCH);

  //----------
  // Window X match

  // polarity or gates wrong
  /*p01.PYRY*/ wire VID_RESET4  = not(rst_sig.VID_RESETn);
  /*p27.REPU*/ wire IN_FRAME_Y  = nor(lcd_sig.PARU_INT_VBL, VID_RESET4);   // schematic wrong, this is NOR
  /*p27.REJO*/ wire WIN_CHECK_X = or(WY_MATCH_SYNC, IN_FRAME_Y); // another weird or gate. should be AND?

  /*p27.MYLO*/ wire WX_MATCH0n = xor(X0, vid_reg2.WX0);
  /*p27.PUWU*/ wire WX_MATCH1n = xor(X1, vid_reg2.WX1);
  /*p27.PUHO*/ wire WX_MATCH2n = xor(X2, vid_reg2.WX2);
  /*p27.NYTU*/ wire WX_MATCH3n = xor(X3, vid_reg2.WX3);
  /*p27.NEZO*/ wire WX_MATCH4n = xor(X4, vid_reg2.WX4);
  /*p27.NORY*/ wire WX_MATCH5n = xor(X5, vid_reg2.WX5);
  /*p27.NONO*/ wire WX_MATCH6n = xor(X6, vid_reg2.WX6);
  /*p27.PASE*/ wire WX_MATCH7n = xor(X7, vid_reg2.WX7);

  /*p27.PUKY*/ wire WX_MATCH_HIn = nand(WIN_CHECK_X, WX_MATCH4n, WX_MATCH5n, WX_MATCH6n, WX_MATCH7n);
  /*p27.NUFA*/ wire WX_MATCH_HI  = not (WX_MATCH_HIn);
  /*p27.NOGY*/ wire WIN_MATCHn   = nand(WX_MATCH_HI, WX_MATCH0n, WX_MATCH1n, WX_MATCH2n, WX_MATCH3n);
  /*p27.NUKO*/ wire WIN_MATCH    = not (WIN_MATCHn);

  /*p27.ROZE*/ wire FINE_COUNT_STOPn = nand(FINE_CNT0, FINE_CNT1, FINE_CNT2);
  /*p27.PANY*/ wire WIN_MATCH_ONSCREEN = nor(WIN_MATCH, FINE_COUNT_STOPn);

  /*p27.RYFA*/ WIN_MATCH_ONSCREEN_SYNC1.set(SEGU_4M,               XYMU_RENDERING_LATCH, WIN_MATCH_ONSCREEN);
  /*p27.RENE*/ WIN_MATCH_ONSCREEN_SYNC2.set(clk_sig.ALET_AxCxExGx, XYMU_RENDERING_LATCH, WIN_MATCH_ONSCREEN_SYNC1);

  /*p27.MEHE*/ wire MEHE_xBxDxFxH = not(clk_sig.ALET_AxCxExGx);

  /*p27.PYCO*/ WIN_MATCH_SYNC1.set(ROCO_4M,       rst_sig.VID_RESETn, WIN_MATCH);
  /*p27.NUNU*/ WIN_MATCH_SYNC2.set(MEHE_xBxDxFxH, rst_sig.VID_RESETn, WIN_MATCH_SYNC1);

  /*p27.XOFO*/ wire WIN_MODE_RST = nand(vid_reg2.LCDC_WINEN, lcd_sig.XAHY_VID_LINE_TRIG_d4n, rst_sig.VID_RESETn);
  /*p27.PYNU*/ WIN_MODE_LATCH1.sr_latch(WIN_MATCH_SYNC2, WIN_MODE_RST);

  /*p27.NOCU*/ wire WIN_MODE_NOCUn = not(WIN_MODE_LATCH1);
  /*p27.PORE*/ wire WIN_MODE_PORE  = not(WIN_MODE_NOCUn);
  /*p26.AXAD*/ wire WIN_MODE_AXADn = not(WIN_MODE_PORE);

  /*p27.NOPA*/ NOPA_WIN_MODE_SYNC.set(clk_sig.ALET_AxCxExGx, rst_sig.VID_RESETn, WIN_MODE_LATCH1);
  /*p27.NUNY*/ wire NUNY_WIN_MODE_TRIG = and(WIN_MODE_LATCH1, !NOPA_WIN_MODE_SYNC);

  // PUKU/RYDY form a NOR latch. WIN_MODE_TRIG is SET, (VID_RESET | BG_SEQ_7) is RESET.
  ///*p27.PUKU*/ PUKU = nor(RYDY, WIN_MODE_TRIG);
  ///*p27.RYDY*/ RYDY = nor(PUKU, rst_reg.VID_RESET4, BG_SEQ_7);

  /*p27.RYDY*/ RYDY_WIN_MODE_LATCH.sr_latch(NUNY_WIN_MODE_TRIG, VID_RESET4 || BG_SEQ_7);

  /*p27.SOVY*/ SOVY_WIN_MODE_SYNC.set(clk_sig.ALET_AxCxExGx, rst_sig.VID_RESETn, RYDY_WIN_MODE_LATCH);

  return WIN_MATCH;
}

#endif



#if 0
VramBus VramBus::tick(const SysSignals& sys_sig,
                      const LcdRegisters& lcd_reg,
                      const ConfigRegisters& vid_cfg,
                      const SpriteSignals& spr_sig,
                      const DmaSignals& dma_sig,
                      const DmaRegisters& dma_reg,
                      const OamRegisters& oam_reg,
                      const VidSignals& vid_sig,
                      const VidRegisters& vid_reg,
                      const PixelPipeSignals& pix_sig,
                      const JoypadPins& joy_pins,
                      const SpriteTristate& spr_tri,
                      const BusTristates& bus_tri) {

  VramBus vram_bus = {};

  //----------
  // vid x/y + scroll x/y

  /*p26.FAFO*/ wire TILE_Y0C  = add_c(lcd_reg.Y0, vid_cfg.SCY0, 0);
  /*p26.EMUX*/ wire TILE_Y1C  = add_c(lcd_reg.Y1, vid_cfg.SCY1, TILE_Y0C);
  /*p26.ECAB*/ wire TILE_Y2C  = add_c(lcd_reg.Y2, vid_cfg.SCY2, TILE_Y1C);
  /*p26.ETAM*/ wire MAP_Y0C   = add_c(lcd_reg.Y3, vid_cfg.SCY3, TILE_Y2C);
  /*p26.DOTO*/ wire MAP_Y1C   = add_c(lcd_reg.Y4, vid_cfg.SCY4, MAP_Y0C);
  /*p26.DABA*/ wire MAP_Y2C   = add_c(lcd_reg.Y5, vid_cfg.SCY5, MAP_Y1C);
  /*p26.EFYK*/ wire MAP_Y3C   = add_c(lcd_reg.Y6, vid_cfg.SCY6, MAP_Y2C);
  /*p26.EJOK*/ wire MAP_Y4C   = add_c(lcd_reg.Y7, vid_cfg.SCY7, MAP_Y3C);

  /*p26.FAFO*/ wire TILE_Y0S  = add_s(lcd_reg.Y0, vid_cfg.SCY0, 0);
  /*p26.EMUX*/ wire TILE_Y1S  = add_s(lcd_reg.Y1, vid_cfg.SCY1, TILE_Y0C);
  /*p26.ECAB*/ wire TILE_Y2S  = add_s(lcd_reg.Y2, vid_cfg.SCY2, TILE_Y1C);
  /*p26.ETAM*/ wire MAP_Y0S   = add_s(lcd_reg.Y3, vid_cfg.SCY3, TILE_Y2C);
  /*p26.DOTO*/ wire MAP_Y1S   = add_s(lcd_reg.Y4, vid_cfg.SCY4, MAP_Y0C);
  /*p26.DABA*/ wire MAP_Y2S   = add_s(lcd_reg.Y5, vid_cfg.SCY5, MAP_Y1C);
  /*p26.EFYK*/ wire MAP_Y3S   = add_s(lcd_reg.Y6, vid_cfg.SCY6, MAP_Y2C);
  /*p26.EJOK*/ wire MAP_Y4S   = add_s(lcd_reg.Y7, vid_cfg.SCY7, MAP_Y3C);

  /*p26.ATAD*/ wire TILE_X0C  = add_c(vid_reg.X0, vid_cfg.SCX0, 0);
  /*p26.BEHU*/ wire TILE_X1C  = add_c(vid_reg.X1, vid_cfg.SCX1, TILE_X0C);
  /*p26.APYH*/ wire TILE_X2C  = add_c(vid_reg.X2, vid_cfg.SCX2, TILE_X1C);
  /*p26.BABE*/ wire MAP_X0C   = add_c(vid_reg.X3, vid_cfg.SCX3, TILE_X2C);
  /*p26.ABOD*/ wire MAP_X1C   = add_c(vid_reg.X4, vid_cfg.SCX4, MAP_X0C);
  /*p26.BEWY*/ wire MAP_X2C   = add_c(vid_reg.X5, vid_cfg.SCX5, MAP_X1C);
  /*p26.BYCA*/ wire MAP_X3C   = add_c(vid_reg.X6, vid_cfg.SCX6, MAP_X2C);
  /*p26.ACUL*/ wire MAP_X4C   = add_c(vid_reg.X7, vid_cfg.SCX7, MAP_X3C);

  /*p26.ATAD*/ wire TILE_X0S  = add_s(vid_reg.X0, vid_cfg.SCX0, 0);
  /*p26.BEHU*/ wire TILE_X1S  = add_s(vid_reg.X1, vid_cfg.SCX1, TILE_X0C);
  /*p26.APYH*/ wire TILE_X2S  = add_s(vid_reg.X2, vid_cfg.SCX2, TILE_X1C);
  /*p26.BABE*/ wire MAP_X0S   = add_s(vid_reg.X3, vid_cfg.SCX3, TILE_X2C);
  /*p26.ABOD*/ wire MAP_X1S   = add_s(vid_reg.X4, vid_cfg.SCX4, MAP_X0C);
  /*p26.BEWY*/ wire MAP_X2S   = add_s(vid_reg.X5, vid_cfg.SCX5, MAP_X1C);
  /*p26.BYCA*/ wire MAP_X3S   = add_s(vid_reg.X6, vid_cfg.SCX6, MAP_X2C);
  /*p26.ACUL*/ wire MAP_X4S   = add_s(vid_reg.X7, vid_cfg.SCX7, MAP_X3C);

  //----------

  /*p29.WUKY*/ wire FLIP_Y = not(oam_reg.REG_OAM_A6);
  /*p29.XUQU*/ wire SPRITE_AB = not(!spr_sig.VONU_SEQ_xxx34xn);
  /*p29.FUFO*/ wire LCDC_SPSIZEn = not(vid_cfg.LCDC_SPSIZE);

  /*p29.WAGO*/ wire WAGO      = xor(FLIP_Y, spr_tri.TS_LINE_0);
  /*p29.CYVU*/ wire SPRITE_Y0 = xor(FLIP_Y, spr_tri.TS_LINE_1);
  /*p29.BORE*/ wire SPRITE_Y1 = xor(FLIP_Y, spr_tri.TS_LINE_2);
  /*p29.BUVY*/ wire SPRITE_Y2 = xor(FLIP_Y, spr_tri.TS_LINE_3);
  /*p29.GEJY*/ wire SPRITE_Y3 = amux2(LCDC_SPSIZEn, !oam_reg.REG_OAM_B0, vid_cfg.LCDC_SPSIZE,  WAGO);

  /*p29.ABON*/ wire SPRITE_READn = not(spr_sig.TEXY_SPRITE_READ);
  /*p29.ABEM*/ if (!SPRITE_READn) vram_bus.MA00 = SPRITE_AB;
  /*p29.BAXE*/ if (!SPRITE_READn) vram_bus.MA01 = SPRITE_Y0;
  /*p29.ARAS*/ if (!SPRITE_READn) vram_bus.MA02 = SPRITE_Y1;
  /*p29.AGAG*/ if (!SPRITE_READn) vram_bus.MA03 = SPRITE_Y2;
  /*p29.FAMU*/ if (!SPRITE_READn) vram_bus.MA04 = SPRITE_Y3;
  /*p29.FUGY*/ if (!SPRITE_READn) vram_bus.MA05 = oam_reg.REG_OAM_B1;
  /*p29.GAVO*/ if (!SPRITE_READn) vram_bus.MA06 = oam_reg.REG_OAM_B2;
  /*p29.WYGA*/ if (!SPRITE_READn) vram_bus.MA07 = oam_reg.REG_OAM_B3;
  /*p29.WUNE*/ if (!SPRITE_READn) vram_bus.MA08 = oam_reg.REG_OAM_B4;
  /*p29.GOTU*/ if (!SPRITE_READn) vram_bus.MA09 = oam_reg.REG_OAM_B5;
  /*p29.GEGU*/ if (!SPRITE_READn) vram_bus.MA10 = oam_reg.REG_OAM_B6;
  /*p29.XEHE*/ if (!SPRITE_READn) vram_bus.MA11 = oam_reg.REG_OAM_B7;
  /*p29.DYSO*/ if (!SPRITE_READn) vram_bus.MA12 = joy_pins.P10_B;   // sprites always in low half of tile store

  /*p25.VUZA*/ wire TILE_BANK = nor(vid_cfg.LCDC_BGTILE, pix_sig.BG_PIX_B7); // register reused

  /*p25.XONU*/ if (vid_sig.XUCY_WIN_TILE_READ) vram_bus.MA00 = vid_sig.XUHA_TILE_READ_AB;
  /*p25.WUDO*/ if (vid_sig.XUCY_WIN_TILE_READ) vram_bus.MA01 = vid_reg.TILE_Y0;
  /*p25.WAWE*/ if (vid_sig.XUCY_WIN_TILE_READ) vram_bus.MA02 = vid_reg.TILE_Y1;
  /*p25.WOLU*/ if (vid_sig.XUCY_WIN_TILE_READ) vram_bus.MA03 = vid_reg.TILE_Y2;

  /*p26.ASUM*/ if (vid_sig.BEJE_BG_TILE_READ) vram_bus.MA00 = vid_sig.XUHA_TILE_READ_AB;
  /*p26.EVAD*/ if (vid_sig.BEJE_BG_TILE_READ) vram_bus.MA01 = TILE_Y0S;
  /*p26.DAHU*/ if (vid_sig.BEJE_BG_TILE_READ) vram_bus.MA02 = TILE_Y1S;
  /*p26.DODE*/ if (vid_sig.BEJE_BG_TILE_READ) vram_bus.MA03 = TILE_Y2S;

  /*p32.RAWU*/ bool BG_PIX_B0;
  /*p32.POZO*/ bool BG_PIX_B1;
  /*p32.PYZO*/ bool BG_PIX_B2;
  /*p32.POXA*/ bool BG_PIX_B3;
  /*p32.PULO*/ bool BG_PIX_B4;
  /*p32.POJU*/ bool BG_PIX_B5;
  /*p32.POWY*/ bool BG_PIX_B6;
  /*p32.PYJU*/ bool BG_PIX_B7;

  /*p25.VAPY*/ if (vid_sig.NETA_TILE_READ) vram_bus.MA04 = pix_sig.BG_PIX_B0; // register reused
  /*p25.SEZU*/ if (vid_sig.NETA_TILE_READ) vram_bus.MA05 = pix_sig.BG_PIX_B1;
  /*p25.VEJY*/ if (vid_sig.NETA_TILE_READ) vram_bus.MA06 = pix_sig.BG_PIX_B2;
  /*p25.RUSA*/ if (vid_sig.NETA_TILE_READ) vram_bus.MA07 = pix_sig.BG_PIX_B3;
  /*p25.ROHA*/ if (vid_sig.NETA_TILE_READ) vram_bus.MA08 = pix_sig.BG_PIX_B4;
  /*p25.RESO*/ if (vid_sig.NETA_TILE_READ) vram_bus.MA09 = pix_sig.BG_PIX_B5;
  /*p25.SUVO*/ if (vid_sig.NETA_TILE_READ) vram_bus.MA10 = pix_sig.BG_PIX_B6;
  /*p25.TOBO*/ if (vid_sig.NETA_TILE_READ) vram_bus.MA11 = pix_sig.BG_PIX_B7;
  /*p25.VUZA*/ if (vid_sig.NETA_TILE_READ) vram_bus.MA12 = TILE_BANK;

  // small tri
  /*p25.WUKO*/ wire WIN_MAP_READn = not(vid_sig.XEZE_WIN_MAP_READ);
  /*p27.XEJA*/ if (!WIN_MAP_READn) vram_bus.MA00 = vid_reg.MAP_X0;
  /*p27.XAMO*/ if (!WIN_MAP_READn) vram_bus.MA01 = vid_reg.MAP_X1;
  /*p27.XAHE*/ if (!WIN_MAP_READn) vram_bus.MA02 = vid_reg.MAP_X2;
  /*p27.XULO*/ if (!WIN_MAP_READn) vram_bus.MA03 = vid_reg.MAP_X3;
  /*p27.WUJU*/ if (!WIN_MAP_READn) vram_bus.MA04 = vid_reg.MAP_X4;
  /*p27.VYTO*/ if (!WIN_MAP_READn) vram_bus.MA05 = vid_reg.MAP_Y0;
  /*p27.VEHA*/ if (!WIN_MAP_READn) vram_bus.MA06 = vid_reg.MAP_Y1;
  /*p27.VACE*/ if (!WIN_MAP_READn) vram_bus.MA07 = vid_reg.MAP_Y2;
  /*p27.VOVO*/ if (!WIN_MAP_READn) vram_bus.MA08 = vid_reg.MAP_Y3;
  /*p27.VULO*/ if (!WIN_MAP_READn) vram_bus.MA09 = vid_reg.MAP_Y4;
  /*p27.VEVY*/ if (!WIN_MAP_READn) vram_bus.MA10 = vid_cfg.LCDC_WINMAP;
  /*p27.VEZA*/ if (!WIN_MAP_READn) vram_bus.MA11 = sys_sig.P10_Bn;
  /*p27.VOGU*/ if (!WIN_MAP_READn) vram_bus.MA12 = sys_sig.P10_Bn;

  // small tri
  /*p26.BAFY*/ wire BG_MAP_READn  = not(vid_sig.ACEN_BG_MAP_READ);
  /*p26.AXEP*/ if (!BG_MAP_READn) vram_bus.MA00 = MAP_X0S;
  /*p26.AFEB*/ if (!BG_MAP_READn) vram_bus.MA01 = MAP_X1S;
  /*p26.ALEL*/ if (!BG_MAP_READn) vram_bus.MA02 = MAP_X2S;
  /*p26.COLY*/ if (!BG_MAP_READn) vram_bus.MA03 = MAP_X3S;
  /*p26.AJAN*/ if (!BG_MAP_READn) vram_bus.MA04 = MAP_X4S;
  /*p26.DUHO*/ if (!BG_MAP_READn) vram_bus.MA05 = MAP_Y0S;
  /*p26.CASE*/ if (!BG_MAP_READn) vram_bus.MA06 = MAP_Y1S;
  /*p26.CYPO*/ if (!BG_MAP_READn) vram_bus.MA07 = MAP_Y2S;
  /*p26.CETA*/ if (!BG_MAP_READn) vram_bus.MA08 = MAP_Y3S;
  /*p26.DAFE*/ if (!BG_MAP_READn) vram_bus.MA09 = MAP_Y4S;
  /*p26.AMUV*/ if (!BG_MAP_READn) vram_bus.MA10 = vid_cfg.LCDC_BGMAP;
  /*p26.COVE*/ if (!BG_MAP_READn) vram_bus.MA11 = sys_sig.P10_Bn;
  /*p26.COXO*/ if (!BG_MAP_READn) vram_bus.MA12 = sys_sig.P10_Bn;

  /*p04.AHOC*/ wire DMA_READ_VRAMo = not(dma_sig.DMA_READ_VRAM);
  /*p04.ECAL*/ if (!DMA_READ_VRAMo) vram_bus.MA00 = dma_reg.DMA_A00;
  /*p04.EGEZ*/ if (!DMA_READ_VRAMo) vram_bus.MA01 = dma_reg.DMA_A01;
  /*p04.FUHE*/ if (!DMA_READ_VRAMo) vram_bus.MA02 = dma_reg.DMA_A02;
  /*p04.FYZY*/ if (!DMA_READ_VRAMo) vram_bus.MA03 = dma_reg.DMA_A03;
  /*p04.DAMU*/ if (!DMA_READ_VRAMo) vram_bus.MA04 = dma_reg.DMA_A04;
  /*p04.DAVA*/ if (!DMA_READ_VRAMo) vram_bus.MA05 = dma_reg.DMA_A05;
  /*p04.ETEG*/ if (!DMA_READ_VRAMo) vram_bus.MA06 = dma_reg.DMA_A06;
  /*p04.EREW*/ if (!DMA_READ_VRAMo) vram_bus.MA07 = dma_reg.DMA_A07;
  /*p04.EVAX*/ if (!DMA_READ_VRAMo) vram_bus.MA08 = dma_reg.DMA_A08;
  /*p04.DUVE*/ if (!DMA_READ_VRAMo) vram_bus.MA09 = dma_reg.DMA_A09;
  /*p04.ERAF*/ if (!DMA_READ_VRAMo) vram_bus.MA10 = dma_reg.DMA_A10;
  /*p04.FUSY*/ if (!DMA_READ_VRAMo) vram_bus.MA11 = dma_reg.DMA_A11;
  /*p04.EXYF*/ if (!DMA_READ_VRAMo) vram_bus.MA12 = dma_reg.DMA_A12;

  /*p25.XANE*/ wire VRAM_LOCKn = nor(dma_sig.DMA_READ_VRAM, vid_reg.XYMU_RENDERING_LATCH);
  /*p25.XEDU*/ wire VRAM_LOCK = not(VRAM_LOCKn);

  /*p25.XAKY*/ if (!VRAM_LOCK) vram_bus.MA00 = bus_tri.A00();
  /*p25.XUXU*/ if (!VRAM_LOCK) vram_bus.MA01 = bus_tri.A01();
  /*p25.XYNE*/ if (!VRAM_LOCK) vram_bus.MA02 = bus_tri.A02();
  /*p25.XODY*/ if (!VRAM_LOCK) vram_bus.MA03 = bus_tri.A03();
  /*p25.XECA*/ if (!VRAM_LOCK) vram_bus.MA04 = bus_tri.A04();
  /*p25.XOBA*/ if (!VRAM_LOCK) vram_bus.MA05 = bus_tri.A05();
  /*p25.XOPO*/ if (!VRAM_LOCK) vram_bus.MA06 = bus_tri.A06();
  /*p25.XYBO*/ if (!VRAM_LOCK) vram_bus.MA07 = bus_tri.A07();
  /*p25.RYSU*/ if (!VRAM_LOCK) vram_bus.MA08 = bus_tri.A08();
  /*p25.RESE*/ if (!VRAM_LOCK) vram_bus.MA09 = bus_tri.A09();
  /*p25.RUSE*/ if (!VRAM_LOCK) vram_bus.MA10 = bus_tri.A10();
  /*p25.RYNA*/ if (!VRAM_LOCK) vram_bus.MA11 = bus_tri.A11();
  /*p25.RUMO*/ if (!VRAM_LOCK) vram_bus.MA12 = bus_tri.A12();

  // big tri
  /*p25.TEME*/ if (!ctl.CBUS_TO_VBUSn) vram_bus.MD0 = bus_tri.D0();
  /*p25.TEWU*/ if (!ctl.CBUS_TO_VBUSn) vram_bus.MD1 = bus_tri.D1();
  /*p25.TYGO*/ if (!ctl.CBUS_TO_VBUSn) vram_bus.MD2 = bus_tri.D2();
  /*p25.SOTE*/ if (!ctl.CBUS_TO_VBUSn) vram_bus.MD3 = bus_tri.D3();
  /*p25.SEKE*/ if (!ctl.CBUS_TO_VBUSn) vram_bus.MD4 = bus_tri.D4();
  /*p25.RUJO*/ if (!ctl.CBUS_TO_VBUSn) vram_bus.MD5 = bus_tri.D5();
  /*p25.TOFA*/ if (!ctl.CBUS_TO_VBUSn) vram_bus.MD6 = bus_tri.D6();
  /*p25.SUZA*/ if (!ctl.CBUS_TO_VBUSn) vram_bus.MD7 = bus_tri.D7();

  // small tri
  /*p25.RODY*/ if (!ctl.VEXT_TO_VBUSn) vram_bus.MD0 = vram_pins.MD0_C;
  /*p25.REBA*/ if (!ctl.VEXT_TO_VBUSn) vram_bus.MD1 = vram_pins.MD1_C;
  /*p25.RYDO*/ if (!ctl.VEXT_TO_VBUSn) vram_bus.MD2 = vram_pins.MD2_C;
  /*p25.REMO*/ if (!ctl.VEXT_TO_VBUSn) vram_bus.MD3 = vram_pins.MD3_C;
  /*p25.ROCE*/ if (!ctl.VEXT_TO_VBUSn) vram_bus.MD4 = vram_pins.MD4_C;
  /*p25.ROPU*/ if (!ctl.VEXT_TO_VBUSn) vram_bus.MD5 = vram_pins.MD5_C;
  /*p25.RETA*/ if (!ctl.VEXT_TO_VBUSn) vram_bus.MD6 = vram_pins.MD6_C;
  /*p25.RAKU*/ if (!ctl.VEXT_TO_VBUSn) vram_bus.MD7 = vram_pins.MD7_C;

  return vram_bus;
}

//-----------------------------------------------------------------------------
// VRAM data driver

void Vram_tick2(const BusTristates& /*bus*/, const BusSignals& /*ctl*/, const Vram& /*vram*/, Vram& /*next*/, BusTristates& /*bus_out*/) {

  /*p25.RUGA*/ if (!ctl.MD_TO_Dn) bus_out.D0 = /*p25.RERY*/ not(vram.MD0);
  /*p25.ROTA*/ if (!ctl.MD_TO_Dn) bus_out.D1 = /*p25.RUNA*/ not(vram.MD1);
  /*p25.RYBU*/ if (!ctl.MD_TO_Dn) bus_out.D2 = /*p25.RONA*/ not(vram.MD2);
  /*p25.RAJU*/ if (!ctl.MD_TO_Dn) bus_out.D3 = /*p25.RUNO*/ not(vram.MD3);
  /*p25.TYJA*/ if (!ctl.MD_TO_Dn) bus_out.D4 = /*p25.SANA*/ not(vram.MD4);
  /*p25.REXU*/ if (!ctl.MD_TO_Dn) bus_out.D5 = /*p25.RORO*/ not(vram.MD5);
  /*p25.RUPY*/ if (!ctl.MD_TO_Dn) bus_out.D6 = /*p25.RABO*/ not(vram.MD6);
  /*p25.TOKU*/ if (!ctl.MD_TO_Dn) bus_out.D7 = /*p25.SAME*/ not(vram.MD7);

}
#endif












#if 0
void PixelPipeRegisters::tick(const SysSignals& sys_sig,
                              const ConfigRegisters& vid_reg,
                              const VramBus& vram_bus,
                              const OamRegisters& oam_reg,
                              /*p32.LOMA*/ wire BG_LATCH,
                              /*p32.LABU*/ wire VRAM_TEMP_CLK,
                              /*p29.XADO*/ wire SPRITE_VRAM_LATCH_A,
                              /*p29.PUCO*/ wire SPRITE_VRAM_LATCH_B,
                              /*p29.WUTY*/ wire SPRITE_DONE,
                              /*p29.XONO*/ wire FLIP_X,
                              /*p24.SACU*/ wire CLKPIPE,
                              /*p27.NYXU*/ wire NYXU_BG_SEQ_RSTn) {

  /*p32.LEGU*/ BG_PIX_A0.tp_latch(BG_LATCH, vram_bus.MD0);
  /*p32.NUDU*/ BG_PIX_A1.tp_latch(BG_LATCH, vram_bus.MD1);
  /*p32.MUKU*/ BG_PIX_A2.tp_latch(BG_LATCH, vram_bus.MD2);
  /*p32.LUZO*/ BG_PIX_A3.tp_latch(BG_LATCH, vram_bus.MD3);
  /*p32.MEGU*/ BG_PIX_A4.tp_latch(BG_LATCH, vram_bus.MD4);
  /*p32.MYJY*/ BG_PIX_A5.tp_latch(BG_LATCH, vram_bus.MD5);
  /*p32.NASA*/ BG_PIX_A6.tp_latch(BG_LATCH, vram_bus.MD6);
  /*p32.NEFO*/ BG_PIX_A7.tp_latch(BG_LATCH, vram_bus.MD7);

  /*p32.RAWU*/ BG_PIX_B0.set(VRAM_TEMP_CLK, sys_sig.P10_Bn, vram_bus.MD0);
  /*p32.POZO*/ BG_PIX_B1.set(VRAM_TEMP_CLK, sys_sig.P10_Bn, vram_bus.MD1);
  /*p32.PYZO*/ BG_PIX_B2.set(VRAM_TEMP_CLK, sys_sig.P10_Bn, vram_bus.MD2);
  /*p32.POXA*/ BG_PIX_B3.set(VRAM_TEMP_CLK, sys_sig.P10_Bn, vram_bus.MD3);
  /*p32.PULO*/ BG_PIX_B4.set(VRAM_TEMP_CLK, sys_sig.P10_Bn, vram_bus.MD4);
  /*p32.POJU*/ BG_PIX_B5.set(VRAM_TEMP_CLK, sys_sig.P10_Bn, vram_bus.MD5);
  /*p32.POWY*/ BG_PIX_B6.set(VRAM_TEMP_CLK, sys_sig.P10_Bn, vram_bus.MD6);
  /*p32.PYJU*/ BG_PIX_B7.set(VRAM_TEMP_CLK, sys_sig.P10_Bn, vram_bus.MD7);

  /*p33.POBE*/ wire SPR_PIX_FLIP0 = mux2(vram_bus.MD7, vram_bus.MD0, FLIP_X);
  /*p33.PACY*/ wire SPR_PIX_FLIP1 = mux2(vram_bus.MD6, vram_bus.MD1, FLIP_X);
  /*p33.PONO*/ wire SPR_PIX_FLIP2 = mux2(vram_bus.MD5, vram_bus.MD2, FLIP_X);
  /*p33.PUGU*/ wire SPR_PIX_FLIP3 = mux2(vram_bus.MD4, vram_bus.MD3, FLIP_X);
  /*p33.PUTE*/ wire SPR_PIX_FLIP4 = mux2(vram_bus.MD3, vram_bus.MD4, FLIP_X);
  /*p33.PULY*/ wire SPR_PIX_FLIP5 = mux2(vram_bus.MD2, vram_bus.MD5, FLIP_X);
  /*p33.PELO*/ wire SPR_PIX_FLIP6 = mux2(vram_bus.MD1, vram_bus.MD6, FLIP_X);
  /*p33.PAWE*/ wire SPR_PIX_FLIP7 = mux2(vram_bus.MD0, vram_bus.MD7, FLIP_X);

  /*p33.PEFO*/ SPR_PIX_A0.tp_latch(SPRITE_VRAM_LATCH_A, SPR_PIX_FLIP0);
  /*p33.ROKA*/ SPR_PIX_A1.tp_latch(SPRITE_VRAM_LATCH_A, SPR_PIX_FLIP1);
  /*p33.MYTU*/ SPR_PIX_A2.tp_latch(SPRITE_VRAM_LATCH_A, SPR_PIX_FLIP2);
  /*p33.RAMU*/ SPR_PIX_A3.tp_latch(SPRITE_VRAM_LATCH_A, SPR_PIX_FLIP3);
  /*p33.SELE*/ SPR_PIX_A4.tp_latch(SPRITE_VRAM_LATCH_A, SPR_PIX_FLIP4);
  /*p33.SUTO*/ SPR_PIX_A5.tp_latch(SPRITE_VRAM_LATCH_A, SPR_PIX_FLIP5);
  /*p33.RAMA*/ SPR_PIX_A6.tp_latch(SPRITE_VRAM_LATCH_A, SPR_PIX_FLIP6);
  /*p33.RYDU*/ SPR_PIX_A7.tp_latch(SPRITE_VRAM_LATCH_A, SPR_PIX_FLIP7);

  /*p33.REWO*/ SPR_PIX_B0.tp_latch(SPRITE_VRAM_LATCH_B, SPR_PIX_FLIP0);
  /*p33.PEBA*/ SPR_PIX_B1.tp_latch(SPRITE_VRAM_LATCH_B, SPR_PIX_FLIP1);
  /*p33.MOFO*/ SPR_PIX_B2.tp_latch(SPRITE_VRAM_LATCH_B, SPR_PIX_FLIP2);
  /*p33.PUDU*/ SPR_PIX_B3.tp_latch(SPRITE_VRAM_LATCH_B, SPR_PIX_FLIP3);
  /*p33.SAJA*/ SPR_PIX_B4.tp_latch(SPRITE_VRAM_LATCH_B, SPR_PIX_FLIP4);
  /*p33.SUNY*/ SPR_PIX_B5.tp_latch(SPRITE_VRAM_LATCH_B, SPR_PIX_FLIP5);
  /*p33.SEMO*/ SPR_PIX_B6.tp_latch(SPRITE_VRAM_LATCH_B, SPR_PIX_FLIP6);
  /*p33.SEGA*/ SPR_PIX_B7.tp_latch(SPRITE_VRAM_LATCH_B, SPR_PIX_FLIP7);

  /*p29.XEFY*/ wire LOAD_SPRITE_TO_PIPE = not(SPRITE_DONE);
  /*p34.MEFU*/ wire SPRITE_MASK0 = or(LOAD_SPRITE_TO_PIPE, SPR_PIPE_A0, SPR_PIPE_B0);
  /*p34.MEVE*/ wire SPRITE_MASK1 = or(LOAD_SPRITE_TO_PIPE, SPR_PIPE_A1, SPR_PIPE_B1);
  /*p34.MYZO*/ wire SPRITE_MASK2 = or(LOAD_SPRITE_TO_PIPE, SPR_PIPE_A2, SPR_PIPE_B2);
  /*p34.RUDA*/ wire SPRITE_MASK3 = or(LOAD_SPRITE_TO_PIPE, SPR_PIPE_A3, SPR_PIPE_B3);
  /*p34.VOTO*/ wire SPRITE_MASK4 = or(LOAD_SPRITE_TO_PIPE, SPR_PIPE_A4, SPR_PIPE_B4);
  /*p34.VYSA*/ wire SPRITE_MASK5 = or(LOAD_SPRITE_TO_PIPE, SPR_PIPE_A5, SPR_PIPE_B5);
  /*p34.TORY*/ wire SPRITE_MASK6 = or(LOAD_SPRITE_TO_PIPE, SPR_PIPE_A6, SPR_PIPE_B6);
  /*p34.WOPE*/ wire SPRITE_MASK7 = or(LOAD_SPRITE_TO_PIPE, SPR_PIPE_A7, SPR_PIPE_B7);

  /*p34.LESY*/ wire SPRITE_MASK0n = not(SPRITE_MASK0);
  /*p34.LOTA*/ wire SPRITE_MASK1n = not(SPRITE_MASK1);
  /*p34.LYKU*/ wire SPRITE_MASK2n = not(SPRITE_MASK2);
  /*p34.ROBY*/ wire SPRITE_MASK3n = not(SPRITE_MASK3);
  /*p34.TYTA*/ wire SPRITE_MASK4n = not(SPRITE_MASK4);
  /*p34.TYCO*/ wire SPRITE_MASK5n = not(SPRITE_MASK5);
  /*p34.SOKA*/ wire SPRITE_MASK6n = not(SPRITE_MASK6);
  /*p34.XOVU*/ wire SPRITE_MASK7n = not(SPRITE_MASK7);

  //----------
  // Background pipe A

  {
    /*p32.LOZE*/ wire LOZE_PIPE_A_LOAD = not(NYXU_BG_SEQ_RSTn);
    /*p32.LAKY*/ wire BG_PIPE_A_SET0 = nand(LOZE_PIPE_A_LOAD, BG_PIX_A0);
    /*p32.NYXO*/ wire BG_PIPE_A_SET1 = nand(LOZE_PIPE_A_LOAD, BG_PIX_A1);
    /*p32.LOTO*/ wire BG_PIPE_A_SET2 = nand(LOZE_PIPE_A_LOAD, BG_PIX_A2);
    /*p32.LYDU*/ wire BG_PIPE_A_SET3 = nand(LOZE_PIPE_A_LOAD, BG_PIX_A3);
    /*p32.MYVY*/ wire BG_PIPE_A_SET4 = nand(LOZE_PIPE_A_LOAD, BG_PIX_A4);
    /*p32.LODO*/ wire BG_PIPE_A_SET5 = nand(LOZE_PIPE_A_LOAD, BG_PIX_A5);
    /*p32.NUTE*/ wire BG_PIPE_A_SET6 = nand(LOZE_PIPE_A_LOAD, BG_PIX_A6);
    /*p32.NAJA*/ wire BG_PIPE_A_SET7 = nand(LOZE_PIPE_A_LOAD, BG_PIX_A7);

    /*p32.LOTY*/ wire BG_PIPE_A_RST0 = nand(LOZE_PIPE_A_LOAD, /*p32.LUHE*/ not(BG_PIX_A0));
    /*p32.NEXA*/ wire BG_PIPE_A_RST1 = nand(LOZE_PIPE_A_LOAD, /*p32.NOLY*/ not(BG_PIX_A1));
    /*p32.LUTU*/ wire BG_PIPE_A_RST2 = nand(LOZE_PIPE_A_LOAD, /*p32.LEKE*/ not(BG_PIX_A2));
    /*p32.LUJA*/ wire BG_PIPE_A_RST3 = nand(LOZE_PIPE_A_LOAD, /*p32.LOMY*/ not(BG_PIX_A3));
    /*p32.MOSY*/ wire BG_PIPE_A_RST4 = nand(LOZE_PIPE_A_LOAD, /*p32.LALA*/ not(BG_PIX_A4));
    /*p32.LERU*/ wire BG_PIPE_A_RST5 = nand(LOZE_PIPE_A_LOAD, /*p32.LOXA*/ not(BG_PIX_A5));
    /*p32.NYHA*/ wire BG_PIPE_A_RST6 = nand(LOZE_PIPE_A_LOAD, /*p32.NEZE*/ not(BG_PIX_A6));
    /*p32.NADY*/ wire BG_PIPE_A_RST7 = nand(LOZE_PIPE_A_LOAD, /*p32.NOBO*/ not(BG_PIX_A7));

    /*p32.MYDE*/ BG_PIPE_A0.set(CLKPIPE, BG_PIPE_A_SET0, BG_PIPE_A_RST0, sys_sig.PIN_P10_B);
    /*p32.NOZO*/ BG_PIPE_A1.set(CLKPIPE, BG_PIPE_A_SET1, BG_PIPE_A_RST1, BG_PIPE_A0);
    /*p32.MOJU*/ BG_PIPE_A2.set(CLKPIPE, BG_PIPE_A_SET2, BG_PIPE_A_RST2, BG_PIPE_A1);
    /*p32.MACU*/ BG_PIPE_A3.set(CLKPIPE, BG_PIPE_A_SET3, BG_PIPE_A_RST3, BG_PIPE_A2);
    /*p32.NEPO*/ BG_PIPE_A4.set(CLKPIPE, BG_PIPE_A_SET4, BG_PIPE_A_RST4, BG_PIPE_A3);
    /*p32.MODU*/ BG_PIPE_A5.set(CLKPIPE, BG_PIPE_A_SET5, BG_PIPE_A_RST5, BG_PIPE_A4);
    /*p32.NEDA*/ BG_PIPE_A6.set(CLKPIPE, BG_PIPE_A_SET6, BG_PIPE_A_RST6, BG_PIPE_A5);
    /*p32.PYBO*/ BG_PIPE_A7.set(CLKPIPE, BG_PIPE_A_SET7, BG_PIPE_A_RST7, BG_PIPE_A6);
  }

  //----------
  // Background pipe B

  {
    /*p32.LUXA*/ wire LUXA_PIPE_B_LOAD = not(NYXU_BG_SEQ_RSTn);
    /*p32.TUXE*/ wire BG_PIPE_B_SET0 = nand(LUXA_PIPE_B_LOAD, !BG_PIX_B0);
    /*p32.SOLY*/ wire BG_PIPE_B_SET1 = nand(LUXA_PIPE_B_LOAD, !BG_PIX_B1);
    /*p32.RUCE*/ wire BG_PIPE_B_SET2 = nand(LUXA_PIPE_B_LOAD, !BG_PIX_B2);
    /*p32.RYJA*/ wire BG_PIPE_B_SET3 = nand(LUXA_PIPE_B_LOAD, !BG_PIX_B3);
    /*p32.RUTO*/ wire BG_PIPE_B_SET4 = nand(LUXA_PIPE_B_LOAD, !BG_PIX_B4);
    /*p32.RAJA*/ wire BG_PIPE_B_SET5 = nand(LUXA_PIPE_B_LOAD, !BG_PIX_B5);
    /*p32.RAJO*/ wire BG_PIPE_B_SET6 = nand(LUXA_PIPE_B_LOAD, !BG_PIX_B6);
    /*p32.RAGA*/ wire BG_PIPE_B_SET7 = nand(LUXA_PIPE_B_LOAD, !BG_PIX_B7);

    /*p32.SEJA*/ wire BG_PIPE_B_RST0 = nand(LUXA_PIPE_B_LOAD, /*p32.TOSA*/ not(!BG_PIX_B0));
    /*p32.SENO*/ wire BG_PIPE_B_RST1 = nand(LUXA_PIPE_B_LOAD, /*p32.RUCO*/ not(!BG_PIX_B1));
    /*p32.SURE*/ wire BG_PIPE_B_RST2 = nand(LUXA_PIPE_B_LOAD, /*p32.TYCE*/ not(!BG_PIX_B2));
    /*p32.SEBO*/ wire BG_PIPE_B_RST3 = nand(LUXA_PIPE_B_LOAD, /*p32.REVY*/ not(!BG_PIX_B3));
    /*p32.SUCA*/ wire BG_PIPE_B_RST4 = nand(LUXA_PIPE_B_LOAD, /*p32.RYGA*/ not(!BG_PIX_B4));
    /*p32.SYWE*/ wire BG_PIPE_B_RST5 = nand(LUXA_PIPE_B_LOAD, /*p32.RYLE*/ not(!BG_PIX_B5));
    /*p32.SUPU*/ wire BG_PIPE_B_RST6 = nand(LUXA_PIPE_B_LOAD, /*p32.RAPU*/ not(!BG_PIX_B6));
    /*p32.RYJY*/ wire BG_PIPE_B_RST7 = nand(LUXA_PIPE_B_LOAD, /*p32.SOJA*/ not(!BG_PIX_B7));

    /*p32.TOMY*/ BG_PIPE_B0.set(CLKPIPE, BG_PIPE_B_SET0, BG_PIPE_B_RST0, sys_sig.PIN_P10_B);
    /*p32.TACA*/ BG_PIPE_B1.set(CLKPIPE, BG_PIPE_B_SET1, BG_PIPE_B_RST1, BG_PIPE_B0);
    /*p32.SADY*/ BG_PIPE_B2.set(CLKPIPE, BG_PIPE_B_SET2, BG_PIPE_B_RST2, BG_PIPE_B1);
    /*p32.RYSA*/ BG_PIPE_B3.set(CLKPIPE, BG_PIPE_B_SET3, BG_PIPE_B_RST3, BG_PIPE_B2);
    /*p32.SOBO*/ BG_PIPE_B4.set(CLKPIPE, BG_PIPE_B_SET4, BG_PIPE_B_RST4, BG_PIPE_B3);
    /*p32.SETU*/ BG_PIPE_B5.set(CLKPIPE, BG_PIPE_B_SET5, BG_PIPE_B_RST5, BG_PIPE_B4);
    /*p32.RALU*/ BG_PIPE_B6.set(CLKPIPE, BG_PIPE_B_SET6, BG_PIPE_B_RST6, BG_PIPE_B5);
    /*p32.SOHU*/ BG_PIPE_B7.set(CLKPIPE, BG_PIPE_B_SET7, BG_PIPE_B_RST7, BG_PIPE_B6);
  }

  //----------
  // Sprite pipe A

  {
    /*p33.MEZU*/ wire SPR_PIX_A_SET0 = nand(SPRITE_MASK0n, SPR_PIX_A0);
    /*p33.RUSY*/ wire SPR_PIX_A_SET1 = nand(SPRITE_MASK1n, SPR_PIX_A1);
    /*p33.MYXA*/ wire SPR_PIX_A_SET2 = nand(SPRITE_MASK2n, SPR_PIX_A2);
    /*p33.RANO*/ wire SPR_PIX_A_SET3 = nand(SPRITE_MASK3n, SPR_PIX_A3);
    /*p33.TYGA*/ wire SPR_PIX_A_SET4 = nand(SPRITE_MASK4n, SPR_PIX_A4);
    /*p33.VUME*/ wire SPR_PIX_A_SET5 = nand(SPRITE_MASK5n, SPR_PIX_A5);
    /*p33.TAPO*/ wire SPR_PIX_A_SET6 = nand(SPRITE_MASK6n, SPR_PIX_A6);
    /*p33.TESO*/ wire SPR_PIX_A_SET7 = nand(SPRITE_MASK7n, SPR_PIX_A7);

    /*p33.MOFY*/ wire SPR_PIX_A_RST0 = nand(SPRITE_MASK0n, /*p33.LOZA*/ not(SPR_PIX_A0));
    /*p33.RUCA*/ wire SPR_PIX_A_RST1 = nand(SPRITE_MASK1n, /*p33.SYBO*/ not(SPR_PIX_A1));
    /*p33.MAJO*/ wire SPR_PIX_A_RST2 = nand(SPRITE_MASK2n, /*p33.LUMO*/ not(SPR_PIX_A2));
    /*p33.REHU*/ wire SPR_PIX_A_RST3 = nand(SPRITE_MASK3n, /*p33.SOLO*/ not(SPR_PIX_A3));
    /*p33.WAXO*/ wire SPR_PIX_A_RST4 = nand(SPRITE_MASK4n, /*p33.VOBY*/ not(SPR_PIX_A4));
    /*p33.XOLE*/ wire SPR_PIX_A_RST5 = nand(SPRITE_MASK5n, /*p33.WYCO*/ not(SPR_PIX_A5));
    /*p33.TABY*/ wire SPR_PIX_A_RST6 = nand(SPRITE_MASK6n, /*p33.SERY*/ not(SPR_PIX_A6));
    /*p33.TULA*/ wire SPR_PIX_A_RST7 = nand(SPRITE_MASK7n, /*p33.SELU*/ not(SPR_PIX_A7));

    /*p33.NYLU*/ SPR_PIPE_A0.set(CLKPIPE, SPR_PIX_A_SET0, SPR_PIX_A_RST0, sys_sig.PIN_P10_B);
    /*p33.PEFU*/ SPR_PIPE_A1.set(CLKPIPE, SPR_PIX_A_SET1, SPR_PIX_A_RST1, SPR_PIPE_A0);
    /*p33.NATY*/ SPR_PIPE_A2.set(CLKPIPE, SPR_PIX_A_SET2, SPR_PIX_A_RST2, SPR_PIPE_A1);
    /*p33.PYJO*/ SPR_PIPE_A3.set(CLKPIPE, SPR_PIX_A_SET3, SPR_PIX_A_RST3, SPR_PIPE_A2);
    /*p33.VARE*/ SPR_PIPE_A4.set(CLKPIPE, SPR_PIX_A_SET4, SPR_PIX_A_RST4, SPR_PIPE_A3);
    /*p33.WEBA*/ SPR_PIPE_A5.set(CLKPIPE, SPR_PIX_A_SET5, SPR_PIX_A_RST5, SPR_PIPE_A4);
    /*p33.VANU*/ SPR_PIPE_A6.set(CLKPIPE, SPR_PIX_A_SET6, SPR_PIX_A_RST6, SPR_PIPE_A5);
    /*p33.VUPY*/ SPR_PIPE_A7.set(CLKPIPE, SPR_PIX_A_SET7, SPR_PIX_A_RST7, SPR_PIPE_A6);
  }

  //----------
  // Sprite pipe B

  {
    /*p33.PABE*/ wire SPR_PIX_B_SET0 = nand(SPRITE_MASK0n, SPR_PIX_B0);
    /*p33.MYTO*/ wire SPR_PIX_B_SET1 = nand(SPRITE_MASK1n, SPR_PIX_B1);
    /*p33.LELA*/ wire SPR_PIX_B_SET2 = nand(SPRITE_MASK2n, SPR_PIX_B2);
    /*p33.MAME*/ wire SPR_PIX_B_SET3 = nand(SPRITE_MASK3n, SPR_PIX_B3);
    /*p33.VEXU*/ wire SPR_PIX_B_SET4 = nand(SPRITE_MASK4n, SPR_PIX_B4);
    /*p33.VABY*/ wire SPR_PIX_B_SET5 = nand(SPRITE_MASK5n, SPR_PIX_B5);
    /*p33.TUXA*/ wire SPR_PIX_B_SET6 = nand(SPRITE_MASK6n, SPR_PIX_B6);
    /*p33.VUNE*/ wire SPR_PIX_B_SET7 = nand(SPRITE_MASK7n, SPR_PIX_B7);

    /*p33.PYZU*/ wire SPR_PIX_B_RST0 = nand(SPRITE_MASK0n, /*p33.RATA*/ not(SPR_PIX_B0));
    /*p33.MADA*/ wire SPR_PIX_B_RST1 = nand(SPRITE_MASK1n, /*p33.NUCA*/ not(SPR_PIX_B1));
    /*p33.LYDE*/ wire SPR_PIX_B_RST2 = nand(SPRITE_MASK2n, /*p33.LASE*/ not(SPR_PIX_B2));
    /*p33.LUFY*/ wire SPR_PIX_B_RST3 = nand(SPRITE_MASK3n, /*p33.LUBO*/ not(SPR_PIX_B3));
    /*p33.XATO*/ wire SPR_PIX_B_RST4 = nand(SPRITE_MASK4n, /*p33.WERY*/ not(SPR_PIX_B4));
    /*p33.XEXU*/ wire SPR_PIX_B_RST5 = nand(SPRITE_MASK5n, /*p33.WURA*/ not(SPR_PIX_B5));
    /*p33.TUPE*/ wire SPR_PIX_B_RST6 = nand(SPRITE_MASK6n, /*p33.SULU*/ not(SPR_PIX_B6));
    /*p33.XYVE*/ wire SPR_PIX_B_RST7 = nand(SPRITE_MASK7n, /*p33.WAMY*/ not(SPR_PIX_B7));

    /*p33.NURO*/ SPR_PIPE_B0.set(CLKPIPE, SPR_PIX_B_SET0, SPR_PIX_B_RST0, sys_sig.PIN_P10_B);
    /*p33.MASO*/ SPR_PIPE_B1.set(CLKPIPE, SPR_PIX_B_SET1, SPR_PIX_B_RST1, SPR_PIPE_B0);
    /*p33.LEFE*/ SPR_PIPE_B2.set(CLKPIPE, SPR_PIX_B_SET2, SPR_PIX_B_RST2, SPR_PIPE_B1);
    /*p33.LESU*/ SPR_PIPE_B3.set(CLKPIPE, SPR_PIX_B_SET3, SPR_PIX_B_RST3, SPR_PIPE_B2);
    /*p33.WYHO*/ SPR_PIPE_B4.set(CLKPIPE, SPR_PIX_B_SET4, SPR_PIX_B_RST4, SPR_PIPE_B3);
    /*p33.WORA*/ SPR_PIPE_B5.set(CLKPIPE, SPR_PIX_B_SET5, SPR_PIX_B_RST5, SPR_PIPE_B4);
    /*p33.VAFO*/ SPR_PIPE_B6.set(CLKPIPE, SPR_PIX_B_SET6, SPR_PIX_B_RST6, SPR_PIPE_B5);
    /*p33.WUFY*/ SPR_PIPE_B7.set(CLKPIPE, SPR_PIX_B_SET7, SPR_PIX_B_RST7, SPR_PIPE_B6);
  }

  //----------
  // Palette pipe

  {
    /*p34.PUME*/ wire SPRITE_PAL_PIPE_SET0n = nand(SPRITE_MASK0n, oam_reg.REG_OAM_A4);
    /*p34.SORO*/ wire SPRITE_PAL_PIPE_SET1n = nand(SPRITE_MASK1n, oam_reg.REG_OAM_A4);
    /*p34.PAMO*/ wire SPRITE_PAL_PIPE_SET2n = nand(SPRITE_MASK2n, oam_reg.REG_OAM_A4);
    /*p34.SUKY*/ wire SPRITE_PAL_PIPE_SET3n = nand(SPRITE_MASK3n, oam_reg.REG_OAM_A4);
    /*p34.RORA*/ wire SPRITE_PAL_PIPE_SET4n = nand(SPRITE_MASK4n, oam_reg.REG_OAM_A4);
    /*p34.MENE*/ wire SPRITE_PAL_PIPE_SET5n = nand(SPRITE_MASK5n, oam_reg.REG_OAM_A4);
    /*p34.LUKE*/ wire SPRITE_PAL_PIPE_SET6n = nand(SPRITE_MASK6n, oam_reg.REG_OAM_A4);
    /*p34.LAMY*/ wire SPRITE_PAL_PIPE_SET7n = nand(SPRITE_MASK7n, oam_reg.REG_OAM_A4);

    /*p34.SUCO*/ wire SPRITE_PAL_PIPE_RST0n = nand(SPRITE_MASK0n, /*p34.SYPY*/ not(oam_reg.REG_OAM_A4));
    /*p34.TAFA*/ wire SPRITE_PAL_PIPE_RST1n = nand(SPRITE_MASK1n, /*p34.TOTU*/ not(oam_reg.REG_OAM_A4));
    /*p34.PYZY*/ wire SPRITE_PAL_PIPE_RST2n = nand(SPRITE_MASK2n, /*p34.NARO*/ not(oam_reg.REG_OAM_A4));
    /*p34.TOWA*/ wire SPRITE_PAL_PIPE_RST3n = nand(SPRITE_MASK3n, /*p34.WEXY*/ not(oam_reg.REG_OAM_A4));
    /*p34.RUDU*/ wire SPRITE_PAL_PIPE_RST4n = nand(SPRITE_MASK4n, /*p34.RYZY*/ not(oam_reg.REG_OAM_A4));
    /*p34.PAZO*/ wire SPRITE_PAL_PIPE_RST5n = nand(SPRITE_MASK5n, /*p34.RYFE*/ not(oam_reg.REG_OAM_A4));
    /*p34.LOWA*/ wire SPRITE_PAL_PIPE_RST6n = nand(SPRITE_MASK6n, /*p34.LADY*/ not(oam_reg.REG_OAM_A4));
    /*p34.LUNU*/ wire SPRITE_PAL_PIPE_RST7n = nand(SPRITE_MASK7n, /*p34.LAFY*/ not(oam_reg.REG_OAM_A4));

    /*p34.RUGO*/ PAL_PIPE_0.set(CLKPIPE, SPRITE_PAL_PIPE_SET0n, SPRITE_PAL_PIPE_RST0n, sys_sig.PIN_P10_B);
    /*p34.SATA*/ PAL_PIPE_1.set(CLKPIPE, SPRITE_PAL_PIPE_SET1n, SPRITE_PAL_PIPE_RST1n, PAL_PIPE_0);
    /*p34.ROSA*/ PAL_PIPE_2.set(CLKPIPE, SPRITE_PAL_PIPE_SET2n, SPRITE_PAL_PIPE_RST2n, PAL_PIPE_1);
    /*p34.SOMY*/ PAL_PIPE_3.set(CLKPIPE, SPRITE_PAL_PIPE_SET3n, SPRITE_PAL_PIPE_RST3n, PAL_PIPE_2);
    /*p34.PALU*/ PAL_PIPE_4.set(CLKPIPE, SPRITE_PAL_PIPE_SET4n, SPRITE_PAL_PIPE_RST4n, PAL_PIPE_3);
    /*p34.NUKE*/ PAL_PIPE_5.set(CLKPIPE, SPRITE_PAL_PIPE_SET5n, SPRITE_PAL_PIPE_RST5n, PAL_PIPE_4);
    /*p34.MODA*/ PAL_PIPE_6.set(CLKPIPE, SPRITE_PAL_PIPE_SET6n, SPRITE_PAL_PIPE_RST6n, PAL_PIPE_5);
    /*p34.LYME*/ PAL_PIPE_7.set(CLKPIPE, SPRITE_PAL_PIPE_SET7n, SPRITE_PAL_PIPE_RST7n, PAL_PIPE_6);
  }

  //----------
  // Background mask pipe
  
  {
    /*p26.TEDE*/ wire MASK_PIPE_SET0 = nand(SPRITE_MASK0n, oam_reg.REG_OAM_A7);
    /*p26.XALA*/ wire MASK_PIPE_SET1 = nand(SPRITE_MASK1n, oam_reg.REG_OAM_A7);
    /*p26.TYRA*/ wire MASK_PIPE_SET2 = nand(SPRITE_MASK2n, oam_reg.REG_OAM_A7);
    /*p26.XYRU*/ wire MASK_PIPE_SET3 = nand(SPRITE_MASK3n, oam_reg.REG_OAM_A7);
    /*p26.XUKU*/ wire MASK_PIPE_SET4 = nand(SPRITE_MASK4n, oam_reg.REG_OAM_A7);
    /*p26.XELY*/ wire MASK_PIPE_SET5 = nand(SPRITE_MASK5n, oam_reg.REG_OAM_A7);
    /*p26.TYKO*/ wire MASK_PIPE_SET6 = nand(SPRITE_MASK6n, oam_reg.REG_OAM_A7);
    /*p26.TUWU*/ wire MASK_PIPE_SET7 = nand(SPRITE_MASK7n, oam_reg.REG_OAM_A7);

    /*p26.WOKA*/ wire MASK_PIPE_RST0 = nand(SPRITE_MASK0n, /*p26.XOGA*/ not(oam_reg.REG_OAM_A7));
    /*p26.WEDE*/ wire MASK_PIPE_RST1 = nand(SPRITE_MASK1n, /*p26.XURA*/ not(oam_reg.REG_OAM_A7));
    /*p26.TUFO*/ wire MASK_PIPE_RST2 = nand(SPRITE_MASK2n, /*p26.TAJO*/ not(oam_reg.REG_OAM_A7));
    /*p26.WEVO*/ wire MASK_PIPE_RST3 = nand(SPRITE_MASK3n, /*p26.XENU*/ not(oam_reg.REG_OAM_A7));
    /*p26.WEDY*/ wire MASK_PIPE_RST4 = nand(SPRITE_MASK4n, /*p26.XYKE*/ not(oam_reg.REG_OAM_A7));
    /*p26.WUJA*/ wire MASK_PIPE_RST5 = nand(SPRITE_MASK5n, /*p26.XABA*/ not(oam_reg.REG_OAM_A7));
    /*p26.TENA*/ wire MASK_PIPE_RST6 = nand(SPRITE_MASK6n, /*p26.TAFU*/ not(oam_reg.REG_OAM_A7));
    /*p26.WUBU*/ wire MASK_PIPE_RST7 = nand(SPRITE_MASK7n, /*p26.XUHO*/ not(oam_reg.REG_OAM_A7));

    /*p26.VEZO*/ MASK_PIPE_0.set(CLKPIPE, MASK_PIPE_SET0, MASK_PIPE_RST0, sys_sig.PIN_P10_B);
    /*p26.WURU*/ MASK_PIPE_1.set(CLKPIPE, MASK_PIPE_SET1, MASK_PIPE_RST1, MASK_PIPE_0);
    /*p26.VOSA*/ MASK_PIPE_2.set(CLKPIPE, MASK_PIPE_SET2, MASK_PIPE_RST2, MASK_PIPE_1);
    /*p26.WYFU*/ MASK_PIPE_3.set(CLKPIPE, MASK_PIPE_SET3, MASK_PIPE_RST3, MASK_PIPE_2);
    /*p26.XETE*/ MASK_PIPE_4.set(CLKPIPE, MASK_PIPE_SET4, MASK_PIPE_RST4, MASK_PIPE_3);
    /*p26.WODA*/ MASK_PIPE_5.set(CLKPIPE, MASK_PIPE_SET5, MASK_PIPE_RST5, MASK_PIPE_4);
    /*p26.VUMO*/ MASK_PIPE_6.set(CLKPIPE, MASK_PIPE_SET6, MASK_PIPE_RST6, MASK_PIPE_5);
    /*p26.VAVA*/ MASK_PIPE_7.set(CLKPIPE, MASK_PIPE_SET7, MASK_PIPE_RST7, MASK_PIPE_6);
  }

    //----------
  // Pixel mux

  /*p35.RAJY*/ wire PIX_BG0  = and(vid_reg.LCDC_BGEN, BG_PIPE_A7);
  /*p35.TADE*/ wire PIX_BG1  = and(vid_reg.LCDC_BGEN, BG_PIPE_B7);

  /*p35.WOXA*/ wire PIX_SP0  = and(vid_reg.LCDC_SPEN, SPR_PIPE_A7);
  /*p35.XULA*/ wire PIX_SP1  = and(vid_reg.LCDC_SPEN, SPR_PIPE_B7);

  /*p35.NULY*/ wire PIX_SPn  = nor(PIX_SP0, PIX_SP1);

  /*p35.RYFU*/ wire MASK_BG0 = and(PIX_BG0, MASK_PIPE_7);
  /*p35.RUTA*/ wire MASK_BG1 = and(PIX_BG1, MASK_PIPE_7);
  /*p35.POKA*/ wire BGPIXELn = nor(PIX_SPn, MASK_BG0, MASK_BG1);

  /*p34.LOME*/ wire SPRITE_PAL_PIPE_7n = not(PAL_PIPE_7);
  /*p34.LAFU*/ wire OBP0PIXELn = nand(SPRITE_PAL_PIPE_7n, BGPIXELn);
  /*p34.LEKA*/ wire OBP1PIXELn = nand(SPRITE_PAL_PIPE_7n, BGPIXELn);

  //----------
  // Sprite palette 0 lookup

  /*p35.VUMU*/ wire PIX_SP0n = not(PIX_SP0);
  /*p35.WYRU*/ wire PIX_SP0a = not(PIX_SP0n);
  /*p35.WELE*/ wire PIX_SP1n = not(PIX_SP1);
  /*p35.WOLO*/ wire PIX_SP1a = not(PIX_SP1n);

  /*p35.LAVA*/ wire MASK_OPB0  = not(OBP0PIXELn);
  /*p35.VYRO*/ wire PAL_OBP0D = and(PIX_SP0a, PIX_SP1a, MASK_OPB0);
  /*p35.VATA*/ wire PAL_OBP0C = and(PIX_SP0a, PIX_SP1n, MASK_OPB0);
  /*p35.VOLO*/ wire PAL_OBP0B = and(PIX_SP0n, PIX_SP1a, MASK_OPB0);
  /*p35.VUGO*/ wire PAL_OBP0A = and(PIX_SP0n, PIX_SP1n, MASK_OPB0);
    

  /*p35.WUFU*/ wire COL_OBP00 = amux4(vid_reg.OBP07, PAL_OBP0D,
                                      vid_reg.OBP05, PAL_OBP0C,
                                      vid_reg.OBP03, PAL_OBP0B,
                                      vid_reg.OBP01, PAL_OBP0A);

  /*p35.WALY*/ wire COL_OBP01 = amux4(vid_reg.OBP06, PAL_OBP0D,
                                      vid_reg.OBP04, PAL_OBP0C,
                                      vid_reg.OBP02, PAL_OBP0B,
                                      vid_reg.OBP00, PAL_OBP0A);

  //----------
  // Sprite palette 1 lookup

  /*p35.MEXA*/ wire PIX_SP0o = not(PIX_SP0);
  /*p35.LOZO*/ wire PIX_SP0b = not(PIX_SP0o);
  /*p35.MABY*/ wire PIX_SP1o = not(PIX_SP1);
  /*p35.LYLE*/ wire PIX_SP1b = not(PIX_SP1o);

  /*p35.LUKU*/ wire MASK_OBP1  = not(OBP1PIXELn);
  /*p35.LEDO*/ wire PAL_OBP1D = and(PIX_SP1b, PIX_SP0b, MASK_OBP1);
  /*p35.LYKY*/ wire PAL_OBP1C = and(PIX_SP1b, PIX_SP0o, MASK_OBP1);
  /*p35.LARU*/ wire PAL_OBP1B = and(PIX_SP1o, PIX_SP0b, MASK_OBP1);
  /*p35.LOPU*/ wire PAL_OBP1A = and(PIX_SP1o, PIX_SP0o, MASK_OBP1);

  /*p35.MOKA*/ wire COL_OBP10 = amux4(vid_reg.OBP17, PAL_OBP1D,
                                      vid_reg.OBP15, PAL_OBP1C,
                                      vid_reg.OBP13, PAL_OBP1B,
                                      vid_reg.OBP11, PAL_OBP1A);

  /*p35.MUFA*/ wire COL_OBP11 = amux4(vid_reg.OBP16, PAL_OBP1D,
                                      vid_reg.OBP14, PAL_OBP1C,
                                      vid_reg.OBP12, PAL_OBP1B,
                                      vid_reg.OBP10, PAL_OBP1A);

  //----------
  // Background/window palette lookup

  /*p35.SOBA*/ wire PIX_BG0n = not(PIX_BG0);
  /*p35.VYCO*/ wire PIX_BG1n = not(PIX_BG1);
  /*p35.NUPO*/ wire PIX_BG0a = not(PIX_BG0n);
  /*p35.NALE*/ wire PIX_BG1a = not(PIX_BG1n);

  /*p35.MUVE*/ wire MASK_BGP = not(BGPIXELn);
  /*p35.POBU*/ wire PAL_BGPA = and(PIX_BG1n, PIX_BG0n, MASK_BGP);
  /*p35.NUMA*/ wire PAL_BGPB = and(PIX_BG1a, PIX_BG0n, MASK_BGP);
  /*p35.NUXO*/ wire PAL_BGPC = and(PIX_BG1n, PIX_BG0a, MASK_BGP);
  /*p35.NYPO*/ wire PAL_BGPD = and(PIX_BG1a, PIX_BG0a, MASK_BGP);

  /*p35.NURA*/ wire COL_BGP1 = amux4(vid_reg.BGP7, PAL_BGPD,
                                     vid_reg.BGP5, PAL_BGPC,
                                     vid_reg.BGP3, PAL_BGPB,
                                     vid_reg.BGP1, PAL_BGPA);

  /*p35.NELO*/ wire COL_BGP0 = amux4(vid_reg.BGP6, PAL_BGPD,
                                     vid_reg.BGP4, PAL_BGPC,
                                     vid_reg.BGP2, PAL_BGPB,
                                     vid_reg.BGP0, PAL_BGPA);

  //----------
  // Pixel merge and send

  // bits 0 and 1 swapped somewhere...

  /*p35.PATY*/ wire PATY_LD0 = or(COL_BGP1, COL_OBP00, COL_OBP10);
  /*p35.PERO*/ wire PERO_LD1 = or(COL_BGP0, COL_OBP01, COL_OBP11);

  /*p35.REMY*/ wire REMY_LD0n = not(PATY_LD0);
  /*p35.RAVO*/ wire RAVO_LD1n = not(PERO_LD1);

  LD0 = not(REMY_LD0n);
  LD1 = not(RAVO_LD1n);
}
#endif














#if 0
//-----------------------------------------------------------------------------

void VramPins::tick(const SysSignals& sys_sig,
                    const ClkSignals& clk_sig,
                    const RstSignals& rst_sig,
                    const BusSignals& bus_sig,
                    const DecoderSignals& dec_sig,
                    const VidRegisters& vid_reg,
                    const SpriteRegisters& /*spr_reg*/,
                    const VramBus& vram_bus,
                    BusTristates& bus_tri) {

  /*p25.TEFY*/ wire MCS_Cn = not(MCS_C);
  /*p25.TAVY*/ wire MOE_Cn = not(MOE_C);
  /*p25.SUDO*/ wire MWR_Cn = not(MWR_C);

  // this decoder chunk seems way overcomplicated...
  /*p08.SORE*/ wire ADDR_0000_7FFF = not(bus_tri.A15());
  /*p08.TEVY*/ wire ADDR_NOT_VRAM = or(bus_tri.A13(), bus_tri.A14(), ADDR_0000_7FFF);
  /*p08.TEXO*/ wire ADDR_VALID_AND_NOT_VRAM = and(sys_sig.CPU_ADDR_VALID, ADDR_NOT_VRAM);

  /*p07.TUNA*/ wire ADDR_0000_FDFF = nand(bus_tri.A15(), bus_tri.A14(), bus_tri.A13(),
                                          bus_tri.A12(), bus_tri.A11(), bus_tri.A10(),
                                          bus_tri.A09());
  /*p07.RYCU*/ wire ADDR_FE00_FFFF = not(ADDR_0000_FDFF);
  /*p25.TEFA*/ wire TEFA = nor(ADDR_FE00_FFFF, ADDR_VALID_AND_NOT_VRAM);
  /*p25.SOSE*/ wire SOSE_ADDR_VRAM = and(bus_tri.A15(), TEFA);

  // so is one of the debug pins a clock, and this is a clock divider?
  /*p25.SYCY*/ wire SYCY_CLK = not(sys_sig.MODE_DBG2);
  /*p01.CUNU*/ wire CUNU_RST = not(rst_sig.SYS_RESET);
  /*p25.SOTO*/ SOTO_DBG.set(SYCY_CLK, CUNU_RST, !SOTO_DBG);
  /*p25.TUTO*/ wire TUTO_DBG_VRAM  = and(sys_sig.MODE_DBG2, !SOTO_DBG);
  /*p25.RACO*/ wire RACO_DBG_VRAMn = not(TUTO_DBG_VRAM);

  /*p25.TEGU*/ wire CPU_RAM_CLK = nand(SOSE_ADDR_VRAM, clk_sig.AFAS_xxxxEFGx);
  /*p25.SALE*/ wire CPU_RAM_CLK2 = mux2(MOE_Cn, CPU_RAM_CLK, TUTO_DBG_VRAM);
  /*p25.TUCA*/ wire CPU_VRAM_RD  = and (SOSE_ADDR_VRAM, dec_sig.ADDR_VALID_xBCxxxxx);
  /*p25.TOLE*/ wire CPU_VRAM_RD2 = mux2(MCS_Cn, CPU_VRAM_RD, TUTO_DBG_VRAM);
  /*p25.ROPY*/ wire ROPY_RENDERINGn = not(vid_reg.XYMU_RENDERING_LATCH);
  /*p25.SERE*/ wire SERE = and(CPU_VRAM_RD2, ROPY_RENDERINGn);
  /*p04.DECY*/ wire FROM_CPU5n = not(sys_sig.CPU_FROM_CPU5);
  /*p04.CATY*/ wire FROM_CPU5  = not(FROM_CPU5n);

  /*p28.MYNU*/ wire CPU_READ_MYSTERYn = nand(bus_sig.ASOT_CPURD, FROM_CPU5);
  /*p28.LEKO*/ wire CPU_READ_MYSTERY  = not(CPU_READ_MYSTERYn);
  /*p25.TYVY*/ wire MD_TO_D = nand(SERE, CPU_READ_MYSTERY);
  /*p25.SEBY*/ wire MD_TO_Dn = not(MD_TO_D);

  // this can't be right, md_outd and md_outc are effectively the same signal
  // yeah that's what it looks like on the schematic
  /*p25.RUVY*/       wire CPU_RAM_CLK2n = not(CPU_RAM_CLK2);
  /*p25.SAZO*/     wire MD_OUTd = and(CPU_RAM_CLK2n, SERE);
  /*p25.RYJE*/   wire MD_INb  = not(MD_OUTd);
  /*p25.REVO*/ wire MD_OUTc = not(MD_INb);

  /*p25.RELA*/ wire VEXT_TO_VBUSb  = or(MD_OUTc, MD_OUTd);
  /*p25.ROCY*/ wire CBUS_TO_VBUSb = and(MD_OUTc, MD_OUTd);
  /*p25.RAHU*/ wire CBUS_TO_VBUSn = not(CBUS_TO_VBUSb);
  /*p25.RENA*/ wire VEXT_TO_VBUSn = not(VEXT_TO_VBUSb);

  {
    /*p25.TEGU*/     wire CPU_VRAM_CLK = nand(SOSE_ADDR_VRAM, clk_sig.AFAS_xxxxEFGx);
    /*p25.SALE*/   wire CPU_VRAM_CLK2 = mux2(MOE_Cn, CPU_VRAM_CLK, TUTO_DBG_VRAM);
    /*p25.RYLU*/ wire CPU_READ_VRAMn = nand(CPU_VRAM_CLK2, ROPY_RENDERINGn);

#if 0
    /*p29.TACU*/     wire SPR_SEQ_5_TRIG = nand(spr_reg.TYFO_SEQ_B0d, TYTU);
    /*p25.SOHO*/   wire SPR_READ_VRAM = and(SPR_SEQ_5_TRIG, spr.SPRITE_READn);
    /*p25.RAWA*/ wire SPR_READ_VRAMn = not(SPR_READ_VRAM);

    /*p27.LURY*/     wire LURY = and(!vid.BG_SEQ5_SYNC, vid.RENDERING); // probably SYNCn
    /*p27.LONY*/   wire BG_READ_VRAM = and(LURY, vid.BG_SEQ_RSTn);
    /*p27.MYMA*/ wire BG_READ_VRAMn = not(BG_READ_VRAM);

    /*p25.APAM*/ wire DMA_READ_VRAMn = not(dma.DMA_READ_VRAM);

    /*p25.RACU*/ wire MOEn = and(CPU_READ_VRAMn,
                                 SPR_READ_VRAMn,
                                 BG_READ_VRAMn,
                                 DMA_READ_VRAMn);

    /*p25.SEMA*/ wire MOE_An = and(MOEn, DBG_VRAMn);
    /*p25.RUTE*/ wire MOE_Dn = or (MOEn, DBG_VRAM); // schematic wrong, second input is RACU
    /*p25.REFO*/ MOE_A = not(MOE_An);
    /*p25.SAHA*/ MOE_D = not(MOE_Dn);
#endif
  }

  {
#if 0
    /*p25.TEFY*/ wire MCS_Cn = not(MCS_C);
    /*p25.SERE*/ wire SERE = and(CPU_VRAM_RD2, ROPY_RENDERINGn);
    /*p25.SUTU*/ wire MCS = nor(vid_sig.BG_READ_VRAM, dma_sig.DMA_READ_VRAM, spr_sig.SPRITE_READn, SERE);

    /*p25.TODE*/ wire MCS_An = and(MCS, RACO_DBG_VRAMn);
    /*p25.SEWO*/ wire MCS_Dn = or (MCS, TUTO_DBG_VRAM);
    /*p25.SOKY*/ MCS_A = not(MCS_An);
    /*p25.SETY*/ MCS_D = not(MCS_Dn);
#endif
  }


  {
    /*p01.AREV*/ wire AREV = nand(sys_sig.CPU_RAW_WR, clk_sig.AFAS_xxxxEFGx);
    /*p01.APOV*/ wire CPU_WR_xxxxEFGx  = not(AREV);
  
    /*p01.AGUT*/ wire AGUT_xxCDEFGH = and(or(clk_sig.AJAX_xxxxEFGH, clk_sig.AROV_xxCDEFxx), sys_sig.CPU_ADDR_VALID);
    /*p01.AWOD*/ wire AWOD = or(sys_sig.MODE_DBG2, AGUT_xxCDEFGH);
    /*p01.ABUZ*/ wire ADDR_VALID_xBCxxxxx = not(AWOD);

    /*p25.TUJA*/ wire CPU_VRAM_WR    = and (SOSE_ADDR_VRAM, CPU_WR_xxxxEFGx);

    /*p25.TYJY*/ wire CPU_VRAM_WR2   = mux2(MWR_Cn, CPU_VRAM_WR , TUTO_DBG_VRAM);
    /*p25.SOHY*/ wire MWR = nand(CPU_VRAM_WR2, SERE);
    /*p25.TAXY*/ wire MWR_An = and(MWR, RACO_DBG_VRAMn);
    /*p25.SOFY*/ wire MWR_Dn = or (MWR, TUTO_DBG_VRAM);
    /*p25.SYSY*/ MWR_A = not(MWR_An);
    /*p25.RAGU*/ MWR_D = not(MWR_Dn);
  }

  /*p25.LEXE*/ MA00_D = not(/*p25.MYFU*/ not(vram_bus.MA00));
  /*p25.LOZU*/ MA01_D = not(/*p25.MASA*/ not(vram_bus.MA01));
  /*p25.LACA*/ MA02_D = not(/*p25.MYRE*/ not(vram_bus.MA02));
  /*p25.LUVO*/ MA03_D = not(/*p25.MAVU*/ not(vram_bus.MA03));
  /*p25.LOLY*/ MA04_D = not(/*p25.MEPA*/ not(vram_bus.MA04));
  /*p25.LALO*/ MA05_D = not(/*p25.MYSA*/ not(vram_bus.MA05));
  /*p25.LEFA*/ MA06_D = not(/*p25.MEWY*/ not(vram_bus.MA06));
  /*p25.LUBY*/ MA07_D = not(/*p25.MUME*/ not(vram_bus.MA07));
  /*p25.TUJY*/ MA08_D = not(/*p25.VOVA*/ not(vram_bus.MA08));
  /*p25.TAGO*/ MA09_D = not(/*p25.VODE*/ not(vram_bus.MA09));
  /*p25.NUVA*/ MA10_D = not(/*p25.RUKY*/ not(vram_bus.MA10));
  /*p25.PEDU*/ MA11_D = not(/*p25.RUMA*/ not(vram_bus.MA11));
  /*p25.PONY*/ MA12_D = not(/*p25.REHO*/ not(vram_bus.MA12));

  /*p25.ROVE*/ wire CBUS_TO_VBUS = not(CBUS_TO_VBUSn);
  /*p25.REGE*/ MD0_A = not(/*p25.SEFA*/ and(vram_bus.MD0, CBUS_TO_VBUS));
  /*p25.RYKY*/ MD1_A = not(/*p25.SOGO*/ and(vram_bus.MD1, CBUS_TO_VBUS));
  /*p25.RAZO*/ MD2_A = not(/*p25.SEFU*/ and(vram_bus.MD2, CBUS_TO_VBUS));
  /*p25.RADA*/ MD3_A = not(/*p25.SUNA*/ and(vram_bus.MD3, CBUS_TO_VBUS));
  /*p25.RYRO*/ MD4_A = not(/*p25.SUMO*/ and(vram_bus.MD4, CBUS_TO_VBUS));
  /*p25.REVU*/ MD5_A = not(/*p25.SAZU*/ and(vram_bus.MD5, CBUS_TO_VBUS));
  /*p25.REKU*/ MD6_A = not(/*p25.SAMO*/ and(vram_bus.MD6, CBUS_TO_VBUS));
  /*p25.RYZE*/ MD7_A = not(/*p25.SUKE*/ and(vram_bus.MD7, CBUS_TO_VBUS));

  /*p25.ROFA*/ MD0_B = not(VEXT_TO_VBUSn);
  /*p25.ROFA*/ MD1_B = not(VEXT_TO_VBUSn);
  /*p25.ROFA*/ MD2_B = not(VEXT_TO_VBUSn);
  /*p25.ROFA*/ MD3_B = not(VEXT_TO_VBUSn);
  /*p25.ROFA*/ MD4_B = not(VEXT_TO_VBUSn);
  /*p25.ROFA*/ MD5_B = not(VEXT_TO_VBUSn);
  /*p25.ROFA*/ MD6_B = not(VEXT_TO_VBUSn);
  /*p25.ROFA*/ MD7_B = not(VEXT_TO_VBUSn);

  /*p25.RURA*/ MD0_D = not(/*p25.SYNU*/ or(vram_bus.MD0, CBUS_TO_VBUSn));
  /*p25.RULY*/ MD1_D = not(/*p25.SYMA*/ or(vram_bus.MD1, CBUS_TO_VBUSn));
  /*p25.RARE*/ MD2_D = not(/*p25.ROKO*/ or(vram_bus.MD2, CBUS_TO_VBUSn));
  /*p25.RODU*/ MD3_D = not(/*p25.SYBU*/ or(vram_bus.MD3, CBUS_TO_VBUSn));
  /*p25.RUBE*/ MD4_D = not(/*p25.SAKO*/ or(vram_bus.MD4, CBUS_TO_VBUSn));
  /*p25.RUMU*/ MD5_D = not(/*p25.SEJY*/ or(vram_bus.MD5, CBUS_TO_VBUSn));
  /*p25.RYTY*/ MD6_D = not(/*p25.SEDO*/ or(vram_bus.MD6, CBUS_TO_VBUSn));
  /*p25.RADY*/ MD7_D = not(/*p25.SAWU*/ or(vram_bus.MD7, CBUS_TO_VBUSn));
}

//-----------------------------------------------------------------------------

void CartPins::tick(const SysSignals& sys_sig,
                    const BusSignals& bus_sig,
                    const ClkSignals& clk_sig,
                    const DecoderSignals& dec_sig,
                    const DmaSignals& dma_sig,
                    const DmaRegisters& dma_reg,
                    BusTristates& bus_tri) {
  CartPins& cart_pins = *this;

  /*p08.SOGY*/ wire SOGY = not(bus_tri.A14());
  /*p08.TUMA*/ wire A000_BFFF = and(bus_tri.A13(), SOGY, bus_tri.A15());
  /*p08.TYNU*/ wire A000_FFFF = or(and(bus_tri.A15(), bus_tri.A14()), A000_BFFF);
  /*p08.TOZA*/ wire A000_FDFF_ABxxxxxx = and(dec_sig.ADDR_VALID_xBCxxxxx, A000_FFFF, dec_sig.ADDR_0000_FE00);
  /*p08.TYHO*/ wire CSn_A = mux2(dma_reg.DMA_A15, A000_FDFF_ABxxxxxx, dma_sig.DMA_READ_CART); // ABxxxxxx

  /*p08.MOCA*/ wire DBG_EXT_RDn = nor(bus_sig.ADDR_VALID_AND_NOT_VRAM, sys_sig.MODE_DBG1);
  /*p08.LAGU*/ wire LAGU = or(and(sys_sig.CPU_RAW_RD, bus_sig.ADDR_VALID_AND_NOT_VRAMn), sys_sig.CPU_RAW_WR);
  /*p08.LYWE*/ wire LYWE = not(LAGU);
  /*p08.MOTY*/ wire CPU_EXT_RD = or(DBG_EXT_RDn, LYWE);
  /*p08.TYMU*/ wire RD_OUT = nor(dma_sig.DMA_READ_CART, CPU_EXT_RD);

  /*p08.TOVA*/ wire MODE_DBG2n1 = not(sys_sig.MODE_DBG2);
  /*p08.RYCA*/ wire MODE_DBG2n2 = not(sys_sig.MODE_DBG2);

  /*p08.UGAC*/ wire RDn_A = nand(RD_OUT, MODE_DBG2n1);
  /*p08.URUN*/ wire RDn_D = nor (RD_OUT, sys_sig.MODE_DBG2);

  /*p01.AREV*/ wire AREV = nand(sys_sig.CPU_RAW_WR, clk_sig.AFAS_xxxxEFGx);
  /*p01.APOV*/ wire CPU_WR_xxxxEFGx  = not(AREV);
  /*p08.MEXO*/ wire MEXO_ABCDxxxH = not(CPU_WR_xxxxEFGx);
  /*p08.NEVY*/ wire NEVY = or(MEXO_ABCDxxxH, DBG_EXT_RDn);
  /*p08.PUVA*/ wire WR_OUT = or(NEVY, dma_sig.DMA_READ_CART);

  /*p08.UVER*/ wire WRn_A = nand(WR_OUT, MODE_DBG2n1);
  /*p08.USUF*/ wire WRn_D = nor (WR_OUT, sys_sig.MODE_DBG2);

  //----------

  /* PIN_75 */ cart_pins.PHI   = clk_sig.BUDE_xxxxEFGH;
  /* PIN_78 */ cart_pins.WRn_A = WRn_A;
  /* PIN_78 */ cart_pins.WRn_D = WRn_D;
  /* PIN_79 */ cart_pins.RDn_A = RDn_A;
  /* PIN_79 */ cart_pins.RDn_D = RDn_D;
  /* PIN_80 */ cart_pins.CSn_A = CSn_A;

  //----------
  // Address pin driver

  // Cart responds to 0x0000 - 0x7FFF (rom)
  // Cart responds to 0xA000 - 0xBFFF (ram)
  // Ram  responds to 0xC000 - 0xDFFF (ram)

  // Address bus to address pins mux
  /*p08.MULE*/ wire MODE_DBG1o = not(sys_sig.MODE_DBG1);
  /*p08.LOXO*/ wire LOXO = or(and(MODE_DBG1o, bus_sig.ADDR_VALID_AND_NOT_VRAM), sys_sig.MODE_DBG1);
  /*p08.LASY*/ wire LASY = not(LOXO);
  /*p08.MATE*/ wire ADDR_LATCH  = not(LASY);

  /*p08.ALOR*/ cart_pins.ADDR_LATCH_00.tp_latch(ADDR_LATCH, bus_tri.A00());
  /*p08.APUR*/ cart_pins.ADDR_LATCH_01.tp_latch(ADDR_LATCH, bus_tri.A01());
  /*p08.ALYR*/ cart_pins.ADDR_LATCH_02.tp_latch(ADDR_LATCH, bus_tri.A02());
  /*p08.ARET*/ cart_pins.ADDR_LATCH_03.tp_latch(ADDR_LATCH, bus_tri.A03());
  /*p08.AVYS*/ cart_pins.ADDR_LATCH_04.tp_latch(ADDR_LATCH, bus_tri.A04());
  /*p08.ATEV*/ cart_pins.ADDR_LATCH_05.tp_latch(ADDR_LATCH, bus_tri.A05());
  /*p08.AROS*/ cart_pins.ADDR_LATCH_06.tp_latch(ADDR_LATCH, bus_tri.A06());
  /*p08.ARYM*/ cart_pins.ADDR_LATCH_07.tp_latch(ADDR_LATCH, bus_tri.A07());
  /*p08.LUNO*/ cart_pins.ADDR_LATCH_08.tp_latch(ADDR_LATCH, bus_tri.A08());
  /*p08.LYSA*/ cart_pins.ADDR_LATCH_09.tp_latch(ADDR_LATCH, bus_tri.A09());
  /*p08.PATE*/ cart_pins.ADDR_LATCH_10.tp_latch(ADDR_LATCH, bus_tri.A10());
  /*p08.LUMY*/ cart_pins.ADDR_LATCH_11.tp_latch(ADDR_LATCH, bus_tri.A11());
  /*p08.LOBU*/ cart_pins.ADDR_LATCH_12.tp_latch(ADDR_LATCH, bus_tri.A12());
  /*p08.LONU*/ cart_pins.ADDR_LATCH_13.tp_latch(ADDR_LATCH, bus_tri.A13());
  /*p08.NYRE*/ cart_pins.ADDR_LATCH_14.tp_latch(ADDR_LATCH, bus_tri.A14());

  // FIXME
  wire ADDR_BOOT = false;
  // something wrong here...
  /*p08.SOBY*/ wire SOBY = nor(bus_tri.A15(), ADDR_BOOT);
  /*p08.SEPY*/ wire ADDR_LATCH_15 = nand(dec_sig.ADDR_VALID_xBCxxxxx, SOBY); // wat?

  /*p08.AMET*/ wire ADDR_OUT_00 = mux2(dma_reg.DMA_A00, cart_pins.ADDR_LATCH_00, dma_sig.DMA_READ_CART);
  /*p08.ATOL*/ wire ADDR_OUT_01 = mux2(dma_reg.DMA_A01, cart_pins.ADDR_LATCH_01, dma_sig.DMA_READ_CART);
  /*p08.APOK*/ wire ADDR_OUT_02 = mux2(dma_reg.DMA_A02, cart_pins.ADDR_LATCH_02, dma_sig.DMA_READ_CART);
  /*p08.AMER*/ wire ADDR_OUT_03 = mux2(dma_reg.DMA_A03, cart_pins.ADDR_LATCH_03, dma_sig.DMA_READ_CART);
  /*p08.ATEM*/ wire ADDR_OUT_04 = mux2(dma_reg.DMA_A04, cart_pins.ADDR_LATCH_04, dma_sig.DMA_READ_CART);
  /*p08.ATOV*/ wire ADDR_OUT_05 = mux2(dma_reg.DMA_A05, cart_pins.ADDR_LATCH_05, dma_sig.DMA_READ_CART);
  /*p08.ATYR*/ wire ADDR_OUT_06 = mux2(dma_reg.DMA_A06, cart_pins.ADDR_LATCH_06, dma_sig.DMA_READ_CART);
  /*p08.ASUR*/ wire ADDR_OUT_07 = mux2(dma_reg.DMA_A07, cart_pins.ADDR_LATCH_07, dma_sig.DMA_READ_CART);
  /*p08.MANO*/ wire ADDR_OUT_08 = mux2(dma_reg.DMA_A08, cart_pins.ADDR_LATCH_08, dma_sig.DMA_READ_CART);
  /*p08.MASU*/ wire ADDR_OUT_09 = mux2(dma_reg.DMA_A09, cart_pins.ADDR_LATCH_09, dma_sig.DMA_READ_CART);
  /*p08.PAMY*/ wire ADDR_OUT_10 = mux2(dma_reg.DMA_A10, cart_pins.ADDR_LATCH_10, dma_sig.DMA_READ_CART);
  /*p08.MALE*/ wire ADDR_OUT_11 = mux2(dma_reg.DMA_A11, cart_pins.ADDR_LATCH_11, dma_sig.DMA_READ_CART);
  /*p08.MOJY*/ wire ADDR_OUT_12 = mux2(dma_reg.DMA_A12, cart_pins.ADDR_LATCH_12, dma_sig.DMA_READ_CART);
  /*p08.MUCE*/ wire ADDR_OUT_13 = mux2(dma_reg.DMA_A13, cart_pins.ADDR_LATCH_13, dma_sig.DMA_READ_CART);
  /*p08.PEGE*/ wire ADDR_OUT_14 = mux2(dma_reg.DMA_A14, cart_pins.ADDR_LATCH_14, dma_sig.DMA_READ_CART);

  // wat?
  /*p08.TAZY*/ wire ADDR_OUT_15 = mux2(dma_reg.DMA_A15, ADDR_LATCH_15, dma_sig.DMA_READ_CART);

  //----------
  // if NET01 high, drive external address bus onto internal address
  // these may be backwards, probably don't want to drive external address onto bus normally...

  /*p08.KOVA*/ wire A00_Cn = not(A00_C);
  /*p08.CAMU*/ wire A01_Cn = not(A01_C);
  /*p08.BUXU*/ wire A02_Cn = not(A02_C);
  /*p08.BASE*/ wire A03_Cn = not(A03_C);
  /*p08.AFEC*/ wire A04_Cn = not(A04_C);
  /*p08.ABUP*/ wire A05_Cn = not(A05_C);
  /*p08.CYGU*/ wire A06_Cn = not(A06_C);
  /*p08.COGO*/ wire A07_Cn = not(A07_C);
  /*p08.MUJY*/ wire A08_Cn = not(A08_C);
  /*p08.NENA*/ wire A09_Cn = not(A09_C);
  /*p08.SURA*/ wire A10_Cn = not(A10_C);
  /*p08.MADY*/ wire A11_Cn = not(A11_C);
  /*p08.LAHE*/ wire A12_Cn = not(A12_C);
  /*p08.LURA*/ wire A13_Cn = not(A13_C);
  /*p08.PEVO*/ wire A14_Cn = not(A14_C);
  /*p08.RAZA*/ wire A15_Cn = not(A15_C);

  /*p08.TOVA*/ wire TOVA_DBG2 = not(sys_sig.MODE_DBG2);
  if (TOVA_DBG2) bus_tri.set_addr(
    /*p08.KEJO*/ not(A00_Cn),
    /*p08.BYXE*/ not(A01_Cn),
    /*p08.AKAN*/ not(A02_Cn),
    /*p08.ANAR*/ not(A03_Cn),
    /*p08.AZUV*/ not(A04_Cn),
    /*p08.AJOV*/ not(A05_Cn),
    /*p08.BYNE*/ not(A06_Cn),
    /*p08.BYNA*/ not(A07_Cn),
    /*p08.LOFA*/ not(A08_Cn),
    /*p08.MAPU*/ not(A09_Cn),
    /*p08.RALA*/ not(A10_Cn),
    /*p08.LORA*/ not(A11_Cn),
    /*p08.LYNA*/ not(A12_Cn),
    /*p08.LEFY*/ not(A13_Cn),
    /*p08.NEFE*/ not(A14_Cn),
    /*p08.SYZU*/ not(A15_Cn)
  );

  /*p08.KUPO*/ cart_pins.A00_A = nand(ADDR_OUT_00, MODE_DBG2n1);
  /*p08.CABA*/ cart_pins.A01_A = nand(ADDR_OUT_01, MODE_DBG2n1);
  /*p08.BOKU*/ cart_pins.A02_A = nand(ADDR_OUT_02, MODE_DBG2n1);
  /*p08.BOTY*/ cart_pins.A03_A = nand(ADDR_OUT_03, MODE_DBG2n1);
  /*p08.BYLA*/ cart_pins.A04_A = nand(ADDR_OUT_04, MODE_DBG2n1);
  /*p08.BADU*/ cart_pins.A05_A = nand(ADDR_OUT_05, MODE_DBG2n1);
  /*p08.CEPU*/ cart_pins.A06_A = nand(ADDR_OUT_06, MODE_DBG2n1);
  /*p08.DEFY*/ cart_pins.A07_A = nand(ADDR_OUT_07, MODE_DBG2n1);
  /*p08.MYNY*/ cart_pins.A08_A = nand(ADDR_OUT_08, MODE_DBG2n1);
  /*p08.MUNE*/ cart_pins.A09_A = nand(ADDR_OUT_09, MODE_DBG2n1);
  /*p08.ROXU*/ cart_pins.A10_A = nand(ADDR_OUT_10, MODE_DBG2n1);
  /*p08.LEPY*/ cart_pins.A11_A = nand(ADDR_OUT_11, MODE_DBG2n1);
  /*p08.LUCE*/ cart_pins.A12_A = nand(ADDR_OUT_12, MODE_DBG2n1);
  /*p08.LABE*/ cart_pins.A13_A = nand(ADDR_OUT_13, MODE_DBG2n1);
  /*p08.PUHE*/ cart_pins.A14_A = nand(ADDR_OUT_14, MODE_DBG2n1);
  /*p08.SUZE*/ cart_pins.A15_A = nand(ADDR_OUT_15, MODE_DBG2n2);

  /*p08.KOTY*/ cart_pins.A00_D = nor (ADDR_OUT_00, sys_sig.MODE_DBG2);
  /*p08.COTU*/ cart_pins.A01_D = nor (ADDR_OUT_01, sys_sig.MODE_DBG2);
  /*p08.BAJO*/ cart_pins.A02_D = nor (ADDR_OUT_02, sys_sig.MODE_DBG2);
  /*p08.BOLA*/ cart_pins.A03_D = nor (ADDR_OUT_03, sys_sig.MODE_DBG2);
  /*p08.BEVO*/ cart_pins.A04_D = nor (ADDR_OUT_04, sys_sig.MODE_DBG2);
  /*p08.AJAV*/ cart_pins.A05_D = nor (ADDR_OUT_05, sys_sig.MODE_DBG2);
  /*p08.CYKA*/ cart_pins.A06_D = nor (ADDR_OUT_06, sys_sig.MODE_DBG2);
  /*p08.COLO*/ cart_pins.A07_D = nor (ADDR_OUT_07, sys_sig.MODE_DBG2);
  /*p08.MEGO*/ cart_pins.A08_D = nor (ADDR_OUT_08, sys_sig.MODE_DBG2);
  /*p08.MENY*/ cart_pins.A09_D = nor (ADDR_OUT_09, sys_sig.MODE_DBG2);
  /*p08.RORE*/ cart_pins.A10_D = nor (ADDR_OUT_10, sys_sig.MODE_DBG2);
  /*p08.LYNY*/ cart_pins.A11_D = nor (ADDR_OUT_11, sys_sig.MODE_DBG2);
  /*p08.LOSO*/ cart_pins.A12_D = nor (ADDR_OUT_12, sys_sig.MODE_DBG2);
  /*p08.LEVA*/ cart_pins.A13_D = nor (ADDR_OUT_13, sys_sig.MODE_DBG2);
  /*p08.PAHY*/ cart_pins.A14_D = nor (ADDR_OUT_14, sys_sig.MODE_DBG2);
  /*p08.RULO*/ cart_pins.A15_D = nor (ADDR_OUT_15, sys_sig.MODE_DBG2);

  //----------
  // Data pin driver.

  // Data bus driver
  /*p08.LAVO*/ wire DATA_LATCH = nand(sys_sig.CPU_RAW_RD, bus_sig.ADDR_VALID_AND_NOT_VRAM, sys_sig.CPU_FROM_CPU5); // polarity?

  /*p08.SOMA*/ DATA_LATCH_00.tp_latch(DATA_LATCH, D0_C);
  /*p08.RONY*/ DATA_LATCH_01.tp_latch(DATA_LATCH, D1_C);
  /*p08.RAXY*/ DATA_LATCH_02.tp_latch(DATA_LATCH, D2_C);
  /*p08.SELO*/ DATA_LATCH_03.tp_latch(DATA_LATCH, D3_C);
  /*p08.SODY*/ DATA_LATCH_04.tp_latch(DATA_LATCH, D4_C);
  /*p08.SAGO*/ DATA_LATCH_05.tp_latch(DATA_LATCH, D5_C);
  /*p08.RUPA*/ DATA_LATCH_06.tp_latch(DATA_LATCH, D6_C);
  /*p08.SAZY*/ DATA_LATCH_07.tp_latch(DATA_LATCH, D7_C);

  if (DATA_LATCH) bus_tri.set_data(
    /*p08.RYMA*/ DATA_LATCH_00,
    /*p08.RUVO*/ DATA_LATCH_01,
    /*p08.RYKO*/ DATA_LATCH_02,
    /*p08.TAVO*/ DATA_LATCH_03,
    /*p08.TEPE*/ DATA_LATCH_04,
    /*p08.SAFO*/ DATA_LATCH_05,
    /*p08.SEVU*/ DATA_LATCH_06,
    /*p08.TAJU*/ DATA_LATCH_07
  );

  /*p08.LULA*/ cart_pins.D0_B = bus_sig.CBUS_TO_CEXT;
  /*p08.LULA*/ cart_pins.D1_B = bus_sig.CBUS_TO_CEXT;
  /*p08.LULA*/ cart_pins.D2_B = bus_sig.CBUS_TO_CEXT;
  /*p08.LULA*/ cart_pins.D3_B = bus_sig.CBUS_TO_CEXT;
  /*p08.LULA*/ cart_pins.D4_B = bus_sig.CBUS_TO_CEXT;
  /*p08.LULA*/ cart_pins.D5_B = bus_sig.CBUS_TO_CEXT;
  /*p08.LULA*/ cart_pins.D6_B = bus_sig.CBUS_TO_CEXT;
  /*p08.LULA*/ cart_pins.D7_B = bus_sig.CBUS_TO_CEXT;

  /*p25.RUXA*/ cart_pins.D0_A = nand(bus_tri.D0(), bus_sig.CBUS_TO_CEXT);
  /*p25.RUJA*/ cart_pins.D1_A = nand(bus_tri.D1(), bus_sig.CBUS_TO_CEXT);
  /*p25.RABY*/ cart_pins.D2_A = nand(bus_tri.D2(), bus_sig.CBUS_TO_CEXT);
  /*p25.RERA*/ cart_pins.D3_A = nand(bus_tri.D3(), bus_sig.CBUS_TO_CEXT);
  /*p25.RORY*/ cart_pins.D4_A = nand(bus_tri.D4(), bus_sig.CBUS_TO_CEXT);
  /*p25.RYVO*/ cart_pins.D5_A = nand(bus_tri.D5(), bus_sig.CBUS_TO_CEXT);
  /*p25.RAFY*/ cart_pins.D7_A = nand(bus_tri.D6(), bus_sig.CBUS_TO_CEXT);
  /*p25.RAVU*/ cart_pins.D6_A = nand(bus_tri.D7(), bus_sig.CBUS_TO_CEXT);

  /*p08.RUNE*/ cart_pins.D0_D = nor (bus_tri.D0(), bus_sig.CBUS_TO_CEXTn);
  /*p08.RYPU*/ cart_pins.D1_D = nor (bus_tri.D1(), bus_sig.CBUS_TO_CEXTn);
  /*p08.SULY*/ cart_pins.D2_D = nor (bus_tri.D2(), bus_sig.CBUS_TO_CEXTn);
  /*p08.SEZE*/ cart_pins.D3_D = nor (bus_tri.D3(), bus_sig.CBUS_TO_CEXTn);
  /*p08.RESY*/ cart_pins.D4_D = nor (bus_tri.D4(), bus_sig.CBUS_TO_CEXTn);
  /*p08.TAMU*/ cart_pins.D5_D = nor (bus_tri.D5(), bus_sig.CBUS_TO_CEXTn);
  /*p08.ROGY*/ cart_pins.D6_D = nor (bus_tri.D6(), bus_sig.CBUS_TO_CEXTn);
  /*p08.RYDA*/ cart_pins.D7_D = nor (bus_tri.D7(), bus_sig.CBUS_TO_CEXTn);
}
#endif












#if 0
void OamRegisters::tick(const SysSignals& sys_sig,
                              const CpuPins& cpu_pins,
                              const ClkSignals& clk_sig,
                              const BusSignals& bus_sig,
                              const CartPins& cart_pins,
                              const VclkSignals& vclk_sig,
                              const DecoderSignals& dec_sig,
                              const SpriteSignals& spr_sig,
                              const DmaSignals& dma_sig,
                              const DmaRegisters& dma_reg,
                              const VidRegisters& vid_reg,
                              const SpriteTristate& spr_tri,
                              const VramBus& vram_tri,
                              BusTristates& bus_tri,
                              OamPins& oam_pins) {

  // TUVO is low for the first 4 phases of a sprite fetch
  // XUJY seems to have a one-phase glitch?

  // ppu can read oam when there's rendering but no dma
  /*p24.LOBY*/ wire LOBY_RENDERINGn = not(vid_reg.XYMU_RENDERING_LATCH);
  /*p28.BOGE*/ wire DMA_CLKENn = not(dma_reg.REG_DMA_RW_EN);
  /*p28.AJON*/ wire OAM_ADDR_RENDER  = and(DMA_CLKENn, vid_reg.XYMU_RENDERING_LATCH);
  /*p28.BETE*/ wire OAM_ADDR_RENDERn = not(OAM_ADDR_RENDER);
  /*p28.AJUJ*/ wire OAM_BUSYn = nor(dma_reg.REG_DMA_RW_EN, spr_sig.ACYL_OAM_ADDR_PARSE, OAM_ADDR_RENDERn);
  /*p28.AMAB*/ wire OAM_LOCKn = and(dec_sig.ADDR_OAM, OAM_BUSYn);

  //----------
  // OAM address generator

  {
    /*p28.GARO*/ wire A0n = not(bus_tri.A00());
    /*p28.WACU*/ wire A1n = not(bus_tri.A01());
    /*p28.GOSE*/ wire A2n = not(bus_tri.A02());
    /*p28.WAPE*/ wire A3n = not(bus_tri.A03());
    /*p28.FEVU*/ wire A4n = not(bus_tri.A04());
    /*p28.GERA*/ wire A5n = not(bus_tri.A05());
    /*p28.WAXA*/ wire A6n = not(bus_tri.A06());
    /*p28.FOBY*/ wire A7n = not(bus_tri.A07());

    /*p28.FYKE*/ wire IDX_0n = not(spr_tri.TS_IDX_0);
    /*p28.FUGU*/ wire IDX_1n = not(spr_tri.TS_IDX_1);
    /*p28.FACO*/ wire IDX_2n = not(spr_tri.TS_IDX_2);
    /*p28.FABY*/ wire IDX_3n = not(spr_tri.TS_IDX_3);
    /*p28.GYKA*/ wire IDX_4n = not(spr_tri.TS_IDX_4);
    /*p28.GYBU*/ wire IDX_5n = not(spr_tri.TS_IDX_5);

    /*p28.WUWE*/ wire SCANZn = not(sys_sig.PIN_P10_B);
    /*p28.GUSE*/ wire SCAN0n = not(spr_sig.SCAN0);
    /*p28.GEMA*/ wire SCAN1n = not(spr_sig.SCAN1);
    /*p28.FUTO*/ wire SCAN2n = not(spr_sig.SCAN2);
    /*p28.FAKU*/ wire SCAN3n = not(spr_sig.SCAN3);
    /*p28.GAMA*/ wire SCAN4n = not(spr_sig.SCAN4);
    /*p28.GOBY*/ wire SCAN5n = not(spr_sig.SCAN5);

    /*p28.FETU*/ wire DMA_A07n = not(dma_reg.DMA_A07);
    /*p28.FYDU*/ wire DMA_A06n = not(dma_reg.DMA_A06);
    /*p28.EDOL*/ wire DMA_A05n = not(dma_reg.DMA_A05);
    /*p28.ELUG*/ wire DMA_A04n = not(dma_reg.DMA_A04);
    /*p28.FYKY*/ wire DMA_A03n = not(dma_reg.DMA_A03);
    /*p28.FAGO*/ wire DMA_A02n = not(dma_reg.DMA_A02);
    /*p28.FESA*/ wire DMA_A01n = not(dma_reg.DMA_A01);
    /*p28.FODO*/ wire DMA_A00n = not(dma_reg.DMA_A00);

    // so if dma happens during oam parse, both drive the address line - bus conflict?

    /*p04.DUGA*/ wire OAM_ADDR_DMAn   = not(dma_reg.REG_DMA_RW_EN);
    /*p28.APAR*/ wire OAM_ADDR_PARSEn = not(spr_sig.ACYL_OAM_ADDR_PARSE);

    // cpu can read oam when there's no parsing, rendering, or dma
    /*p28.ASAM*/ wire OAM_ADDR_CPUn  = or(spr_sig.ACYL_OAM_ADDR_PARSE, vid_reg.XYMU_RENDERING_LATCH, dma_reg.REG_DMA_RW_EN);

    /*p28.WEFE*/ wire WEFE = not(sys_sig.PIN_P10_B);
    /*p28.GEFY*/ wire GEFY = not(sys_sig.PIN_P10_B);
    /*p28.GECA*/ wire GECA = not(WEFE);
    /*p28.WYDU*/ wire WYDU = not(WEFE);

    // so byte 0 is the palette number? something wrong here...

    // this is 8 sets of 4 tribuffers feeding into an inverter, which we'll model as an amux4n
    // except the inputs are negated or something?

    /*p28.GEKA*/ oam_pins.A0 = not((A0n & !OAM_ADDR_CPUn) | (GECA   & !OAM_ADDR_RENDERn) | (GEFY   & !OAM_ADDR_PARSEn) | (DMA_A00n & !OAM_ADDR_DMAn));
    /*p28.ZYFO*/ oam_pins.A1 = not((A1n & !OAM_ADDR_CPUn) | (WYDU   & !OAM_ADDR_RENDERn) | (SCANZn & !OAM_ADDR_PARSEn) | (DMA_A01n & !OAM_ADDR_DMAn));
    /*p28.YFOT*/ oam_pins.A2 = not((A2n & !OAM_ADDR_CPUn) | (IDX_5n & !OAM_ADDR_RENDERn) | (SCAN0n & !OAM_ADDR_PARSEn) | (DMA_A02n & !OAM_ADDR_DMAn));
    /*p28.YFOC*/ oam_pins.A3 = not((A3n & !OAM_ADDR_CPUn) | (IDX_4n & !OAM_ADDR_RENDERn) | (SCAN1n & !OAM_ADDR_PARSEn) | (DMA_A03n & !OAM_ADDR_DMAn));
    /*p28.YVOM*/ oam_pins.A4 = not((A4n & !OAM_ADDR_CPUn) | (IDX_3n & !OAM_ADDR_RENDERn) | (SCAN2n & !OAM_ADDR_PARSEn) | (DMA_A04n & !OAM_ADDR_DMAn));
    /*p28.YMEV*/ oam_pins.A5 = not((A5n & !OAM_ADDR_CPUn) | (IDX_2n & !OAM_ADDR_RENDERn) | (SCAN3n & !OAM_ADDR_PARSEn) | (DMA_A05n & !OAM_ADDR_DMAn));
    /*p28.XEMU*/ oam_pins.A6 = not((A6n & !OAM_ADDR_CPUn) | (IDX_1n & !OAM_ADDR_RENDERn) | (SCAN4n & !OAM_ADDR_PARSEn) | (DMA_A06n & !OAM_ADDR_DMAn));
    /*p28.YZET*/ oam_pins.A7 = not((A7n & !OAM_ADDR_CPUn) | (IDX_0n & !OAM_ADDR_RENDERn) | (SCAN5n & !OAM_ADDR_PARSEn) | (DMA_A07n & !OAM_ADDR_DMAn));
  }


  {
    /*p29.TUVO*/ wire SPR_OAM_RDn = or(LOBY_RENDERINGn, spr_sig.TULY_SPR_SEQ1, spr_sig.VONU_SEQ_xxx34xn);
    /*p28.AJEP*/   wire AJEP = and(spr_sig.ACYL_OAM_ADDR_PARSE, vclk_sig.XOCE_ABxxEFxx);
    /*p28.WEFY*/     wire WEFY = and(SPR_OAM_RDn, spr_sig.TYFO_SEQ_B0d);
    /*p28.XUJA*/   wire XUJA = not(WEFY);
    /*p04.DECY*/     wire FROM_CPU5n  = not(cpu_pins.FROM_CPU5);
    /*p28.BOTA*/   wire CPU_RD_OAMn = nand(FROM_CPU5n, dec_sig.ADDR_OAM, bus_sig.ASOT_CPURD); // Schematic wrong, this is NAND
    /*p28.ASYT*/ wire OAM_LATCHn = and (AJEP, XUJA, CPU_RD_OAMn);

    /*p29.TAME*/ wire TAME = nand(spr_sig.TESE_SPR_SEQ2, spr_sig.TOXE_SPR_SEQ0);
    /*p29.TYTU*/ wire TYTU = not(TAME);
    /*p29.TACU*/ wire TACU = nand(TYTU, spr_sig.TYFO_SEQ_B0d);
    /*p25.AVER*/ wire AVER = and(spr_sig.ACYL_OAM_ADDR_PARSE, vclk_sig.XYSO_ABCxDEFx);
    /*p25.VAPE*/ wire VAPE = and(SPR_OAM_RDn, TACU);
    /*p25.XUJY*/ wire XUJY = not(VAPE);

    /*p25.CUFE*/ wire CUFE_OAM_WR = and(or(dec_sig.ADDR_OAM, dma_reg.REG_DMA_RW_EN), clk_sig.MOPA_xxxxEFGH); // Possible schematic error - CUFE doesn't make sense as or(and()), only as and(or())
    /*p25.BYCU*/ wire OAM_IN_CLK = nand(AVER, XUJY, CUFE_OAM_WR); // schematic wrong, this is NAND... but that doesn't make sense?
    
    /*p25.COTA*/ oam_pins.CLK_A = not(OAM_IN_CLK);

    /*p28.BODE*/ wire OAM_LATCH  = not(OAM_LATCHn); // to the tribus receiver below
    /*p28.YVAL*/ wire OAM_LATCHo = not(OAM_LATCH);
    /*p28.YRYV*/ wire OAM_LATCHb = not(OAM_LATCHo);
    /*p28.ZODO*/ oam_pins.OE = not(OAM_LATCHb);

    /*p28.WAFO*/ wire WAFO_OAM_A0n   = not(oam_pins.A0);
    /*p04.WYJA*/ wire WYJA_OAM_WR    = or(and(OAM_LOCKn, bus_sig.CUPA_CPUWR), dma_sig.DMA_WRITE_OAM);
    /*p28.YNYC*/ wire YNYC_OAM_B_WRn = and(WYJA_OAM_WR, WAFO_OAM_A0n);
    /*p28.YLYC*/ wire YLYC_OAM_A_WRn = and(WYJA_OAM_WR, oam_pins.A0);

    /*p28.ZONE*/ oam_pins.WR_A = not(YLYC_OAM_A_WRn);
    /*p28.ZOFE*/ oam_pins.WR_B = not(YNYC_OAM_B_WRn);

    /*p31.XYKY*/ LATCH_OAM_A0.tp_latch(OAM_LATCH, oam_pins.A_D0);
    /*p31.YRUM*/ LATCH_OAM_A1.tp_latch(OAM_LATCH, oam_pins.A_D1);
    /*p31.YSEX*/ LATCH_OAM_A2.tp_latch(OAM_LATCH, oam_pins.A_D2);
    /*p31.YVEL*/ LATCH_OAM_A3.tp_latch(OAM_LATCH, oam_pins.A_D3);
    /*p31.WYNO*/ LATCH_OAM_A4.tp_latch(OAM_LATCH, oam_pins.A_D4);
    /*p31.CYRA*/ LATCH_OAM_A5.tp_latch(OAM_LATCH, oam_pins.A_D5);
    /*p31.ZUVE*/ LATCH_OAM_A6.tp_latch(OAM_LATCH, oam_pins.A_D6);
    /*p31.ECED*/ LATCH_OAM_A7.tp_latch(OAM_LATCH, oam_pins.A_D7);

    /*p29.YDYV*/ LATCH_OAM_B0.tp_latch(OAM_LATCH, oam_pins.B_D0);
    /*p29.YCEB*/ LATCH_OAM_B1.tp_latch(OAM_LATCH, oam_pins.B_D1);
    /*p29.ZUCA*/ LATCH_OAM_B2.tp_latch(OAM_LATCH, oam_pins.B_D2);
    /*p29.WONE*/ LATCH_OAM_B3.tp_latch(OAM_LATCH, oam_pins.B_D3);
    /*p29.ZAXE*/ LATCH_OAM_B4.tp_latch(OAM_LATCH, oam_pins.B_D4);
    /*p29.XAFU*/ LATCH_OAM_B5.tp_latch(OAM_LATCH, oam_pins.B_D5);
    /*p29.YSES*/ LATCH_OAM_B6.tp_latch(OAM_LATCH, oam_pins.B_D6);
    /*p29.ZECA*/ LATCH_OAM_B7.tp_latch(OAM_LATCH, oam_pins.B_D7);

    /*p31.XEGA*/ wire OAM_A_IN_CLK = not(oam_pins.CLK_A);
    /*p31.YLOR*/ REG_OAM_A0.set(OAM_A_IN_CLK, LATCH_OAM_A0);
    /*p31.ZYTY*/ REG_OAM_A1.set(OAM_A_IN_CLK, LATCH_OAM_A1);
    /*p31.ZYVE*/ REG_OAM_A2.set(OAM_A_IN_CLK, LATCH_OAM_A2);
    /*p31.ZEZY*/ REG_OAM_A3.set(OAM_A_IN_CLK, LATCH_OAM_A3);
    /*p31.GOMO*/ REG_OAM_A4.set(OAM_A_IN_CLK, LATCH_OAM_A4);
    /*p31.BAXO*/ REG_OAM_A5.set(OAM_A_IN_CLK, LATCH_OAM_A5);
    /*p31.YZOS*/ REG_OAM_A6.set(OAM_A_IN_CLK, LATCH_OAM_A6);
    /*p31.DEPO*/ REG_OAM_A7.set(OAM_A_IN_CLK, LATCH_OAM_A7);

    /*p29.YWOK*/ wire OAM_B_IN_CLK = not(oam_pins.CLK_A);
    /*p29.XUSO*/ REG_OAM_B0.set(OAM_B_IN_CLK, LATCH_OAM_B0);
    /*p29.XEGU*/ REG_OAM_B1.set(OAM_B_IN_CLK, LATCH_OAM_B1);
    /*p29.YJEX*/ REG_OAM_B2.set(OAM_B_IN_CLK, LATCH_OAM_B2);
    /*p29.XYJU*/ REG_OAM_B3.set(OAM_B_IN_CLK, LATCH_OAM_B3);
    /*p29.YBOG*/ REG_OAM_B4.set(OAM_B_IN_CLK, LATCH_OAM_B4);
    /*p29.WYSO*/ REG_OAM_B5.set(OAM_B_IN_CLK, LATCH_OAM_B5);
    /*p29.XOTE*/ REG_OAM_B6.set(OAM_B_IN_CLK, LATCH_OAM_B6);
    /*p29.YZAB*/ REG_OAM_B7.set(OAM_B_IN_CLK, LATCH_OAM_B7);
  }

  // OAM data tribus driver

  {
    /*p28.ADAH*/ wire ADDR_OAMn = not(dec_sig.ADDR_OAM);
    /*p28.XUTO*/ wire CPU_OAM_WR = and(dec_sig.ADDR_OAM, bus_sig.CUPA_CPUWR);
    /*p28.XYNY*/ wire CPU_OAM_WR_CLK1 = not(clk_sig.MOPA_xxxxEFGH);
    /*p28.WUJE*/ wire CPU_OAM_WR_CLK2 = or(CPU_OAM_WR_CLK1, CPU_OAM_WR);
    /*p28.XUPA*/ wire CPU_OAM_WR_CLK3 = not(CPU_OAM_WR_CLK2);
    /*p28.APAG*/ wire D_TO_OAMDn = amux2(CPU_OAM_WR_CLK3, OAM_LOCKn, OAM_BUSYn, ADDR_OAMn);
    /*p28.AZUL*/ wire D_TO_OAMD = not(D_TO_OAMDn);
    /*p28.ZAXA*/ if (D_TO_OAMD) oam_pins.A_D0 = bus_tri.D0();
    /*p28.ZAKY*/ if (D_TO_OAMD) oam_pins.A_D1 = bus_tri.D1();
    /*p28.WULE*/ if (D_TO_OAMD) oam_pins.A_D2 = bus_tri.D2();
    /*p28.ZOZO*/ if (D_TO_OAMD) oam_pins.A_D3 = bus_tri.D3();
    /*p28.ZUFO*/ if (D_TO_OAMD) oam_pins.A_D4 = bus_tri.D4();
    /*p28.ZATO*/ if (D_TO_OAMD) oam_pins.A_D5 = bus_tri.D5();
    /*p28.YVUC*/ if (D_TO_OAMD) oam_pins.A_D6 = bus_tri.D6();
    /*p28.ZUFE*/ if (D_TO_OAMD) oam_pins.A_D7 = bus_tri.D7();

    /*p28.ZAMY*/ if (D_TO_OAMD) oam_pins.B_D0 = bus_tri.D0();
    /*p28.ZOPU*/ if (D_TO_OAMD) oam_pins.B_D1 = bus_tri.D1();
    /*p28.WYKY*/ if (D_TO_OAMD) oam_pins.B_D2 = bus_tri.D2();
    /*p28.ZAJA*/ if (D_TO_OAMD) oam_pins.B_D3 = bus_tri.D3();
    /*p28.ZUGA*/ if (D_TO_OAMD) oam_pins.B_D4 = bus_tri.D4();
    /*p28.ZUMO*/ if (D_TO_OAMD) oam_pins.B_D5 = bus_tri.D5();
    /*p28.XYTO*/ if (D_TO_OAMD) oam_pins.B_D6 = bus_tri.D6();
    /*p28.ZYFA*/ if (D_TO_OAMD) oam_pins.B_D7 = bus_tri.D7();
  
    /*p28.AZAR*/ wire DMA_READ_VRAMp = not(dma_sig.DMA_READ_VRAM);
    /*p28.WUZU*/ if (!DMA_READ_VRAMp) oam_pins.A_D0 = vram_tri.MD0;
    /*p28.AXER*/ if (!DMA_READ_VRAMp) oam_pins.A_D1 = vram_tri.MD1;
    /*p28.ASOX*/ if (!DMA_READ_VRAMp) oam_pins.A_D2 = vram_tri.MD2;
    /*p28.CETU*/ if (!DMA_READ_VRAMp) oam_pins.A_D3 = vram_tri.MD3;
    /*p28.ARYN*/ if (!DMA_READ_VRAMp) oam_pins.A_D4 = vram_tri.MD4;
    /*p28.ACOT*/ if (!DMA_READ_VRAMp) oam_pins.A_D5 = vram_tri.MD5;
    /*p28.CUJE*/ if (!DMA_READ_VRAMp) oam_pins.A_D6 = vram_tri.MD6;
    /*p28.ATER*/ if (!DMA_READ_VRAMp) oam_pins.A_D7 = vram_tri.MD7;

    /*p28.WOWA*/ if (!DMA_READ_VRAMp) oam_pins.B_D0 = vram_tri.MD0;
    /*p28.AVEB*/ if (!DMA_READ_VRAMp) oam_pins.B_D1 = vram_tri.MD1;
    /*p28.AMUH*/ if (!DMA_READ_VRAMp) oam_pins.B_D2 = vram_tri.MD2;
    /*p28.COFO*/ if (!DMA_READ_VRAMp) oam_pins.B_D3 = vram_tri.MD3;
    /*p28.AZOZ*/ if (!DMA_READ_VRAMp) oam_pins.B_D4 = vram_tri.MD4;
    /*p28.AGYK*/ if (!DMA_READ_VRAMp) oam_pins.B_D5 = vram_tri.MD5;
    /*p28.BUSE*/ if (!DMA_READ_VRAMp) oam_pins.B_D6 = vram_tri.MD6;
    /*p28.ANUM*/ if (!DMA_READ_VRAMp) oam_pins.B_D7 = vram_tri.MD7;

    /*p25.WEJO*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.A_D0 = not(/*p25.RALO*/ not(cart_pins.D0_C));
    /*p25.BUBO*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.A_D1 = not(/*p25.TUNE*/ not(cart_pins.D1_C));
    /*p25.BETU*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.A_D2 = not(/*p25.SERA*/ not(cart_pins.D2_C));
    /*p25.CYME*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.A_D3 = not(/*p25.TENU*/ not(cart_pins.D3_C));
    /*p25.BAXU*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.A_D4 = not(/*p25.SYSA*/ not(cart_pins.D4_C));
    /*p25.BUHU*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.A_D5 = not(/*p25.SUGY*/ not(cart_pins.D5_C));
    /*p25.BYNY*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.A_D6 = not(/*p25.TUBE*/ not(cart_pins.D6_C));
    /*p25.BYPY*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.A_D7 = not(/*p25.SYZO*/ not(cart_pins.D7_C));

    /*p25.WASA*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.B_D0 = not(/*p25.RALO*/ not(cart_pins.D0_C));
    /*p25.BOMO*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.B_D1 = not(/*p25.TUNE*/ not(cart_pins.D1_C));
    /*p25.BASA*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.B_D2 = not(/*p25.SERA*/ not(cart_pins.D2_C));
    /*p25.CAKO*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.B_D3 = not(/*p25.TENU*/ not(cart_pins.D3_C));
    /*p25.BUMA*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.B_D4 = not(/*p25.SYSA*/ not(cart_pins.D4_C));
    /*p25.BUPY*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.B_D5 = not(/*p25.SUGY*/ not(cart_pins.D5_C));
    /*p25.BASY*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.B_D6 = not(/*p25.TUBE*/ not(cart_pins.D6_C));
    /*p25.BAPE*/ if (!dma_sig.DMA_READ_CARTn) oam_pins.B_D7 = not(/*p25.SYZO*/ not(cart_pins.D7_C));
  }

  {
    /*p28.MYNU*/ wire CPU_READ_MYSTERYn = nand(bus_sig.ASOT_CPURD, cpu_pins.FROM_CPU5);
    /*p28.LEKO*/ wire CPU_READ_MYSTERY  = not(CPU_READ_MYSTERYn);
  
    /*p28.WAFO*/ wire OAM_A0n   = not(oam_pins.A0);
    /*p28.WUKU*/ wire OAM_A_CPU_RDn = and(CPU_READ_MYSTERY, OAM_LOCKn, oam_pins.A0);
    /*p28.GUKO*/ wire OAM_B_CPU_RDn = and(CPU_READ_MYSTERY, OAM_LOCKn, OAM_A0n);
    /*p28.WEWU*/ wire OAM_A_CPU_RD  = not(OAM_A_CPU_RDn);
    /*p28.WUME*/ wire OAM_B_CPU_RD  = not(OAM_B_CPU_RDn);

    if (OAM_A_CPU_RD) bus_tri.set_data(
      /*p31.XACA*/ LATCH_OAM_A0,
      /*p31.XAGU*/ LATCH_OAM_A1,
      /*p31.XEPU*/ LATCH_OAM_A2,
      /*p31.XYGU*/ LATCH_OAM_A3,
      /*p31.XUNA*/ LATCH_OAM_A4,
      /*p31.DEVE*/ LATCH_OAM_A5,
      /*p31.ZEHA*/ LATCH_OAM_A6,
      /*p31.FYRA*/ LATCH_OAM_A7
    );

    if (OAM_B_CPU_RD) bus_tri.set_data(
      /*p29.YFAP*/ LATCH_OAM_B0,
      /*p29.XELE*/ LATCH_OAM_B1,
      /*p29.YPON*/ LATCH_OAM_B2,
      /*p29.XUVO*/ LATCH_OAM_B3,
      /*p29.ZYSA*/ LATCH_OAM_B4,
      /*p29.YWEG*/ LATCH_OAM_B5,
      /*p29.XABU*/ LATCH_OAM_B6,
      /*p29.YTUX*/ LATCH_OAM_B7
    );
  }
}

#endif








#if 0
void InterruptRegisters::tick(const SysSignals& sys_sig,
                              const BusSignals& bus_sig,
                              const CpuPins& cpu_pins,
                              const LcdSignals& lcd_sig,
                              const SerialSignals& ser_sig,
                              const JoypadSignals& joy_sig,
                              const VidSignals& vid_sig,
                              const TimerSignals& tim_sig,
                              const RstSignals& rst_sig,
                              const DecoderSignals& dec_sig,
                              BusTristates& bus_tri) {

  /*p07.ROLO*/ wire FF0F_RDn = nand(dec_sig.ADDR_XX0X, dec_sig.ADDR_XXXF, dec_sig.ADDR_FFXX, bus_sig.TEDO_CPURD); // schematic wrong, is NAND
  /*p02.POLA*/ wire FF0F_RDa = not(FF0F_RDn);
  /*p07.REFA*/ wire FF0F_WRn = nand(dec_sig.ADDR_XX0X, dec_sig.ADDR_XXXF, dec_sig.ADDR_FFXX, bus_sig.TAPU_CPUWR); // schematic wrong, is NAND
  /*p02.ROTU*/ wire FF0F_WRa = not(FF0F_WRn);

  /*p02.LETY*/ wire INT_VBL_ACK  = not(cpu_pins.FROM_CPU9);
  /*p02.LEJA*/ wire INT_SER_ACK  = not(cpu_pins.FROM_CPU8);
  /*p02.LESA*/ wire INT_JOY_ACK  = not(cpu_pins.FROM_CPU10);
  /*p02.LUFE*/ wire INT_STAT_ACK = not(cpu_pins.FROM_CPU7);
  /*p02.LAMO*/ wire INT_TIM_ACK  = not(cpu_pins.FROM_CPU11);

  /*p02.MUXE*/ wire MUXE = or(bus_tri.D0(), FF0F_WRn);
  /*p02.NABE*/ wire NABE = or(bus_tri.D1(), FF0F_WRn);
  /*p02.RAKE*/ wire RAKE = or(bus_tri.D2(), FF0F_WRn);
  /*p02.SULO*/ wire SULO = or(bus_tri.D3(), FF0F_WRn);
  /*p02.SEME*/ wire SEME = or(bus_tri.D4(), FF0F_WRn);

  /*p02.MYZU*/ wire FF0F_SET0 = nand(FF0F_WRa, INT_VBL_ACK,  bus_tri.D0());
  /*p02.MODY*/ wire FF0F_SET1 = nand(FF0F_WRa, INT_SER_ACK,  bus_tri.D1());
  /*p02.PYHU*/ wire FF0F_SET2 = nand(FF0F_WRa, INT_JOY_ACK,  bus_tri.D2());
  /*p02.TOME*/ wire FF0F_SET3 = nand(FF0F_WRa, INT_STAT_ACK, bus_tri.D3());
  /*p02.TOGA*/ wire FF0F_SET4 = nand(FF0F_WRa, INT_TIM_ACK,  bus_tri.D4());

  /*p02.LYTA*/ wire FF0F_RST0 = and(MUXE, INT_VBL_ACK,  rst_sig.SYS_RESETn);
  /*p02.MOVU*/ wire FF0F_RST1 = and(NABE, INT_SER_ACK,  rst_sig.SYS_RESETn);
  /*p02.PYGA*/ wire FF0F_RST2 = and(RAKE, INT_JOY_ACK,  rst_sig.SYS_RESETn);
  /*p02.TUNY*/ wire FF0F_RST3 = and(SULO, INT_STAT_ACK, rst_sig.SYS_RESETn);
  /*p02.TYME*/ wire FF0F_RST4 = and(SEME, INT_TIM_ACK,  rst_sig.SYS_RESETn);

  /*p02.PESU*/ wire FF0F_IN = not(sys_sig.PIN_P10_B);

  /*p21.PARU*/ wire VBLANK = not(!lcd_sig.POPU_VBLANK_d4);
  /*p21.TOLU*/ wire INT_VBLn = not(VBLANK);
  /*p21.VYPU*/ wire INT_VBL  = not(INT_VBLn);

  /*p02.LOPE*/ FF0F_0.set(INT_VBL,           FF0F_SET0, FF0F_RST0, FF0F_IN);
  /*p02.UBUL*/ FF0F_1.set(ser_sig.SER_CNT3,  FF0F_SET1, FF0F_RST1, FF0F_IN);
  /*p02.ULAK*/ FF0F_2.set(joy_sig.INT_JP,    FF0F_SET2, FF0F_RST2, FF0F_IN);
  /*p02.LALU*/ FF0F_3.set(vid_sig.VOTY_INT_STAT,  FF0F_SET3, FF0F_RST3, FF0F_IN);
  /*p02.NYBO*/ FF0F_4.set(tim_sig.INT_TIMER, FF0F_SET4, FF0F_RST4, FF0F_IN);

  /*p02.MATY*/ FF0F_L0.tp_latch(FF0F_RDn, FF0F_0);
  /*p02.NEJY*/ FF0F_L1.tp_latch(FF0F_RDn, FF0F_1);
  /*p02.NUTY*/ FF0F_L2.tp_latch(FF0F_RDn, FF0F_2);
  /*p02.MOPO*/ FF0F_L3.tp_latch(FF0F_RDn, FF0F_3);
  /*p02.PAVY*/ FF0F_L4.tp_latch(FF0F_RDn, FF0F_4);

  if (FF0F_RDa) bus_tri.set_data(
    /*p02.NELA*/ FF0F_L0,
    /*p02.NABO*/ FF0F_L1,
    /*p02.ROVA*/ FF0F_L2,
    /*p02.PADO*/ FF0F_L3,
    /*p02.PEGY*/ FF0F_L4
  );
}
#endif






























#if 0
void Cart::tick(CartPins& cart_pins) {

  if (!cart_pins.RDn_A && !cart_pins.CSn_A) {
    uint16_t cart_addr = (uint16_t)pack(cart_pins.A00_A, cart_pins.A01_A, cart_pins.A02_A, cart_pins.A03_A,
                                        cart_pins.A04_A, cart_pins.A05_A, cart_pins.A06_A, cart_pins.A07_A,
                                        cart_pins.A08_A, cart_pins.A09_A, cart_pins.A10_A, cart_pins.A11_A,
                                        cart_pins.A12_A, cart_pins.A13_A, cart_pins.A14_A, cart_pins.A15_A);

    if (cart_addr < 0x8000) {
      uint8_t d = rom[cart_addr];
      cart_pins.D0_C = d & 0x01;
      cart_pins.D1_C = d & 0x02;
      cart_pins.D2_C = d & 0x04;
      cart_pins.D3_C = d & 0x08;
      cart_pins.D4_C = d & 0x10;
      cart_pins.D5_C = d & 0x20;
      cart_pins.D6_C = d & 0x40;
      cart_pins.D7_C = d & 0x80;
    }

    // move to ExtRam later
    if (0xC000 <= cart_addr && cart_addr <= 0xDFFF) {
      uint8_t d = ram[cart_addr - 0xC000];
      cart_pins.D0_C = d & 0x01;
      cart_pins.D1_C = d & 0x02;
      cart_pins.D2_C = d & 0x04;
      cart_pins.D3_C = d & 0x08;
      cart_pins.D4_C = d & 0x10;
      cart_pins.D5_C = d & 0x20;
      cart_pins.D6_C = d & 0x40;
      cart_pins.D7_C = d & 0x80;
    }
  }
}


#endif









#if 0
    X_8_SYNC.val = false; X_8_SYNC.clk = false;
    LINE_STROBE.val = false; LINE_STROBE.clk = false;

    /*p21.WUSA*/ CPEN_LATCH = 0;

    /*p24.POME*/ POME = 0;
    /*p24.RUJU*/ RUJU = 0;

    /*p24.MEDA*/ VSYNC_OUTn.val = false; VSYNC_OUTn.clk = false;
    /*p24.LUCA*/ LINE_EVEN.val = true;  LINE_EVEN.clk = true;
    /*p21.NAPO*/ FRAME_EVEN.val = true; FRAME_EVEN.clk = true;

    /* PIN_50 */ PIN_LD1 = 0;
    /* PIN_51 */ PIN_LD0 = 0;
    /* PIN_52 */ PIN_CPG = 1;
    /* PIN_53 */ PIN_CP = 1;
    /* PIN_54 */ PIN_ST = 0;
    /* PIN_55 */ PIN_CPL = 1;
    /* PIN_56 */ PIN_FR = 1;
    /* PIN_57 */ PIN_S = 1;
#endif



#if 0
  /*p21.SYGU*/ Reg2 LINE_STROBE;
  /*p24.PAHO*/ Reg2 X_8_SYNC;

  /*p21.WUSA*/ bool CPEN_LATCH;

  /*p24.POME*/ bool POME; 
  /*p24.RUJU*/ bool RUJU;

  /*p24.MEDA*/ Reg2 VSYNC_OUTn;
  /*p24.LUCA*/ Reg2 LINE_EVEN;
  /*p21.NAPO*/ Reg2 FRAME_EVEN;

  //----------
  // LCD pins

  /* PIN_50 */ bool PIN_LD1;
  /* PIN_51 */ bool PIN_LD0;
  /* PIN_52 */ bool PIN_CPG;
  /* PIN_53 */ bool PIN_CP;
  /* PIN_54 */ bool PIN_ST;
  /* PIN_55 */ bool PIN_CPL;
  /* PIN_56 */ bool PIN_FR;
  /* PIN_57 */ bool PIN_S;
#endif


#if 0

  {
    bool C0 = lcd.X0;
    bool C1 = lcd.X1;
    bool C2 = lcd.X2;
    bool C3 = lcd.X3;
    bool C4 = lcd.X4;
    bool C5 = lcd.X5;
    bool C6 = lcd.X6;

    /*p21.TOCU*/ bool C0n = not(C0);
    /*p21.VEPE*/ bool C1n = not(C1);
    /*p21.VUTY*/ bool C2n = not(C2);
    /*p21.VATE*/ bool C3n = not(C3);
    /*p21.TUDA*/ bool C4n = not(C4);
    /*p21.TAFY*/ bool C5n = not(C5);
    /*p21.TUJU*/ bool C6n = not(C6);

    // so this is like a strobe that fires 4x per line
    /*p21.VOKU*/ bool CNT_000n = nand(C6n, C5n, C4n, C3n, C2n, C1n, C0n); // 0000000 == 0
    /*p21.TOZU*/ bool CNT_007n = nand(C6n, C5n, C4n, C3n, C2,  C1,  C0 ); // 0000111 == 7
    /*p21.TECE*/ bool CNT_045n = nand(C6n, C5,  C4n, C3,  C2,  C1n, C0 ); // 0101101 == 45
    /*p21.TEBO*/ bool CNT_083n = nand(C6,  C5n, C4,  C3n, C2n, C1,  C0 ); // 1010011 == 83

    /*p21.TEGY*/ bool LINE_STROBEa = nand(CNT_000n, CNT_007n, CNT_045n, CNT_083n);

    /*p21.SYGU*/ next.LINE_STROBE.tock(vid_clk.SONO_AxxxxFGH, VID_RESETn2, LINE_STROBEa);

    /*p21.RYNO*/ bool CPGn = or(lcd.LINE_STROBE, lcd.NEW_LINE_d0a);
    /*p21.POGU*/ bool POGU = not(CPGn);
    next.PIN_CPG = POGU;
  }


  {
    /*p01.TOFU*/ bool VID_RESET3  = not(vid_rst.VID_RESETn);
    /*p21.WEGO*/ bool WEGO  = or(VID_RESET3, vid.RENDER_DONE_SYNC);
    /*p21.XAJO*/ bool X_009 = and(vid.X0, vid.X3);

    // weird gate
    /*p21.WUSA*/ if (X_009) next.CPEN_LATCH = 1;
    /*p21.WUSA*/ if (WEGO)  next.CPEN_LATCH = 0;

    /*p21.TOBA*/ bool TOBA = and(vid.CLKPIPE, lcd.CPEN_LATCH);
    /*p21.SEMU*/ bool CPn  = or(TOBA, vid.FINE_MATCH_TRIG);
    /*p21.RYPO*/ bool RYPO = not(CPn);
    next.PIN_CP = RYPO;
  }

  {
    // Horizontal sync

    /*p01.TOFU*/ bool VID_RESET3  = not(vid_rst.VID_RESETn);

    // Latch loop
    ///*p24.POME*/ bool POME = nor(SCAN_DONE_d0_TRIG, POFY);
    ///*p24.RUJU*/ bool RUJU = or(X_8_SYNC, VID_RESET3, POME);
    ///*p24.POFY*/ bool POFY = not(RUJU);

    if (lcd.X_8_SYNC || VID_RESET3) {
      next.RUJU = true;
      next.POME = true;
    }

    if (SCAN_DONE_d0_TRIG) {
      next.RUJU = false;
      next.POME = false;
    }

    /*p24.POFY*/ bool POFY = not(lcd.RUJU);
    /*p24.RUZE*/ bool RUZE = not(POFY);
    /*p24.PAHO*/ next.X_8_SYNC.tock(vid.ROXO_4M, vid.RENDERING_LATCH, vid.X3);
    next.PIN_ST = RUZE;
  }

  {
    /*p24.KEDY*/ bool LCDC_ENn = not(LCDC_EN);
    /*p21.PURE*/ bool LINE_DONEn = not(lcd.NEW_LINE_d0a);
    /*p24.KASA*/ bool LINE_DONEb = not(LINE_DONEn);
    /*p24.UMOB*/ bool DIV_06 = not(DIV_06n);
    /*p24.KAHE*/ bool CPLn = amux2(LCDC_EN, LINE_DONEb, LCDC_ENn, DIV_06);
    /*p24.KYMO*/ bool KYMO = not(CPLn);
    next.PIN_CPL = KYMO;
  }

  {
    // if LCDC_ENn, FR = 4k div clock. Otherwise FR = xor(LINE_EVEN,FRAME_EVEN)

    /*p24.KEDY*/ bool LCDC_ENn = not(LCDC_EN);
    /*p24.LOFU*/ bool CLK_LINE_EVEN = not(lcd.NEW_LINE_d0a);
    /*p24.LUCA*/ next.LINE_EVEN.tock(CLK_LINE_EVEN, VID_RESETn2, !lcd.LINE_EVEN);
    /*p21.NAPO*/ next.FRAME_EVEN.tock(lcd.VBLANK_d4, VID_RESETn2, !lcd.FRAME_EVEN);

    /*p24.MAGU*/ bool MAGU = xor(lcd.FRAME_EVEN, lcd.LINE_EVEN);
    /*p24.MECO*/ bool MECO = not(MAGU);
    /*p24.KEBO*/ bool KEBO = not(MECO);
    /*p24.USEC*/ bool USEC = not(DIV_07n);
    /*p24.KUPA*/ bool FRn  = amux2(LCDC_EN, KEBO, LCDC_ENn, USEC);
    /*p24.KOFO*/ bool KOFO = not(FRn);
    next.PIN_FR = KOFO;
  }

  {
    // Vertical sync
    /*p24.NERU*/ bool LINE_000n = nor(lcd.Y0, lcd.Y1, lcd.Y2, lcd.Y3, lcd.Y4, lcd.Y5, lcd.Y6, lcd.Y7);
    /*p24.MEDA*/ next.VSYNC_OUTn.tock(lcd.NEW_LINE_d4a, VID_RESETn2, LINE_000n);
    /*p24.MURE*/ bool MURE = not(lcd.VSYNC_OUTn);
    next.PIN_S = MURE;
  }

#endif


#if 0
    //next.LINE_STROBE.reset(vid_clk.SONO_AxxxxFGH);
    //next.PIN_CPG = nor(false, next.NEW_LINE_d0a);
    //next.CPEN_LATCH = 0;
    //next.PIN_CP = !vid.FINE_MATCH_TRIG;

    next.RUJU = true;
    next.POME = true;
    next.PIN_ST  = next.RUJU;
    next.PIN_S   = !next.VSYNC_OUTn;
    next.PIN_CPL = !amux2(LCDC_EN, next.NEW_LINE_d0a, !LCDC_EN, !DIV_06n);
    next.PIN_FR  = !amux2(LCDC_EN, xor(next.FRAME_EVEN, next.LINE_EVEN), !LCDC_EN, !DIV_07n);

    //next.X_8_SYNC.tock(vid.ROXO_4M, vid.RENDERING_LATCH, vid.X3);
    //next.FRAME_EVEN.reset(next.VBLANK_d4);
    //next.VSYNC_OUTn.reset(next.NEW_LINE_d4a);
    //next.LINE_EVEN.reset(!next.NEW_LINE_d0a);

#endif




#if 0
    ///*p21.PARU*/ next.VBLANK_d4b = next.VBLANK_d4;


    ///*p21.TEGY*/ bool LINE_STROBEa = nand(lcd_x != 0, lcd_x != 7, lcd_x != 45, lcd_x != 83);
    ///*p21.SYGU*/ next.LINE_STROBE.tock(vid_clk.SONO_AxxxxFGH, 1, LINE_STROBEa);
    //next.PIN_CPG = nor(next.LINE_STROBE, next.NEW_LINE_d0a);

    {
      // weird gate
      ///*p21.WUSA*/ if (vid.X0 && vid.X3) next.CPEN_LATCH = 1;
      ///*p21.WUSA*/ if (vid.RENDER_DONE_SYNC)  next.CPEN_LATCH = 0;

      //next.PIN_CP = !or(and(vid.CLKPIPE, next.CPEN_LATCH), vid.FINE_MATCH_TRIG);
    }

    if (next.X_8_SYNC) {
      next.RUJU = true;
      next.POME = true;
    }

    if (SCAN_DONE_d0_TRIG) {
      next.RUJU = false;
      next.POME = false;
    }

    //next.PIN_ST  = next.RUJU;
    //next.PIN_S   = !next.VSYNC_OUTn;
    //next.PIN_CPL = !amux2(LCDC_EN, next.NEW_LINE_d0a, !LCDC_EN, !DIV_06n);
    //next.PIN_FR  = !amux2(LCDC_EN, xor(next.FRAME_EVEN, next.LINE_EVEN), !LCDC_EN, !DIV_07n);

    //next.X_8_SYNC.tock(vid.ROXO_4M, vid.RENDERING_LATCH, vid.X3);
    //next.FRAME_EVEN.tock(next.VBLANK_d4, 1, !next.FRAME_EVEN);
    //next.VSYNC_OUTn.tock(next.NEW_LINE_d4a, 1, lcd_y == 0);
    //next.LINE_EVEN.tock(!next.NEW_LINE_d0a, 1, !next.LINE_EVEN);

#endif




#if 0
  if (!vid_rst.VID_RESETn) {
    next.VBLANK_d4b = next.VBLANK_d4;


    next.VID_LINE_TRIG_d4n = 0;
    next.VID_LINE_TRIG_d4a = 1;
    next.XAHY_VID_LINE_TRIG_d4n = 0;

    next.VID_LINE_TRIG_d4p = 0;
    next.VID_LINE_TRIG_d4c = 1;

    next.VID_LINE_d4.reset(vid_clk.XUPY_xBCxxFGx);
    next.VID_LINE_d6.reset(vid_clk.AWOH_AxxDExxH);

    next.VBLANK_d4.reset(next.NEW_LINE_d4a);
  
    next.NEW_LINE_d4a.reset(vid_clk.TALU_xBCDExxx);
    next.LINE_153_d4.reset(next.NEW_LINE_d4a);

    next.X0.reset(vid_clk.TALU_xBCDExxx);
    next.X1.reset(!next.X0);
    next.X2.reset(!next.X1);
    next.X3.reset(!next.X2);
    next.X4.reset(!next.X3);
    next.X5.reset(!next.X4);
    next.X6.reset(!next.X5);

    next.NEW_LINE_d0a.reset(vid_clk.SONO_AxxxxFGH);

    next.Y0.reset(next.NEW_LINE_d0a);
    next.Y1.reset(!next.Y0);
    next.Y2.reset(!next.Y1);
    next.Y3.reset(!next.Y2);
    next.Y4.reset(!next.Y3);
    next.Y5.reset(!next.Y4);
    next.Y6.reset(!next.Y5);
    next.Y7.reset(!next.Y6);

    return;
  }


  {
    int lcd_x = next.x();
    int lcd_y = next.y();

    /*p28.BYHA*/ next.VID_LINE_TRIG_d4n = or(next.VID_LINE_d6, !next.VID_LINE_d4);
    /*p28.ATEJ*/ next.VID_LINE_TRIG_d4a = not(next.VID_LINE_TRIG_d4n);
    /*p27.XAHY*/ next.XAHY_VID_LINE_TRIG_d4n = not(next.VID_LINE_TRIG_d4a);

    /*p28.ABAK*/ bool VID_LINE_TRIG_d4b = next.VID_LINE_TRIG_d4a;
    /*p28.BYVA*/ next.VID_LINE_TRIG_d4p = not(VID_LINE_TRIG_d4b);
    /*p29.DYBA*/ next.VID_LINE_TRIG_d4c = not(next.VID_LINE_TRIG_d4p);

    // Horizontal sync

    /*p21.LAMA*/ bool FRAME_RSTn = nor(next.LINE_153_d4, !1);


    /*p29.ABOV*/ bool VID_LINE_d0 = and(next.NEW_LINE_d0a, lcd_y < 144);
    next.VID_LINE_d4.tock(vid_clk.XUPY_xBCxxFGx, 1, VID_LINE_d0);
    next.VID_LINE_d6.tock(vid_clk.AWOH_AxxDExxH, 1, next.VID_LINE_d4);

    next.VBLANK_d4.tock(next.NEW_LINE_d4a, 1, lcd_y >= 144);
  
    next.NEW_LINE_d4a.tock(vid_clk.TALU_xBCDExxx, 1, next.NEW_LINE_d0a);
    next.LINE_153_d4.tock(next.NEW_LINE_d4a, 1, lcd_y == 153);

    /*p21.MUDE*/ bool X_RSTn = not(next.NEW_LINE_d0a);
    next.X0.tock(vid_clk.TALU_xBCDExxx, X_RSTn, !next.X0);
    next.X1.tock(!next.X0, X_RSTn, !next.X1);
    next.X2.tock(!next.X1, X_RSTn, !next.X2);
    next.X3.tock(!next.X2, X_RSTn, !next.X3);
    next.X4.tock(!next.X3, X_RSTn, !next.X4);
    next.X5.tock(!next.X4, X_RSTn, !next.X5);
    next.X6.tock(!next.X5, X_RSTn, !next.X6);

    next.NEW_LINE_d0a.tock(vid_clk.SONO_AxxxxFGH, 1, lcd_x == 113);

    next.Y0.tock(next.NEW_LINE_d0a, FRAME_RSTn, !next.Y0);
    next.Y1.tock(!next.Y0, FRAME_RSTn, !next.Y1);
    next.Y2.tock(!next.Y1, FRAME_RSTn, !next.Y2);
    next.Y3.tock(!next.Y2, FRAME_RSTn, !next.Y3);
    next.Y4.tock(!next.Y3, FRAME_RSTn, !next.Y4);
    next.Y5.tock(!next.Y4, FRAME_RSTn, !next.Y5);
    next.Y6.tock(!next.Y5, FRAME_RSTn, !next.Y6);
    next.Y7.tock(!next.Y6, FRAME_RSTn, !next.Y7);
  }
#endif