#include <stdio.h>
#include <stdbool.h>

extern bool test_planner_basic(void);
extern bool test_kinematics_roundtrip(void);
extern bool test_storage_cycle(void);
extern bool test_selftest_sequence(void);

int main(void)
{
    bool ok = true;
    bool planner = test_planner_basic();
    bool kine = test_kinematics_roundtrip();
    bool storage = test_storage_cycle();
    bool integration = test_selftest_sequence();
    ok = planner && kine && storage && integration;
    printf("planner=%d kinematics=%d storage=%d integration=%d\n",
           planner ? 1 : 0,
           kine ? 1 : 0,
           storage ? 1 : 0,
           integration ? 1 : 0);
    printf("Tests %s\n", ok ? "passed" : "failed");
    return ok ? 0 : 1;
}
