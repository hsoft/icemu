# Interrupt

(If you haven't yet, look at the `blink` example first, it's s simpler first step)

Like the `button` project, but uses an interrupt instead of polling.

Pin `B0` is wired to a button output pin and toggling the button (pressing it or depressing it)
triggers an interrupt. the interrupt reads the value on `B0` and sets `B1` to it.

To demonstrate the "atomicity" of the interrupt (we stop running the simulation until the interrupt
is finished running, we mess with `B0`'s value by inverting it right after the call to the
interrupt. This value should never be read by the MCU.

Therefore, in the simulation, `B1`'s value will always be the opposite of `B0` and always be equal
to the `_button_state` variable.
