from .chip import ActivableChip
from .util import set_binary_value

class ShiftRegister(ActivableChip):
    SERIAL_PINS = []
    CLOCK_PIN = ''
    BUFFER_PIN = '' # Pin that makes the SR buffer go to output pin. Leave blank if there's none
    RESET_PIN = ''
    RESULT_PINS = [] # Data is pushed from first pin to last

    def __init__(self, *args, **kwargs):
        self.buffer = 0
        super().__init__(*args, **kwargs)

    def is_resetting(self):
        if self.RESET_PIN:
            return self.getpin(self.RESET_PIN).isenabled()
        else:
            return False

    def _pin_change(self, pin):
        super()._pin_change(pin)
        if self.is_resetting():
            self.setpins(low=self.RESULT_PINS, high=[])
            self.buffer = 0
        elif pin.code == self.CLOCK_PIN and pin.ishigh():
            newbuffer = self.buffer << 1
            if all(self.getpin(code).ishigh() for code in self.SERIAL_PINS):
                newbuffer |= 0x1
            self.buffer = newbuffer
            if not self.BUFFER_PIN:
                # if there's not buffering, there's no delay
                self._update_outputs()
        elif pin.code == self.BUFFER_PIN and pin.ishigh():
            self._update_outputs()

    def _was_disabled(self):
        self.setpins(low=self.RESULT_PINS, high=[])

    def _was_enabled(self):
        self._update_outputs()

    def _update_outputs(self):
        if self.is_enabled():
            set_binary_value(self.buffer, self.getpins(self.RESULT_PINS))


class CD74AC164(ShiftRegister):
    OUTPUT_PINS = ['Q0', 'Q1', 'Q2', 'Q3', 'Q4', 'Q5', 'Q6', 'Q7']
    INPUT_PINS = ['DS1', 'DS2', 'CP', '~MR']
    SERIAL_PINS = ['DS1', 'DS2']
    RESULT_PINS = OUTPUT_PINS
    RESET_PIN = '~MR'
    CLOCK_PIN = 'CP'
    STARTING_HIGH = ['~MR', 'DS2']


class SN74HC595(ShiftRegister):
    OUTPUT_PINS = ['QA', 'QB', 'QC', 'QD', 'QE', 'QF', 'QG', 'QH']
    INPUT_PINS = ['~OE', 'RCLK', 'SER', 'SRCLK', '~SRCLR']
    SERIAL_PINS = ['SER']
    RESULT_PINS = OUTPUT_PINS
    ENABLE_PINS = ['~OE']
    RESET_PIN = '~SRCLR'
    CLOCK_PIN = 'SRCLK'
    BUFFER_PIN = 'RCLK'
    STARTING_HIGH = ['~SRCLR']

