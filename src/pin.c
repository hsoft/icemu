#include "pin.h"

void icemu_pin_set(Pin *pin, bool high)
{
    pin->high = high;
}

