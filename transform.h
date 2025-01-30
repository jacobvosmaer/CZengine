#ifndef TRANSFORM_H
#define TRANSFORM_H
#define nelem(x) (sizeof(x) / sizeof(*(x)))

struct curve {
  float x, m, y;
} sawtooth[] = {{0, 0, 0}, {0, 1, 0.5}, {1, 0, 1}},
  square[] = {{0, 0, 0},           {0.25f, -0.5f, 0},
              {0.25f, 0.5f, 0.5f}, {0.75f, -0.5f, 0.5f},
              {0.75f, 0.5f, 1},    {1, 0, 1}},
  impulse[] = {{0, 0, 0}, {0.5f, -1, 0}, {0.5f, 1, 1}, {1, 0, 1}},
  null[] = {{0, 0}}, sineimpulse[] = {{0, 00}, {0, 1, 1}, {1, 0, 2}},
  sawsquare[] = {{0, 0, 0}, {0.5, 0, 0.5}, {0.5f, 1, 1}, {1, 0, 1}};

struct transform {
  struct curve *curve;
  int n;
} transform[] = {
    {sawtooth, nelem(sawtooth)},       {square, nelem(square)},
    {impulse, nelem(impulse)},         {null, nelem(null)},
    {sineimpulse, nelem(sineimpulse)}, {sawsquare, nelem(sawsquare)}};
#endif
