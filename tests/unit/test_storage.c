#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "storage/storage.h"

bool test_storage_cycle(void)
{
    storage_options opts;
    storage_default_options(&opts);
    opts.path = "test_store.bin";
    storage_init(&opts);
    uint32_t value = 0x12345678u;
    if (!storage_write("example", &value, sizeof(value)))
    {
        return false;
    }
    uint32_t readback = 0u;
    if (!storage_read("example", &readback, sizeof(readback)))
    {
        return false;
    }
    storage_shutdown();
    return value == readback;
}
