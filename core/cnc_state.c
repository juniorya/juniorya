#include "cnc_state.h"

void cnc_runtime_init(cnc_runtime_t *runtime)
{
    runtime->state = CNC_STATE_IDLE;
    runtime->drives_enabled = false;
    runtime->alarm_active = false;
}

void cnc_runtime_set_state(cnc_runtime_t *runtime, cnc_state_t state)
{
    runtime->state = state;
    if (state == CNC_STATE_ALARM || state == CNC_STATE_ESTOP) {
        runtime->alarm_active = true;
    }
}
