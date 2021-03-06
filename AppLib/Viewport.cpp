#include "AppLib/Viewport.h"
#include <math.h>

//-----------------------------------------------------------------------------

double ema_coeff1(double fc, double fs) {
  double c = cos(2.0 * 3.14159265358979 * fc / fs);
  double a = c - 1 + sqrt(c * (c - 4) + 3);
  return a;
}

double ease(double a, double b, double delta) {
  if (a == b) return b;
  double t = 1.0 - pow(0.1, delta / 0.08);
  double c = a + (b - a) * t;
  return ((float)c == (float)a) ? b : c;
}

dvec2 ease(dvec2 a, dvec2 b, double delta) {
  return {
    ease(a.x, b.x, delta),
    ease(a.y, b.y, delta),
  };
}

Viewport ease(Viewport a, Viewport b, double delta) {
  return {
    ease(a.min, b.min, delta),
    ease(a.max, b.max, delta),
    a.screen_size
  };
}

//-----------------------------------------------------------------------------

dvec2 Viewport::worldToScreen(dvec2 v) const {
  double x = screen_size.x * (v.x - min.x) / (max.x - min.x);
  double y = screen_size.y * (v.y - min.y) / (max.y - min.y);
  return {x, y};
}

dvec2 Viewport::screenToWorld(dvec2 v) const {
  double x = (v.x / screen_size.x) * (max.x - min.x) + min.x;
  double y = (v.y / screen_size.y) * (max.y - min.y) + min.y;
  return {x, y};
}

dvec2 Viewport::deltaScreenToWorld(dvec2 delta) const {
  double x = (delta.x / screen_size.x) * (max.x - min.x);
  double y = (delta.y / screen_size.y) * (max.y - min.y);
  return {x, y};
}

double Viewport::get_zoom() const {
  double w = screen_size.x / (max.x - min.x);
  double z = log2(w);
  return z;
}

//-----------------------------------------------------------------------------

Viewport Viewport::center_on(dvec2 c) {
  return {
    {
      c.x - screen_size.x / 8,
      c.y - screen_size.y / 8,
    },
    {
      c.x + screen_size.x / 8,
      c.y + screen_size.y / 8,
    },
    screen_size,
  };
}

//-----------------------------------------------------------------------------

Viewport Viewport::zoom(dvec2 screen_pos, double zoom) {

  Viewport& a = *this;

  double nx = screen_pos.x / screen_size.x;
  double ny = screen_pos.y / screen_size.y;

  double aw = a.max.x - a.min.x;
  double ah = a.max.y - a.min.y;

  Viewport b;
  b.screen_size = a.screen_size;

  double scale = exp2(-zoom);
  b.min.x = a.min.x + nx * aw * (1 - scale);
  b.min.y = a.min.y + ny * ah * (1 - scale);
  b.max.x = b.min.x + aw * scale;
  b.max.y = b.min.y + ah * scale;

  return b;
}

//-----------------------------------------------------------------------------

Viewport Viewport::pan(dvec2 delta) {
  const Viewport& a = *this;
  Viewport b;
  b.screen_size = screen_size;

  const double scale = screen_size.x / (max.x - min.x);
  b.min.x = a.min.x + double(-delta.x) / scale;
  b.min.y = a.min.y + double(-delta.y) / scale;
  b.max.x = a.max.x + double(-delta.x) / scale;
  b.max.y = a.max.y + double(-delta.y) / scale;

  return b;
}

//-----------------------------------------------------------------------------

Viewport Viewport::snap() {
  const Viewport& a = *this;
  Viewport b;
  b.screen_size = a.screen_size;

  const double zoom  = round(a.get_zoom() * 4.0) / 4.0;
  const double scale = exp2(zoom);

  b.min.x = round(a.min.x * scale) / scale;
  b.min.y = round(a.min.y * scale) / scale;
  b.max.x = round(a.max.x * scale) / scale;
  b.max.y = round(a.max.y * scale) / scale;

  return b;
}

//-----------------------------------------------------------------------------

Viewport Viewport::ease(Viewport target, double delta) {
  return ::ease(*this, target, delta);
}

//-----------------------------------------------------------------------------
