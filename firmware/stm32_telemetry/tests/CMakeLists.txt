cmake_minimum_required(VERSION 3.22)

project(sensor_conversion_tests C)

enable_testing()

add_executable(sensor_conversion_tests
    test_sensor_conversion.c
    ../App/Src/sensor_conversion.c
)

target_include_directories(sensor_conversion_tests PRIVATE
    ../App/Inc
)

add_test(
    NAME sensor_conversion_tests
    COMMAND sensor_conversion_tests
)