# Multi-MCUs

(If you haven't yet, look at the `blink` example first, it's a simpler first step)

This project demonstrates the support for running multiple MCUs at the same time by re-using the
code from the `oscillate` project!

The MCU code in `oscillate` was about counting frequencies on `PB0`? Alright, let's use another
MCU to send it a signal.

To demonstrate that `delay()` works well in multi-MCU, we artificially use delays to generate
signals in our runloop. Using a timer would be better, but we want to demonstrate delays.

You can see that unlike other examples, `oscillate` and `multimcu` bother with prefixing functions
with a namespace. That's because otherwise, they would clash in the simulation because all these
functions end up in the same executable.
