cmake_minimum_required(VERSION 3.22)

project(sensor_conversion_tests C)

enable_testing()

# The receive buffer uses C11 atomic indices for interrupt/main-loop sharing.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(sensor_conversion_tests
    test_sensor_conversion.c
    ../App/Src/sensor_conversion.c
)

add_executable(command_parser_tests
    test_command_parser.c
    ../App/Src/command_parser.c
)

add_executable(command_framer_tests
    test_command_framer.c
    ../App/Src/command_framer.c
)

target_include_directories(sensor_conversion_tests PRIVATE
    ../App/Inc
)

target_include_directories(command_parser_tests PRIVATE
    ../App/Inc
)

target_include_directories(command_framer_tests PRIVATE
    ../App/Inc
)

add_test(
    NAME sensor_conversion_tests
    COMMAND sensor_conversion_tests
)

add_test(
    NAME command_parser_tests
    COMMAND command_parser_tests
)

add_test(
    NAME command_framer_tests
    COMMAND command_framer_tests
)

# Build and register the HAL-independent receive-buffer behavior tests.
add_executable(uart_rx_buffer_tests
    test_uart_rx_buffer.c
    ../App/Src/uart_rx_buffer.c
)
target_include_directories(uart_rx_buffer_tests PRIVATE ../App/Inc)
add_test(NAME uart_rx_buffer_tests COMMAND uart_rx_buffer_tests)

# Test the real receive queue, framing recovery and STATUS dispatch together.
add_executable(command_service_tests
    test_command_service.c
    ../App/Src/command_service.c
    ../App/Src/uart_rx_buffer.c
    ../App/Src/command_framer.c
    ../App/Src/command_parser.c
)
target_include_directories(command_service_tests PRIVATE ../App/Inc)
add_test(NAME command_service_tests COMMAND command_service_tests)
