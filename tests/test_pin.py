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
    # whether we oscillate slowly or rapidly depends on the ticks we send the pin.
    pin = Pin(code='FOO', oscillating_freq=1000)
    # 1 kHz means a tick every 1000 usec
    assert pin.is_oscillating_rapidly() # we always start oscillating rapidly
    pin.tick(50) # we become slow because it takes a lot of ticks to toggle.
    assert not pin.is_oscillating_rapidly()
    assert pin.is_oscillating_slowly()
    assert pin.next_oscillation_in() == 950
    pin.setlow()
    pin.tick(50) # no toggle
    assert pin.is_oscillating_slowly()
    assert not pin.ishigh()
    pin.tick(1000) # still slow, toggles
    assert pin.is_oscillating_slowly()
    assert pin.ishigh()

