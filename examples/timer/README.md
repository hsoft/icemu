# Timer

(If you haven't yet, look at the `blink` example first, it's a simpler first step)

This project demonstrates AVR timers simulation. In our abstraction layer, we have a helper
function that sets up a timer with an interval of 0.5s that triggers an interrupt that will then
make `PB1` blink.

On the simulation side, we see that ICemu will properly simulate timers by calling our timer
interrupt function at the proper intervals.
