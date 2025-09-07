#include "daisy_pod.h"

extern "C" {
#include "engine.h"
}

#define nelem(x) (sizeof(x) / sizeof(*(x)))

struct curve sawtooth[] = {{0, 0, 0}, {0, 1, 0.5}, {1, 0, 1}},
             square[] = {{0, 0, 0},           {0.25f, -0.5f, 0},
                         {0.25f, 0.5f, 0.5f}, {0.75f, -0.5f, 0.5f},
                         {0.75f, 0.5f, 1},    {1, 0, 1}},
             impulse[] = {{0, 0, 0}, {0.5f, -1, 0}, {0.5f, 1, 1}, {1, 0, 1}},
             null[] = {{0, 0}}, sineimpulse[] = {{0, 00}, {0, 1, 1}, {1, 0, 2}},
             sawsquare[] = {{0, 0, 0}, {0.5, 0, 0.5}, {0.5f, 1, 1}, {1, 0, 1}};

struct transform transform[] = {
    {sawtooth, nelem(sawtooth)},       {square, nelem(square)},
    {impulse, nelem(impulse)},         {null, nelem(null)},
    {sineimpulse, nelem(sineimpulse)}, {sawsquare, nelem(sawsquare)}};

using namespace daisy;
DaisyPod hw;

struct {
  float phase;
  float freq;
} dco;

struct {
  unsigned wav;
  float M;
} dcw;

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

  for (int i = 0; i < (int)size; i += 2)
    out[i] = out[i + 1] =
        dcwshape(0.25 * cosf(2.0 * pi * phasorupdate(&dco.phase, dco.freq)),
                 transform[dcw.wav], dcw.M);
}

int main(void) {
  hw.Init();
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_96KHZ);
  hw.StartAdc();
  hw.StartAudio(AudioCallback);
  while (1)
    ;
}
