/**
 * @file trajectory_adapter.h
 * @brief Interface for converting C previews into STL containers.
 */

#ifndef TRAJECTORY_ADAPTER_H
#define TRAJECTORY_ADAPTER_H

#include "core/include/cnc_api.h"

#include <vector>

void adapter_fill_vectors(const cnc_preview_t &preview,
                          std::vector<float> &positions,
                          std::vector<float> *velocities);

#endif /* TRAJECTORY_ADAPTER_H */
