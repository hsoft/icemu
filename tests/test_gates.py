from itertools import product
import random

import pytest

from icemu.gates import (
    CD4001B, CD4002B, CD4025B, SN74LS02, SN74LS27, SN54S260, SN74HC14
)
from icemu.pin import Pin

ALL_NORS = [
    CD4001B,
    CD4002B,
    CD4025B,
    SN74LS02,
    SN74LS27,
    SN54S260,
]

ALL_INVERTERS = [
    SN74HC14,
]

def assert_output(sr, expected_value):
    value = 0
    for index, code in enumerate(sr.OUTPUT_PINS):
        pin = sr.getpin(code)
        if pin.ishigh():
            value |= 1 << index
    assert expected_value == value

def push_value(sr, input_pin, value):
    clock_pin = sr.getpin(sr.CLOCK_PIN)
    for index in range(len(sr.OUTPUT_PINS)):
        input_pin.set(bool(value & (1 << (len(sr.OUTPUT_PINS) - index - 1))))
        clock_pin.setlow()
        clock_pin.sethigh()

@pytest.mark.parametrize('nor_class', ALL_NORS)
def test_nor(nor_class):
    nor = nor_class()

    for *in_, out in nor.IO_MAPPING:
        pins_in = list(nor.getpins(in_))
        pin_out = nor.getpin(out)
        for input_states in product((False, True), repeat=len(pins_in)):
            for pin, ishigh in zip(pins_in, input_states):
                pin.set(ishigh)
            assert pin_out.ishigh() == (not any(input_states))

def test_sr_latch():
    # let's implement an S-R latch!
    # https://en.wikipedia.org/wiki/Flip-flop_(electronics)
    nor = CD4001B()

    nor.pin_D.wire_to(nor.pin_J)
    nor.pin_B.wire_to(nor.pin_K)

    # Initial state: unknown! but what we know is that J == ~K
    assert nor.pin_J.ishigh() == (not nor.pin_K.ishigh())

    # set J through C
    nor.pin_C.sethigh()
    nor.pin_C.setlow()
    assert nor.pin_J.ishigh()
    assert not nor.pin_K.ishigh()

    # set K through A
    nor.pin_A.sethigh()
    nor.pin_A.setlow()
    assert not nor.pin_J.ishigh()
    assert nor.pin_K.ishigh()

@pytest.mark.parametrize('inv_class', ALL_INVERTERS)
def test_inverters(inv_class):
    inv = inv_class()

    val = random.randint(0, 0xff)

    for i, in_ in enumerate(inv.INPUT_PINS):
        pin = inv.getpin(in_)
        pin.set(bool(val & (1 << i)))

    inv.update()

    for i, out in enumerate(inv.OUTPUT_PINS):
        pin = inv.getpin(out)
        assert pin.ishigh() == (not bool(val & (1 << i)))

def test_nor_oscillating():
    nor = CD4001B()

    FREQ = 42
    inpt = Pin(code='FOO', oscillating_freq=FREQ)

    nor.pin_A.wire_to(inpt)
    # Because the position of pin_B makes a positive outcome for pin_J possible, it's oscillating.
    assert nor.pin_J.oscillating_freq() == FREQ
    nor.pin_B.sethigh()
    # Because the position of pin_B makes a positive outcome for pin_J impossible, it's not oscillating.
    assert nor.pin_J.oscillating_freq() == 0
    assert not nor.pin_J.ishigh()

def test_inverters_oscillating():
    inv = SN74HC14()

    FREQ = 42
    inpt = Pin(code='FOO', oscillating_freq=FREQ)

    inv.getpin(inv.INPUT_PINS[0]).wire_to(inpt)
    assert inv.getpin(inv.OUTPUT_PINS[0]).oscillating_freq() == FREQ
    assert inv.getpin(inv.OUTPUT_PINS[1]).oscillating_freq() == 0
