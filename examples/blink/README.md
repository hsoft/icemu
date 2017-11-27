# Blink

This project makes the `B1` pin blink on a ATtiny45. This will actually compile if you have the
`avr-libc` toolchain installed and if you flash the executable to a ATtiny45 MCU, `B1` will
actually blink. You can do so with `make` and `make send`.

... But you don't even need to because you can simulate the exact same code!

Run `make sim` and then `./blink` to run the simulated MCU. You'll see the ascii art
reprensentation of an ATtiny MCU with a `B1` pin blinking. The code powering the blinking is the
exact same code as the real deal.

Around this, there's significant boilerplate, I'll admit. But such coolness doesn't some for
free...

First, there's `layer.c` and `layer.h` which is an abstraction layer over `avr-libc`'s API. In your
projects, you'll have to write your own because it will vary wildly depending on your needs.

This layer allows us to, during `make sim`, substitute `layer.c` with shims in `sim.c` which is the
unit that configures and run the simulation using `icemu`.

## Runtime features

This example also showcases runtime features of `icemu`. While it's running, you'll notice the
"(?) Keybindings" label. That means that you can see the keybindings list by pressing `?`. If you
do that you'll see that `icemu` already has some time control mechasism built-in such as Pause, run
a single "tick" or the really cool "run until next trigger".

For that last feature to work, you have to tell the simulation, in `sim.c`, which pins are going
to be your triggers. In the blink example, we set `PB1` to be a trigger. This means that, when in
"run until next trigger" mode, as soon as `PB1` changes state, we pause the simulation. Very useful
for debugging!
