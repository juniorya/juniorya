#include "drivers/drivers.h"

#include <stddef.h>

bool drivers_init(drivers_context *ctx)
{
    if (ctx == NULL)
    {
        return false;
    }
    ctx->estop_cleared = true;
    return true;
}

void drivers_shutdown(drivers_context *ctx)
{
    (void)ctx;
}

bool drivers_estop_asserted(const drivers_context *ctx)
{
    return (ctx == NULL) ? true : !ctx->estop_cleared;
}
