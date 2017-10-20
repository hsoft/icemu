# Multiplex

(If you haven't yet, look at the `blink` example first, it's a simpler first step)

This project demonstrates the fading mechanism of simulated LEDs. We have an array of 8 LEDs
connected to a decoder and have the decoder change its selected pin every 5 ms. Because the fade of
our simulated LED is 10ms, that means that we always have 2-3 LEDs being lit even if only one of
them has current at any given time.
