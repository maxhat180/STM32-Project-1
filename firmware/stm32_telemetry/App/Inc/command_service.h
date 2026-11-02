#ifndef COMMAND_SERVICE_H
#define COMMAND_SERVICE_H

#include "command_framer.h"
#include "uart_rx_buffer.h"

typedef enum
{
    COMMAND_EVENT_NONE,
    COMMAND_EVENT_STATUS,
    COMMAND_EVENT_INVALID,
    COMMAND_EVENT_TOO_LONG,
    COMMAND_EVENT_RX_LOSS
} CommandEvent;

/* Shared queue/counters; frame and flags belong exclusively to the main loop. */
typedef struct
{
    UartRxBuffer rx;
    atomic_uint dropped;
    atomic_uint loss_pending;
    CommandFrame frame;
    bool saw_cr;
    bool malformed;
} CommandService;

/* service: storage to initialize before enabling reception. */
void command_service_init(CommandService *service);
/* service: shared state; byte: received value. Called only by the UART ISR. */
void command_service_receive(CommandService *service, uint8_t byte);
/* service: shared state. ISR reports damaged/missing data without parsing it. */
void command_service_rx_lost(CommandService *service);
/* service: shared state. Main loop processes at most 16 bytes or one event. */
CommandEvent command_service_poll(CommandService *service);

#endif /* COMMAND_SERVICE_H */
