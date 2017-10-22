from icemu.mcu import ATtiny
from icemu.sim import Simulation
from icemu.ui import UIScreen

class Circuit(Simulation):
    def __init__(self):
        super().__init__()
        self.uiscreen = UIScreen(self)
        self.mcu = ATtiny()
        self.add_mcu(self.mcu)
        self.mcu.run_program('timer')
        self.uiscreen.add_element(
            "MCU:",
            self.mcu.asciiart
        )
        self.uiscreen.refresh()

    def _process(self):
        self.uiscreen.refresh()

    def stop(self):
        super().stop()
        self.uiscreen.stop()

def main():
    circuit = Circuit()
    circuit.run()

if __name__ == '__main__':
    main()
