from .util import fmtfreq, USECS_PER_SECOND
from ._pin import Pin as PinC

# About oscillation
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

C_OVERRIDES = ['isoutput', 'setoutput', 'setinput']

class Pin:
    def __init__(self, code, high=False, chip=None, output=False, oscillating_freq=0):
        self.code = code.replace('~', '')
        output = bool(oscillating_freq) or output
        self._c = PinC(output=output, high=high)
        for override in C_OVERRIDES:
            setattr(self, override, getattr(self._c, override))
        self.chip = chip
        # an oscillating pin is always output.
        self._oscillating_freq = oscillating_freq # in Hz
        # None means "rapid oscillation"
        self._next_oscillation_in = None
        self.low_means_enabled = code.startswith('~')
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

    def _get_wired_oscillator(self):
        assert not self.isoutput()
        wired_outputs = [p for p in self.wires if p.isoutput()]
        return max(wired_outputs, key=lambda p: p.oscillating_freq(), default=None)

    def ishigh(self):
        if self.is_oscillating_rapidly():
            return True
        if not self.isoutput() and self.wires:
            wired_outputs = [p for p in self.wires if p.isoutput()]
            if wired_outputs:
                return any(p.ishigh() for p in wired_outputs)
            else:
                return self._c.ishigh()
        else:
            return self._c.ishigh()

    def oscillating_freq(self):
        if self.isoutput():
            return self._oscillating_freq
        else:
            wired = self._get_wired_oscillator()
            return wired.oscillating_freq() if wired else 0

    def is_oscillating_rapidly(self):
        return self.oscillating_freq() and self.next_oscillation_in() is None

    def is_oscillating_slowly(self):
        return self.oscillating_freq() and self.next_oscillation_in() is not None

    def is_oscillating(self):
        return self.oscillating_freq() > 0

    def next_oscillation_in(self):
        # usecs, only words when is_oscillating_slowly()
        if self.isoutput():
            return self._next_oscillation_in
        else:
            wired = self._get_wired_oscillator()
            return wired.next_oscillation_in() if wired else None

    def tick(self, usecs):
        if self.isoutput() and self.oscillating_freq():
            freq = self.oscillating_freq()
            every_usecs = USECS_PER_SECOND / freq
            rapid = every_usecs * 2 <= usecs
            if rapid:
                self._next_oscillation_in = None
            else:
                self._next_oscillation_in = (self._next_oscillation_in or 0) - usecs
                while self._next_oscillation_in < 0:
                    self.toggle()
                    self._next_oscillation_in += every_usecs

    def propagate_to(self):
        if self.isoutput():
            return {
                p.chip for p in self.wires
                if not p.isoutput() and p.chip is not self and p.chip is not None
            }
        else:
            return set()

    def set(self, val):
        if self.isoutput() and self.is_oscillating_rapidly():
            self._oscillating_freq = 0

        if not self._c.set(val):
            return

        if not self.isoutput() and self.chip:
            self.chip.update()

        if self.isoutput():
            wired_chips = self.propagate_to()
            for chip in wired_chips:
                chip.update()

    def set_oscillating_freq(self, freq):
        self._oscillating_freq = freq
        self._next_oscillation_in = None
        wired_chips = self.propagate_to()
        for chip in wired_chips:
            chip.update()

    def sethigh(self):
        self.set(True)

    def setlow(self):
        self.set(False)

    def toggle(self):
        self.set(not self.ishigh())

    def enable(self):
        self.set(not self.low_means_enabled)

    def disable(self):
        self.set(self.low_means_enabled)

    def isenabled(self):
        return self.low_means_enabled != self.ishigh()

    def wire_to(self, output_pin):
        self.wires.add(output_pin)
        output_pin.wires.add(self)
        if self.chip:
            self.chip.update()

def pinrange(prefix, start, end):
    # pinrange('Y', 0, 3) -> ['Y0', 'Y1', 'Y2', 'Y3']
    return [prefix + str(i) for i in range(start, end+1)]
