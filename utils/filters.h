/**
 * @file filters.h
 * @brief Discrete-time filtering helpers for feedrate smoothing.
 */

#ifndef FILTERS_H
#define FILTERS_H

#include "q16.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief First-order low-pass filter state. */
typedef struct
{
    q16_16 value; /**< Current output. */
    q16_16 alpha; /**< Filter coefficient (0-1). */
} lp_filter_q16;

/**
 * @brief Configure first-order low-pass filter.
 * @param filter Filter instance.
 * @param alpha Normalised coefficient (0 disables filtering, 1 is direct copy).
 */
void lp_filter_init(lp_filter_q16 *filter, q16_16 alpha);

/**
 * @brief Update low-pass filter with new sample.
 * @param filter Filter instance.
 * @param sample New input sample.
 * @return Filtered output.
 */
q16_16 lp_filter_update(lp_filter_q16 *filter, q16_16 sample);

#ifdef __cplusplus
}
#endif

#endif
