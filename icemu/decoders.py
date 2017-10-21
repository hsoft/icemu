from .chip import ActivableChip

class Decoder(ActivableChip):
    SERIAL_PINS = [] # LSB pin goes first
    RESULT_PINS = [] # LSB pin goes first

    def update(self):
        super().update()
        selection = 0 if self.is_enabled() else 0xff
        for index, code in enumerate(self.SERIAL_PINS):
            pin = self.getpin(code)
            selection |= int(pin.ishigh()) << index
        for index, code in enumerate(self.RESULT_PINS):
            pin = self.getpin(code)
            pin.set(index != selection)


class SN74HC138(Decoder):
    OUTPUT_PINS = ['Y0', 'Y1', 'Y2', 'Y3', 'Y4', 'Y5', 'Y6', 'Y7']
    INPUT_PINS = ['A', 'B', 'C', 'G2A', 'G2B', 'G1']
    SERIAL_PINS = ['A', 'B', 'C']
    RESULT_PINS = OUTPUT_PINS
    ENABLE_PINS = ['G1', '~G2A', '~G2B']
    STARTING_HIGH = ['G1'] + RESULT_PINS

