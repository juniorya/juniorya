/**
 * @file board.h
 * @brief Board-level configuration parameters.
 */

#ifndef BOARD_H
#define BOARD_H

#include "kinematics/delta_kinematics.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Obtain default delta robot configuration.
 * @param cfg Configuration structure.
 */
void board_default_delta_config(delta_cfg_t *cfg);

#ifdef __cplusplus
}
#endif

#endif
