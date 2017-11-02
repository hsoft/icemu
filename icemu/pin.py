from .util import fmtfreq
from ._pin import Pin as PinC

# About oscillation
#
# Oscillation frequence is the number of times per second that an oscillation between low and high
# will occur. By definition, that number is in hertz. A pin with a oscillating_freq of 1 will toggle
# *twice* a second (once for low, once for high).
#
# Setting an oscillation_freq triggers a system where there's two types of oscillation: slow and
# rapid. Slow oscillation is an oscillation that happens faster than twice the time resolution (
# the `usec` value we send to `tick()`. As long as we're slowly oscillating, the pin will stay in
# "normal logic" mode, that is, the same logic of a non-oscillating pin. It would be the equivalent
# of manually writing a timed loop that called `toggle()` on the pin.
#
# Past that threshold, however, the "oscillating" logic takes over. To be able to simulate more
# `update()` calls to our ICs, we stop calling `update()` at all and tell the ICs to "propagate
# oscillation". For example, a 4 bit counter with a rapidly oscillating input would end up with
# would end up with outputs of oscillating freqs or rate/2, rate/4, rate/8 and rate/16, and this,
# without further need to call `update()`, thus saving a lot of processing.
#
# The visual symbol we use for a rapidly oscillating pin is "~" and always evaluates to "high".

C_OVERRIDES = [
    'disable', 'enable', 'isenabled', 'ishigh', 'isoutput', 'is_oscillating',
    'is_oscillating_rapidly', 'is_oscillating_slowly', 'oscillating_freq', 'set', 'sethigh',
    'setlow', 'setoutput', 'setinput', 'set_oscillating_freq', 'tick', 'toggle'
]

class Pin:
    def __init__(self, code, high=False, chip=None, output=False, oscillating_freq=0):
        self.code = code.replace('~', '')
        output = bool(oscillating_freq) or output
        low_means_enabled = code.startswith('~')
        self._c = PinC(
            output=output, high=high, low_means_enabled=low_means_enabled,
            oscillating_freq=oscillating_freq, chip=chip
        )
        for override in C_OVERRIDES:
            setattr(self, override, getattr(self._c, override))
        self.chip = chip
        self.wires = set()

    def __str__(self):
        isoutput = 'O' if self.isoutput() else 'I'
        oscillating_freq = self.oscillating_freq()
        if oscillating_freq:
            ishigh = '~{}'.format(fmtfreq(oscillating_freq))
        else:
            ishigh = 'H' if self.ishigh() else 'L'
        return '{}/{}/{}'.format(self.code, isoutput, ishigh)

    __repr__ = __str__

    def propagate_to(self):
        if self.isoutput():
            return {
                p.chip for p in self.wires
                if not p.isoutput() and p.chip is not self and p.chip is not None
            }
        else:
            return set()

    def wire_to(self, output_pin):
        self.wires.add(output_pin)
        self._c.wire_to(output_pin._c)
        output_pin.wires.add(self)
        output_pin._c.wire_to(self._c)
        if self.chip:
            self.chip.update()

def pinrange(prefix, start, end):
    # pinrange('Y', 0, 3) -> ['Y0', 'Y1', 'Y2', 'Y3']
    return [prefix + str(i) for i in range(start, end+1)]
