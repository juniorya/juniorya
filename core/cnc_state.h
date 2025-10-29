#ifndef CORE_CNC_STATE_H
#define CORE_CNC_STATE_H

#include <stdbool.h>

typedef enum {
    CNC_STATE_IDLE = 0,
    CNC_STATE_RUN,
    CNC_STATE_HOLD,
    CNC_STATE_ALARM,
    CNC_STATE_ESTOP
} cnc_state_t;

typedef struct {
    cnc_state_t state;
    bool drives_enabled;
    bool alarm_active;
} cnc_runtime_t;

void cnc_runtime_init(cnc_runtime_t *runtime);
void cnc_runtime_set_state(cnc_runtime_t *runtime, cnc_state_t state);

#endif
