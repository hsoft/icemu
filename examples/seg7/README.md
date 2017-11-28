# Seg7 - a 7-segments display

(If you haven't yet, look at the `blink` example first, it's s simpler first step)

This project hooks 3 pins from the MCU to a shift register which then controls the 7 segments of
a 7-segments LED display. We displays digits from 0 to 9 with 1 second between every change.

You can run the simulation with `make sim` followed by `./seg7`.

You can compile the real thing and send it to your MCU with `make` and `make send`.
