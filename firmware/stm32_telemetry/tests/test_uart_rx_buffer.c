#include <assert.h>
#include <stddef.h>

#include "uart_rx_buffer.h"

/* No parameters: exercise queue behavior without HAL or a connected board. */
int main(void)
{
    UartRxBuffer buffer;
    uint8_t byte = 0xAAU;
    uart_rx_buffer_init(&buffer);

    /* Empty and invalid calls must not alter the output or consume data. */
    assert(!uart_rx_buffer_pop(&buffer, &byte));
    assert(byte == 0xAAU);
    uart_rx_buffer_init(NULL);
    assert(!uart_rx_buffer_push(NULL, 'S'));
    assert(!uart_rx_buffer_pop(NULL, &byte));
    assert(uart_rx_buffer_push(&buffer, 'S'));
    assert(!uart_rx_buffer_pop(&buffer, NULL));
    assert(uart_rx_buffer_pop(&buffer, &byte));
    assert(byte == 'S');

    /* Fill to the usable limit; a rejected byte must not overwrite older bytes. */
    for (unsigned int i = 0U; i < UART_RX_BUFFER_CAPACITY - 1U; ++i)
    {
        assert(uart_rx_buffer_push(&buffer, (uint8_t)i));
    }
    assert(!uart_rx_buffer_push(&buffer, 0xFFU));

    /* Free part of the queue, then refill across the array boundary. */
    for (unsigned int i = 0U; i < 20U; ++i)
    {
        assert(uart_rx_buffer_pop(&buffer, &byte));
        assert(byte == (uint8_t)i);
    }
    for (unsigned int i = 0U; i < 20U; ++i)
    {
        assert(uart_rx_buffer_push(&buffer, (uint8_t)(100U + i)));
    }
    assert(!uart_rx_buffer_push(&buffer, 0xFFU));

    /* Original bytes precede the newer bytes, including after wrapping. */
    for (unsigned int i = 20U; i < UART_RX_BUFFER_CAPACITY - 1U; ++i)
    {
        assert(uart_rx_buffer_pop(&buffer, &byte));
        assert(byte == (uint8_t)i);
    }
    for (unsigned int i = 0U; i < 20U; ++i)
    {
        assert(uart_rx_buffer_pop(&buffer, &byte));
        assert(byte == (uint8_t)(100U + i));
    }
    assert(!uart_rx_buffer_pop(&buffer, &byte));

    /* Repeated reuse preserves every possible byte value, including zero. */
    for (unsigned int i = 0U; i < 1024U; ++i)
    {
        assert(uart_rx_buffer_push(&buffer, (uint8_t)i));
        assert(uart_rx_buffer_pop(&buffer, &byte));
        assert(byte == (uint8_t)i);
    }

    /* Reset is supported when neither producer nor consumer is active. */
    assert(uart_rx_buffer_push(&buffer, 'X'));
    uart_rx_buffer_init(&buffer);
    assert(!uart_rx_buffer_pop(&buffer, &byte));
    return 0;
}
