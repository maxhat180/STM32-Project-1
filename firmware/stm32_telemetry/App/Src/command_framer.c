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
    if(frame->discarding)
    {
        if (byte == '\n')
        {
            command_frame_init(frame);
        }
        return COMMAND_FRAME_IN_PROGRESS;
    }

    if (byte =='\n')
    {
        return COMMAND_FRAME_READY;
    }

    if (byte == '\r')
    {
        return COMMAND_FRAME_IN_PROGRESS;
    }

    if (frame->length >= COMMAND_FRAME_CAPACITY)
    {
        frame->discarding = true;
        return COMMAND_FRAME_OVERFLOW;
    }

    frame->buffer[frame->length] = byte;
    frame->length += 1U;
    return COMMAND_FRAME_IN_PROGRESS;
}
