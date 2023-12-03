#include "daisy_pod.h"
#include <string.h>

#undef assert
#define assert(x)                                                              \
  if (!(x))                                                                    \
  asm("bkpt 255")

#define seg(x, x0, y0, x1, y1, z)                                              \
  ((x) >= x0 && (x) < x1) ? y0 + ((x)-x0) * (y1 - y0) / (x1 - x0) : (z)

float pi = 3.141592653;

using namespace daisy;
DaisyPod hw;

struct {
  float phase;
  float freq;
} dco;

struct {
  int sat, squ;
  float M;
} dcw;

float dcw_process(float phase) {
  if (dcw.sat)
    phase = seg(phase, 0.0, 0.0, dcw.M, 0.5,
                seg(phase, dcw.M, 0.5, 1.0, 1.0, phase));
  else if (dcw.squ)
    phase = seg(
        phase, 0.0, 0.0, 0.25 - 0.5 * dcw.M, 0.0,
        seg(phase, 0.25 - 0.5 * dcw.M, 0.0, 0.25 + 0.5 * dcw.M, 0.5,
            seg(phase, 0.25 + 0.5 * dcw.M, 0.5, 0.75 - 0.5 * dcw.M, 0.5,
                seg(phase, 0.75 - 0.5 * dcw.M, 0.5, 0.75 + 0.5 * dcw.M, 1.0,
                    seg(phase, 0.75 + 0.5 * dcw.M, 1.0, 1.0, 1.0, phase)))));
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
