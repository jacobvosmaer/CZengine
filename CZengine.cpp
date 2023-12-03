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

float pwlin(float x, struct point *curve, int np) {
  for (; np-- >= 2; curve++)
    if (x < curve[1].x)
      return curve[0].y + (x - curve[0].x) * (curve[1].y - curve[0].y) /
                              (curve[1].x - curve[0].x);
  return x;
}

struct {
  float phase;
  float freq;
} dco;

struct {
  int sat, squ;
  float M;
} dcw;

float dcw_process(float phase) {
  struct point sat[] = {{0, 0}, {dcw.M, 0.5}, {1, 1}},
               squ[] = {{0, 0},
                        {0.25f - 0.5f * dcw.M, 0},
                        {0.25f + 0.5f * dcw.M, 0.5f},
                        {0.75f - 0.5f * dcw.M, 0.5f},
                        {0.75f + 0.5f * dcw.M, 1},
                        {1, 1}};
  if (dcw.sat)
    phase = pwlin(phase, sat, nelem(sat));
  else if (dcw.squ)
    phase = pwlin(phase, squ, nelem(squ));
  return cosf(2.0 * pi * phase);
}

float hztofreq(float hz) { return hz / hw.AudioSampleRate(); }

static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size) {
  hw.ProcessAllControls();
  if (hw.button1.RisingEdge()) {
    dcw.sat = !dcw.sat;
    dcw.squ = !dcw.squ;
  }
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
  dcw.sat = 1;
  dcw.squ = 0;
  hw.StartAudio(AudioCallback);
  while (1)
    ;
}
