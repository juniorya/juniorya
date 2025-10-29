#include "timer.h"

static volatile uint32_t s_ticks = 0;

void timer_tick_isr(void)
{
    ++s_ticks;
}

uint32_t timer_get_ticks(void)
{
    return s_ticks;
}

void timer_delay_ticks(uint32_t ticks)
{
    uint32_t start = timer_get_ticks();
    while ((timer_get_ticks() - start) < ticks) {
        /* busy wait for deterministic delay */
    }
}
