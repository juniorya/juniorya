#ifndef PLANNER_LOOKAHEAD_H
#define PLANNER_LOOKAHEAD_H

#include <stddef.h>

#include "utils/q16.h"

/** \brief Segment descriptor used by the look-ahead algorithm. */
typedef struct
{
    q16_16 length;     /**< Segment length. */
    q16_16 curvature;  /**< Curvature metric. */
    q16_16 feed;       /**< Requested feed rate. */
} lookahead_segment;

/** \brief Compute allowable feed for the current segment. */
q16_16 lookahead_evaluate(const lookahead_segment *segments, size_t count);

#endif
