#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "command_service.h"

/* service: receiver; text: NUL-terminated terminal input to enqueue without polling. */
static void send_text(CommandService *service, const char *text)
{
    while (*text != '\0')
    {
        command_service_receive(service, (uint8_t)*text++);
    }
}

/* service: receiver; expected: next event, allowing bounded polls to finish a line. */
static void expect_event(CommandService *service, CommandEvent expected)
{
    CommandEvent event = COMMAND_EVENT_NONE;
    for (unsigned int i = 0U; i < 8U && event == COMMAND_EVENT_NONE; ++i)
    {
        event = command_service_poll(service);
    }
    assert(event == expected);
}

/* No parameters: exercise real buffer -> framer -> parser integration on the host. */
int main(void)
{
    CommandService service;
    command_service_init(&service);
    assert(command_service_poll(&service) == COMMAND_EVENT_NONE);

    /* Fragmented commands wait for LF; CRLF and consecutive commands each work. */
    send_text(&service, "STA");
    assert(command_service_poll(&service) == COMMAND_EVENT_NONE);
    send_text(&service, "TUS\nSTATUS\r\nSTATUS\n");
    expect_event(&service, COMMAND_EVENT_STATUS);
    expect_event(&service, COMMAND_EVENT_STATUS);
    expect_event(&service, COMMAND_EVENT_STATUS);
    assert(command_service_poll(&service) == COMMAND_EVENT_NONE);

    /* Unknown, empty, suffixed, lowercase and embedded-CR lines are rejected. */
    send_text(&service, "status\n\nSTATUSX\nSTA\rTUS\nHELLO\nSTATUS\n");
    for (unsigned int i = 0U; i < 5U; ++i)
    {
        expect_event(&service, COMMAND_EVENT_INVALID);
    }
    expect_event(&service, COMMAND_EVENT_STATUS);
    send_text(&service, "STA");
    command_service_receive(&service, 0U);
    send_text(&service, "TUS\nSTATUS\n");
    expect_event(&service, COMMAND_EVENT_INVALID);
    expect_event(&service, COMMAND_EVENT_STATUS);

    /* Exactly 32 payload bytes fit the framer; 33 causes one overflow event. */
    for (unsigned int i = 0U; i < COMMAND_FRAME_CAPACITY; ++i)
    {
        command_service_receive(&service, 'A');
    }
    send_text(&service, "\nSTATUS\n");
    expect_event(&service, COMMAND_EVENT_INVALID);
    expect_event(&service, COMMAND_EVENT_STATUS);
    for (unsigned int i = 0U; i <= COMMAND_FRAME_CAPACITY; ++i)
    {
        command_service_receive(&service, 'A');
    }
    send_text(&service, "STATUS\nSTATUS\n");
    expect_event(&service, COMMAND_EVENT_TOO_LONG);
    expect_event(&service, COMMAND_EVENT_STATUS);
    assert(command_service_poll(&service) == COMMAND_EVENT_NONE);

    /* A full RX queue rejects bytes; it cannot dispatch any damaged queued command. */
    command_service_init(&service);
    send_text(&service, "STA");
    assert(command_service_poll(&service) == COMMAND_EVENT_NONE);
    for (unsigned int i = 0U; i < UART_RX_BUFFER_CAPACITY - 1U; ++i)
    {
        command_service_receive(&service, 'X');
    }
    assert(atomic_load(&service.dropped) == 0U);
    command_service_receive(&service, 'Y');
    command_service_receive(&service, '\n');
    assert(atomic_load(&service.dropped) == 2U);
    expect_event(&service, COMMAND_EVENT_RX_LOSS);
    assert(command_service_poll(&service) == COMMAND_EVENT_NONE);
    send_text(&service, "TUS\nSTATUS\nSTATUS\n");
    expect_event(&service, COMMAND_EVENT_STATUS);
    expect_event(&service, COMMAND_EVENT_STATUS);
    assert(command_service_poll(&service) == COMMAND_EVENT_NONE);

    /* UART faults use the same resynchronization path, without claiming a byte count. */
    send_text(&service, "STATUS\n");
    command_service_rx_lost(&service);
    expect_event(&service, COMMAND_EVENT_RX_LOSS);
    send_text(&service, "\nSTATUS\n");
    expect_event(&service, COMMAND_EVENT_STATUS);
    assert(atomic_load(&service.dropped) == 2U);

    /* Repeated complete commands wrap the queue; partial input has no response. */
    for (unsigned int i = 0U; i < 100U; ++i)
    {
        send_text(&service, "STATUS\n");
        expect_event(&service, COMMAND_EVENT_STATUS);
    }
    send_text(&service, "STATUS");
    assert(command_service_poll(&service) == COMMAND_EVENT_NONE);
    return 0;
}
