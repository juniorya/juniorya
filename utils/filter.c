#include "filter.h"

void lp_filter_init(lp_filter_q16_16_t *filter, q16_16_t alpha)
{
    filter->value = 0;
    filter->alpha = alpha;
}

q16_16_t lp_filter_update(lp_filter_q16_16_t *filter, q16_16_t input)
{
    q16_16_t delta = input - filter->value;
    q16_16_t scaled = q16_16_mul(delta, filter->alpha);
    filter->value += scaled;
    return filter->value;
}
