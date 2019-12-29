#include "../pages/Sch_Clocks.h"
#include "../pages/Sch_BusControl.h"
#include "../pages/Sch_Resets.h"
#include "../pages/Sch_Decoder.h"
#include "../pages/Sch_Video.h"
#include "../pages/Sch_Sprites.h"
#include "../pages/Sch_Registers.h"
#include "../pages/Sch_Timer.h"
#include "../pages/Sch_LCD.h"

#include <memory.h>

#ifdef _MSC_VER
#include <include/SDL.h>
#else
#include <SDL2/SDL.h>
#endif

// 912 phases per line?

#define TCYCLES_LINE      456
#define TCYCLES_OAM       80
#define TCYCLES_VRAM      172
#define TCYCLES_HBLANK    204

#pragma warning(disable:4702)
#pragma warning(disable:4100)

using namespace Schematics;
struct TestGB;

uint32_t phase_to_color[8] = {
  0xFF808080,
  0xFFB0B070,
  0xFFFF8080,
  0xFFFFC080,

  0xFFFFFF80,
  0xFF80FF80,
  0xFF8080FF,
  0xFFD080D0,

  /*
  0xFF6fff6f,
  0xFFb8e343,
  0xFFf29f4c,
  0xFFfb5b86,
  0xFFcf40cf,
  0xFF865bfb,
  0xFF4c9ff2,
  0xFF43e3b8,
  */
};

//-----------------------------------------------------------------------------

#pragma optimize("", off)

template<typename T>
__declspec(noinline) void check_match(const T& a, const T& b) {
  uint8_t* blobA = (uint8_t*)&a;
  uint8_t* blobB = (uint8_t*)&b;

  for (int i = 0; i < sizeof(T); i++) {
    if (blobA[i] != blobB[i]) {
      printf("Mismatch at %d - 0x%02x 0x%02x", i, blobA[i], blobB[i]);
      __debugbreak();
    }
  }
}


//template<>
//__declspec(noinline) void check_match(const TestGB& a, const TestGB& b);

__declspec(noinline) void check(bool x) {
  if (!x) {
    printf("check failed\n");
    __debugbreak();
  }
}

#pragma optimize("", on)

//-----------------------------------------------------------------------------

struct TestGB {

  //----------------------------------------

  void reset() {
    memset(this, 0, sizeof(*this));

    clk_phase = 5;

    sys_sig.reset();
    clk_sig1.reset();
    clk_sig2.reset();
    clk_reg.reset();

    rst_sig1.reset();
    rst_reg.reset();

    //bus.reset();
    //lcd.reset();
    //spr.reset();
    //dec.reset();
    //vid.reset();

    //check(lcd.x() == 113);
    //check(lcd.y() == 0);
  }

  //----------------------------------------

  void run_reset_sequence() {
    memset(this, 0, sizeof(*this));
    
    clk_phase = -1;

    sys_sig.set_pwron();

    sim_slow(16);
    //check(lcd.x() == 0);
    //check(lcd.y() == 0);

    sys_sig.set_rst(false);
    sim_slow(16);
    //check(lcd.x() == 0);
    //check(lcd.y() == 0);

    sys_sig.set_clk_good(true);
    sim_slow(16);
    //check(lcd.x() == 0);
    //check(lcd.y() == 0);

    sys_sig.set_clk_req(true);
    sim_slow(16);
    //check(lcd.x() == 0);
    //check(lcd.y() == 0);

    sys_sig.set_lcdc_en(true);
    sim_slow(456*2*154 - 10);

    //printf("%d %d\n", lcd.x(), lcd.y());

    //check(lcd.x() == 113);
    //check(lcd.y() == 0);
  }

  //----------------------------------------

  void sim_slow(int phases) {
    for (int p = 0; p < phases; p++) {
      clk_phase = (clk_phase + 1) & 7;
      bool CLKIN = !(clk_phase & 1);

      // lcd currently requires 12 passes, ouch
      for (int pass = 0; pass < 12; pass++) {
        TestGB prev = *this;
        
        clk_sig1 = ClockSignals1::tick_slow(sys_sig, prev.clk_reg, CLKIN);
        rst_sig1 = ResetSignals1::tick(sys_sig, clk_sig1, prev.rst_reg);
        clk_sig2 = ClockSignals2::tick_slow(prev.clk_reg);

        //----------

        Clocks::tock_slow1(sys_sig, clk_sig1, clk_reg);
        Clocks::tock_slow2(sys_sig, clk_sig1, clk_sig2, rst_sig1, clk_reg);
        ResetRegisters::tock(sys_sig, clk_sig1, prev.rst_reg, rst_reg);
      }
    }
  }

  void sim_fast(int phases) {
    for (int p = 0; p < phases; p++) {
      clk_phase = (clk_phase + 1) & 7;
      //bool CLKIN = !(clk_phase & 1);

      // lcd currently requires 12 passes, ouch
      for (int pass = 0; pass < 12; pass++) {
        TestGB prev = *this;

        clk_sig1 = ClockSignals1::tick_fast(sys_sig, clk_phase);
        rst_sig1 = ResetSignals1::tick(sys_sig, clk_sig1, prev.rst_reg);
        clk_sig2 = ClockSignals2::tick_slow(prev.clk_reg);

        //----------

        Clocks::tock_fast1(sys_sig, clk_phase, clk_reg);
        Clocks::tock_slow2(sys_sig, clk_sig1, clk_sig2, rst_sig1, clk_reg);
        ResetRegisters::tock(sys_sig, clk_sig1, prev.rst_reg, rst_reg);
      }
    }
  }

  //LCD::tick_slow(clk_sig1, clk_sig2, rst_sig1, prev.lcd, prev.vid, prev.spr.SCAN_DONE_d0_TRIG, DIV_06n, DIV_07n, LCDC_EN, lcd);
  //Sprites_tickScanner(clk_sig1, clk_sig2, prev.lcd, rst_sig1, prev.spr, spr);
  //dec.tick(bus, prev.clk_reg, BOOT_BIT, MODE_DBG2, ADDR_VALID);

  //----------------------------------------

  int clk_phase;

  SystemSignals  sys_sig;
  ClockSignals1  clk_sig1;
  ClockSignals2  clk_sig2;
  Clocks         clk_reg;

  ResetSignals1  rst_sig1;
  ResetRegisters rst_reg;

  /*
  Bus     bus;
  LCD     lcd;
  Sprites spr;
  Decoder dec;
  Video   vid;
  */

  uint64_t alignment_pad = 0;
};

//-----------------------------------------------------------------------------

struct LCDTest {

  //----------------------------------------
  // The result of calling reset() should match the result of calling
  // run_reset_sequence();

  void check_match_gb(const TestGB& gb1, const TestGB& gb2) {
    check_match(gb1.clk_phase,  gb2.clk_phase);
    check_match(gb1.sys_sig,    gb2.sys_sig);
    check_match(gb1.clk_sig1,   gb2.clk_sig1);
    check_match(gb1.clk_sig2,   gb2.clk_sig2);
    check_match(gb1.clk_reg,    gb2.clk_reg);

    check_match(gb1.rst_sig1,   gb2.rst_sig1);
    check_match(gb1.rst_reg,    gb2.rst_reg);

    //check_match(gb1.bus, gb2.bus);
    //check_match(gb1.lcd, gb2.lcd);
    //check_match(gb1.spr, gb2.spr);
    //check_match(gb1.vid, gb2.vid);
  }

  void test_reset() {
    TestGB gb1;
    TestGB gb2;

    memset(&gb1, 0, sizeof(gb1));
    memset(&gb2, 0, sizeof(gb2));

    gb1.run_reset_sequence();
    gb2.reset();

    //check(gb1.lcd.x() == 113);
    //check(gb1.lcd.y() == 0);
    //check(gb2.lcd.x() == 113);
    //check(gb2.lcd.y() == 0);

    //check_match(gb1, gb2);
    check_match_gb(gb1, gb2);

    gb1.sim_slow(1);
    gb2.sim_slow(1);

    //check(gb1.lcd.x() == 0);
    //check(gb1.lcd.y() == 0);
    //check(gb2.lcd.x() == 0);
    //check(gb2.lcd.y() == 0);

    //check_match(gb1, gb2);
    check_match_gb(gb1, gb2);

    printf("test_reset3 pass\n");
  }

  //----------------------------------------

  void sim_fast_slow(TestGB& gb1, TestGB& gb2, int phases) {
    for (int i = 0; i < phases; i++) {
      gb1.sim_slow(1);
      gb2.sim_fast(1);
      //check_match(gb1, gb2);
      check_match_gb(gb1, gb2);
    }
  }

  void test_fast_slow() {
    TestGB gb1;
    TestGB gb2;

    memset(&gb1, 0, sizeof(gb1));
        
    gb1.clk_phase = -1;
    gb1.sys_sig.set_pwron();

    memset(&gb2, 0, sizeof(gb2));
    gb2.clk_phase = -1;
    gb2.sys_sig.set_pwron();

    sim_fast_slow(gb1, gb2, 16);

    gb1.sys_sig.set_rst(false);
    gb2.sys_sig.set_rst(false);
    sim_fast_slow(gb1, gb2, 16);

    gb1.sys_sig.set_clk_good(true);
    gb2.sys_sig.set_clk_good(true);
    sim_fast_slow(gb1, gb2, 16);

    gb1.sys_sig.set_clk_req(true);
    gb2.sys_sig.set_clk_req(true);
    sim_fast_slow(gb1, gb2, 16);

    gb1.sys_sig.set_lcdc_en(true);
    gb2.sys_sig.set_lcdc_en(true);
    sim_fast_slow(gb1, gb2, 456*2*154*2);

    printf("test_fast_slow pass\n");
  }

  //----------------------------------------

  void sdl_run() {

    TestGB gb;
    gb.reset();

    const int fb_width = 1900;
    const int fb_height = 1000;
    int scale = 2;

    SDL_Window* window = SDL_CreateWindow("MetroBoy Trace Debugger", 4, 35, fb_width, fb_height, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture* fb_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, fb_width, fb_height);

    uint32_t* background = new uint32_t[fb_width * fb_height];
    for (int y = 0; y < fb_height; y++) {
      for (int x = 0; x < fb_width; x++) {
        int c = ((x ^ y) & 0x20) ? 0x10101010 : 0x15151515;
        background[x + y * fb_width] = c;
        //background[x + y * fb_width] = 0;
      }
    }

    //----------

    int frame = 0;

    bool quit = false;
    while (!quit) {
      SDL_Event event;
      while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) quit = true;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) quit = true;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RIGHT) scale++;
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_LEFT) scale--;

        if (scale < 1) scale = 1;
      }

      //----------------------------------------
      // Clear screen

      uint32_t* framebuffer = nullptr;
      int pitch = 0;
      SDL_LockTexture(fb_tex, NULL, (void**)(&framebuffer), &pitch);
      memcpy(framebuffer, background, fb_width * fb_height * 4);

      //----------------------------------------

      for (int y = 0; y < 154; y++) {
        for (int x = 0; x < TCYCLES_LINE * 2; x++) {
          gb.sim_fast(1);
          
          uint32_t color = 0;

          //if (gb1.spr.SCAN_DONE_d0)       color = 0x8080FF;
          //if (spr.SCAN_DONE_d4) color = 0xFF8080;
          //if (spr.SCAN_DONE_d5) color = 0x80FF80;
          //if (gb1.spr.SCAN_DONE_d0_TRIG)  color = 0xFFFFFF;

          //color = (gb.lcd.x() << 8) | (gb.lcd.y() << 0);

          //color = phase_to_color[gb.clk_phase & 7];

          if (scale == 1) {
            int px = x * scale + 10;
            int py = y * scale + 10;
            if (px < fb_width && py < fb_height) {
              framebuffer[px + py * (pitch / 4)] = color;
            }
          }
          else {
            for (int dy = 0; dy < scale - 1; dy++) {
              for (int dx = 0; dx < scale - 1; dx++) {
                int px = x * scale + dx + 10;
                int py = y * scale + dy + 10;
                if (px < fb_width && py < fb_height) {
                  framebuffer[px + py * (pitch / 4)] = color;
                }
              }
            }
          }

        }
      }

      if (frame & 1) {
        for (int y = 0; y < 10; y++) {
          for (int x = 0; x < 10; x++) {
            framebuffer[(x + 0) + (y + 0) * (pitch / 4)] = 0xFFAAAA00;
          }
        }
      }

      //----------------------------------------
      // Swap

      SDL_UnlockTexture(fb_tex);
      SDL_RenderCopy(renderer, fb_tex, NULL, NULL);
      SDL_RenderPresent(renderer);
      frame++;
    }
  }
};

//-----------------------------------------------------------------------------

void TestLCD() {

  printf("TestGB size %zd\n", sizeof(TestGB));

  LCDTest test;

  test.test_reset();
  test.test_fast_slow();
  //test.sdl_run();

  return;
}

//-----------------------------------------------------------------------------


