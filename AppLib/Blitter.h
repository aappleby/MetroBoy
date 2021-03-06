#pragma once

#include "AppLib/Viewport.h"

//-----------------------------------------------------------------------------

class Blitter {
public:
  void init();

  void blit(Viewport view,
            uint32_t tex, int tw, int th,
            int sx, int sy, int sw, int sh,
            int dx, int dy, int dw, int dh);

  void blit(Viewport view, uint32_t tex, int x, int y, int w, int h) {
    blit(view, tex, w, h, 0, 0, w, h, x, y, w, h);
  }

  void blit_mono(Viewport view,
                 uint32_t tex, int tw, int th,
                 int sx, int sy, int sw, int sh,
                 int dx, int dy, int dw, int dh);

  uint32_t blit_vao = 0;
  uint32_t blit_vbo = 0;
  uint32_t blit_ubo = 0;
};

//-----------------------------------------------------------------------------