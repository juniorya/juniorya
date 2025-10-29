#ifndef UTILS_TRIG_H
#define UTILS_TRIG_H

#include "fixed.h"

#ifdef __cplusplus
extern "C" {
#endif

q16_16_t trig_sin(q16_16_t angle_rad);
q16_16_t trig_cos(q16_16_t angle_rad);

#ifdef __cplusplus
}
#endif

#endif
