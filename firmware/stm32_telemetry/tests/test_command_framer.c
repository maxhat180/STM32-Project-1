#include <assert.h>

#include "command_framer.h"

int main(void)
{
    CommandFrame frame;
    frame.length = 5U;
    frame.discarding = true;
    command_frame_init(&frame);
    assert(frame.length == 0U);
    assert(frame.discarding == false);
    command_frame_init(NULL);  // Should not crash
    command_frame_init(&frame);
    const CommandFrameResult result = command_frame_push(&frame, 'S');
    assert(result == COMMAND_FRAME_IN_PROGRESS);
    assert(frame.length == 1U);
    assert(frame.buffer[0] == 'S');
    assert(frame.discarding == false);

    const CommandFrameResult newline_result = command_frame_push(&frame, '\n');
    assert(newline_result == COMMAND_FRAME_READY);
    assert(frame.length == 1U);
    assert(frame.buffer[0] == 'S');
    assert(frame.discarding == false);

    command_frame_init(&frame);
    command_frame_push(&frame, 'S');
    const CommandFrameResult carriage_return_result = command_frame_push(&frame, '\r');
    assert(carriage_return_result == COMMAND_FRAME_IN_PROGRESS);
    assert(frame.length == 1U);
    assert(frame.buffer[0] == 'S');
    const CommandFrameResult newline_return_result = command_frame_push(&frame, '\n');
    assert(newline_return_result == COMMAND_FRAME_READY);
    assert(frame.length == 1U);

    command_frame_init(&frame);
    size_t i;
    for (i = 0; i < COMMAND_FRAME_CAPACITY; ++i) {
        const CommandFrameResult result = command_frame_push(&frame, 'A');
        assert(result == COMMAND_FRAME_IN_PROGRESS);
        assert(frame.buffer[i] == 'A');
        assert(frame.length == i + 1U);
    }
    assert(frame.length == COMMAND_FRAME_CAPACITY);
    const CommandFrameResult overflow_result = command_frame_push(&frame, 'B');
    assert(overflow_result == COMMAND_FRAME_OVERFLOW);
    assert(frame.length == COMMAND_FRAME_CAPACITY);
    assert(frame.discarding == true);
    return 0;
}
