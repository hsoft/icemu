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

def test_low_high():
    pin = Pin(code='FOO', high=True)
    assert pin.ishigh()
    pin.setlow()
    assert not pin.ishigh()
    pin.sethigh()
    assert pin.ishigh()
    pin.toggle()
    assert not pin.ishigh()

def test_sethigh_and_freq():
    # setting a pin high or low stops oscillation
    FREQ = 1000
    pin = Pin(code='FOO', oscillating_freq=FREQ)
    assert pin.is_oscillating_rapidly()
    pin.toggle()
    assert pin.oscillating_freq() == 0
    pin.set_oscillating_freq(FREQ)
    pin.tick(50) # oscillate slowly
    assert pin.is_oscillating_slowly()
    pin.toggle()
    assert pin.oscillating_freq() == 0

def test_oscillating_threshold():
    # whether we oscillate slowly or rapidly depends on the ticks we send the pin.
    pin = Pin(code='FOO', oscillating_freq=1000)
    # 1 kHz means an oscillation every 1000 usec
    assert pin.is_oscillating_rapidly() # we always start oscillating rapidly
    pin.tick(50) # we become slow because it takes a lot of ticks to toggle.
    washigh = pin.ishigh()
    assert not pin.is_oscillating_rapidly()
    assert pin.is_oscillating_slowly()
    # verify that we actually oscillate after a couple of ticks. At a freq of 1000Hz, we oscillate
    # at every 1ms, which means that we toggle every 500us. 9 more ticks like that and we'll
    # toggle.
    for _ in range(9):
        assert pin.ishigh() == washigh
        pin.tick(50)
    assert pin.ishigh() == (not washigh) # toggled!
    pin.tick(1000) # now, we're oscillating rapidly (we're right on the edge)
    assert pin.is_oscillating_rapidly()
    assert not pin.is_oscillating_slowly()
    assert pin.ishigh()

