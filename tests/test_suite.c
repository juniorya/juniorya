/**
 * @file test_suite.c
 * @brief Minimal unit test coverage for planner, kinematics and storage.
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "board/board.h"
#include "kinematics/delta_kinematics.h"
#include "planner/splines/splines.h"
#include "storage/storage.h"
#include "utils/vec3.h"

static void test_spline_generation(void)
{
    vec3_q16 pts[4];
    for (int i = 0; i < 4; ++i)
    {
        pts[i] = vec3_from_float((float)i * 10.0f, 0.0f, 0.0f);
    }
    spl_plan_t plan;
    assert(spl_make_from_waypoints(pts, 4U, NULL, NULL, &plan));
    vec3_q16 pos;
    vec3_q16 deriv;
    assert(spl_sample_arc(&plan, q16_from_float(0.5f), &pos, &deriv));
}

static void test_kinematics_inverse(void)
{
    delta_cfg_t cfg;
    board_default_delta_config(&cfg);
    vec3_q16 pos = vec3_from_float(0.0f, 0.0f, q16_to_float(cfg.z_offset));
    q16_16 theta[3];
    assert(delta_inverse_kinematics(&cfg, &pos, theta));
}

static void test_storage_roundtrip(void)
{
    storage_init_options opts;
    storage_default_options(&opts);
    assert(storage_init(&opts));
    char payload[] = "test";
    assert(storage_write(payload, sizeof(payload)));
    char buf[8];
    assert(storage_read(buf, sizeof(buf)));
    assert(strcmp(buf, payload) == 0);
    storage_shutdown();
}

int main(void)
{
    test_spline_generation();
    test_kinematics_inverse();
    test_storage_roundtrip();
    printf("All tests passed\n");
    return 0;
}
