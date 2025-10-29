#ifndef DRIVERS_GPIO_H
#define DRIVERS_GPIO_H

#include <stdbool.h>

void gpio_init(void);
bool gpio_estop_triggered(void);
bool gpio_limit_triggered(int axis);
void gpio_clear_faults(void);
void gpio_set_status_led(bool on);

#endif
