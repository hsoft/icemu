from icemu.mcu import ATtiny
from icemu.pin import Pin
from icemu.ui import SimulationWithUI

class Circuit(SimulationWithUI):
    def __init__(self):
        super().__init__()
        self._button_state = False
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
            lambda: 'yes' if self._button_state else 'no'
        )
        self.uiscreen.add_action(
            'b', "Toggle button",
            self.toggle_button,
        )
        self.uiscreen.refresh()

    def toggle_button(self):
        self._button_state = not self._button_state
        self.button_pin.set(self._button_state)
        self.mcu.interrupt(0)
        # This is to demonstrate that interrupt() doesn't return until our program is finished
        # going through the interrupt. If it did, the state of `B0` wouldn't be the same as
        # _button_state
        self.button_pin.set(not self._button_state)

def main():
    circuit = Circuit()
    circuit.run()

if __name__ == '__main__':
    main()
