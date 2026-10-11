#include "command_parser.h"
#include <string.h>

CommandType command_parse(const char *frame, size_t length)
{
    if ((frame == NULL) || (length == 0U))
    {
        return COMMAND_INVALID;
    }
    if ((length == 6U) && (memcmp(frame, "STATUS", 6U) == 0))
    {
        return COMMAND_STATUS;
    }
    else
    {
        return COMMAND_INVALID;
    }
}
