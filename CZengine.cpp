#include "daisy_pod.h"
#include <string.h>

#undef assert
#define assert(x)                                                              \
  if (!(x))                                                                    \
  asm("bkpt 255")

float pi = 3.141592653;

using namespace daisy;
DaisyPod hw;

struct {
  float phase;
  float freq;
} dco;

struct {
  float M;
} dcw;

float dcw_process(float phase) {
  phase = (phase < dcw.M) ? phase / dcw.M : phase / (2.0 - dcw.M);
  return cosf(pi * phase);
}

float hztofreq(float hz) { return hz / hw.AudioSampleRate(); }

static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size) {
  hw.ProcessAllControls();
  dco.freq = hztofreq(20.0 * powf(2.0, 11.0 * hw.knob2.Process()));
  dcw.M = 0.1 + 1.9 * hw.knob1.Process();
  for (int i = 0; i < (int)size; i += 2) {
    dco.phase += dco.freq;
    while (dco.phase > 2.0)
      dco.phase -= 2.0;
    out[i] = out[i + 1] = dcw_process(dco.phase);
  }
}

int main(void) {
  hw.Init();
  hw.StartAdc();
  hw.StartAudio(AudioCallback);
  while (1)
    ;
}
