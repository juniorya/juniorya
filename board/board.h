/**
 * @file board.h
 * @brief Board-level configuration helpers.
 */
#ifndef BOARD_BOARD_H
#define BOARD_BOARD_H

#include <stdbool.h>
#include "ethcat/ethcat.h"

/**
 * @brief Configure network stack for EtherCAT.
 *
 * @param master EtherCAT master.
 * @return true on success.
 */
bool board_configure_network(ethcat_master_t *master);

#endif /* BOARD_BOARD_H */
