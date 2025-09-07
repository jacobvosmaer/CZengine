#include "engine.h"
static float phasewrap(float f) {
  if (f > 1.0)
    f -= (float)(int)f;
  else if (f < 0.0)
    f += (float)(1 + (int)(-f));
  return f;
}
float phasorupdate(float *phasor, float frequency) {
  *phasor = phasewrap(*phasor + frequency);
  return *phasor;
}
static float pwlin(float x, float m, struct transform t) {
  struct curve *c = t.curve;
  float x1, x0;
  x0 = c[0].x + c[0].m * m;
  if (x < x0)
    return c[0].y;
  for (; t.n-- >= 2; c++, x0 = x1) {
    x1 = c[1].x + c[1].m * m;
    if (x < x1)
      return c[0].y + (x - x0) * (c[1].y - c[0].y) / (x1 - x0);
  }
  return c[0].y;
}
float dcwshape(float phase, struct transform t, float shape) {
  return cosf(2.0 * pi * pwlin(phase, shape, t));
}
