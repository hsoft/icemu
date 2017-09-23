from itertools import chain

from .chip import Chip

class DoubleInput(Chip):
    IO_MAPPING = None # [(I1, I2, O)]

    def _test(self, pin_in1, pin_in2):
        raise NotImplementedError()

    def update(self):
        for in1, in2, out in self.IO_MAPPING:
            pin_in1 = self.getpin(in1)
            pin_in2 = self.getpin(in2)
            pin_out = self.getpin(out)
            pin_out.set(self._test(pin_in1, pin_in2))

class TripleInput(Chip):
    IO_MAPPING = None # [(I1, I2, I3, O)]

    def _test(self, pin_in1, pin_in2, pin_in3):
        raise NotImplementedError()

    def update(self):
        for in1, in2, in3, out in self.IO_MAPPING:
            pin_in1 = self.getpin(in1)
            pin_in2 = self.getpin(in2)
            pin_in3 = self.getpin(in3)
            pin_out = self.getpin(out)
            pin_out.set(self._test(pin_in1, pin_in2, pin_in3))

class QuadInput(Chip):
    IO_MAPPING = None # [(I1, I2, I3, I4, O)]

    def _test(self, pin_in1, pin_in2, pin_in3, pin_in4):
        raise NotImplementedError()

    def update(self):
        for in1, in2, in3, in4, out in self.IO_MAPPING:
            pin_in1 = self.getpin(in1)
            pin_in2 = self.getpin(in2)
            pin_in3 = self.getpin(in3)
            pin_in4 = self.getpin(in4)
            pin_out = self.getpin(out)
            pin_out.set(self._test(pin_in1, pin_in2, pin_in3, pin_in4))

class PentaInput(Chip):
    IO_MAPPING = None # [(I1, I2, I3, I4, I5, O)]

    def _test(self, pin_in1, pin_in2, pin_in3, pin_in4, pin_in5):
        raise NotImplementedError()

    def update(self):
        for in1, in2, in3, in4, in5, out in self.IO_MAPPING:
            pin_in1 = self.getpin(in1)
            pin_in2 = self.getpin(in2)
            pin_in3 = self.getpin(in3)
            pin_in4 = self.getpin(in4)
            pin_in5 = self.getpin(in5)
            pin_out = self.getpin(out)
            pin_out.set(self._test(pin_in1, pin_in2, pin_in3, pin_in4, pin_in5))


class NOR(DoubleInput):
    def _test(self, pin_in1, pin_in2):
        return not (pin_in1.ishigh() or pin_in2.ishigh())

class NOR3(TripleInput):
    def _test(self, pin_in1, pin_in2, pin_in3):
        return not (pin_in1.ishigh() or pin_in2.ishigh() or pin_in3.ishigh())

class NOR4(QuadInput):
    def _test(self, pin_in1, pin_in2, pin_in3, pin_in4):
        return not (pin_in1.ishigh() or pin_in2.ishigh() or pin_in3.ishigh() or pin_in4.ishigh())

class NOR5(PentaInput):
    def _test(self, pin_in1, pin_in2, pin_in3, pin_in4, pin_in5):
        return not (pin_in1.ishigh() or pin_in2.ishigh() or pin_in3.ishigh() or pin_in4.ishigh() or pin_in5.ishigh())

class NAND(DoubleInput):
    def _test(self, pin_in1, pin_in2):
        return not (pin_in1.ishigh() and pin_in2.ishigh())

class OR(DoubleInput):
    def _test(self, pin_in1, pin_in2):
        return (pin_in1.ishigh() or pin_in2.ishigh())

class AND(DoubleInput):
    def _test(self, pin_in1, pin_in2):
        return (pin_in1.ishigh() and pin_in2.ishigh())


class CD4001B(NOR):
    IO_MAPPING = [
        ('A', 'B', 'J'),
        ('C', 'D', 'K'),
        ('G', 'H', 'M'),
        ('E', 'F', 'L'),
    ]
    INPUT_PINS = list(chain(*(t[:2] for t in IO_MAPPING)))
    OUTPUT_PINS = [t[2] for t in IO_MAPPING]

class CD4002B(NOR4):
    IO_MAPPING = [
        ('A', 'B', 'C', 'D', 'J'),
        ('E', 'F', 'G', 'H', 'K'),
    ]
    INPUT_PINS = list(chain(*(t[:4] for t in IO_MAPPING)))
    OUTPUT_PINS = [t[4] for t in IO_MAPPING]

class CD4025B(NOR3):
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

class SN74LS27(NOR3):
    IO_MAPPING = [
        ('A1', 'B1', 'C1', 'Y1'),
        ('A2', 'B2', 'C2', 'Y2'),
        ('A3', 'B3', 'C3', 'Y3'),
    ]
    INPUT_PINS = list(chain(*(t[:3] for t in IO_MAPPING)))
    OUTPUT_PINS = [t[3] for t in IO_MAPPING]

class SN54ALS27A(SN74LS27):
    ()
    
class SN54AS27(SN74LS27):
    ()
    
class SN5427(SN74LS27):
    ()
    
class SN7427(SN74LS27):
    ()
    
class SN54LS27(SN74LS27):
    ()
    
class SN74ALS27A(SN74LS27):
    ()
    
class SN74AS27(SN74LS27):
    ()
    
class SN54S260(NOR5):
    IO_MAPPING = [
        ('A1', 'B1', 'C1', 'D1', 'E1', 'Y1'),
        ('A2', 'B2', 'C2', 'D2', 'E2', 'Y2'),
    ]
    INPUT_PINS = list(chain(*(t[:5] for t in IO_MAPPING)))
    OUTPUT_PINS = [t[5] for t in IO_MAPPING]

class SN74S260(SN54S260):
    ()
class SN74F260(SN54S260):
    ()

class Inverter(Chip):
    def update(self):
        for in_, out in zip(self.INPUT_PINS, self.OUTPUT_PINS):
            pin_in = self.getpin(in_)
            pin_out = self.getpin(out)
            pin_out.set(not pin_in.ishigh())


class SN74HC14(Inverter):
    INPUT_PINS = ['1A', '2A', '3A', '4A', '5A', '6A']
    OUTPUT_PINS = ['1Y', '2Y', '3Y', '4Y', '5Y', '6Y']

