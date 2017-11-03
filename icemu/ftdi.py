try:
    import ftdi1
except ImportError as e:
    raise ImportError("libftdi with python 3 bindings is required to use icemu.ftdi") from e

from .chip import Chip
from .util import get_binary_value

# This chip is special. If you have a FTDI device, you can use it to wire *actual* stuff in your
# simulation. You can wire the input pin the same way you would with any other simulated chip, but
# when you do, pin changes will actually be sent to the FTDI device, banging actual bits. It's very
# useful to test partially built prototypes: your simulation is known to work, and gradually, you
# solder small parts of your circuit, test them through FTDI, and then you know that what you've
# soldered works!

class FT232H(Chip):
    INPUT_PINS = ['D0', 'D1', 'D2', 'D3', 'D4', 'D5', 'D6', 'D7']
    USB_VENDOR_ID = 0x0403
    USB_PRODUCT_ID = 0x6014

    def __init__(self, *args, **kwargs):
        self._ftdi = ftdi1.new()
        res = ftdi1.usb_open(self._ftdi, self.USB_VENDOR_ID, self.USB_PRODUCT_ID)
        if res != 0:
            raise RuntimeError("usb_open() failed with error {}".format(res))
        ftdi1.set_bitmode(self._ftdi, 0xff, ftdi1.BITMODE_BITBANG)
        super().__init__(*args, **kwargs)

    def __del__(self):
        ftdi1.disable_bitbang(self._ftdi)
        ftdi1.usb_close(self._ftdi)
        ftdi1.free(self._ftdi)

    def _pin_change(self, pin):
        buf = get_binary_value(self.getinputpins())
        ftdi1.write_data(self._ftdi, bytes([buf]))
