# Oscillate

(If you haven't yet, look at the `blink` example first, it's a simpler first step)

Demonstrates the oscillating feature in `icemu`. This circuit has an oscillator being the input for
ICs that currently support (propagate) oscillating signals to their outputs.

For example, we see that if we have a 1MHz input plugged in a counter IC, we end up
with a prescaler that give us 500kHz, 250kHz, etc. outputs.

And the end of this all is our MCU which has an interrupt set on `PB0` and a timer that runs every
100ms and records frequency. We use `icemu` debug capabilities to get that frequency which is
displayed at the bottom right of the screen. We thus see that we get the proper amount of interrupt
calls on oscillating pins!

You'll notice that functions in this project are prefixed with `osc_` when other examples don't
have such prefixes. That is because this project is reused in the `multimcu` example and we need
namespaces for these functions.
