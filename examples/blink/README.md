# Blink

This project makes the `B1` pin blink on a ATtiny45. This will actually compile if you have the
`avr-libc` toolchain installed and if you flash the executable to a ATtiny45 MCU, `B1` will
actually blink. You can do so with `make` and `make send`.

... But you don't even need to because you can simulate the exact same code!

Run `make sim` and then `python3 circuit.py` to run the simulated MCU. You'll see the ascii art
reprensentation of an ATtiny MCU with a `B1` pin blinking. The code powering the blinking is the
exact same code as the real deal.

Around this, there's significant boilerplate, I'll admit. But such coolness doesn't some for
free...

First, there's `layer.c` and `layer.h` which is an abstraction layer over `avr-libc`'s API. In your
projects, you'll have to write your own because it will vary wildly depending on your needs.

This layer allows us to, during `make sim`, substitute `layer.c` with `layer_sim.c` which is a thin
layer that calls ICemu's C api (in the `capi` folder`).

Then, there's `circuit.py` which defines and runs our circuit. It will call the `blink` executable
we've built with `make sim` and communicate with it through `stdin` and `stdout`, translating this
communcation to simulated pin action.
