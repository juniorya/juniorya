/**
 * @file lut_trig.c
 * @brief Fixed sine/cosine lookup table implementation.
 */

#include "lut_trig.h"

#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define LUT_SIZE 1024

static q16_16 sin_table[LUT_SIZE];
static q16_16 cos_table[LUT_SIZE];
static bool initialised = false;

void lut_trig_init(void)
{
    if (initialised)
    {
        return;
    }
    for (int i = 0; i < LUT_SIZE; ++i)
    {
        double angle = (double)i / (double)LUT_SIZE * 2.0 * M_PI;
        sin_table[i] = q16_from_float((float)sin(angle));
        cos_table[i] = q16_from_float((float)cos(angle));
    }
    initialised = true;
}

static q16_16 interpolate(const q16_16 table[LUT_SIZE], q16_16 angle_rad)
{
    const int32_t wrap = 2 * 3.14159265f * (float)(1 << 16);
    while (angle_rad < 0)
    {
        angle_rad = q16_add(angle_rad, wrap);
    }
    while (angle_rad >= wrap)
    {
        angle_rad = q16_sub(angle_rad, wrap);
    }
    int32_t index = (int32_t)((int64_t)angle_rad * LUT_SIZE / wrap);
    int32_t next = (index + 1) % LUT_SIZE;
    q16_16 frac = angle_rad - (q16_16)((int64_t)index * wrap / LUT_SIZE);
    q16_16 span = (q16_16)(wrap / LUT_SIZE);
    bool ok = true;
    q16_16 t = q16_div(frac, span, &ok);
    if (!ok)
    {
        t = 0;
    }
    q16_16 one_minus_t = q16_sub(q16_from_int(1), t);
    q16_16 term1 = q16_mul(table[index], one_minus_t);
    q16_16 term2 = q16_mul(table[next], t);
    return q16_add(term1, term2);
}

q16_16 lut_sin(q16_16 angle_rad)
{
    if (!initialised)
    {
        lut_trig_init();
    }
    return interpolate(sin_table, angle_rad);
}

q16_16 lut_cos(q16_16 angle_rad)
{
    if (!initialised)
    {
        lut_trig_init();
    }
    return interpolate(cos_table, angle_rad);
}
