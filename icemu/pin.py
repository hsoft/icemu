from .util import fmtfreq

class Pin:
    def __init__(self, code, high=False, chip=None, output=False, oscillating_freq=0):
        self.code = code.replace('~', '')
        self.high = high
        self.chip = chip
        # an oscillating pin is always output.
        self.output = bool(oscillating_freq) or output
        self._oscillating_freq = oscillating_freq # in Hz
        self.low_means_enabled = code.startswith('~')
        self.wires = set()

    def __str__(self):
        isoutput = 'O' if self.output else 'I'
        oscillating_freq = self.oscillating_freq()
        if oscillating_freq:
            ishigh = '~{}'.format(fmtfreq(oscillating_freq))
        else:
            ishigh = 'H' if self.ishigh() else 'L'
        return '{}/{}/{}'.format(self.code, isoutput, ishigh)

    __repr__ = __str__

    def ishigh(self):
        if self.is_oscillating():
            return True
        if not self.output and self.wires:
            wired_outputs = [p for p in self.wires if p.output]
            if wired_outputs:
                return any(p.ishigh() for p in wired_outputs)
            else:
                return self.high
        else:
            return self.high

    # An oscillator is a pin that oscillates between low and high at a high frequency, too high
    # for the simulator to compute logic changes. The visual symbol we use for it is "~". When
    # plugging such a pin in a chip, the logic changes completely and, instead of having low or
    # high outputs, we have oscillating outputs with the same or a different frequency. An
    # oscillating pin, logically, always evaluates to "high".
    def oscillating_freq(self):
        if self.output:
            return self._oscillating_freq
        else:
            wired_outputs = [p for p in self.wires if p.output]
            max_freq = max((p.oscillating_freq() for p in wired_outputs), default=0)
            return max_freq

    def is_oscillating(self):
        return bool(self.oscillating_freq())

    def propagate_to(self):
        if self.output:
            return {
                p.chip for p in self.wires
                if not p.output and p.chip is not self and p.chip is not None
            }
        else:
            return set()

    def set(self, val):
        if self.is_oscillating():
            self._oscillating_freq = 0

        if val == self.high:
            return

        self.high = val
        if not self.output and self.chip:
            self.chip.update()

        if self.output:
            wired_chips = self.propagate_to()
            for chip in wired_chips:
                chip.update()

    def set_oscillating_freq(self, freq):
        self._oscillating_freq = freq
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
