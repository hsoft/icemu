import subprocess
import threading

SEND_PINLOW = 0x0
SEND_PINHIGH = 0x1
SEND_PINREAD = 0x2

RECV_PINLOW = 0x0
RECV_PINHIGH = 0x1

MAX_5BITS = 0x1f

class CodeWrapper:
    def __init__(self, executable, mcu):
        self.proc = subprocess.Popen(executable, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        self.mcu = mcu
        self.lock = threading.Lock()
        self.msgin = b''
        self.msgout = b''
        threading.Thread(target=self.communicate_forever, daemon=True).start()

    def communicate_forever(self):
        while True:
            with self.lock:
                if self.msgin:
                    self.proc.stdin.write(self.msgin)
                    self.msgin = b''
            msgout = self.proc.stdout.read(1)
            with self.lock:
                self.msgout += msgout

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

