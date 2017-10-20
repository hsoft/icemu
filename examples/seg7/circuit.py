from icemu.mcu import ATtiny
from icemu.sim import Simulation, TIME_RESOLUTION
from icemu.shiftregisters import SN74HC595
from icemu.seg7 import Segment7, combine_repr
from icemu.ui import UIScreen

class Circuit(Simulation):
    def __init__(self):
        super().__init__()
        self.uiscreen = UIScreen(self)
        self.mcu = ATtiny()
        self.sr = SN74HC595()
        self.seg = Segment7()

        self.sr.pin_SRCLK.wire_to(self.mcu.pin_B0)
        self.sr.pin_SER.wire_to(self.mcu.pin_B1)
        self.sr.pin_RCLK.wire_to(self.mcu.pin_B2)

        self.seg.wirepins(
            self.sr,
            ['F', 'G', 'E', 'D', 'C', 'B', 'A', 'DP'],
            self.sr.OUTPUT_PINS,
        )

        self.run_program('seg7', self.mcu)
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

    def _process(self):
        self.seg.tick(TIME_RESOLUTION)
        self.uiscreen.refresh()

    def stop(self):
        super().stop()
        self.uiscreen.stop()

def main():
    circuit = Circuit()
    circuit.run()

if __name__ == '__main__':
    main()
