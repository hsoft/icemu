import os
import subprocess
import threading
from collections import namedtuple

from .chip import Chip
from .util import USECS_PER_SECOND

SEND_PINLOW = 0x0
SEND_PINHIGH = 0x1
SEND_PININPUT = 0x2
SEND_PINOUTPUT = 0x3
SEND_ENDINTERRUPT = 0x4

RECV_PINLOW = 0x0
RECV_PINHIGH = 0x1
RECV_TICK = 0x2
RECV_INTERRUPT = 0x3
RECV_REPEAT = 0x4

MAX_5BITS = 0x1f

Interrupt = namedtuple('Interrupt', 'code interrupt_id rising falling')

class MCU(Chip):
    TIME_RESOLUTION = 50 # in usecs

    def __init__(self):
        self._waiting_for_interrupt = False
        # {code: Interrupt}
        self._interrupt_enabled = {}
        self._debug_msgs_to = None
        # There's too many ticks to debug them. Let's just count them and report the count at every
        # non-tick msg
        self._debug_tick_count = 0
        # number of usecs elapsed since we've sent the last tick
        self._elapsed_usecs = 0
        self.lock = threading.Lock()
        self.msgin = b''
        self.msgout = b''
        self.running = False
        super().__init__()

    def _pin_from_intid(self, pinid):
        return self.getpin(self._pin_codes_in_order()[pinid])

    def _intid_from_pin(self, pin):
        return self._pin_codes_in_order().index(pin.code)

    def _push_pin_state(self, pin):
        msg = RECV_PINHIGH if pin.ishigh() else RECV_PINLOW
        self.push_msgin((msg << 5) | self._intid_from_pin(pin))

    def _debug_msg(self, send, msg):
        msgtype = (msg & 0xe0) >> 5
        msgarg = msg & MAX_5BITS
        if not send and msgtype == RECV_TICK:
            self._debug_tick_count += msgarg + 1
            return
        if self._debug_tick_count:
            print("%d ticks" % self._debug_tick_count, file=self._debug_msgs_to)
            self._debug_tick_count = 0
        s = "%s %d %d" % ('S' if send else 'R', msgtype, msgarg)
        print(s, file=self._debug_msgs_to)

    def _pin_change(self, pin):
        if not pin.isoutput() and not pin.is_oscillating_rapidly():
            self._push_pin_state(pin)
            interrupt = self._interrupt_enabled.get(pin.code)
            if interrupt is not None:
                high = pin.ishigh()
                if (high and interrupt.rising) or (not high and interrupt.falling):
                    self.interrupt(interrupt.interrupt_id)

    def run_program(self, executable, debug_msgs_to=None):
        if debug_msgs_to:
            self._debug_msgs_to = open(debug_msgs_to, 'wt', encoding='ascii')
        if hasattr(executable, 'stdin') and hasattr(executable, 'stdout'):
            # we're passing a fake (or real) process that's already running. Just use those already-
            # opened stdin/stdout.
            self.proc = executable
        else:
            if not os.path.isabs(executable):
                executable = os.path.join(os.getcwd(), executable)
            self.proc = subprocess.Popen(
                executable, stdin=subprocess.PIPE, stdout=subprocess.PIPE, bufsize=0)
        self.running = True
        threading.Thread(target=self.read_forever, daemon=True).start()

    def read_forever(self):
        while self.running:
            msgout = self.proc.stdout.read(1)
            with self.lock:
                self.msgout += msgout

    def stop(self):
        self.running = False
        if self._debug_msgs_to:
            self._debug_msgs_to.close()

    def push_msgin(self, msg):
        if hasattr(self, 'proc'):
            self.proc.stdin.write(bytes([msg]))
            if self._debug_msgs_to:
                self._debug_msg(False, msg)

    def process_msgout(self):
        with self.lock:
            msgout = self.msgout
            self.msgout = b''
        for msg in msgout:
            if self._debug_msgs_to:
                self._debug_msg(True, msg)
            self.process_sent_msg(msg)

    def process_sent_msg(self, msg):
        msgid = msg >> 5
        pinid = msg & MAX_5BITS
        pin = self._pin_from_intid(pinid)
        if msgid == SEND_PINLOW:
            pin.setlow()
        elif msgid == SEND_PINHIGH:
            pin.sethigh()
        elif msgid == SEND_PININPUT:
            pin.setinput()
            self._push_pin_state(pin)
        elif msgid == SEND_PINOUTPUT:
            pin.setoutput()
        elif msgid == SEND_ENDINTERRUPT:
            self._waiting_for_interrupt = False

    def enable_interrupt_on_pin(self, pin, rising=False, falling=False):
        assert pin.code in self.INTERRUPT_PINS
        assert rising or falling
        interrupt_id = self.INTERRUPT_PINS.index(pin.code)
        interrupt = Interrupt(pin.code, interrupt_id, rising, falling)
        self._interrupt_enabled[pin.code] = interrupt

    def interrupt(self, interrupt_id, count=1):
        while count >= 1:
            self._waiting_for_interrupt = True
            subcount = min(MAX_5BITS + 1, count)
            count -= subcount
            if subcount > 1:
                self.push_msgin((RECV_REPEAT << 5) | (subcount - 1))
            self.push_msgin((RECV_INTERRUPT << 5) | interrupt_id)
            while self._waiting_for_interrupt:
                self.process_msgout()

    def tick(self, usecs):
        for interrupt in self._interrupt_enabled.values():
            pin = self.getpin(interrupt.code)
            if pin.is_oscillating_rapidly():
                count = (pin.oscillating_freq() / USECS_PER_SECOND) * usecs
                if interrupt.rising and interrupt.falling:
                    count *= 2
                count = round(count)
                if count >= 1:
                    self.interrupt(self.INTERRUPT_PINS.index(pin.code), count=int(count))

        self._elapsed_usecs += usecs
        while self._elapsed_usecs >= self.TIME_RESOLUTION:
            # we can only send a maximum of 32 ticks at once
            tick_count = min(32, self._elapsed_usecs // self.TIME_RESOLUTION)
            self._elapsed_usecs -= tick_count * self.TIME_RESOLUTION
            self.process_msgout()
            self.push_msgin((RECV_TICK << 5) | (tick_count - 1))


class ATtiny(MCU):
    INPUT_PINS = ['B0', 'B1', 'B2', 'B3', 'B4', 'B5']
    INTERRUPT_PINS = INPUT_PINS # all pins have an associated interrupt

