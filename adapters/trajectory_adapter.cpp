/**
 * @file trajectory_adapter.cpp
 * @brief C++ helper translating C previews into GPU friendly buffers.
 */

#include "core/include/cnc_api.h"

#include <vector>

/**
 * @brief Populate std::vector containers with preview data.
 *
 * @param preview Source preview.
 * @param positions Output vector of 3D positions.
 * @param velocities Output vector of 3D velocities (optional).
 */
void adapter_fill_vectors(const cnc_preview_t &preview,
                          std::vector<float> &positions,
                          std::vector<float> *velocities)
{
    const size_t sample_count = preview.samples.count;
    positions.resize(sample_count * 3U);
    float *pos_ptr = positions.data();
    float *vel_ptr = nullptr;
    if (velocities != nullptr)
    {
        velocities->resize(sample_count * 3U);
        vel_ptr = velocities->data();
    }
    cnc_api_preview_as_float(&preview, sample_count, pos_ptr, vel_ptr);
}
