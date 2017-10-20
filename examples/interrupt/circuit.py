from icemu.mcu import ATtiny
from icemu.sim import Simulation, TIME_RESOLUTION
from icemu.ui import UIScreen

class Circuit(Simulation):
    def __init__(self):
        super().__init__()
        self.mcu = ATtiny()
        self.code = self.run_program('interrupt', self.mcu)
        self.uiscreen = UIScreen(refresh_rate_us=(100 * 1000)) # 100ms
        self.uiscreen.add_element(
            "MCU:",
            self.mcu.asciiart
        )
        self.uiscreen.add_action(
            'b', "Toggle button",
            self.toggle_button,
        )
        self.uiscreen.add_action(
            'q', "Quit",
            self.stop,
        )
        self.uiscreen.refresh()

    def _process(self):
        self.uiscreen.tick(TIME_RESOLUTION)
        self.uiscreen.refresh()

    def stop(self):
        super().stop()
        self.uiscreen.stop()

    def toggle_button(self):
        self.mcu.pin_B0.toggle()
        self.code.interrupt(0)

def main():
    circuit = Circuit()
    circuit.run()

if __name__ == '__main__':
    main()
