/**
 * @file qemu_selftest.c
 * @brief Minimal program executed under QEMU MIPS64 to validate fixed-point math.
 */

#include "utils/q16.h"

int main(void)
{
    q16_16 a = q16_from_float(1.5f);
    q16_16 b = q16_from_float(2.0f);
    q16_16 c = q16_mul(a, b);
    if (q16_to_float(c) < 2.9f || q16_to_float(c) > 3.1f)
    {
        return 1;
    }
    return 0;
}
