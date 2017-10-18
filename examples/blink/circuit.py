import os
import time

from icemu.mcu import ATtiny
from icemu.cwrapper import CodeWrapper
from icemu.ui import UIScreen

class Circuit:
    def __init__(self):
        self.uiscreen = UIScreen(refresh_rate_us=(100 * 1000)) # 100ms
        self.mcu = ATtiny()
        binpath = os.path.join(os.getcwd(), 'blink')
        self.code = CodeWrapper(executable=binpath, mcu=self.mcu)
        self.running = True
        self.uiscreen.add_element(
            "MCU:",
            self.mcu.asciiart
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
            time.sleep(0.0001)
            self.uiscreen.refresh()

    def stop(self):
        self.running = False

def main():
    circuit = Circuit()
    circuit.run()

if __name__ == '__main__':
    main()
