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
    FREQ = 4242
    pin = Pin(code='FOO', oscillating_freq=FREQ)
    pin.setlow()
    assert pin.oscillating_freq() == 0
    pin.set_oscillating_freq(FREQ)
    pin.sethigh()
    assert pin.oscillating_freq() == 0

def test_oscillating_threshold():
    pin = Pin(code='FOO', oscillating_freq=Pin.OSCILLATION_THRESHOLD)
    assert pin.is_oscillating()
    pin.set_oscillating_freq(Pin.OSCILLATION_THRESHOLD-1)
    assert not pin.is_oscillating()

def test_oscillating_flip_under_threshold():
    tick = (Pin.OSCILLATION_THRESHOLD // 2) + 1
    pin = Pin(code='FOO', oscillating_freq=Pin.OSCILLATION_THRESHOLD-1)
    pin.sethigh()
    pin.tick(tick)
    assert pin.ishigh()
    pin.tick(tick)
    assert not pin.ishigh()
    pin.tick(tick)
    pin.tick(tick)
    assert pin.ishigh()
