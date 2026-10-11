#include <assert.h>

#include "command_parser.h"

int main(void)
{
    assert(command_parse(NULL, 0U) == COMMAND_INVALID);
    assert(command_parse("STATUS", 6U) == COMMAND_STATUS);
    assert(command_parse("status", 6U) == COMMAND_INVALID);
    assert(command_parse("STATUS\n", 7U) == COMMAND_INVALID);
    assert(command_parse("STAT", 4U) == COMMAND_INVALID);
    assert(command_parse("", 0U) == COMMAND_INVALID);
    assert(command_parse(NULL, 6U) == COMMAND_INVALID);

    return 0;
}
