#include "daisy_pod.h"

#undef assert
#define assert(x) if (!(x)) asm("bkpt 255")

using namespace daisy;
DaisyPod hw;

int main(void) {
  hw.Init();
  while(1)
    ;
}
