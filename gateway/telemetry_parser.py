"""Validation boundary for newline-delimited STM32 telemetry."""

from __future__ import annotations

from dataclasses import dataclass
from enum import Enum
import json


@dataclass(frozen=True, slots=True)
class TelemetryRecord:
    """A validated schema-1 telemetry sample."""

    type: str
    schema: int
    uptime_ms: int
    temp_raw: int
    temp_mv: int
    temp_x10_c: int
    light_raw: int


class NonTelemetryLineKind(Enum):
    """Known categories of text sharing the firmware UART stream."""

    EMPTY = "empty"
    STARTUP = "startup"
    BUTTON_EVENT = "button_event"
    DIAGNOSTIC = "diagnostic"
    OTHER_TEXT = "other_text"


class TelemetryParseError(ValueError):
    """Base class for rejected host-boundary input."""


class MalformedJsonError(TelemetryParseError):
    """Raised when a JSON-looking line cannot be decoded."""


class InvalidTelemetryError(TelemetryParseError):
    """Raised when decoded JSON does not match the telemetry contract."""


class UnsupportedSchemaError(TelemetryParseError):
    """Raised when telemetry uses a schema version this gateway cannot read."""


class NonTelemetryLineError(TelemetryParseError):
    """Raised for a classified, non-JSON line from the shared UART stream."""

    def __init__(self, kind: NonTelemetryLineKind, line: str) -> None:
        self.kind = kind
        self.line = line
        super().__init__(f"non-telemetry UART line ({kind.value}): {line!r}")


_REQUIRED_FIELDS = {
    "type",
    "schema",
    "uptime_ms",
    "temp_raw",
    "temp_mv",
    "temp_x10_c",
    "light_raw",
}

_INTEGER_FIELDS = (
    "schema",
    "uptime_ms",
    "temp_raw",
    "temp_mv",
    "temp_x10_c",
    "light_raw",
)

_STARTUP_LINES = {
    "STM32 telemetry smoke test ready",
    "LD2 is blinking; press blue B1 to test input.",
}

_BUTTON_LINES = {"B1 pressed", "B1 released"}


def classify_non_telemetry_line(line: str) -> NonTelemetryLineKind:
    """Classify text currently emitted alongside JSON by the firmware."""

    stripped = line.strip()
    if not stripped:
        return NonTelemetryLineKind.EMPTY
    if stripped in _STARTUP_LINES:
        return NonTelemetryLineKind.STARTUP
    if stripped in _BUTTON_LINES:
        return NonTelemetryLineKind.BUTTON_EVENT
    if stripped.startswith("ADC error:"):
        return NonTelemetryLineKind.DIAGNOSTIC
    return NonTelemetryLineKind.OTHER_TEXT


def parse_telemetry_line(line: str) -> TelemetryRecord:
    """Decode and validate one UART line without coercing field values."""

    stripped = line.strip()

    try:
        decoded = json.loads(stripped)
    except json.JSONDecodeError as error:
        if stripped.startswith(("{", "[")):
            raise MalformedJsonError("malformed JSON telemetry") from error
        kind = classify_non_telemetry_line(line)
        raise NonTelemetryLineError(kind, stripped) from error

    if type(decoded) is not dict:
        raise InvalidTelemetryError("telemetry JSON must be an object")

    missing_fields = sorted(_REQUIRED_FIELDS - decoded.keys())
    if missing_fields:
        raise InvalidTelemetryError(
            f"missing required telemetry fields: {', '.join(missing_fields)}"
        )

    if type(decoded["type"]) is not str:
        raise InvalidTelemetryError("telemetry field 'type' must be a string")
    if decoded["type"] != "telemetry":
        raise InvalidTelemetryError("unsupported telemetry record type")

    for field in _INTEGER_FIELDS:
        # Exact type comparison matters because bool is a subclass of int in Python.
        if type(decoded[field]) is not int:
            raise InvalidTelemetryError(
                f"telemetry field {field!r} must be an integer"
            )

    if decoded["schema"] != 1:
        raise UnsupportedSchemaError(
            f"unsupported telemetry schema: {decoded['schema']}"
        )

    return TelemetryRecord(
        type=decoded["type"],
        schema=decoded["schema"],
        uptime_ms=decoded["uptime_ms"],
        temp_raw=decoded["temp_raw"],
        temp_mv=decoded["temp_mv"],
        temp_x10_c=decoded["temp_x10_c"],
        light_raw=decoded["light_raw"],
    )
