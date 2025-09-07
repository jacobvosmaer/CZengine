float phasorupdate(float *phasor, float frequency);
struct curve {
  float x, m, y;
};
struct transform {
  struct curve *curve;
  int n;
};
float dcwshape(float phase, struct transform t, float shape);
