#ifndef COMMAND_FRAMER_H
#define COMMAND_FRAMER_H

#include <stddef.h>
#include <stdbool.h>

#define COMMAND_FRAME_CAPACITY 32U

typedef enum
{
    COMMAND_FRAME_IN_PROGRESS,
    COMMAND_FRAME_READY,
    COMMAND_FRAME_OVERFLOW
} CommandFrameResult;

typedef struct
{
    char buffer[COMMAND_FRAME_CAPACITY];
    size_t length;
    bool discarding;
} CommandFrame;

void command_frame_init(CommandFrame *frame);

CommandFrameResult command_frame_push(CommandFrame *frame, char byte);

#endif /* COMMAND_FRAMER_H */