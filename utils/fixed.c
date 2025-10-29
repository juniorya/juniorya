#include "fixed.h"

q16_16_t q16_16_sqrt(q16_16_t value)
{
    if (value <= 0) {
        return 0;
    }
    q16_16_t x = value;
    for (int i = 0; i < 16; ++i) {
        x = (x + q16_16_div(value, x)) >> 1;
    }
    return x;
}
