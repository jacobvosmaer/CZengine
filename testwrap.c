
#include "engine.h"
#include <stdio.h>
#define nelem(x) (sizeof(x) / sizeof(*(x)))
#define eps 0.01
float phasewrap(float f);
int main(void) {
  struct {
    float in, out;
  } *t, tests[] = {{0.0, 0.0},  {1.0, 1.0}, {0.5, 0.5}, {-0.3, 0.7},
                   {-1.2, 0.8}, {1.2, 0.2}, {3.4, 0.4}};
  for (t = tests; t < tests + nelem(tests); t++) {
    float x = phasewrap(t->in);
    if (x < t->out - eps || x > t->out + eps)
      fprintf(stderr, "fail: phasewrap(%g)=%g, expect %g\n", t->in, x, t->out);
  }
}
