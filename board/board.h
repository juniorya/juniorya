#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

/** \brief EtherCAT slave descriptor. */
typedef struct
{
    uint32_t vendor_id; /**< Vendor identifier. */
    uint32_t product_code; /**< Product code. */
} board_drive_descriptor;

/** \brief Default drive mapping. */
board_drive_descriptor board_default_drive(void);

#endif
