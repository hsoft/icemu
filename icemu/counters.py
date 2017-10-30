from .chip import ActivableChip
from .util import set_binary_value

class BinaryCounter(ActivableChip):
    CLOCK_PIN = ''

    def __init__(self, *args, **kwargs):
        self.value = 0
        super().__init__(*args, **kwargs)

    def _pin_change(self, pin):
        super()._pin_change(pin)
        if not self.is_enabled():
            return

        if pin.code == self.CLOCK_PIN:
            if pin.is_oscillating():
                self._update_oscillating(pin)
            elif pin.ishigh():
                self._update_normal(pin)

    def _update_normal(self, clock):
        self.value += 1
        if self.value > self.maxvalue():
            self.value = 0

        set_binary_value(self.value, self.getpins(self.OUTPUT_PINS))

    def _update_oscillating(self, clock):
        for i, opin in enumerate(self.getoutputpins(), start=1):
            opin.set_oscillating_freq(clock.oscillating_freq() // (2 ** i))

    def maxvalue(self):
        return (1 << len(self.OUTPUT_PINS)) - 1


class SN74F161AN(BinaryCounter):
    CLOCK_PIN = 'CLK'
    ENABLE_PINS = ['ENT', 'ENP']
    STARTING_HIGH = ENABLE_PINS
    INPUT_PINS = [CLOCK_PIN] + ENABLE_PINS
    OUTPUT_PINS = ['QA', 'QB', 'QC', 'QD']

