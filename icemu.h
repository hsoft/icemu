#include <stdbool.h>
#include <stdint.h>

#define MAX_PINS_PER_CHIP 64
#define MAX_SHIFTREGISTER_OUTPUTS 8

typedef struct Chip Chip;

typedef struct Pin {
    Chip *chip;
    char *code;
    bool output;
    bool low_means_high;
    bool high;
} Pin;

typedef void (*PinChangeFunc)(Chip *, Pin *);

struct Chip {
    void *logical_unit;
    PinChangeFunc pin_change_func;
    Pin * pins[MAX_PINS_PER_CHIP];
};

typedef struct {
    uint8_t buffer;
    bool isbuffered;
    Pin *clock;
    Pin *serial1;
    Pin *serial2;
    Pin * outputs[MAX_SHIFTREGISTER_OUTPUTS];
} ShiftRegister;

void icemu_pin_set(Pin *pin, bool high);
Pin* icemu_chip_addpin(Chip *chip, char *code, bool output, bool low_means_high);
uint8_t icemu_shiftregister_outputcount(ShiftRegister *sr);
void icemu_CD74AC164_init(Chip *chip);
