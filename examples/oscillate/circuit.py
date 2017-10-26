from icemu.counters import SN74F161AN
from icemu.decoders import SN74HC138
from icemu.pin import Pin
from icemu.ui import SimulationWithUI

class Circuit(SimulationWithUI):
    def __init__(self):
        super().__init__()
        self.oscillator = Pin('XTAL', oscillating_freq=(10**6))
        self.cnt = self.add_chip(SN74F161AN())
        self.dec = self.add_chip(SN74HC138())
        self.cnt2 = self.add_chip(SN74F161AN())

        clk = self.cnt.getpin(self.cnt.CLOCK_PIN)
        clk.wire_to(self.oscillator)

        self.dec.pin_A.wire_to(self.cnt.pin_QD)
        self.dec.pin_C.sethigh()

        clk = self.cnt2.getpin(self.cnt2.CLOCK_PIN)
        clk.wire_to(self.dec.pin_Y5)

        self.uiscreen.add_element(
            "Counter 1:",
            self.cnt.asciiart
        )
        self.uiscreen.add_element(
            "Decoder:",
            self.dec.asciiart
        )
        self.uiscreen.add_element(
            "Counter 2:",
            self.cnt2.asciiart
        )
        self.uiscreen.add_action(
            '+', "Faster", self.faster)
        self.uiscreen.add_action(
            '-', "Slower", self.slower)
        self.uiscreen.refresh()

    def faster(self):
        osc = self.oscillator
        osc.set_oscillating_freq(osc.oscillating_freq() * 10)

    def slower(self):
        osc = self.oscillator
        osc.set_oscillating_freq(osc.oscillating_freq() // 10)

def main():
    circuit = Circuit()
    circuit.run()

if __name__ == '__main__':
    main()
