from icemu.mcu import ATtiny
from icemu.sim import Simulation
from icemu.ui import UIScreen

class Circuit(Simulation):
    def __init__(self):
        super().__init__()
        self.mcu = ATtiny()
        self.run_program('blink', self.mcu)
        self.uiscreen = UIScreen(self)
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
