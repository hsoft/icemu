from icemu.mcu import ATtiny
from icemu.shiftregisters import SN74HC595
from icemu.seg7 import Segment7, combine_repr
from icemu.ui import SimulationWithUI

class Circuit(SimulationWithUI):
    def __init__(self):
        super().__init__()
        self.mcu = self.add_chip(ATtiny())
        self.sr = self.add_chip(SN74HC595())
        self.seg = self.add_chip(Segment7())

        self.sr.pin_SRCLK.wire_to(self.mcu.pin_B0)
        self.sr.pin_SER.wire_to(self.mcu.pin_B1)
        self.sr.pin_RCLK.wire_to(self.mcu.pin_B2)

        self.seg.wirepins(
            self.sr,
            ['F', 'G', 'E', 'D', 'C', 'B', 'A', 'DP'],
            self.sr.OUTPUT_PINS,
        )

        self.mcu.run_program('seg7')
        self.uiscreen.add_element(
            "LED Matrix output:",
            lambda: combine_repr(self.seg)
        )
        self.uiscreen.add_element(
            "MCU:",
            self.mcu.asciiart
        )
        self.uiscreen.add_element(
            "Shift register:",
            self.sr.asciiart
        )
        self.uiscreen.refresh()


def main():
    circuit = Circuit()
    circuit.run()

if __name__ == '__main__':
    main()
