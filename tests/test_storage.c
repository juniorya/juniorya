#include "test_suite.h"

#include <assert.h>
#include <string.h>

#include "../storage/storage.h"

void test_storage(void)
{
    storage_t storage;
    storage_init(&storage);
    const uint8_t value[] = {1U, 2U, 3U};
    assert(storage_put(&storage, "key", value, sizeof(value)));
    assert(storage_save(&storage));
    uint8_t out[3];
    size_t len = sizeof(out);
    assert(storage_get(&storage, "key", out, &len));
    assert(memcmp(out, value, sizeof(value)) == 0);
    assert(storage_load(&storage));
}
