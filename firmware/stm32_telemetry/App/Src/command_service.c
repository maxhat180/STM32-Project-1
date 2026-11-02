#include "command_service.h"
#include "command_parser.h"

/* service: valid storage; startup initialization occurs before either caller runs. */
void command_service_init(CommandService *service)
{
    uart_rx_buffer_init(&service->rx);
    atomic_init(&service->dropped, 0U);
    atomic_init(&service->loss_pending, 0U);
    command_frame_init(&service->frame);
    service->saw_cr = false;
    service->malformed = false;
}

void command_service_receive(CommandService *service, uint8_t byte)
{
    /* Reject new bytes when full and publish the break in the input stream. */
    if (!uart_rx_buffer_push(&service->rx, byte))
    {
        atomic_fetch_add_explicit(&service->dropped, 1U, memory_order_relaxed);
        command_service_rx_lost(service);
    }
}

void command_service_rx_lost(CommandService *service)
{
    atomic_store_explicit(&service->loss_pending, 1U, memory_order_release);
}

CommandEvent command_service_poll(CommandService *service)
{
    /* Bound each visit so a busy terminal cannot indefinitely occupy the loop. */
    for (unsigned int i = 0U; i < 16U; ++i)
    {
        if (atomic_exchange_explicit(&service->loss_pending, 0U,
                                     memory_order_acq_rel) != 0U)
        {
            /* Discard queued data, then wait for a fresh newline to resynchronize. */
            uart_rx_buffer_discard(&service->rx);
            command_frame_init(&service->frame);
            service->frame.discarding = true;
            service->saw_cr = false;
            service->malformed = false;
            return COMMAND_EVENT_RX_LOSS;
        }

        uint8_t byte;
        if (!uart_rx_buffer_pop(&service->rx, &byte))
        {
            return COMMAND_EVENT_NONE;
        }

        /* Framer owns discard-until-newline recovery after a damaged/long line. */
        if (service->frame.discarding)
        {
            (void)command_frame_push(&service->frame, (char)byte);
            continue;
        }

        /* Accept LF or CRLF; reject embedded CR and non-printable command bytes. */
        if ((service->saw_cr && byte != '\n') ||
            (byte != '\r' && byte != '\n' && (byte < 32U || byte > 126U)))
        {
            service->malformed = true;
        }
        service->saw_cr = (byte == '\r');
        const CommandFrameResult result = command_frame_push(&service->frame, (char)byte);
        if (result == COMMAND_FRAME_OVERFLOW)
        {
            service->saw_cr = false;
            service->malformed = false;
            return COMMAND_EVENT_TOO_LONG;
        }
        if (result == COMMAND_FRAME_READY)
        {
            const CommandType command = service->malformed ? COMMAND_INVALID :
                command_parse(service->frame.buffer, service->frame.length);
            command_frame_init(&service->frame);
            service->saw_cr = false;
            service->malformed = false;
            return command == COMMAND_STATUS ? COMMAND_EVENT_STATUS : COMMAND_EVENT_INVALID;
        }
    }
    return COMMAND_EVENT_NONE;
}
