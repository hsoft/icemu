from icemu.mcu import ATtiny
from icemu.sim import Simulation
from icemu.ui import UIScreen

class Circuit(Simulation):
    def __init__(self):
        super().__init__()
        self.mcu = ATtiny()
        self.run_program('button', self.mcu)
        self.uiscreen = UIScreen(self)
        self.uiscreen.add_element(
            "MCU:",
            self.mcu.asciiart
        )
        self.uiscreen.add_action(
            'b', "Toggle button",
            self.toggle_button,
        )
        self.uiscreen.refresh()

    def _process(self):
        self.uiscreen.refresh()

    def stop(self):
        super().stop()
        self.uiscreen.stop()

    def toggle_button(self):
        self.mcu.pin_B0.toggle()

def main():
    circuit = Circuit()
    circuit.run()

if __name__ == '__main__':
    main()
