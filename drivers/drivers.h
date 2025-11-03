#ifndef DRIVERS_H
#define DRIVERS_H

#include <stdbool.h>

/** \brief Hardware driver context (mock). */
typedef struct
{
    bool estop_cleared; /**< Emergency stop status. */
} drivers_context;

bool drivers_init(drivers_context *ctx);

void drivers_shutdown(drivers_context *ctx);

bool drivers_estop_asserted(const drivers_context *ctx);

#endif
