#include "uart_rx_buffer.h"

#include <stddef.h>

/* Interrupt use requires atomics that cannot wait for a software lock. */
_Static_assert(ATOMIC_INT_LOCK_FREE == 2, "UART buffer requires lock-free indices");
_Static_assert(UART_RX_BUFFER_CAPACITY >= 2U, "UART buffer needs an empty slot");

void uart_rx_buffer_discard(UartRxBuffer *buffer)
{
    /* Snapshot head without resetting it; the interrupt remains its only writer. */
    const unsigned int head = atomic_load_explicit(&buffer->head, memory_order_acquire);
    atomic_store_explicit(&buffer->tail, head, memory_order_release);
}

void uart_rx_buffer_init(UartRxBuffer *buffer)
{
    if (buffer == NULL)
    {
        return;
    }

    /* Empty indices make old array contents irrelevant. */
    atomic_init(&buffer->head, 0U);
    atomic_init(&buffer->tail, 0U);
}

bool uart_rx_buffer_push(UartRxBuffer *buffer, uint8_t byte)
{
    if (buffer == NULL)
    {
        return false;
    }

    /* Wrap at the array end; reject new data rather than overwrite unread data. */
    const unsigned int head = atomic_load_explicit(&buffer->head, memory_order_relaxed);
    const unsigned int next_head = (head + 1U) % UART_RX_BUFFER_CAPACITY;
    if (next_head == atomic_load_explicit(&buffer->tail, memory_order_acquire))
    {
        return false;
    }

    /* Publish the new head only after the byte is stored. */
    buffer->data[head] = byte;
    atomic_store_explicit(&buffer->head, next_head, memory_order_release);
    return true;
}

bool uart_rx_buffer_pop(UartRxBuffer *buffer, uint8_t *byte)
{
    if ((buffer == NULL) || (byte == NULL))
    {
        return false;
    }

    /* Acquire makes bytes published by the producer visible before reading. */
    const unsigned int tail = atomic_load_explicit(&buffer->tail, memory_order_relaxed);
    if (tail == atomic_load_explicit(&buffer->head, memory_order_acquire))
    {
        return false;
    }

    /* Release the slot only after copying its byte to the caller. */
    *byte = buffer->data[tail];
    atomic_store_explicit(&buffer->tail,
                          (tail + 1U) % UART_RX_BUFFER_CAPACITY,
                          memory_order_release);
    return true;
}
