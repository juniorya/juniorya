/**
 * @file filters.c
 * @brief Implementation of simple low-pass filter.
 */

#include "filters.h"

void lp_filter_init(lp_filter_q16 *filter, q16_16 alpha)
{
    filter->value = 0;
    filter->alpha = alpha;
}

q16_16 lp_filter_update(lp_filter_q16 *filter, q16_16 sample)
{
    q16_16 delta = q16_sub(sample, filter->value);
    q16_16 scaled = q16_mul(delta, filter->alpha);
    filter->value = q16_add(filter->value, scaled);
    return filter->value;
}
