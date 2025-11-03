#ifndef DDS_H
#define DDS_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** \brief DDS reliability quality. */
typedef enum
{
    DDS_RELIABILITY_BEST_EFFORT,
    DDS_RELIABILITY_RELIABLE
} dds_reliability_t;

/** \brief QoS parameters for DDS entities. */
typedef struct
{
    dds_reliability_t reliability;
    uint32_t history_depth;
} dds_qos_t;

/** \brief DDS writer handle. */
typedef struct dds_writer
{
    const char *topic;
    size_t sample_size;
    const dds_qos_t *qos;
} dds_writer;

/** \brief DDS reader handle. */
typedef struct dds_reader
{
    const char *topic;
    size_t sample_size;
    const dds_qos_t *qos;
} dds_reader;

bool dds_init(uint32_t domain_id, const dds_qos_t *default_qos);

dds_writer *dds_writer_create(const char *topic, size_t sample_size, const dds_qos_t *qos);

dds_reader *dds_reader_create(const char *topic, size_t sample_size, const dds_qos_t *qos);

bool dds_write(dds_writer *writer, const void *sample, size_t size);

bool dds_read(dds_reader *reader, void *sample, size_t size);

void dds_shutdown(void);

#endif
