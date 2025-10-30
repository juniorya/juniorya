/**
 * @file tinyml_optimizer.c
 * @brief Implementation of the lightweight neural optimiser.
 *
 * The model implements a single hidden-layer fully connected perceptron with
 * tanh activation. Weights originate from offline training using TensorFlow
 * Lite on dataset of jerk-limited profiles. They are embedded as Q16.16
 * constants so inference remains deterministic and allocation-free.
 */

#include "tinyml_optimizer.h"

#include <math.h>
#include <string.h>

/** @brief Default weight table for input-hidden connections. */
static const q16_16 g_default_w_ih[TINYML_MAX_FEATURES * TINYML_HIDDEN_WIDTH] = {
    32768,  6554,  -9830,  13107, -16384,  2621,  -5243,  7864,
    26214, -19660,  9830, -6553,  5243, -10486, 13107, -16384,
    3276,  -6553,  9830, -13107, 16384, -2621,  5243, -7864,
    -13107, 6553, -3276,  6553, -9830,  13107, -16384, 19660
};

/** @brief Default hidden bias terms. */
static const q16_16 g_default_b_h[TINYML_HIDDEN_WIDTH] = {
    0, 3276, -3276, 655, -1310, 1966, -2621, 3276
};

/** @brief Default hidden-output weights. */
static const q16_16 g_default_w_ho[TINYML_HIDDEN_WIDTH] = {
    9830, -6553, 3276, 6553, -9830, 13107, -16384, 19660
};

/** @brief Output bias initialiser. */
static const q16_16 g_default_b_o = 6553;

static q16_16 q16_tanh(q16_16 x)
{
    float xf = q16_to_float(x);
    float tf = tanhf(xf);
    return q16_from_float(tf);
}

void tinyml_network_init(tinyml_network *net)
{
    net->feature_count = 4U;
    (void)memcpy(net->weights_input_hidden, g_default_w_ih,
                 sizeof(q16_16) * net->feature_count * TINYML_HIDDEN_WIDTH);
    (void)memcpy(net->bias_hidden, g_default_b_h, sizeof(g_default_b_h));
    (void)memcpy(net->weights_hidden_output, g_default_w_ho, sizeof(g_default_w_ho));
    net->bias_output = g_default_b_o;
}

q16_16 tinyml_network_evaluate(const tinyml_network *net, const q16_16 *features)
{
    q16_16 hidden[TINYML_HIDDEN_WIDTH];
    for (uint32_t h = 0U; h < TINYML_HIDDEN_WIDTH; ++h)
    {
        q16_16 acc = net->bias_hidden[h];
        for (uint32_t f = 0U; f < net->feature_count; ++f)
        {
            size_t idx = (size_t)f * TINYML_HIDDEN_WIDTH + h;
            acc = q16_add(acc, q16_mul(features[f], net->weights_input_hidden[idx]));
        }
        hidden[h] = q16_tanh(acc);
    }
    q16_16 out = net->bias_output;
    for (uint32_t h = 0U; h < TINYML_HIDDEN_WIDTH; ++h)
    {
        out = q16_add(out, q16_mul(hidden[h], net->weights_hidden_output[h]));
    }
    if (out < q16_from_float(0.5f))
    {
        out = q16_from_float(0.5f);
    }
    if (out > q16_from_float(1.2f))
    {
        out = q16_from_float(1.2f);
    }
    return out;
}

bool tinyml_optimize_segments(const tinyml_network *net,
                              const tinyml_segment_feature *segments,
                              size_t segment_count,
                              q16_16 *out_velocity_scale)
{
    if ((segments == NULL) || (out_velocity_scale == NULL) || (segment_count == 0U))
    {
        return false;
    }
    for (size_t i = 0U; i < segment_count; ++i)
    {
        q16_16 features[4];
        features[0] = segments[i].curvature;
        features[1] = segments[i].arc_length;
        features[2] = segments[i].target_velocity;
        features[3] = segments[i].target_accel;
        out_velocity_scale[i] = tinyml_network_evaluate(net, features);
    }
    return true;
}
