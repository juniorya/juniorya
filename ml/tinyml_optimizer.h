/**
 * @file tinyml_optimizer.h
 * @brief Lightweight neural network for trajectory optimisation.
 */

#ifndef TINYML_OPTIMIZER_H
#define TINYML_OPTIMIZER_H

#include <stddef.h>
#include <stdint.h>

#include "utils/q16.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Maximum supported feature vector length for the optimiser. */
#define TINYML_MAX_FEATURES 16U

/** @brief Maximum number of spline segments the optimiser evaluates in one batch. */
#define TINYML_MAX_SEGMENTS 64U

/** @brief Hidden layer width for the embedded neural network. */
#define TINYML_HIDDEN_WIDTH 8U

/**
 * @brief Feature metrics describing a spline segment for optimisation.
 */
typedef struct
{
    q16_16 curvature;       /**< Normalised curvature magnitude. */
    q16_16 arc_length;      /**< Segment arc length in workspace units. */
    q16_16 target_velocity; /**< Nominal target velocity prior to optimisation. */
    q16_16 target_accel;    /**< Nominal acceleration. */
} tinyml_segment_feature;

/**
 * @brief Neural network instance data for the embedded optimiser.
 */
typedef struct
{
    uint32_t feature_count;                    /**< Number of input features used by the model. */
    q16_16 weights_input_hidden[TINYML_MAX_FEATURES * TINYML_HIDDEN_WIDTH]; /**< Dense input-hidden weights. */
    q16_16 bias_hidden[TINYML_HIDDEN_WIDTH];   /**< Hidden layer bias terms. */
    q16_16 weights_hidden_output[TINYML_HIDDEN_WIDTH]; /**< Hidden-output weights. */
    q16_16 bias_output;                        /**< Output bias. */
} tinyml_network;

/**
 * @brief Initialise optimiser with baked in weights.
 *
 * The coefficients were obtained from an offline training session on a mix of
 * delta robot trajectories including circles, splines and jerk-limited blends.
 * They are scaled to Q16.16 and normalised so the resulting multiplier stays in
 * the range [0.5, 1.2].
 *
 * @param net Network instance to initialise.
 */
void tinyml_network_init(tinyml_network *net);

/**
 * @brief Evaluate neural network on feature vector.
 *
 * @param net Network instance.
 * @param features Normalised feature array of length @ref tinyml_network::feature_count.
 * @return Scaling multiplier for the trajectory velocity (Q16.16).
 */
q16_16 tinyml_network_evaluate(const tinyml_network *net, const q16_16 *features);

/**
 * @brief Optimise velocity scaling for pending spline segments.
 *
 * @param net Network instance.
 * @param segments Array of feature metrics, one per spline segment.
 * @param segment_count Number of entries in @p segments.
 * @param out_velocity_scale Output array receiving velocity multipliers per segment.
 * @return True on successful optimisation.
 */
bool tinyml_optimize_segments(const tinyml_network *net,
                              const tinyml_segment_feature *segments,
                              size_t segment_count,
                              q16_16 *out_velocity_scale);

#ifdef __cplusplus
}
#endif

#endif
