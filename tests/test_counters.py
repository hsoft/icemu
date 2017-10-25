import random

import pytest

from icemu.counters import SN74F161AN
from icemu.pin import Pin
from icemu.util import get_binary_value

ALL_COUNTER_CLASSES = [SN74F161AN]

@pytest.mark.parametrize('cnt_class', ALL_COUNTER_CLASSES)
def test_counters(cnt_class):
    cnt = cnt_class()

    clk = cnt.getpin(cnt.CLOCK_PIN)
    val = random.randint(0, cnt.maxvalue())
    for _ in range(val):
        clk.setlow()
        clk.sethigh()

    assert get_binary_value(cnt.getpins(cnt.OUTPUT_PINS)) == val

@pytest.mark.parametrize('cnt_class', ALL_COUNTER_CLASSES)
def test_oscillating_input(cnt_class):
    # When the input is oscillating, we end up with outputs that are oscillating at
    # various rates. Pin 1 is going to be freq/2, Pin2, freq/4 and so on.
    cnt = cnt_class()

    FREQ = 4242
    inpt = Pin(code='FOO', oscillating_freq=FREQ)
    clk = cnt.getpin(cnt.CLOCK_PIN)
    clk.wire_to(inpt)

    # oscillating pins always evaluate to high
    assert get_binary_value(cnt.getoutputpins()) == cnt.maxvalue()

    for i, opin in enumerate(cnt.getoutputpins(), start=1):
        assert opin.oscillating_freq() == (FREQ // (2 ** i))
