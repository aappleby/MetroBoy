#include "Platform.h"
#include "main.h"

#include "Audio.h"
#include "Common.h"
#include "MetroBoy.h"

#include "test_micro.h"
#include "test_mooneye.h"
#include "test_wpol.h"

//-----------------------------------------------------------------------------

int MetroBoyApp::main_(int /*argc*/, char** /*argv*/) {
  run_microtests();
  //run_mooneye_acceptance();
  //run_wpol_acceptance();
  //return 0;

  enum RunMode {
    RUN_FAST,
    RUN_VSYNC,
    STEP_FRAME,
    STEP_LINE,
    STEP_CYCLE
  };

  bool quit = false;
  int step_forward = 0;
  int step_backward = 0;
  bool step_up = false;
  bool step_down = false;
  bool load_dump = false;
  bool save_dump = false;
  bool reset = false;
  RunMode mode = STEP_FRAME;

  //---------

  bool rom_loaded = false;

  const char* filename = nullptr;
  
  //filename = "wpol-gb/tests/build/acceptance/gpu/hblank_ly_scx_timing_variant_nops.gb";

  filename = "mooneye-gb/tests/build/acceptance/ppu/stat_lyc_onoff.gb";

  //filename = "microtests/build/dmg/hblank_scx_if_a.gb";
  //filename = "microtests/build/dmg/lcdon_to_hblank_int_scx3.gb";
  //filename = "microtests/build/dmg/hblank_scx3_int_a.gb";

  MetroBoy metroboy;

  if (filename) {
    metroboy.load_rom(MODEL_DMG, filename);
    rom_loaded = true;
    mode = RUN_FAST;
  }

  //----------

  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

  window = SDL_CreateWindow("MetroBoy Gameboy Simulator", 4, 34, fb_width, fb_height, SDL_WINDOW_SHOWN);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  fb_tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, fb_width, fb_height);
  terminus_surface = SDL_LoadBMP("terminus2.bmp");
  terminus_font = (uint8_t*)terminus_surface->pixels;
  keyboard_state = SDL_GetKeyboardState(nullptr);

  audio_init();

  uint64_t freq = SDL_GetPerformanceFrequency();

  char text_buf[65536];
  char* cursor = text_buf;

  printf_console("MetroBoy starting...\n");

  uint64_t frame_begin, frame_end, frame_time = 0;
  static double fast_cycles = 114 * 154;

  int frame_count = 0;
  uint8_t buttons = 0;

  while (!quit) {
    frame_begin = SDL_GetPerformanceCounter();

    //----------------------------------------
    // Process events

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_KEYDOWN) switch (event.key.keysym.sym) {
        case SDLK_f:      mode = RUN_FAST; break;
        case SDLK_v:      mode = RUN_VSYNC; break;
        case SDLK_s:      mode = STEP_FRAME; break;
        case SDLK_RIGHT:  step_forward++; break;
        case SDLK_LEFT:   step_backward++; break;
        case SDLK_UP:     step_up = true; break;
        case SDLK_DOWN:   step_down = true; break;

        case SDLK_r:      reset = true; break;
        case SDLK_F1:     load_dump = true; break;
        case SDLK_F4:     save_dump = true; break;
        case SDLK_ESCAPE: quit = true; break;
      }

      if (event.type == SDL_QUIT) quit = true;

      if (event.type == SDL_DROPFILE) {
        metroboy.load_rom(MODEL_DMG, event.drop.file);
        rom_loaded = true;
        mode = RUN_VSYNC;
        SDL_free(event.drop.file);
      }
    }

    //----------------------------------------
    // Button input

    buttons = 0;
    if (keyboard_state[SDL_SCANCODE_RIGHT])  buttons |= 0x01;
    if (keyboard_state[SDL_SCANCODE_LEFT])   buttons |= 0x02;
    if (keyboard_state[SDL_SCANCODE_UP])     buttons |= 0x04;
    if (keyboard_state[SDL_SCANCODE_DOWN])   buttons |= 0x08;
    if (keyboard_state[SDL_SCANCODE_X])      buttons |= 0x10;
    if (keyboard_state[SDL_SCANCODE_Z])      buttons |= 0x20;
    if (keyboard_state[SDL_SCANCODE_RSHIFT]) buttons |= 0x40;
    if (keyboard_state[SDL_SCANCODE_RETURN]) buttons |= 0x80;

    // probably too fast for debugging
    //if (keyboard_state[SDL_SCANCODE_RIGHT])  step_forward++;
    //if (keyboard_state[SDL_SCANCODE_LEFT])   step_backward++;

    //----------------------------------------
    // Handle sim controls

    if (reset) {
      metroboy.reset(0x100);
      reset = false;
    }

    if (load_dump) {
      metroboy.load_dump();
      load_dump = false;
      rom_loaded = true;
    }

    if (save_dump) {
      metroboy.save_dump();
      save_dump = false;
    }

    if (step_up) {
      if (mode == STEP_CYCLE) mode = STEP_LINE;
      else if (mode == STEP_LINE) mode = STEP_FRAME;
    }

    if (step_down) {
      if (mode == STEP_FRAME) mode = STEP_LINE;
      else if (mode == STEP_LINE) mode = STEP_CYCLE;
    }

    //----------------------------------------
    // Run simulation

    int64_t cycles_begin = metroboy.total_tcycles();

    if (rom_loaded) {
      if (mode == RUN_FAST) {
        fast_cycles += (16.0 - 1000 * (double(frame_time) / double(freq))) * 100;
        metroboy.run_fast(buttons, (int)fast_cycles);
      }
      else if (mode == RUN_VSYNC) {
        metroboy.run_vsync(buttons);
      }
      else if (mode == STEP_CYCLE) {
        while (step_forward--) {
          if (keyboard_state[SDL_SCANCODE_LSHIFT]) {
            metroboy.step_over();
          }
          else {
            metroboy.step_cycle();
          }
        }
        while (step_backward--) {
          metroboy.unstep_cycle();
        }
      }
      else if (mode == STEP_FRAME) {
        while (step_forward--) metroboy.step_frame();
        while (step_backward--) metroboy.unstep_frame();
      }
      else if (mode == STEP_LINE) {
        while (step_forward--) metroboy.step_line();
        while (step_backward--) metroboy.unstep_line();
      }
    }

    step_forward = 0;
    step_backward = 0;
    step_up = false;
    step_down = false;

    int64_t cycles_end = metroboy.total_tcycles();

    //----------------------------------------
    // Render stuff

    uint32_t* framebuffer = NULL;
    int pitch = 0;
    SDL_LockTexture(fb_tex, NULL, (void**)(&framebuffer), &pitch);

    for (int y = 0; y < 1024; y++) {
      for (int x = 0; x < 1024; x++) {
        int c = ((x ^ y) & 0x20) ? 0x10101010 : 0x15151515;
        framebuffer[x + y * 1024] = c;
      }
    }

    //----------------------------------------
    // Left column text

    Gameboy& gameboy = metroboy.gb();
    gameboy.dump(text_buf);
    render_text(framebuffer, 4, 4, terminus_font, text_buf);

    gameboy.dump_disasm(text_buf);
    render_text(framebuffer, 140, 4, terminus_font, text_buf);

    //----------------------------------------
    // Gameboy screen

    {
      int sx = (fb_width / 2) - 160;
      int sy = (fb_height / 2) - 128;

      for (int y = 0; y < 144; y++) {
        uint32_t* line1 = &framebuffer[(y * 2 + sy + 0) * fb_width + sx];
        uint32_t* line2 = &framebuffer[(y * 2 + sy + 1) * fb_width + sx];
        for (int x = 0; x < 160; x++) {
          uint32_t c = gb_colors[gameboy.framebuffer[x + (y * 160)] & 7];
          *line1++ = c; *line1++ = c;
          *line2++ = c; *line2++ = c;
        }
      }

      //----------
      // Step highlight

      if (mode == STEP_LINE || mode == STEP_CYCLE) {

        int px = gameboy.get_pix_count();
        int py = gameboy.get_line();

        for (int x = 0; x < 320; x++) {
          int color = (px == (x / 2)) ? 0x00606000 : 0x00600000;
          framebuffer[(sx + x) + (sy + py * 2 + 0) * fb_width] += color;
          framebuffer[(sx + x) + (sy + py * 2 + 1) * fb_width] += color;
        }
      }

      const char* mode_names[] = {
        "RUN_FAST",
        "RUN_VSYNC",
        "STEP_FRAME",
        "STEP_LINE",
        "STEP_CYCLE",
      };

      cursor = text_buf;
      cursor += sprintf(cursor, "%s %d", mode_names[mode], (int)(metroboy.current_gameboy->get_tcycle() & 3));
      render_text(framebuffer, 32 * 11, 32 * 11 + 18, terminus_font, text_buf);
    }

    gameboy.get_ppu().dump_tiles(framebuffer, fb_width, 736, 32, 2, gameboy.get_vram());
    gameboy.get_ppu().draw_bg_map(framebuffer, fb_width, 736, 448, 1, gameboy.get_vram());
    gameboy.get_ppu().draw_wm_map(framebuffer, fb_width, 736, 736, 1, gameboy.get_vram());

    //----------------------------------------
    // Trace buffer

    {
      int sx = 32 * 8;
      int sy = 32 * 22;

      for (int y = 0; y < 154; y++) {
        for (int x = 0; x < 456; x++) {
          framebuffer[(sx + x) + (sy + y) * fb_width] = metroboy.tracebuffer[x + y * 456];
        }
      }
    }

    //----------------------------------------
    // Perf timer

    static double time = 0;
    time *= 0.98;
    time += (1000.0 * double(frame_time) / double(freq)) * 0.02;

    sprintf(text_buf, "frame time %2.2f msec, %6d cyc/frame\n", (double)time, (int)(cycles_end - cycles_begin) / 4);
    render_text(framebuffer, 736, 1024 - 12 - 4, terminus_font, text_buf);

    //----------
    // Console

    //render_console(256 - 32, 1024 - glyph_height * console_height - 4, terminus_font);

    //----------------------------------------
    // Swap

    SDL_UnlockTexture(fb_tex);
    SDL_RenderCopy(renderer, fb_tex, NULL, NULL);

    frame_end = SDL_GetPerformanceCounter();
    frame_time = frame_end - frame_begin;

    SDL_RenderPresent(renderer);
    frame_count++;
  }

  audio_stop();

  SDL_Quit();
  return 0;
}

//-----------------------------------------------------------------------------

void MetroBoyApp::render_text(uint32_t* framebuffer, int dst_x, int dst_y, uint8_t* font, const char* text) {
  int xcursor = 0;
  int ycursor = 0;
  const char* c = text;

  while (*c) {
    if (*c == '\n') {
      xcursor = 0;
      ycursor += glyph_height;
      c++;
      continue;
    }

    int row = ((*c) >> 5) * 16 + 3;
    int col = ((*c) & 31) * 8;
    int src_cursor = col + (row * glyph_stride);
    int dst_cursor = (xcursor + dst_x) + (ycursor + dst_y) * fb_width;

    for (int y = 0; y < glyph_height; y++) {
      for (int x = 0; x < glyph_width; x++) {
        if (font[src_cursor]) framebuffer[dst_cursor] = 0xFF00FF00;
        src_cursor++;
        dst_cursor++;
      }
      src_cursor += (glyph_stride - glyph_width);
      dst_cursor += (fb_width - glyph_width);
    }

    xcursor += glyph_width;
    if (xcursor > (fb_width - glyph_width)) {
      xcursor = 0;
      ycursor += glyph_height;
    }
    c++;
  }
}

//-----------------------------------------------------------------------------

void MetroBoyApp::draw_bbox(int sx, int sy, int w, int h, uint32_t* buf) {
  int ax = sx;
  int bx = sx + w;
  int ay = sy;
  int by = sy + h;
  int x, y;

  for (x = ax, y = ay; x <= bx; x++) {
    if (x >= 0 && x <= fb_width && y >= 0 && y <= fb_height) buf[x + y * fb_width] |= 0x00008000;
  }

  for (x = ax, y = by; x <= bx; x++) {
    if (x >= 0 && x <= fb_width && y >= 0 && y <= fb_height) buf[x + y * fb_width] |= 0x00008000;
  }

  for (x = ax, y = ay + 1; y <= by - 1; y++) {
    if (x >= 0 && x <= fb_width && y >= 0 && y <= fb_height) buf[x + y * fb_width] |= 0x00008000;
  }

  for (x = bx, y = ay + 1; y <= by - 1; y++) {
    if (x >= 0 && x <= fb_width && y >= 0 && y <= fb_height) buf[x + y * fb_width] |= 0x00008000;
  }
}


//-----------------------------------------------------------------------------
// Console

void MetroBoyApp::render_console(uint32_t* framebuffer, int sx, int sy, uint8_t* font) {

  for (int cy = 0; cy < console_height; cy++) {
    char* line = &console_buf[(((cy + cursor_y)) % console_height) * console_width];

    for (int cx = 0; cx < console_width; cx++) {
      char c = *line;
      int row = (c >> 5) * 16 + 3;
      int col = (c & 31) * 8;

      int src_cursor = col + (row * glyph_stride);
      int dst_cursor = (sx + (cx * glyph_width)) + (sy + (cy * glyph_height)) * fb_width;

      for (int y = 0; y < glyph_height; y++) {
        for (int x = 0; x < glyph_width; x++) {
          if (font[src_cursor]) framebuffer[dst_cursor] = 0xFF00FF00;
          src_cursor++;
          dst_cursor++;
        }
        src_cursor += (glyph_stride - glyph_width);
        dst_cursor += (fb_width - glyph_width);
      }

      line++;
    }
  }

  draw_bbox(sx - 2, sy - 2, console_width * glyph_width + 4, console_height * glyph_height + 4, framebuffer);
}

void MetroBoyApp::printf_console(const char* format, ...) {

  char buffer[256];
  va_list args;
  va_start(args, format);
  vsprintf(buffer, format, args);
  va_end(args);

  char* text = buffer;
  while (*text) {
    char c = *text;
    if (c == '\n') {
      for (int i = cursor_x; i < console_width; i++) {
        console_buf[i + cursor_y * console_width] = 0;
      }
      cursor_x = 0;
      cursor_y = (cursor_y + 1) % console_height;
    }
    else {
      console_buf[cursor_x + cursor_y * console_width] = c;
      cursor_x++;
      if (cursor_x == console_width) {
        cursor_x = 0;
        cursor_y++;
        if (cursor_y == console_height) {
          cursor_y = 0;
        }
      }
    }
    text++;
  }
}

//-----------------------------------------------------------------------------

int main(int argc, char** argv) {
  MetroBoyApp app;

  return app.main_(argc, argv);
}

//-----------------------------------------------------------------------------
