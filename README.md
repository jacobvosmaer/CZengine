# Casio CZ phase distortion on Daisy Pod

This is an exploration of "phase distortion synthesis" as used by the Casio CZ series synthesizers. The code is based on reading the [original patent](https://patents.google.com/patent/US4658691A/en) and listening to a real CZ. 

Compile with `make` and upload with `make program` or `make program-dfu`.

The synth emits a continus tone.

- Switch SW1 cycles through different phase distortion profiles: sawtooth, square, impulse, null, sine impulse and sawsquare.
- Potentiometer POT1 sets the distortion amount. Fully clockwise is a sine wave, fully counter clockwise is maximal phase distortion.
- POT2 sets the pitch of the drone.
- The other controls of the Daisy Pod are ignored.
