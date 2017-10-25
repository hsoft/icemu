from itertools import combinations

# first pin is the least significant bit
def set_binary_value(value, pins):
    for i, pin in enumerate(pins):
        pin.set(bool((value >> i) & 0x1))

def get_binary_value(pins):
    value = 0
    for index, pin in enumerate(pins):
        if pin.ishigh():
            value |= 1 << index
    return value

FREQ_SUFFIXES = ['Hz', 'kHz', 'MHz', 'THz']
def fmtfreq(freq):
    freq_order = 0
    while (freq_order < len(FREQ_SUFFIXES)-1) and (freq // 1000 > 0):
        freq //= 1000
        freq_order += 1
    return "{}{}".format(freq, FREQ_SUFFIXES[freq_order])

# combinations_inner('abc') -> ['a', 'b', 'c', 'ab', 'ac', 'bc', 'abc']
def combinations_inner(seq):
    yield from ((elem, ) for elem in seq)
    for i in range(2, len(seq)):
        yield from combinations(seq, i)
    yield tuple(seq)
