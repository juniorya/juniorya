/**
 * @file xml_loader.c
 * @brief Minimal XML parsing utilities for EtherCAT drive descriptors.
 */

#include "xml_loader.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/log.h"

static bool parse_numeric(const char *text, uint32_t *out)
{
    if ((text == NULL) || (out == NULL))
    {
        return false;
    }

    char *endptr = NULL;
    unsigned long value = 0UL;
    if ((text[0] == '0') && ((text[1] == 'x') || (text[1] == 'X')))
    {
        value = strtoul(text, &endptr, 16);
    }
    else
    {
        value = strtoul(text, &endptr, 10);
    }
    if ((endptr == text) || (value > 0xFFFFFFFFUL))
    {
        return false;
    }
    *out = (uint32_t)value;
    return true;
}

static const char *find_tag(const char *xml, const char *tag, size_t *len)
{
    char needle[64];
    (void)snprintf(needle, sizeof(needle), "<%s>", tag);
    const char *start = strstr(xml, needle);
    if (start == NULL)
    {
        return NULL;
    }
    start += strlen(needle);
    char closing[64];
    (void)snprintf(closing, sizeof(closing), "</%s>", tag);
    const char *end = strstr(start, closing);
    if (end == NULL)
    {
        return NULL;
    }
    *len = (size_t)(end - start);
    return start;
}

bool ethcat_load_drives_from_xml(const char *path, ethcat_master_config *cfg)
{
    if ((path == NULL) || (cfg == NULL))
    {
        return false;
    }

    FILE *fp = fopen(path, "rb");
    if (fp == NULL)
    {
        log_printf(LOG_ERROR, "Failed to open drive XML %s", path);
        return false;
    }

    (void)fseek(fp, 0, SEEK_END);
    const long size = ftell(fp);
    (void)fseek(fp, 0, SEEK_SET);
    if (size <= 0)
    {
        fclose(fp);
        return false;
    }

    char *buffer = (char *)malloc((size_t)size + 1U);
    if (buffer == NULL)
    {
        fclose(fp);
        return false;
    }
    (void)fread(buffer, 1U, (size_t)size, fp);
    buffer[size] = '\0';
    fclose(fp);

    cfg->drive_count = 0U;

    const char *cursor = buffer;
    while ((cursor = strstr(cursor, "<Device")) != NULL)
    {
        if (cfg->drive_count >= (sizeof(cfg->drives) / sizeof(cfg->drives[0])))
        {
            break;
        }
        const char *block_end = strstr(cursor, "</Device>");
        if (block_end == NULL)
        {
            break;
        }

        size_t len = 0U;
        ethcat_drive_descriptor *desc = &cfg->drives[cfg->drive_count];
        (void)memset(desc, 0, sizeof(*desc));
        const char *name = find_tag(cursor, "Name", &len);
        if ((name != NULL) && (len > 0U))
        {
            const size_t copy_len = (len < (sizeof(desc->name) - 1U))
                                        ? len
                                        : (sizeof(desc->name) - 1U);
            (void)memcpy(desc->name, name, copy_len);
            desc->name[copy_len] = '\0';
        }

        char value[64];
        const char *vendor = find_tag(cursor, "VendorId", &len);
        if (vendor != NULL)
        {
            const size_t copy_len = (len < (sizeof(value) - 1U)) ? len : (sizeof(value) - 1U);
            (void)memcpy(value, vendor, copy_len);
            value[copy_len] = '\0';
            if (!parse_numeric(value, &desc->vendor_id))
            {
                desc->vendor_id = 0U;
            }
        }

        const char *product = find_tag(cursor, "ProductCode", &len);
        if (product != NULL)
        {
            const size_t copy_len = (len < (sizeof(value) - 1U)) ? len : (sizeof(value) - 1U);
            (void)memcpy(value, product, copy_len);
            value[copy_len] = '\0';
            if (!parse_numeric(value, &desc->product_code))
            {
                desc->product_code = 0U;
            }
        }

        const char *revision = find_tag(cursor, "RevisionNo", &len);
        if (revision != NULL)
        {
            const size_t copy_len = (len < (sizeof(value) - 1U)) ? len : (sizeof(value) - 1U);
            (void)memcpy(value, revision, copy_len);
            value[copy_len] = '\0';
            uint32_t tmp = 0U;
            if (parse_numeric(value, &tmp))
            {
                desc->revision = (uint8_t)(tmp & 0xFFU);
            }
        }

        ++cfg->drive_count;
        cursor = block_end + strlen("</Device>");
    }

    free(buffer);

    if (cfg->drive_count > 0U)
    {
        log_printf(LOG_INFO, "Parsed %u EtherCAT drives", (unsigned)cfg->drive_count);
        return true;
    }
    return false;
}
