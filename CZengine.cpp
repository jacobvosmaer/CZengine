#include "daisy_pod.h"
#include <string.h>

#define nelem(x) (sizeof(x) / sizeof(*(x)))

#undef assert
#define assert(x)                                                              \
  if (!(x))                                                                    \
  asm("bkpt 255")

float pi = 3.141592653;

using namespace daisy;
DaisyPod hw;

struct point {
  float x, y;
};

struct transform {
  struct point *curve;
  int np;
};

/* pwlin applies piece-wise the linear function specified by curve to x */
float pwlin(float x, struct transform t) {
  assert(t.np > 0);
  if (x < t.curve[0].x)
    return t.curve[0].y;
  for (; t.np-- >= 2; t.curve++)
    if (x < t.curve[1].x)
      return t.curve[0].y + (x - t.curve[0].x) * (t.curve[1].y - t.curve[0].y) /
                                (t.curve[1].x - t.curve[0].x);
  return t.curve[0].y;
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
  struct point sawtooth[] = {{0, 0}, {dcw.M, 0.5}, {1, 1}},
               square[] = {{0, 0},
                           {0.25f - 0.5f * dcw.M, 0},
                           {0.25f + 0.5f * dcw.M, 0.5f},
                           {0.75f - 0.5f * dcw.M, 0.5f},
                           {0.75f + 0.5f * dcw.M, 1},
                           {1, 1}},
               impulse[] = {{0, 0},
                            {0.5f - dcw.M, 0},
                            {0.5f + dcw.M, 1},
                            {1, 1}},
               null[] = {{0, 0}}, sineimpulse[] = {{0, 0}, {dcw.M, 1}, {1, 2}},
               sawsquare[] = {{0, 0}, {0.5, 0.5}, {0.5f + dcw.M, 1}, {1, 1}};
  struct transform transform[] = {
      {sawtooth, nelem(sawtooth)},       {square, nelem(square)},
      {impulse, nelem(impulse)},         {null, nelem(null)},
      {sineimpulse, nelem(sineimpulse)}, {sawsquare, nelem(sawsquare)}};

  assert(dcw.wav < nelem(transform));
  return cosf(2.0 * pi * pwlin(phase, transform[dcw.wav]));
}

float hztofreq(float hz) { return hz / hw.AudioSampleRate(); }

static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size) {
  hw.ProcessAllControls();
  if (hw.button1.RisingEdge())
    dcw.wav = (dcw.wav + 1) % 6;
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
