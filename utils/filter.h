#ifndef UTILS_FILTER_H
#define UTILS_FILTER_H

#include "fixed.h"

typedef struct {
    q16_16_t value;
    q16_16_t alpha;
} lp_filter_q16_16_t;

void lp_filter_init(lp_filter_q16_16_t *filter, q16_16_t alpha);
q16_16_t lp_filter_update(lp_filter_q16_16_t *filter, q16_16_t input);

#endif
