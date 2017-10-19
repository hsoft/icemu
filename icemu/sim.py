import os
import subprocess
import threading
import time

SEND_PINLOW = 0x0
SEND_PINHIGH = 0x1
SEND_PINREAD = 0x2

RECV_PINLOW = 0x0
RECV_PINHIGH = 0x1
RECV_TICK = 0x2

MAX_5BITS = 0x1f

TIME_RESOLUTION = 50 # in usecs

class CodeWrapper:
    def __init__(self, executable, mcu):
        if not os.path.isabs(executable):
            executable = os.path.join(os.getcwd(), executable)
        self.proc = subprocess.Popen(executable, stdin=subprocess.PIPE, stdout=subprocess.PIPE, bufsize=0)
        self.mcu = mcu
        self.lock = threading.Lock()
        self.msgin = b''
        self.msgout = b''
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
            for msg in self.msgout:
                self.process_sent_msg(msg)
            self.msgout = b''

    def process_sent_msg(self, msg):
        msgid = msg >> 5
        pinid = msg & MAX_5BITS
        pin = self.mcu.pin_from_intid(pinid)
        if msgid == SEND_PINLOW:
            pin.setlow()
        elif msgid == SEND_PINHIGH:
            pin.sethigh()
        elif msgid == SEND_PINREAD:
            newmsg = pinid
            if pin.ishigh():
                newmsg |= RECV_PINHIGH << 5
            else:
                newmsg |= RECV_PINLOW << 5
            self.push_msgin(newmsg)

    def tick(self):
        self.push_msgin(RECV_TICK << 5)

class Simulation:
    def __init__(self):
        self.code_wrappers = []
        self.running = True

    def _process(self):
        pass # override with code you want to execute in the runloop

    def run_program(self, path, on_mcu):
        code = CodeWrapper(path, on_mcu)
        self.code_wrappers.append(code)

    def run(self):
        one_tick_in_seconds = 1 / (1000 * (1000 / TIME_RESOLUTION))
        target_time = time.time() + one_tick_in_seconds
        while self.running:
            for code in self.code_wrappers:
                code.process_msgout()
                code.tick()
            self._process()
            while time.time() < target_time:
                time.sleep(0)
            target_time += one_tick_in_seconds

    def stop(self):
        for code in self.code_wrappers:
            code.stop()
        self.running = False
