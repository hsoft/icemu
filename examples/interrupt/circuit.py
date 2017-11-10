from icemu.mcu import ATtiny
from icemu.pin import Pin
from icemu.ui import SimulationWithUI

class Circuit(SimulationWithUI):
    def __init__(self):
        super().__init__()
        self.button_pin = Pin(code='BTN', output=True)
        self.mcu = self.add_chip(ATtiny())
        self.mcu.pin_B0.wire_to(self.button_pin)
        self.mcu.run_program('interrupt')
        self.uiscreen.add_element(
            "MCU:",
            self.mcu.asciiart
        )
        self.uiscreen.add_element(
            "Button pushed?",
            lambda: 'yes' if self.button_pin.ishigh() else 'no'
        )
        self.uiscreen.add_action(
            'b', "Toggle button",
            self.button_pin.toggle,
        )
        self.uiscreen.refresh()


def main():
    circuit = Circuit()
    circuit.run()

if __name__ == '__main__':
    main()
