#include "storage/storage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define STORAGE_MAGIC 0x44435354u
#define STORAGE_MAX_ENTRIES 32u

typedef struct
{
    char key[32];
    uint8_t *data;
    size_t length;
} storage_entry;

static storage_options g_options;
static storage_entry g_entries[STORAGE_MAX_ENTRIES];
static size_t g_entry_count = 0U;

static uint16_t crc16(const uint8_t *data, size_t length)
{
    uint16_t crc = 0xFFFFu;
    for (size_t i = 0U; i < length; ++i)
    {
        crc ^= (uint16_t)data[i];
        for (int b = 0; b < 8; ++b)
        {
            if ((crc & 1U) != 0U)
            {
                crc = (uint16_t)((crc >> 1U) ^ 0xA001u);
            }
            else
            {
                crc >>= 1U;
            }
        }
    }
    return crc;
}

static bool storage_flush(void)
{
    FILE *fp = fopen(g_options.path, "wb");
    if (fp == NULL)
    {
        return false;
    }
    uint32_t magic = STORAGE_MAGIC;
    fwrite(&magic, sizeof(magic), 1U, fp);
    uint32_t count = (uint32_t)g_entry_count;
    fwrite(&count, sizeof(count), 1U, fp);
    for (size_t i = 0U; i < g_entry_count; ++i)
    {
        uint16_t key_len = (uint16_t)strlen(g_entries[i].key);
        uint16_t len = (uint16_t)g_entries[i].length;
        uint16_t crc = crc16(g_entries[i].data, g_entries[i].length);
        fwrite(&key_len, sizeof(key_len), 1U, fp);
        fwrite(g_entries[i].key, key_len, 1U, fp);
        fwrite(&len, sizeof(len), 1U, fp);
        fwrite(&crc, sizeof(crc), 1U, fp);
        fwrite(g_entries[i].data, len, 1U, fp);
    }
    fclose(fp);
    return true;
}

static void storage_clear(void)
{
    for (size_t i = 0U; i < g_entry_count; ++i)
    {
        free(g_entries[i].data);
        g_entries[i].data = NULL;
    }
    g_entry_count = 0U;
}

void storage_default_options(storage_options *options)
{
    if (options != NULL)
    {
        options->path = "storage.bin";
    }
}

bool storage_init(const storage_options *options)
{
    if ((options == NULL) || (options->path == NULL))
    {
        return false;
    }
    g_options = *options;
    storage_clear();
    FILE *fp = fopen(options->path, "rb");
    if (fp == NULL)
    {
        return true;
    }
    uint32_t magic = 0U;
    if (fread(&magic, sizeof(magic), 1U, fp) != 1U)
    {
        fclose(fp);
        return false;
    }
    if (magic != STORAGE_MAGIC)
    {
        fclose(fp);
        return false;
    }
    uint32_t count = 0U;
    if (fread(&count, sizeof(count), 1U, fp) != 1U)
    {
        fclose(fp);
        return false;
    }
    for (uint32_t i = 0U; i < count; ++i)
    {
        uint16_t key_len = 0U;
        uint16_t len = 0U;
        uint16_t crc = 0U;
        if (fread(&key_len, sizeof(key_len), 1U, fp) != 1U)
        {
            break;
        }
        if (key_len >= sizeof(g_entries[0].key))
        {
            fseek(fp, (long)key_len, SEEK_CUR);
            fseek(fp, sizeof(len) + sizeof(crc), SEEK_CUR);
            continue;
        }
        if (fread(g_entries[g_entry_count].key, key_len, 1U, fp) != 1U)
        {
            break;
        }
        g_entries[g_entry_count].key[key_len] = '\0';
        if (fread(&len, sizeof(len), 1U, fp) != 1U)
        {
            break;
        }
        if (fread(&crc, sizeof(crc), 1U, fp) != 1U)
        {
            break;
        }
        uint8_t *buffer = malloc(len);
        if (buffer == NULL)
        {
            break;
        }
        if (fread(buffer, len, 1U, fp) != 1U)
        {
            free(buffer);
            break;
        }
        if (crc16(buffer, len) != crc)
        {
            free(buffer);
            continue;
        }
        g_entries[g_entry_count].data = buffer;
        g_entries[g_entry_count].length = len;
        g_entry_count++;
        if (g_entry_count >= STORAGE_MAX_ENTRIES)
        {
            break;
        }
    }
    fclose(fp);
    return true;
}

void storage_shutdown(void)
{
    storage_flush();
    storage_clear();
}

static storage_entry *storage_find(const char *key)
{
    for (size_t i = 0U; i < g_entry_count; ++i)
    {
        if (strcmp(g_entries[i].key, key) == 0)
        {
            return &g_entries[i];
        }
    }
    return NULL;
}

bool storage_write(const char *key, const void *data, size_t length)
{
    if ((key == NULL) || (data == NULL) || (length == 0U))
    {
        return false;
    }
    storage_entry *entry = storage_find(key);
    if (entry == NULL)
    {
        if (g_entry_count >= STORAGE_MAX_ENTRIES)
        {
            return false;
        }
        entry = &g_entries[g_entry_count++];
        strncpy(entry->key, key, sizeof(entry->key) - 1U);
        entry->key[sizeof(entry->key) - 1U] = '\0';
        entry->data = NULL;
        entry->length = 0U;
    }
    free(entry->data);
    entry->data = malloc(length);
    if (entry->data == NULL)
    {
        entry->length = 0U;
        return false;
    }
    memcpy(entry->data, data, length);
    entry->length = length;
    return storage_flush();
}

bool storage_read(const char *key, void *data, size_t length)
{
    storage_entry *entry = storage_find(key);
    if ((entry == NULL) || (entry->length != length))
    {
        return false;
    }
    memcpy(data, entry->data, length);
    return true;
}
