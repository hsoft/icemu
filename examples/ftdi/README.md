# FTDI

(If you haven't yet, look at the `blink` example first, it's s simpler first step)

This example demonstrates the use of a FTDI chip. This feature is really cool because it allows
your simulation to leap into the real world incrementally through a USB-plugged FTDI chip.

For this example to work, you need:

* `icemu` compiled with `USE_LIBFTDI` (automatically set if it finds `libftdi`.
* A supported FTDI device plugged into your computer.

In the simulation, we wire the blinking pin of the MCU to one of the pins of our FTDI chip. That
means that if you plug something (a LED for example) to that pin, it's going to blink.
