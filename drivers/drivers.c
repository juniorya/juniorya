/**
 * @file drivers.c
 * @brief Stubbed hardware driver implementation.
 */

#include "drivers.h"

bool drivers_init(drivers_context *ctx)
{
    ctx->estop_active = false;
    ctx->limits_ok = true;
    return true;
}

void drivers_update(drivers_context *ctx)
{
    (void)ctx;
}

void drivers_shutdown(drivers_context *ctx)
{
    (void)ctx;
}
