#include "board/board.h"

board_drive_descriptor board_default_drive(void)
{
    board_drive_descriptor desc = {0x12345678u, 0x00000001u};
    return desc;
}
