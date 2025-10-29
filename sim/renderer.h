/**
 * @file renderer.h
 * @brief Simple software renderer for simulator output.
 */

#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include <stddef.h>

#include "planner/splines/splines.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @brief Software canvas for trajectory rendering. */
typedef struct
{
    size_t width;       /**< Canvas width in pixels. */
    size_t height;      /**< Canvas height in pixels. */
    unsigned char *px;  /**< Pixel buffer (RGB). */
} renderer_canvas;

/**
 * @brief Initialise renderer canvas.
 * @param canvas Canvas instance.
 * @param width Width in pixels.
 * @param height Height in pixels.
 * @return True on success.
 */
bool renderer_init(renderer_canvas *canvas, size_t width, size_t height);

/**
 * @brief Clear canvas to black.
 * @param canvas Canvas instance.
 */
void renderer_clear(renderer_canvas *canvas);

/**
 * @brief Plot a point in millimetres onto canvas.
 * @param canvas Canvas instance.
 * @param x_mm X coordinate.
 * @param y_mm Y coordinate.
 */
void renderer_plot(renderer_canvas *canvas, float x_mm, float y_mm);

/**
 * @brief Save canvas as ASCII PPM file.
 * @param canvas Canvas instance.
 * @param path Output file path.
 * @return True on success.
 */
bool renderer_save_ppm(const renderer_canvas *canvas, const char *path);

/**
 * @brief Write trajectory samples to CSV file.
 * @param traj Time-scaled trajectory.
 * @param path Output file path.
 * @return True on success.
 */
bool renderer_save_csv(const time_scaled_traj_t *traj, const char *path);

/**
 * @brief Release renderer resources.
 * @param canvas Canvas instance.
 */
void renderer_deinit(renderer_canvas *canvas);

#ifdef __cplusplus
}
#endif

#endif
