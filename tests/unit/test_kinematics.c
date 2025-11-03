#include <stdbool.h>

#include "kinematics/delta.h"
#include "utils/q16.h"
#include "utils/vec3.h"

bool test_kinematics_roundtrip(void)
{
    delta_cfg_t cfg;
    delta_default_config(&cfg);
    vec3_q16 pose = {q16_from_int(0), q16_from_int(0), q16_from_int(-300)};
    q16_16 joints[3];
    if (!delta_inverse(&cfg, pose, joints))
    {
        return false;
    }
    vec3_q16 out;
    if (!delta_forward(&cfg, joints, &out))
    {
        return false;
    }
    return q16_abs(out.z - pose.z) < q16_from_float(0.5f);
}
