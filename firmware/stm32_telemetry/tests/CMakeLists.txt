cmake_minimum_required(VERSION 3.22)

project(sensor_conversion_tests C)

enable_testing()

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
