/**
 * @file storage.c
 * @brief In-memory storage implementation with CRC protection.
 */
#include "storage.h"

#include <string.h>

#include "utils/crc16.h"

static uint16_t storage_compute_crc(const storage_t *storage)
{
    return crc16_ccitt((const uint8_t *)storage->entries,
                       sizeof(storage->entries));
}

void storage_init(storage_t *storage)
{
    memset(storage, 0, sizeof(*storage));
    storage->crc = storage_compute_crc(storage);
}

bool storage_put(storage_t *storage,
                 const char *key,
                 const uint8_t *value,
                 size_t len)
{
    if (len > STORAGE_VALUE_SIZE)
    {
        return false;
    }
    for (size_t i = 0U; i < storage->count; ++i)
    {
        if (strncmp(storage->entries[i].key, key, STORAGE_KEY_SIZE) == 0)
        {
            memcpy(storage->entries[i].value, value, len);
            storage->entries[i].value_len = len;
            storage->crc = storage_compute_crc(storage);
            return true;
        }
    }
    if (storage->count >= STORAGE_MAX_ENTRIES)
    {
        return false;
    }
    storage_entry_t *entry = &storage->entries[storage->count++];
    strncpy(entry->key, key, STORAGE_KEY_SIZE - 1U);
    entry->key[STORAGE_KEY_SIZE - 1U] = '\0';
    memcpy(entry->value, value, len);
    entry->value_len = len;
    storage->crc = storage_compute_crc(storage);
    return true;
}

bool storage_get(const storage_t *storage,
                 const char *key,
                 uint8_t *value,
                 size_t *len)
{
    for (size_t i = 0U; i < storage->count; ++i)
    {
        if (strncmp(storage->entries[i].key, key, STORAGE_KEY_SIZE) == 0)
        {
            if (*len < storage->entries[i].value_len)
            {
                return false;
            }
            memcpy(value, storage->entries[i].value, storage->entries[i].value_len);
            *len = storage->entries[i].value_len;
            return true;
        }
    }
    return false;
}

bool storage_save(storage_t *storage)
{
    storage->crc = storage_compute_crc(storage);
    return true;
}

bool storage_load(storage_t *storage)
{
    uint16_t crc = storage_compute_crc(storage);
    return crc == storage->crc;
}
