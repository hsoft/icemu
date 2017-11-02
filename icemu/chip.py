from .pin import Pin
from .util import fmtfreq

class Chip:
    OUTPUT_PINS = []
    INPUT_PINS = []
    STARTING_HIGH = [] # pins that start high
    PIN_ORDER = None

    def __init__(self):
        self._pin_cache = {}
        self.all_pins = []
        for code in self.OUTPUT_PINS:
            pin = Pin(code, chip=self, output=True, high=(code in self.STARTING_HIGH))
            setattr(self, 'pin_{}'.format(pin.code), pin)
            self.all_pins.append(pin)
        for code in self.INPUT_PINS:
            pin = Pin(code, chip=self, high=(code in self.STARTING_HIGH))
            setattr(self, 'pin_{}'.format(pin.code), pin)
            self.all_pins.append(pin)
        self.update()

    def __str__(self):
        inputs = ' '.join(str(self.getpin(code)) for code in self.INPUT_PINS)
        outputs = ' '.join(str(self.getpin(code)) for code in self.OUTPUT_PINS)
        return '{} I: {} O: {}'.format(self.__class__.__name__, inputs, outputs)

    __repr__ = __str__

    def _pin_codes_in_order(self):
        if self.PIN_ORDER:
            return self.PIN_ORDER
        else:
            return self.INPUT_PINS + self.OUTPUT_PINS

    @staticmethod
    def _pin_is_in_code_list(pin, codes):
        return pin.code in codes or '~' + pin.code in codes

    # called during update() when a pin changes its low/high/oscillate state
    def _pin_change(self, pin, rise, fall, oscillate_at):
        pass

    def asciiart(self):
        def pinsymbol(pin):
            if pin.is_oscillating_rapidly():
                return '~'
            else:
                return '+' if pin.ishigh() else '-'

        pin_order = self._pin_codes_in_order()
        pins = list(self.getpins(pin_order))
        max_pin_name_len = max(len(p.code) for p in pins)
        if len(pins) % 2:
            pins.append(None)
        left_pins = pins[:len(pins) // 2]
        right_pins = reversed(pins[len(pins) // 2:])
        lines = []
        spacer = (max_pin_name_len + 1) * ' '
        lines.append(spacer + '_______' + spacer)
        for index, (left, right) in enumerate(zip(left_pins, right_pins)):
            larrow = '<' if left.isoutput() else '>'
            lpol = pinsymbol(left)
            sleft = left.code.rjust(max_pin_name_len) + larrow + '|' + lpol
            if right:
                rarrow = '>' if right.isoutput() else '<'
                rpol = pinsymbol(right)
                sright = rpol + '|' + rarrow + right.code.ljust(max_pin_name_len)
            else:
                sright = ' |     '

            if index == len(left_pins) - 1:
                spacer = '_'
            else:
                spacer = ' '
            line = sleft + 3 * spacer + sright
            lines.append(line)

        for pin in pins:
            if not pin:
                continue
            freq = pin.oscillating_freq()
            if freq:
                line = "~{} = {}".format(pin.code, fmtfreq(freq))
                lines.append(line)
        return '\n'.join(lines)

    def getpin(self, code):
        return getattr(self, 'pin_{}'.format(code.replace('~', '')))

    def getinputpins(self):
        return (p for p in self.all_pins if not p.isoutput())

    def getoutputpins(self):
        return (p for p in self.all_pins if p.isoutput())

    def getpins(self, codes):
        return (self.getpin(code) for code in codes)

    # Set multiple pins on the same chip and only update chips one all pins are updated.
    def setpins(self, low=None, high=None):
        updateself = False
        updatelist = set()
        low = low or []
        high = high or []
        for codes, val in [(low, False), (high, True)]:
            for pin in self.getpins(codes):
                if pin._c.ishigh() != val:
                    pin._c.setraw(val)
                    if pin.isoutput():
                       updatelist |= pin.propagate_to()
                    else:
                        updateself = True
        if updateself:
            self.update()
        for chip in updatelist:
            chip.update()

    def tick(self, usecs):
        for pin in self.all_pins:
            pin.tick(usecs)

    def update(self):
        for pin in self.getinputpins():
            code = pin.code
            prev_val = self._pin_cache.get(code)
            if pin.is_oscillating_rapidly():
                curr_val = pin.oscillating_freq()
            else:
                curr_val = pin.ishigh()
            if prev_val != curr_val:
                self._pin_change(pin)
                self._pin_cache[code] = curr_val

    # Same as with setpins, but for wire_to()
    # Has to be called from the chip having the *input* pins
    def wirepins(self, chip, inputs, outputs):
        for icode, ocode in zip(inputs, outputs):
           ipin = self.getpin(icode)
           opin = chip.getpin(ocode)
           ipin.wires.add(opin)
           ipin._c.wire_to(opin._c)
           opin.wires.add(ipin)
           opin._c.wire_to(ipin._c)
        self.update()

class ActivableChip(Chip):
    ENABLE_PINS = [] # ~ means that low == enabled

    def __init__(self, *args, **kwargs):
        self._is_enabled = True
        super().__init__(*args, **kwargs)

    def _pin_change(self, pin):
        if self._pin_is_in_code_list(pin, self.ENABLE_PINS):
            self._update_enabled()

    def _was_enabled(self):
        pass

    def _was_disabled(self):
        pass

    def _update_enabled(self):
        was_enabled = self._is_enabled
        enabled = True
        for code in self.ENABLE_PINS:
            pin = self.getpin(code.replace('~', ''))
            enabled = pin.ishigh()
            if code.startswith('~'):
                enabled = not enabled
            if not enabled:
                break

        self._is_enabled = enabled
        if enabled and not was_enabled:
            self._was_enabled()
        elif not enabled and was_enabled:
            self._was_disabled()

    def is_enabled(self):
        return self._is_enabled

