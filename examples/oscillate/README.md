# Oscillate

(If you haven't yet, look at the `blink` example first, it's a simpler first step)

Demonstrates the oscillating feature in `icemu`. This MCU-less circuit (which is why
there is no `main.c`) simply has a oscillator being the input for ICs that currently
support (propagate) oscillating signals to their outputs.

For example, we see that if we have a 1MHz input plugged in a counter IC, we end up
with a prescaler that give us 500kHz, 250kHz, etc. outputs.
