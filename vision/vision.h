/**
 * @file vision.h
 * @brief Machine vision interface for camera-assisted delta robot calibration and monitoring.
 */

#ifndef VISION_H
#define VISION_H

#include <stdbool.h>
#include <stddef.h>

#include "utils/q16.h"
#include "utils/vec3.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Descriptor for a detected fiducial marker.
 */
typedef struct
{
    vec3_q16 position;      /**< Marker position in workspace coordinates. */
    q16_16 confidence_q16;  /**< Detection confidence between 0 and 1 in Q16.16. */
} vision_marker;

/**
 * @brief Camera calibration and runtime configuration.
 */
typedef struct
{
    q16_16 focal_length_mm; /**< Camera focal length in millimetres (Q16.16). */
    q16_16 principal_x;     /**< Principal point X coordinate (Q16.16 pixels). */
    q16_16 principal_y;     /**< Principal point Y coordinate (Q16.16 pixels). */
    q16_16 exposure_ms;     /**< Exposure time in milliseconds (Q16.16). */
    bool enable_tracking;   /**< Enables fiducial tracking pipeline. */
    bool enable_depth;      /**< Enables synthetic depth estimation. */
} vision_camera_config;

/**
 * @brief Machine vision system configuration.
 */
typedef struct
{
    vision_camera_config camera; /**< Camera parameters. */
    q16_16 workspace_min[3];     /**< Minimum XYZ workspace for detection filter. */
    q16_16 workspace_max[3];     /**< Maximum XYZ workspace for detection filter. */
    q16_16 alignment_tolerance;  /**< Maximum allowable misalignment in millimetres (Q16.16). */
} vision_config;

/**
 * @brief Vision runtime context.
 */
typedef struct
{
    vision_config config;           /**< Static configuration snapshot. */
    q16_16 last_alignment_error;    /**< Last alignment error magnitude. */
    vision_marker markers[8U];      /**< Fixed-size storage for detected markers. */
    size_t marker_count;            /**< Number of valid markers. */
} vision_context;

/**
 * @brief Initialise machine vision pipeline.
 * @param ctx Vision context to initialise.
 * @param cfg Configuration parameters.
 * @return True when initialisation succeeds.
 */
bool vision_init(vision_context *ctx, const vision_config *cfg);

/**
 * @brief Populate configuration with safe defaults for desktop simulations.
 * @param cfg Configuration structure to populate.
 */
void vision_default_config(vision_config *cfg);

/**
 * @brief Perform a detection pass using synthetic frames from the simulator.
 * @param ctx Vision context.
 * @return True when a new detection result is available.
 */
bool vision_process_frame(vision_context *ctx);

/**
 * @brief Retrieve the alignment error estimated from latest detections.
 * @param ctx Vision context.
 * @param out_error Alignment error magnitude (Q16.16).
 * @return True when error is valid.
 */
bool vision_get_alignment_error(const vision_context *ctx, q16_16 *out_error);

/**
 * @brief Shutdown the machine vision pipeline and release resources.
 * @param ctx Vision context.
 */
void vision_shutdown(vision_context *ctx);

#ifdef __cplusplus
}
#endif

#endif
