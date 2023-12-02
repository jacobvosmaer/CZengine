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

float hztofreq(float hz) { return hz / hw.AudioSampleRate(); }

static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size) {
  hw.ProcessAllControls();
  dco.freq = hztofreq(20.0 * powf(2.0, 11.0 * hw.knob2.Process()));
  for (int i = 0; i < (int)size; i += 2) {
    dco.phase += dco.freq;
    while (dco.phase > 2.0)
      dco.phase -= 2.0;
    out[i] = out[i + 1] = sinf(pi * dco.phase);
  }
}

int main(void) {
  hw.Init();
  hw.StartAdc();
  hw.StartAudio(AudioCallback);
  while (1)
    ;
}
