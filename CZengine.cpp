#include "daisy_pod.h"

#define nelem(x) (sizeof(x) / sizeof(*(x)))

#undef assert
#define assert(x)                                                              \
  if (!(x))                                                                    \
  asm("bkpt 255")

float pi = 3.141592653;

using namespace daisy;
DaisyPod hw;

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

float pwlin(float x, float m, struct transform t) {
  struct curve *c = t.curve;
  float x1, x0;

  assert(t.n > 0);
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

struct {
  float phase;
  float freq;
} dco;

struct {
  unsigned wav;
  float M;
} dcw;

float dcw_process(float phase) {
  assert(dcw.wav < nelem(transform));
  return cosf(2.0 * pi * pwlin(phase, dcw.M, transform[dcw.wav]));
}

float hztofreq(float hz) { return hz / hw.AudioSampleRate(); }

static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size) {
  hw.ProcessAllControls();
  if (hw.button1.RisingEdge())
    dcw.wav = (dcw.wav + 1) % nelem(transform);
  dco.freq = hztofreq(20.0 * powf(2.0, 11.0 * hw.knob2.Process()));
  float minM = 0.01;
  dcw.M = minM + (0.5 - minM) * hw.knob1.Process();

  for (int i = 0; i < (int)size; i += 2) {
    dco.phase += dco.freq;
    while (dco.phase >= 1.0)
      dco.phase -= 1.0;
    out[i] = out[i + 1] = dcw_process(dco.phase);
  }
}

int main(void) {
  hw.Init();
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);
  hw.StartAdc();
  hw.StartAudio(AudioCallback);
  while (1)
    ;
}
