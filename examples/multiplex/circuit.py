from icemu.mcu import ATtiny
from icemu.decoders import SN74HC138
from icemu.seg7 import LED
from icemu.sim import Simulation, TIME_RESOLUTION
from icemu.ui import UIScreen

class Circuit(Simulation):
    def __init__(self):
        super().__init__(usec_value=10)
        self.uiscreen = UIScreen(self)
        self.mcu = ATtiny()
        self.dec = SN74HC138()
        decoutputs = list(self.dec.getpins(self.dec.OUTPUT_PINS))
        self.leds = [LED(vcc=self.mcu.vcc, gnd=decoutputs[i]) for i in range(8)]

        self.dec.wirepins(self.mcu, ['A', 'B', 'C'], ['B0', 'B1', 'B2'])

        self.run_program('multiplex', self.mcu)
        self.uiscreen.add_element(
            "MCU:",
            self.mcu.asciiart
        )
        self.uiscreen.add_element(
            "LEDs:",
            lambda: ''.join(str(l) for l in self.leds)
        )
        self.uiscreen.refresh()

    def _process(self):
        for led in self.leds:
            led.tick(TIME_RESOLUTION)
        self.uiscreen.refresh()

    def stop(self):
        super().stop()
        self.uiscreen.stop()

def main():
    circuit = Circuit()
    circuit.run()

if __name__ == '__main__':
    main()
