#ifndef BOARD_BOARD_H
#define BOARD_BOARD_H

#include "config.h"

void board_clock_init(void);
void board_gpio_init(void);
void board_emac_init(void);
void board_console_init(void);
void board_load_configuration(void);

#endif
