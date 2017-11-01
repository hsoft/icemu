from .chip import ActivableChip
from .util import get_binary_value, combinations_inner

class Decoder(ActivableChip):
    SERIAL_PINS = [] # LSB pin goes first
    RESULT_PINS = [] # LSB pin goes first

    def _update_normal(self, spins):
        selection = get_binary_value(spins)
        for index, pin in enumerate(self.getpins(self.RESULT_PINS)):
            pin.set(index != selection)

    def _update_oscillating(self, spins):
        # determining oscillating behavior in a decoder when considering the possibility
        # of more than one input pin oscillating quickly becomes complicated: the output
        # is practically random. Let's just be statistical and consider that each possible
        # outputs in the oscillating range is going to get an equal part of the output time.
        # we use the maximum frequency value among input pins.
        base_selection = get_binary_value(spins) # selection when all oscillating pins are high
        oscillating_indexes = {i for i, p in enumerate(spins) if p.is_oscillating_rapidly()}
        selection_range = {base_selection,}
        for lowpins in combinations_inner(oscillating_indexes):
            sel = base_selection
            for index in lowpins:
                sel &= ~(1 << index)
            selection_range.add(sel)
        max_freq = max(p.oscillating_freq() for p in spins)
        freq = max_freq // len(selection_range)
        for index, pin in enumerate(self.getpins(self.RESULT_PINS)):
            if index in selection_range:
                pin.set_oscillating_freq(freq)
            else:
                pin.sethigh()

    def update(self):
        super().update()
        if not self.is_enabled():
            self.setpins(high=self.RESULT_PINS)
            return

        spins = list(self.getpins(self.SERIAL_PINS))
        if any(p.is_oscillating_rapidly() for p in spins):
            self._update_oscillating(spins)
        else:
            self._update_normal(spins)


class SN74HC138(Decoder):
    OUTPUT_PINS = ['Y0', 'Y1', 'Y2', 'Y3', 'Y4', 'Y5', 'Y6', 'Y7']
    INPUT_PINS = ['A', 'B', 'C', 'G2A', 'G2B', 'G1']
    SERIAL_PINS = ['A', 'B', 'C']
    RESULT_PINS = OUTPUT_PINS
    ENABLE_PINS = ['G1', '~G2A', '~G2B']
    STARTING_HIGH = ['G1'] + RESULT_PINS

