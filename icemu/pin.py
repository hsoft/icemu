class Pin:
    def __init__(self, code, high=False, chip=None, output=False):
        self.code = code.replace('~', '')
        self.high = high
        self.chip = chip
        self.output = output
        self.low_means_enabled = code.startswith('~')
        self.wires = set()

    def __str__(self):
        isoutput = 'O' if self.output else 'I'
        ishigh = 'H' if self.ishigh() else 'L'
        return '{}/{}/{}'.format(self.code, isoutput, ishigh)

    def ishigh(self):
        if not self.output and self.wires:
            wired_outputs = [p for p in self.wires if p.output]
            if wired_outputs:
                return any(p.ishigh() for p in wired_outputs)
            else:
                return self.high
        else:
            return self.high

    def propagate_to(self):
        if self.output:
            return {
                p.chip for p in self.wires
                if not p.output and p.chip is not self and p.chip is not None
            }
        else:
            return set()

    def set(self, val):
        if val == self.high:
            return

        self.high = val
        if not self.output and self.chip:
            self.chip.update()

        if self.output:
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
