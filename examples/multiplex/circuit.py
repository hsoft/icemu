from icemu.mcu import ATtiny
from icemu.decoders import SN74HC138
from icemu.seg7 import LED
from icemu.ui import SimulationWithUI

class Circuit(SimulationWithUI):
    def __init__(self):
        super().__init__(usec_value=10)
        self.mcu = self.add_chip(ATtiny())
        self.dec = SN74HC138()
        decoutputs = list(self.dec.getpins(self.dec.OUTPUT_PINS))
        self.leds = [self.add_chip(LED(vcc=self.mcu.vcc, gnd=decoutputs[i])) for i in range(8)]

        self.dec.wirepins(self.mcu, ['A', 'B', 'C'], ['B0', 'B1', 'B2'])

        self.mcu.run_program('multiplex')
        self.uiscreen.add_element(
            "MCU:",
            self.mcu.asciiart
        )
        self.uiscreen.add_element(
            "Decoder:",
            self.dec.asciiart
        )
        self.uiscreen.add_element(
            "LEDs:",
            lambda: ''.join(str(l) for l in self.leds)
        )
        self.uiscreen.refresh()


def main():
    circuit = Circuit()
    circuit.run()

if __name__ == '__main__':
    main()
