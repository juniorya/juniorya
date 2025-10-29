#include "gpio.h"

static struct {
    bool estop;
    bool limits[3];
    bool status_led;
} s_gpio;

void gpio_init(void)
{
    s_gpio.estop = false;
    for (int i = 0; i < 3; ++i) {
        s_gpio.limits[i] = false;
    }
    s_gpio.status_led = false;
}

bool gpio_estop_triggered(void)
{
    return s_gpio.estop;
}

bool gpio_limit_triggered(int axis)
{
    if (axis < 0 || axis >= 3) {
        return false;
    }
    return s_gpio.limits[axis];
}

void gpio_clear_faults(void)
{
    s_gpio.estop = false;
    for (int i = 0; i < 3; ++i) {
        s_gpio.limits[i] = false;
    }
}

void gpio_set_status_led(bool on)
{
    s_gpio.status_led = on;
}
