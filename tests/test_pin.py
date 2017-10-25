import pytest

from icemu.pin import Pin

@pytest.mark.parametrize('freq,expected', [
    (42, '42Hz'),
    (1234, '1kHz'),
    (987654321, '987MHz'),
])
def test_oscillating_fmt(freq, expected):
    pin = Pin(code='FOO', oscillating_freq=freq)
    assert pin.oscillating_freq() == freq
    assert str(pin) == "FOO/O/~{}".format(expected)

def test_sethigh_and_freq():
    # setting a pin high or low stops oscillation
    pin = Pin(code='FOO', oscillating_freq=42)
    pin.setlow()
    assert pin.oscillating_freq() == 0
    pin.set_oscillating_freq(42)
    pin.sethigh()
    assert pin.oscillating_freq() == 0

