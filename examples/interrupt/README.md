# Interrupt

(If you haven't yet, look at the `blink` example first, it's s simpler first step)

Like the `button` project, but uses an interrupt instead of polling.

Pin `B0` is wired to a button output pin and toggling the button (pressing it or depressing it)
triggers an interrupt. the interrupt reads the value on `B0` and sets `B1` to it.
