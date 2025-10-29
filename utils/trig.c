#include "trig.h"

static const q16_16_t cordic_angles[] = {
    51472, 30385, 16054, 8149,
    4090, 2045, 1023, 512,
    256, 128, 64, 32,
    16, 8, 4, 2
};

static q16_16_t cordic_gain_inv(void)
{
    /* Precomputed 1/K for 16 iterations */
    return (q16_16_t)39797; /* 0.60725293 * 65536 */
}

static void cordic_rotate(q16_16_t angle, q16_16_t *cos_out, q16_16_t *sin_out)
{
    q16_16_t x = cordic_gain_inv();
    q16_16_t y = 0;
    q16_16_t z = angle;

    for (unsigned i = 0; i < sizeof(cordic_angles) / sizeof(cordic_angles[0]); ++i) {
        q16_16_t x_shift = x >> i;
        q16_16_t y_shift = y >> i;
        if (z >= 0) {
            x -= y_shift;
            y += x_shift;
            z -= cordic_angles[i];
        } else {
            x += y_shift;
            y -= x_shift;
            z += cordic_angles[i];
        }
    }

    *cos_out = x;
    *sin_out = y;
}

q16_16_t trig_sin(q16_16_t angle_rad)
{
    q16_16_t angle = angle_rad % (Q16_16_PI * 2);
    if (angle < -Q16_16_PI) {
        angle += Q16_16_PI * 2;
    } else if (angle > Q16_16_PI) {
        angle -= Q16_16_PI * 2;
    }
    q16_16_t cos_val;
    q16_16_t sin_val;
    cordic_rotate(angle, &cos_val, &sin_val);
    return sin_val;
}

q16_16_t trig_cos(q16_16_t angle_rad)
{
    q16_16_t angle = angle_rad % (Q16_16_PI * 2);
    if (angle < -Q16_16_PI) {
        angle += Q16_16_PI * 2;
    } else if (angle > Q16_16_PI) {
        angle -= Q16_16_PI * 2;
    }
    q16_16_t cos_val;
    q16_16_t sin_val;
    cordic_rotate(angle, &cos_val, &sin_val);
    return cos_val;
}
