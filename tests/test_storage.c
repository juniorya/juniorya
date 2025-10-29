#include "test_suite.h"
#include "../storage/storage.h"
#include <assert.h>

void test_storage(void)
{
    storage_params_t params;
    storage_defaults(&params);
    assert(storage_init("test.kv"));
    assert(storage_save(&params));
    storage_params_t loaded;
    assert(storage_load(&loaded));
}
