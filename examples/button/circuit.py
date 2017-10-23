from icemu.mcu import ATtiny
from icemu.ui import SimulationWithUI

class Circuit(SimulationWithUI):
    def __init__(self):
        super().__init__()
        self.mcu = self.add_chip(ATtiny())
        self.mcu.run_program('button')
        self.uiscreen.add_element(
            "MCU:",
            self.mcu.asciiart
        )
        self.uiscreen.add_action(
            'b', "Toggle button",
            self.toggle_button,
        )
        self.uiscreen.refresh()

    def toggle_button(self):
        self.mcu.pin_B0.toggle()

def main():
    circuit = Circuit()
    circuit.run()

if __name__ == '__main__':
    main()
