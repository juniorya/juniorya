#include "dds/dds.h"

#include <stdlib.h>
#include <string.h>

#define DDS_MAX_TOPICS 16

typedef struct
{
    char name[32];
    uint8_t buffer[256];
    size_t size;
    bool valid;
} dds_topic_slot;

static dds_topic_slot g_topics[DDS_MAX_TOPICS];
static dds_qos_t g_default_qos;
static bool g_initialised = false;

bool dds_init(uint32_t domain_id, const dds_qos_t *default_qos)
{
    (void)domain_id;
    if (default_qos != NULL)
    {
        g_default_qos = *default_qos;
    }
    else
    {
        g_default_qos.reliability = DDS_RELIABILITY_BEST_EFFORT;
        g_default_qos.history_depth = 1U;
    }
    memset(g_topics, 0, sizeof(g_topics));
    g_initialised = true;
    return true;
}

static dds_topic_slot *dds_find_topic(const char *topic)
{
    for (size_t i = 0; i < DDS_MAX_TOPICS; ++i)
    {
        if (g_topics[i].valid && (strcmp(g_topics[i].name, topic) == 0))
        {
            return &g_topics[i];
        }
    }
    return NULL;
}

static dds_topic_slot *dds_reserve_topic(const char *topic)
{
    dds_topic_slot *slot = dds_find_topic(topic);
    if (slot != NULL)
    {
        return slot;
    }
    for (size_t i = 0; i < DDS_MAX_TOPICS; ++i)
    {
        if (!g_topics[i].valid)
        {
            strncpy(g_topics[i].name, topic, sizeof(g_topics[i].name) - 1U);
            g_topics[i].name[sizeof(g_topics[i].name) - 1U] = '\0';
            g_topics[i].size = 0U;
            g_topics[i].valid = true;
            return &g_topics[i];
        }
    }
    return NULL;
}

dds_writer *dds_writer_create(const char *topic, size_t sample_size, const dds_qos_t *qos)
{
    if (!g_initialised)
    {
        return NULL;
    }
    dds_topic_slot *slot = dds_reserve_topic(topic);
    if (slot == NULL)
    {
        return NULL;
    }
    dds_writer *writer = malloc(sizeof(dds_writer));
    if (writer == NULL)
    {
        return NULL;
    }
    writer->topic = slot->name;
    writer->sample_size = sample_size;
    writer->qos = (qos != NULL) ? qos : &g_default_qos;
    return writer;
}

dds_reader *dds_reader_create(const char *topic, size_t sample_size, const dds_qos_t *qos)
{
    if (!g_initialised)
    {
        return NULL;
    }
    dds_topic_slot *slot = dds_reserve_topic(topic);
    if (slot == NULL)
    {
        return NULL;
    }
    dds_reader *reader = malloc(sizeof(dds_reader));
    if (reader == NULL)
    {
        return NULL;
    }
    reader->topic = slot->name;
    reader->sample_size = sample_size;
    reader->qos = (qos != NULL) ? qos : &g_default_qos;
    return reader;
}

bool dds_write(dds_writer *writer, const void *sample, size_t size)
{
    if ((writer == NULL) || (sample == NULL) || (size > sizeof(g_topics[0].buffer)))
    {
        return false;
    }
    dds_topic_slot *slot = dds_reserve_topic(writer->topic);
    if (slot == NULL)
    {
        return false;
    }
    memcpy(slot->buffer, sample, size);
    slot->size = size;
    slot->valid = true;
    return true;
}

bool dds_read(dds_reader *reader, void *sample, size_t size)
{
    if ((reader == NULL) || (sample == NULL))
    {
        return false;
    }
    dds_topic_slot *slot = dds_find_topic(reader->topic);
    if ((slot == NULL) || (slot->size == 0U) || (slot->size != size))
    {
        return false;
    }
    memcpy(sample, slot->buffer, size);
    return true;
}

void dds_shutdown(void)
{
    for (size_t i = 0; i < DDS_MAX_TOPICS; ++i)
    {
        g_topics[i].valid = false;
    }
    g_initialised = false;
}
