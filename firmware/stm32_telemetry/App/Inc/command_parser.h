#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <stddef.h>
typedef enum
{
    COMMAND_INVALID = 0,
    COMMAND_STATUS
} CommandType;

CommandType command_parse(const char *frame, size_t length);

#endif /* COMMAND_PARSER_H */
