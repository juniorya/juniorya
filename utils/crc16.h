#ifndef UTILS_CRC16_H
#define UTILS_CRC16_H

#include <stdint.h>

uint16_t crc16_ccitt(const uint8_t *data, uint16_t length, uint16_t seed);

#endif
