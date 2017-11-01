from .chip import Chip

class LED:
    def __init__(self, vcc, gnd, fade_delay_us=10000):
        self.vcc = vcc
        self.gnd = gnd
        self.fade_delay_us = fade_delay_us
        self.fade_counter_us = fade_delay_us + 1

    def __str__(self):
        return 'X' if self.ishigh() else 'O'

    def tick(self, usecs):
        self.fade_counter_us += usecs
        self.ishigh()

    def ishigh(self):
        if self.vcc.ishigh() and not self.gnd.ishigh():
            self.fade_counter_us = 0
            return True
        else:
            return self.fade_counter_us <= self.fade_delay_us


def combine_repr(*segs):
    """Combine and returns __str__ repr of multiple Segment7
    """
    outputs = [str(seg) for seg in segs]
    line1 = ' '.join(s[:3] for s in outputs)
    line2 = ' '.join(s[4:7] for s in outputs)
    line3 = ' '.join(s[8:] for s in outputs)
    return '\n'.join([line1, line2, line3])

# Remember: segments are visible when pin is *low*
class Segment7(Chip):
    INPUT_PINS = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'DP']
    FADE_DELAY_US = 10**4

    def __init__(self, *args, **kwargs):
        self.fading_pins = {}
        super().__init__(*args, **kwargs)

    def __str__(self):
        SEGMENTS = """
 _.
|_|
|_|""".strip('\n')
        # Remember, each line is **4** chars long if we count the \n!
        SEGPOS = [
            '', 'A', 'DP', '',
            'F', 'G', 'B', '',
            'E', 'D', 'C'
        ]
        highpins = {p.code for p in self.getinputpins() if not p.ishigh() or p.code in self.fading_pins}
        return ''.join(c if seg and seg in highpins else ' ' for c, seg in zip(SEGMENTS, SEGPOS))

    def _pin_change(self, pin):
        if pin.ishigh():
            # pin was just lowered, activate delay
            self.fading_pins[pin.code] = self.FADE_DELAY_US
        else:
            if pin.code in self.fading_pins:
                del self.fading_pins[pin.code]

    def tick(self, usecs):
        for code, delay in list(self.fading_pins.items()):
            delay -= usecs
            if delay <= 0:
                del self.fading_pins[code]
            else:
                self.fading_pins[code] = delay

