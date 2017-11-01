from itertools import chain

from .chip import Chip

class Gate(Chip):
    IO_MAPPING = None # [(I1, I2, ..., IN, O)]

    def _test(self, input_pins):
        raise NotImplementedError()

    def update(self):
        for *in_, out in self.IO_MAPPING:
            pins_in = list(self.getpins(in_))
            pin_out = self.getpin(out)
            normal_pins = [p for p in pins_in if not p.is_oscillating_rapidly()]
            oscillating_pins = [p for p in pins_in if p.is_oscillating_rapidly()]
            # when there's no normal pin, the result is necessarily oscillating.
            if not normal_pins or self._test(normal_pins):
                pin_out.sethigh()
                if oscillating_pins:
                    pin_out.set_oscillating_freq(max(p.oscillating_freq() for p in oscillating_pins))
            else:
                pin_out.setlow()


class NOR(Gate):
    def _test(self, input_pins):
        return not any(p.ishigh() for p in input_pins)

class NAND(Gate):
    def _test(self, input_pins):
        return not all(p.ishigh() for p in input_pins)

class OR(Gate):
    def _test(self, input_pins):
        return any(p.ishigh() for p in input_pins)

class AND(Gate):
    def _test(self, input_pins):
        return all(p.ishigh() for p in input_pins)


class CD4001B(NOR):
    IO_MAPPING = [
        ('A', 'B', 'J'),
        ('C', 'D', 'K'),
        ('G', 'H', 'M'),
        ('E', 'F', 'L'),
    ]
    INPUT_PINS = list(chain(*(t[:2] for t in IO_MAPPING)))
    OUTPUT_PINS = [t[2] for t in IO_MAPPING]

class CD4002B(NOR):
    IO_MAPPING = [
        ('A', 'B', 'C', 'D', 'J'),
        ('E', 'F', 'G', 'H', 'K'),
    ]
    INPUT_PINS = list(chain(*(t[:4] for t in IO_MAPPING)))
    OUTPUT_PINS = [t[4] for t in IO_MAPPING]

class CD4025B(NOR):
    IO_MAPPING = [
        ('A', 'B', 'C', 'J'),
        ('D', 'E', 'F', 'K'),
        ('G', 'H', 'I', 'L'),
    ]
    INPUT_PINS = list(chain(*(t[:3] for t in IO_MAPPING)))
    OUTPUT_PINS = [t[3] for t in IO_MAPPING]

class SN74LS02(NOR):
    IO_MAPPING = [
        ('A1', 'B1', 'Y1'),
        ('A2', 'B2', 'Y2'),
        ('A3', 'B3', 'Y3'),
        ('A4', 'B4', 'Y4'),
    ]
    INPUT_PINS = list(chain(*(t[:2] for t in IO_MAPPING)))
    OUTPUT_PINS = [t[2] for t in IO_MAPPING]

class SN74LS27(NOR):
    IO_MAPPING = [
        ('A1', 'B1', 'C1', 'Y1'),
        ('A2', 'B2', 'C2', 'Y2'),
        ('A3', 'B3', 'C3', 'Y3'),
    ]
    INPUT_PINS = list(chain(*(t[:3] for t in IO_MAPPING)))
    OUTPUT_PINS = [t[3] for t in IO_MAPPING]

class SN54ALS27A(SN74LS27):
    pass

class SN54AS27(SN74LS27):
    pass

class SN5427(SN74LS27):
    pass

class SN7427(SN74LS27):
    pass

class SN54LS27(SN74LS27):
    pass

class SN74ALS27A(SN74LS27):
    pass

class SN74AS27(SN74LS27):
    pass

class SN54S260(NOR):
    IO_MAPPING = [
        ('A1', 'B1', 'C1', 'D1', 'E1', 'Y1'),
        ('A2', 'B2', 'C2', 'D2', 'E2', 'Y2'),
    ]
    INPUT_PINS = list(chain(*(t[:5] for t in IO_MAPPING)))
    OUTPUT_PINS = [t[5] for t in IO_MAPPING]

class SN74S260(SN54S260):
    pass

class SN74F260(SN54S260):
    pass

class Inverter(Chip):
    def update(self):
        for in_, out in zip(self.INPUT_PINS, self.OUTPUT_PINS):
            pin_in = self.getpin(in_)
            pin_out = self.getpin(out)
            pin_out.set(not pin_in.ishigh())
            pin_out.set_oscillating_freq(pin_in.oscillating_freq())


class SN74HC14(Inverter):
    INPUT_PINS = ['1A', '2A', '3A', '4A', '5A', '6A']
    OUTPUT_PINS = ['1Y', '2Y', '3Y', '4Y', '5Y', '6Y']

