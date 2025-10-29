#include "test_suite.h"
#include "../gcode/parser.h"
#include <assert.h>

void test_console(void)
{
    gcode_command_t cmd;
    assert(gcode_parse_line("G1 X1 Y2 Z3 F100", &cmd));
}
