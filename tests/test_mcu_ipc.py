import os
import time

from icemu.mcu import (
    ATtiny, SEND_PINLOW, SEND_PINHIGH
)

class FakeProgram:
    def __init__(self):
        fd, self._stdout = os.pipe()
        self.stdout = os.fdopen(fd, 'rb')
        self._stdin, fd = os.pipe()
        self.stdin = os.fdopen(fd, 'wb')

    def writemsg(self, msg):
        os.write(self._stdout, bytes([msg]))
        # we sleep a little bit to ensure that the read_forever() thread has time to read stdout
        time.sleep(0.0001)

    def pinset(self,pinid, high):
        msg = SEND_PINHIGH if high else SEND_PINLOW
        msg = (msg << 5) | pinid
        self.writemsg(msg)


def test_pinset():
    prog = FakeProgram()
    mcu = ATtiny()
    mcu.run_program(prog)
    prog.pinset(0, True)
    mcu.tick(50)
    assert mcu.pin_B0.ishigh()
    assert not mcu.pin_B1.ishigh()
    prog.pinset(0, False)
    mcu.tick(50)
    assert not mcu.pin_B0.ishigh()
    prog.pinset(1, True)
    mcu.tick(50)
    assert mcu.pin_B1.ishigh()

