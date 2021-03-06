#pragma once
#include <string>

#include "AppLib/Viewport.h"

struct LineUniforms {
  vec4 viewport = {};
  vec4 origin = {};
};

class LinePainter {
public:

  void init();
  void update_buf();
  void render_at(Viewport view, double x, double y, float scale = 1.0f);
  void reset();

  void render(Viewport view, double x, double y, float scale = 1.0f);

  void push(float ax, float ay, uint32_t ac, float bx, float by, uint32_t bc);

  void push(dvec2 a, uint32_t ac, dvec2 b, uint32_t bc) {
    push(float(a.x), float(a.y), ac,
         float(b.x), float(b.y), bc);
  }

  void push_box(dvec2 a, dvec2 b, uint32_t c) {
    push(float(a.x), float(a.y), c, float(b.x), float(a.y), c);
    push(float(b.x), float(a.y), c, float(b.x), float(b.y), c);
    push(float(b.x), float(b.y), c, float(a.x), float(b.y), c);
    push(float(a.x), float(b.y), c, float(a.x), float(a.y), c);
  }

  void push_corner_size(dvec2 a, dvec2 s, uint32_t c) {
    push(float(a.x),       float(a.y),       c, float(a.x + s.x), float(a.y), c);
    push(float(a.x + s.x), float(a.y),       c, float(a.x + s.x), float(a.y + s.y), c);
    push(float(a.x + s.x), float(a.y + s.y), c, float(a.x),       float(a.y + s.y), c);
    push(float(a.x),       float(a.y + s.y), c, float(a.x),       float(a.y), c);
  }

private:

  const int max_line_bytes = 16 * 1024 * 1024;

  int inst_begin = 0;
  int line_end = 0;

  uint32_t  line_vao = 0;
  uint32_t  line_vbo = 0;
  uint32_t* line_data_u32 = 0;
  float*    line_data_f32 = 0;

  uint32_t line_ubo = 0;
  LineUniforms line_uniforms = {};
};
