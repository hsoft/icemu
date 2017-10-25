import pytest

from icemu.pin import pinrange
from icemu.decoders import SN74HC138
from icemu.pin import Pin
from icemu.util import set_binary_value

def assert_pin_is_selected(dec, selected_pin):
    for index, code in enumerate(pinrange('Y', 0, 7)):
        pin = dec.getpin(code)
        assert pin.ishigh() == (index != selected_pin)

def test_IO():
    # make sure that outputs correspond to inputs
    dec = SN74HC138()

    for val in range(len(dec.RESULT_PINS)):
        set_binary_value(val, dec.getpins(dec.SERIAL_PINS))
        assert_pin_is_selected(dec, val)

def test_initial():
    dec = SN74HC138()

    assert_pin_is_selected(dec, 0)

@pytest.mark.parametrize('enable_pin,disabled_value', [
    ('G1', False),
    ('G2A', True),
    ('G2B', True),
])
def test_SN74HC138_disabled(enable_pin, disabled_value):
    # no pin is selected when disabled

    dec = SN74HC138()
    dec.getpin(enable_pin).set(disabled_value)

    assert_pin_is_selected(dec, -1)

def test_oscillating_input():
    dec = SN74HC138()

    FREQ = 4242
    inpt = Pin(code='FOO', oscillating_freq=FREQ)

    dec.pin_B.wire_to(inpt)
    # 2 oscillating pins
    assert dec.pin_Y0.oscillating_freq() == FREQ // 2
    assert dec.pin_Y2.oscillating_freq() == FREQ // 2
    assert dec.pin_Y1.oscillating_freq() == 0
    assert dec.pin_Y1.ishigh()

    dec.pin_C.sethigh()
    # 2 oscillating pins, but higher ones
    assert dec.pin_Y4.oscillating_freq() == FREQ // 2
    assert dec.pin_Y6.oscillating_freq() == FREQ // 2
    assert dec.pin_Y2.oscillating_freq() == 0
    assert dec.pin_Y2.ishigh()

    dec.pin_A.wire_to(inpt)
    # 4 oscillating pins
    assert dec.pin_Y4.oscillating_freq() == FREQ // 4
    assert dec.pin_Y5.oscillating_freq() == FREQ // 4
    assert dec.pin_Y6.oscillating_freq() == FREQ // 4
    assert dec.pin_Y7.oscillating_freq() == FREQ // 4
    assert dec.pin_Y0.ishigh()
