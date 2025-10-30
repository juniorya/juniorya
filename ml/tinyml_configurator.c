/**
 * @file tinyml_configurator.c
 * @brief Implementation of TinyML optimiser configuration helpers.
 */

#include "tinyml_configurator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/q16.h"

void tinyml_profile_default(tinyml_profile *profile)
{
    if (profile == NULL)
    {
        return;
    }
    profile->curvature_gain = q16_from_float(1.0f);
    profile->acceleration_gain = q16_from_float(1.0f);
    profile->velocity_floor = q16_from_float(0.6f);
    profile->velocity_ceiling = q16_from_float(1.4f);
    profile->clamp_output = true;
}

static void trim(char *line)
{
    size_t len = strlen(line);
    while ((len > 0U) && ((line[len - 1U] == '\n') || (line[len - 1U] == '\r') || (line[len - 1U] == ' ')))
    {
        line[len - 1U] = '\0';
        len--;
    }
    while ((line[0] != '\0') && ((line[0] == ' ') || (line[0] == '\t')))
    {
        memmove(line, line + 1, strlen(line));
    }
}

bool tinyml_profile_load(const char *path, tinyml_profile *profile)
{
    if ((path == NULL) || (profile == NULL))
    {
        return false;
    }
    FILE *fp = fopen(path, "r");
    if (fp == NULL)
    {
        return false;
    }
    tinyml_profile_default(profile);
    char line[128];
    while (fgets(line, sizeof(line), fp) != NULL)
    {
        trim(line);
        if ((line[0] == '#') || (line[0] == '\0'))
        {
            continue;
        }
        char key[64];
        char value[64];
        if (sscanf(line, "%63[^=]=%63s", key, value) != 2)
        {
            continue;
        }
        trim(key);
        trim(value);
        if (strcmp(key, "CURVATURE_GAIN") == 0)
        {
            profile->curvature_gain = q16_from_float((float)atof(value));
        }
        else if (strcmp(key, "ACCELERATION_GAIN") == 0)
        {
            profile->acceleration_gain = q16_from_float((float)atof(value));
        }
        else if (strcmp(key, "VELOCITY_FLOOR") == 0)
        {
            profile->velocity_floor = q16_from_float((float)atof(value));
        }
        else if (strcmp(key, "VELOCITY_CEILING") == 0)
        {
            profile->velocity_ceiling = q16_from_float((float)atof(value));
        }
        else if (strcmp(key, "CLAMP_OUTPUT") == 0)
        {
            profile->clamp_output = (atoi(value) != 0);
        }
    }
    fclose(fp);
    return true;
}

static q16_16 clamp(q16_16 value, q16_16 lo, q16_16 hi)
{
    if (value < lo)
    {
        return lo;
    }
    if (value > hi)
    {
        return hi;
    }
    return value;
}

bool tinyml_profile_apply(const tinyml_profile *profile, tinyml_network *net)
{
    if ((profile == NULL) || (net == NULL))
    {
        return false;
    }
    for (uint32_t i = 0U; i < net->feature_count * TINYML_HIDDEN_WIDTH; ++i)
    {
        if ((i % net->feature_count) == 0U)
        {
            net->weights_input_hidden[i] = q16_mul(net->weights_input_hidden[i], profile->curvature_gain);
        }
        else
        {
            net->weights_input_hidden[i] = q16_mul(net->weights_input_hidden[i], profile->acceleration_gain);
        }
    }
    if (profile->clamp_output)
    {
        net->bias_output = clamp(net->bias_output, profile->velocity_floor, profile->velocity_ceiling);
    }
    return true;
}

bool tinyml_profile_describe(const tinyml_profile *profile, char *buffer, size_t capacity)
{
    if ((profile == NULL) || (buffer == NULL) || (capacity == 0U))
    {
        return false;
    }
    int written = snprintf(buffer,
                           capacity,
                           "curv=%.2f acc=%.2f vel[%.2f, %.2f] clamp=%s",
                           q16_to_float(profile->curvature_gain),
                           q16_to_float(profile->acceleration_gain),
                           q16_to_float(profile->velocity_floor),
                           q16_to_float(profile->velocity_ceiling),
                           profile->clamp_output ? "true" : "false");
    return (written >= 0) && ((size_t)written < capacity);
}
