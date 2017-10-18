from .chip import Chip

class MCU(Chip):
    def pin_from_intid(self, pinid):
        return self.getpin(self._pin_codes_in_order()[pinid])


class ATtiny(MCU):
    OUTPUT_PINS = ['B0', 'B1', 'B2', 'B3', 'B4', 'B5']

