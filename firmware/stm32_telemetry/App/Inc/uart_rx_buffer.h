#ifndef UART_RX_BUFFER_H
#define UART_RX_BUFFER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>

/* One slot stays unused so equal indices always mean empty: 63 usable bytes. */
#define UART_RX_BUFFER_CAPACITY 64U

/* Interrupt writes head; main loop writes tail. Atomics publish shared data safely. */
typedef struct
{
    uint8_t data[UART_RX_BUFFER_CAPACITY];
    atomic_uint head;
    atomic_uint tail;
} UartRxBuffer;

/* buffer: storage to reset before reception starts; never reset while in use. */
void uart_rx_buffer_init(UartRxBuffer *buffer);

/* buffer: initialized queue; byte: incoming value. Producer only; false if full. */
bool uart_rx_buffer_push(UartRxBuffer *buffer, uint8_t byte);

/* buffer: initialized queue; byte: output destination. Consumer only; false if empty. */
bool uart_rx_buffer_pop(UartRxBuffer *buffer, uint8_t *byte);

/* buffer: initialized queue. Consumer only; drop currently queued bytes safely. */
void uart_rx_buffer_discard(UartRxBuffer *buffer);

#endif /* UART_RX_BUFFER_H */
