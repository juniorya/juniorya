/**
 * @file cnc.c
 * @brief Implementation of the CNC finite state machine.
 *
 * The FSM enforces safe sequencing for homing, trajectory execution and fault
 * handling. Each control tick processes pending commands and produces telemetry
 * snapshots for diagnostics and the OPC UA server.
 */
#include "cnc.h"

#include <string.h>

void cnc_init(cnc_context_t *ctx)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->state = CNC_STATE_IDLE;
    ctx->feed_override = Q16_16_ONE;
}

void cnc_submit_command(cnc_context_t *ctx, cnc_command_t cmd)
{
    ctx->pending_cmd = cmd;
}

static void handle_transition(cnc_context_t *ctx)
{
    switch (ctx->pending_cmd)
    {
        case CNC_CMD_HOME:
            ctx->state = CNC_STATE_HOMING;
            ctx->drives_enabled = true;
            break;
        case CNC_CMD_START:
            if ((ctx->state == CNC_STATE_READY) || (ctx->state == CNC_STATE_HOLD))
            {
                ctx->state = CNC_STATE_RUNNING;
            }
            break;
        case CNC_CMD_HOLD:
            if (ctx->state == CNC_STATE_RUNNING)
            {
                ctx->state = CNC_STATE_HOLD;
            }
            break;
        case CNC_CMD_RESUME:
            if (ctx->state == CNC_STATE_HOLD)
            {
                ctx->state = CNC_STATE_RUNNING;
            }
            break;
        case CNC_CMD_RESET:
            ctx->state = CNC_STATE_IDLE;
            ctx->fault_latched = false;
            ctx->drives_enabled = false;
            break;
        default:
            break;
    }
    ctx->pending_cmd = CNC_CMD_NONE;
}

void cnc_tick(cnc_context_t *ctx,
              const vec3_q16 *feedback,
              const vec3_q16 *next_target,
              cnc_telemetry_t *telemetry)
{
    if (ctx->pending_cmd != CNC_CMD_NONE)
    {
        handle_transition(ctx);
    }

    if (ctx->state == CNC_STATE_HOMING)
    {
        ctx->state = CNC_STATE_READY;
    }

    if (telemetry != NULL)
    {
        telemetry->state = ctx->state;
        telemetry->target_xyz = *next_target;
        telemetry->actual_xyz = *feedback;
        telemetry->feed_override = ctx->feed_override;
        telemetry->fault_active = ctx->fault_latched;
    }
}

bool cnc_ready_for_motion(const cnc_context_t *ctx)
{
    return (ctx->state == CNC_STATE_READY) || (ctx->state == CNC_STATE_RUNNING) ||
           (ctx->state == CNC_STATE_HOLD);
}
