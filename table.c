/* generate CZ transform numbers for gnuplot */
#include "transform.h"
#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define N 10000
float pwlin(float x, float m, struct transform t);
int main(int argc, char **argv) {
  if (argc != 3)
    errx(-1, "usage: graph TRANSFORM M");
  float m;
  int nt = atoi(argv[1]);
  if (nt < 0 || nt >= nelem(transform))
    errx(-1, "invalid transform index: %s", argv[1]);
  sscanf(argv[2], "%f", &m);
  if (m <= 0.0 || m > 0.5)
    errx(-1, "invalid M value: %s", argv[2]);
  for (int i = 0; i < N; i++) {
    float phase = (float)i / N, pd = pwlin(phase, m, transform[nt]);
    printf("%g\t%g\t%g\n", phase, pd, cosf(2 * 3.141592654 * pd));
  }
  return 0;
}
