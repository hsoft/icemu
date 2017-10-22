import os
import subprocess
import threading
import time

from .chip import Chip

SEND_PINLOW = 0x0
SEND_PINHIGH = 0x1
SEND_PININPUT = 0x2
SEND_PINOUTPUT = 0x3

RECV_PINLOW = 0x0
RECV_PINHIGH = 0x1
RECV_TICK = 0x2
RECV_INTERRUPT = 0x3

MAX_5BITS = 0x1f

class MCU(Chip):
    def __init__(self):
        super().__init__()
        self.lock = threading.Lock()
        self.msgin = b''
        self.msgout = b''
        self.running = False
        self._pin_cache = {p.code: p.ishigh() for p in self.getinputpins()}

    def _pin_from_intid(self, pinid):
        return self.getpin(self._pin_codes_in_order()[pinid])

    def _intid_from_pin(self, pin):
        return self._pin_codes_in_order().index(pin.code)

    def run_program(self, executable):
        if not os.path.isabs(executable):
            executable = os.path.join(os.getcwd(), executable)
        self.proc = subprocess.Popen(executable, stdin=subprocess.PIPE, stdout=subprocess.PIPE, bufsize=0)
        self.running = True
        threading.Thread(target=self.read_forever, daemon=True).start()
        threading.Thread(target=self.write_forever, daemon=True).start()

    def read_forever(self):
        while self.running:
            msgout = self.proc.stdout.read(1)
            with self.lock:
                self.msgout += msgout

    def write_forever(self):
        msgin = b''
        while self.running:
            with self.lock:
                if self.msgin:
                    msgin = self.msgin
                    self.msgin = b''
            if msgin:
                self.proc.stdin.write(msgin)
                msgin = b''
            time.sleep(0)

    def stop(self):
        self.running = False

    def push_msgin(self, msg):
        with self.lock:
            self.msgin += bytes([msg])

    def process_msgout(self):
        with self.lock:
            msgout = self.msgout
            self.msgout = b''
        for msg in msgout:
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
            if pin.output:
                pin.output = False
                self._pin_cache[pin.code] = pin.ishigh()
        elif msgid == SEND_PINOUTPUT:
            pin.output = True

    def interrupt(self, interrupt_id):
        self.push_msgin((RECV_INTERRUPT << 5) | interrupt_id)

    def tick(self):
        self.push_msgin(RECV_TICK << 5)

    def update(self):
        for pin in self.getinputpins():
            if pin.ishigh() != self._pin_cache[pin.code]:
                msg = RECV_PINHIGH if pin.ishigh() else RECV_PINLOW
                self.push_msgin((msg << 5) | self._intid_from_pin(pin))
                self._pin_cache[pin.code] = pin.ishigh()

class ATtiny(MCU):
    OUTPUT_PINS = ['B0', 'B1', 'B2', 'B3', 'B4', 'B5']

