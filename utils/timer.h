#ifndef UTILS_TIMER_H
#define UTILS_TIMER_H

#include <stdint.h>

void timer_tick_isr(void);
uint32_t timer_get_ticks(void);
void timer_delay_ticks(uint32_t ticks);

#endif
