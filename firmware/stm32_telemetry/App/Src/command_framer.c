#include "command_framer.h"

void command_frame_init(CommandFrame *frame)
{
    if (frame == NULL)
    {
        return;
    }
    frame->length = 0U;
    frame->discarding = false;
}

CommandFrameResult command_frame_push(CommandFrame *frame, char byte)
{
    if (byte == '\n')
    {
        return COMMAND_FRAME_READY;
    }
    if (byte == '\r')
    {
        return COMMAND_FRAME_IN_PROGRESS;
    }
    frame->buffer[frame->length] = byte;
    frame->length += 1U;
    return COMMAND_FRAME_IN_PROGRESS;
}
