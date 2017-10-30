from icemu.pin import Pin
from icemu.chip import Chip

def test_pin_rising_falling():
    class MyChip(Chip):
        INPUT_PINS = ['FOO', 'BAR']

        def __init__(self, *args, **kwargs):
            self.pin_rose = []
            self.pin_fell = []
            self.pin_oscillates = []
            super().__init__(*args, **kwargs)

        def _pin_change(self, pin):
            if pin.is_oscillating():
                self.pin_oscillates.append(pin.code)
            elif pin.ishigh():
                self.pin_rose.append(pin.code)
            else:
                self.pin_fell.append(pin.code)

    c = MyChip()
    in_foo = Pin('FOO', output=True)
    in_bar = Pin('BAR', output=True)
    c.pin_FOO.wire_to(in_foo)
    c.pin_BAR.wire_to(in_bar)
    # We get a notification for initial values
    assert c.pin_rose == []
    assert c.pin_fell == ['FOO', 'BAR']
    in_foo.sethigh()
    assert c.pin_rose == ['FOO']
    in_bar.sethigh()
    assert c.pin_rose == ['FOO', 'BAR']
    in_bar.setlow()
    assert c.pin_fell == ['FOO', 'BAR', 'BAR']
    # setting an oscillating freq for a pin also triggers a change
    in_foo.set_oscillating_freq(4242)
    assert c.pin_oscillates == ['FOO']
    # ... even if it's already oscillating
    in_foo.set_oscillating_freq(4243)
    assert c.pin_oscillates == ['FOO', 'FOO']
