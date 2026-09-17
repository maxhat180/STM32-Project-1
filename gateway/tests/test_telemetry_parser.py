import json
import unittest

from gateway.telemetry_parser import (
    InvalidTelemetryError,
    MalformedJsonError,
    NonTelemetryLineError,
    NonTelemetryLineKind,
    TelemetryRecord,
    UnsupportedSchemaError,
    parse_telemetry_line,
)


VALID_RECORD = {
    "type": "telemetry",
    "schema": 1,
    "uptime_ms": 123456,
    "temp_raw": 866,
    "temp_mv": 697,
    "temp_x10_c": 197,
    "light_raw": 3691,
}


class TestTelemetryParser(unittest.TestCase):
    def test_accepts_schema_1_telemetry(self) -> None:
        result = parse_telemetry_line(json.dumps(VALID_RECORD))

        self.assertEqual(
            result,
            TelemetryRecord(
                type="telemetry",
                schema=1,
                uptime_ms=123456,
                temp_raw=866,
                temp_mv=697,
                temp_x10_c=197,
                light_raw=3691,
            ),
        )

    def test_rejects_malformed_json(self) -> None:
        with self.assertRaises(MalformedJsonError):
            parse_telemetry_line('{"type":"telemetry"')

    def test_rejects_missing_required_field(self) -> None:
        record = VALID_RECORD.copy()
        del record["light_raw"]

        with self.assertRaisesRegex(InvalidTelemetryError, "light_raw"):
            parse_telemetry_line(json.dumps(record))

    def test_rejects_unsupported_schema(self) -> None:
        record = VALID_RECORD | {"schema": 2}

        with self.assertRaises(UnsupportedSchemaError):
            parse_telemetry_line(json.dumps(record))

    def test_rejects_incorrect_field_types(self) -> None:
        invalid_values = {
            "type": 1,
            "schema": "1",
            "uptime_ms": "123456",
            "temp_raw": 866.0,
            "temp_mv": "697",
            "temp_x10_c": 19.7,
            "light_raw": None,
        }

        for field, value in invalid_values.items():
            with self.subTest(field=field, value=value):
                record = VALID_RECORD | {field: value}
                with self.assertRaises(InvalidTelemetryError):
                    parse_telemetry_line(json.dumps(record))

    def test_rejects_boolean_for_integer_field(self) -> None:
        for field in (
            "schema",
            "uptime_ms",
            "temp_raw",
            "temp_mv",
            "temp_x10_c",
            "light_raw",
        ):
            with self.subTest(field=field):
                record = VALID_RECORD | {field: True}
                with self.assertRaises(InvalidTelemetryError):
                    parse_telemetry_line(json.dumps(record))

    def test_preserves_integer_values_without_coercion(self) -> None:
        record = VALID_RECORD | {
            "uptime_ms": 4_294_967_295,
            "temp_raw": 4095,
            "temp_mv": 3300,
            "temp_x10_c": -100,
            "light_raw": 0,
        }

        result = parse_telemetry_line(json.dumps(record))

        self.assertEqual(result.uptime_ms, 4_294_967_295)
        self.assertEqual(result.temp_raw, 4095)
        self.assertEqual(result.temp_mv, 3300)
        self.assertEqual(result.temp_x10_c, -100)
        self.assertEqual(result.light_raw, 0)

    def test_rejects_non_object_json(self) -> None:
        with self.assertRaises(InvalidTelemetryError):
            parse_telemetry_line("[]")

    def test_classifies_existing_non_json_uart_lines(self) -> None:
        cases = {
            "": NonTelemetryLineKind.EMPTY,
            "STM32 telemetry smoke test ready": NonTelemetryLineKind.STARTUP,
            "LD2 is blinking; press blue B1 to test input.": (
                NonTelemetryLineKind.STARTUP
            ),
            "B1 pressed": NonTelemetryLineKind.BUTTON_EVENT,
            "B1 released": NonTelemetryLineKind.BUTTON_EVENT,
            "ADC error: start failed": NonTelemetryLineKind.DIAGNOSTIC,
            "ADC error: temperature poll failed": NonTelemetryLineKind.DIAGNOSTIC,
            "ADC error: light poll failed": NonTelemetryLineKind.DIAGNOSTIC,
            "ADC error: stop failed": NonTelemetryLineKind.DIAGNOSTIC,
            "unexpected text": NonTelemetryLineKind.OTHER_TEXT,
        }

        for line, expected_kind in cases.items():
            with self.subTest(line=line):
                with self.assertRaises(NonTelemetryLineError) as context:
                    parse_telemetry_line(line)
                self.assertIs(context.exception.kind, expected_kind)


if __name__ == "__main__":
    unittest.main()
