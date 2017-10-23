from icemu.mcu import ATtiny
from icemu.ui import SimulationWithUI

class Circuit(SimulationWithUI):
    def __init__(self):
        super().__init__()
        self.mcu = self.add_chip(ATtiny())
        self.mcu.run_program('timer')
        self.uiscreen.add_element(
            "MCU:",
            self.mcu.asciiart
        )
        self.uiscreen.refresh()


def main():
    circuit = Circuit()
    circuit.run()

if __name__ == '__main__':
    main()
