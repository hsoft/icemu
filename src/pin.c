#include "pin.h"
#include "chip.h"

void icemu_pin_set(Pin *pin, bool high)
{
    if (high != pin->high) {
        pin->high = high;
        pin->chip->pin_change_func(pin);
    }
}

