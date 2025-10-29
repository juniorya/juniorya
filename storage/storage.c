/**
 * @file storage.c
 * @brief Simple file-backed key-value store with CRC16 protection.
 */

#include "storage.h"

#include "utils/log.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
    uint32_t version;
    uint16_t crc;
    uint16_t size;
    uint8_t payload[256];
} storage_record;

static storage_init_options g_opts;

static uint16_t crc16(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFFU;
    for (size_t i = 0; i < len; ++i)
    {
        crc ^= (uint16_t)data[i];
        for (int bit = 0; bit < 8; ++bit)
        {
            if ((crc & 1U) != 0U)
            {
                crc = (uint16_t)((crc >> 1) ^ 0xA001U);
            }
            else
            {
                crc >>= 1;
            }
        }
    }
    return crc;
}

void storage_default_options(storage_init_options *opts)
{
    opts->path_primary = "storage_primary.bin";
    opts->path_backup = "storage_backup.bin";
}

static bool write_record(const char *path, const storage_record *rec)
{
    FILE *f = fopen(path, "wb");
    if (f == NULL)
    {
        return false;
    }
    size_t written = fwrite(rec, sizeof(*rec), 1U, f);
    fclose(f);
    return written == 1U;
}

static bool read_record(const char *path, storage_record *rec)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL)
    {
        return false;
    }
    size_t read = fread(rec, sizeof(*rec), 1U, f);
    fclose(f);
    return read == 1U;
}

bool storage_init(const storage_init_options *opts)
{
    g_opts = *opts;
    return true;
}

bool storage_write(const void *data, size_t size)
{
    storage_record rec;
    rec.version = 1U;
    rec.size = (uint16_t)size;
    if (size > sizeof(rec.payload))
    {
        return false;
    }
    memcpy(rec.payload, data, size);
    rec.crc = crc16(rec.payload, size);
    if (!write_record(g_opts.path_primary, &rec))
    {
        return false;
    }
    return write_record(g_opts.path_backup, &rec);
}

bool storage_read(void *data, size_t size)
{
    storage_record rec;
    if (!read_record(g_opts.path_primary, &rec))
    {
        if (!read_record(g_opts.path_backup, &rec))
        {
            return false;
        }
    }
    if (rec.size > size)
    {
        return false;
    }
    uint16_t crc = crc16(rec.payload, rec.size);
    if (crc != rec.crc)
    {
        return false;
    }
    memcpy(data, rec.payload, rec.size);
    return true;
}

void storage_shutdown(void)
{
    log_printf(LOG_INFO, "Storage shutdown");
}
