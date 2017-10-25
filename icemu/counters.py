from .chip import ActivableChip
from .util import set_binary_value

class BinaryCounter(ActivableChip):
    CLOCK_PIN = ''

    def __init__(self, *args, **kwargs):
        self.value = 0
        self.prev_clock_high = False
        super().__init__(*args, **kwargs)

    def _update_normal(self, clock):
        if clock.ishigh() and not self.prev_clock_high:
            self.value += 1
            if self.value > self.maxvalue():
                self.value = 0

            set_binary_value(self.value, self.getpins(self.OUTPUT_PINS))

        self.prev_clock_high = clock.ishigh()

    def _update_oscillating(self, clock):
        for i, opin in enumerate(self.getoutputpins(), start=1):
            opin.set_oscillating_freq(clock.oscillating_freq() // (2 ** i))

    def maxvalue(self):
        return (1 << len(self.OUTPUT_PINS)) - 1

    def update(self):
        super().update()
        if not self.is_enabled():
            return

        clock = self.getpin(self.CLOCK_PIN)
        if clock.is_oscillating():
            self._update_oscillating(clock)
        else:
            self._update_normal(clock)


class SN74F161AN(BinaryCounter):
    CLOCK_PIN = 'CLK'
    ENABLE_PINS = ['ENT', 'ENP']
    STARTING_HIGH = ENABLE_PINS
    INPUT_PINS = [CLOCK_PIN] + ENABLE_PINS
    OUTPUT_PINS = ['QA', 'QB', 'QC', 'QD']

