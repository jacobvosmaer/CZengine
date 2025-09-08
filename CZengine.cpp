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

float shapes[] = {0.5, 0.6, 0.7, 0.8, 0.9, 0.99};

using namespace daisy;
DaisyPod hw;

struct {
  float phase;
  float freq;
} dco;

struct {
  int shape1, shape2;
} dcw;

float hztofreq(float hz) { return hz / hw.AudioSampleRate(); }

static void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                          AudioHandle::InterleavingOutputBuffer out,
                          size_t size) {
  hw.ProcessAllControls();
  if (hw.button1.RisingEdge())
    dcw.shape1 = (dcw.shape1 + 1) % nelem(shapes);
  if (hw.button2.RisingEdge())
    dcw.shape2 = (dcw.shape2 + 1) % nelem(shapes);
  dco.freq = hztofreq(20.0 * powf(2.0, 11.0 * hw.knob2.Process()));

  for (int i = 0; i < (int)size; i += 2) {
    float M1 = 4.0 * hw.knob1.Process() *
               dcwshape(phasorupdate(&dco.phase, dco.freq), transform[0],
                        shapes[dcw.shape1]),
          M2 = dcwshape(M1, transform[0], shapes[dcw.shape2]);

    out[i] = out[i + 1] = M2;
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
