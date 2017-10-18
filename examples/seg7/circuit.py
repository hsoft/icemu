import os
import time

from icemu.mcu import ATtiny
from icemu.cwrapper import CodeWrapper
from icemu.shiftregisters import SN74HC595
from icemu.seg7 import Segment7, combine_repr
from icemu.ui import UIScreen

class Circuit:
    def __init__(self):
        self.uiscreen = UIScreen(refresh_rate_us=(100 * 1000)) # 100ms
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

        binpath = os.path.join(os.getcwd(), 'seg7')
        self.code = CodeWrapper(executable=binpath, mcu=self.mcu)
        self.running = True
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
        self.uiscreen.add_action(
            'q', "Quit",
            self.stop,
        )
        self.uiscreen.refresh()

    def run(self):
        while self.running:
            self.code.process_msgout()
            self.uiscreen.tick(100)
            self.seg.tick(100)
            time.sleep(0.0001)
            self.uiscreen.refresh()
        self.uiscreen.stop()

    def stop(self):
        self.running = False

def main():
    circuit = Circuit()
    circuit.run()

if __name__ == '__main__':
    main()
