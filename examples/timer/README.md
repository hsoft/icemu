# Timer

(If you haven't yet, look at the `blink` example first, it's a simpler first step)

This project demonstrates AVR timers simulation. In our abstraction layer, we have a helper
function that sets up a timer with an interval of 0.5s that triggers an interrupt that will then
make `PB1` blink.

On the simulation side, we use the same helper function, but this time, it tells ICemu to start
a simulated timer that makes us call our interrupt function. Because it's a simulated interrupt,
however, we unfortunately have to (in simulation mode only, of course), continually check for a
simulated interrupt in our main loop.
