/**
 * @file storage.h
 * @brief Persistent key-value storage with CRC protection.
 */
#ifndef STORAGE_STORAGE_H
#define STORAGE_STORAGE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define STORAGE_MAX_ENTRIES 16U
#define STORAGE_KEY_SIZE 16U
#define STORAGE_VALUE_SIZE 64U

/**
 * @brief Storage entry.
 */
typedef struct
{
    char key[STORAGE_KEY_SIZE]; /**< Null-terminated key. */
    uint8_t value[STORAGE_VALUE_SIZE]; /**< Stored value. */
    size_t value_len; /**< Length of stored value. */
} storage_entry_t;

/**
 * @brief Storage context.
 */
typedef struct
{
    storage_entry_t entries[STORAGE_MAX_ENTRIES]; /**< Entry table. */
    size_t count; /**< Number of valid entries. */
    uint16_t crc; /**< CRC covering stored data. */
} storage_t;

/**
 * @brief Initialize storage context.
 *
 * @param storage Storage context.
 */
void storage_init(storage_t *storage);

/**
 * @brief Insert or update a key.
 *
 * @param storage Storage context.
 * @param key Null-terminated key string.
 * @param value Value buffer.
 * @param len Length of value.
 * @return true on success.
 */
bool storage_put(storage_t *storage,
                 const char *key,
                 const uint8_t *value,
                 size_t len);

/**
 * @brief Retrieve a stored value.
 *
 * @param storage Storage context.
 * @param key Key to look up.
 * @param value Output buffer.
 * @param len Input: buffer size, Output: actual length.
 * @return true if the key exists.
 */
bool storage_get(const storage_t *storage,
                 const char *key,
                 uint8_t *value,
                 size_t *len);

/**
 * @brief Persist storage contents to non-volatile memory.
 *
 * @param storage Storage context.
 * @return true on success.
 */
bool storage_save(storage_t *storage);

/**
 * @brief Load storage contents from non-volatile memory.
 *
 * @param storage Storage context.
 * @return true on success.
 */
bool storage_load(storage_t *storage);

#endif /* STORAGE_STORAGE_H */
