/**
 * @file crc16.h
 * @brief CRC16-CCITT implementation for storage integrity checks.
 */
#ifndef UTILS_CRC16_H
#define UTILS_CRC16_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Compute CRC16-CCITT checksum.
 *
 * @param data Input buffer.
 * @param length Length of buffer in bytes.
 * @return CRC16 checksum.
 */
uint16_t crc16_ccitt(const uint8_t *data, size_t length);

#endif /* UTILS_CRC16_H */
