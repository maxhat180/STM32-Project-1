# Customer Solution Brief: Connected Environmental Monitor

## Opportunity

A small operator needs visibility into the environmental conditions around
temperature-sensitive assets. Today, conditions are checked manually, so excursions
may be discovered late and there is no trustworthy history for troubleshooting.

The proposed pilot is a connected sensor node that reports temperature, ambient
light, device health, and firmware identity to a dashboard. The pilot is intended
to prove sensing, connectivity, operability, and deployment assumptions before a
custom PCB or wireless transport is selected.

This scenario is a portfolio design assumption, not a statement of requirements
from a real customer. A real engagement begins with the discovery questions below.

## Business Outcome

The pilot should let an operator:

- see recent conditions without visiting the asset;
- identify temperature excursions and loss of device connectivity;
- distinguish a sensor/device fault from an actual environmental change;
- review enough history to investigate when and how an excursion occurred;
- evaluate whether a custom production device is commercially justified.

## Proposed Pilot Architecture

```text
TMP36 + light sensor
        |
        v
NUCLEO-F446RE firmware
        |
        | structured UART over USB
        v
Laptop gateway
        |
        | MQTT with gateway timestamp
        v
Broker -> ingestion service -> PostgreSQL -> dashboard/alerts
```

USB/UART is deliberate for the pilot: it isolates the sensing, firmware, telemetry,
and backend risks before wireless cost, power, coverage, certification, and sourcing
are introduced. The firmware transport boundary should permit later Bluetooth,
Wi-Fi, or LTE evaluation without rewriting the sensor application.

## Pilot Requirements

| ID | Requirement | Verification |
|---|---|---|
| P-01 | Sample temperature and light at a configurable interval | Change the interval through a UART command and observe timestamps |
| P-02 | Publish structured, versioned telemetry | Validate every message against the documented schema |
| P-03 | Report device identity, firmware version, uptime, and sensor status | Observe all fields after boot and during normal operation |
| P-04 | Recover from malformed commands without rebooting | Send invalid frames followed by a valid command |
| P-05 | Detect gateway/device disconnects | Remove and restore USB; dashboard shows offline and recovery states |
| P-06 | Preserve measurements in a queryable history | Query stored data across a controlled warm/light-change experiment |
| P-07 | Make firmware builds traceable and reproducible | Map the running version to a tested CI artifact and source revision |
| P-08 | Avoid silent peripheral failures | Exercise testable error paths and verify distinct diagnostics |

## Current Proof

The project currently demonstrates:

- STM32 GPIO, UART transmit, and a two-channel ADC sequence;
- physical TMP36 temperature response and photoresistor light response;
- explicit ADC lifecycle diagnostics with successful-start cleanup preserved;
- physically verified, versioned newline-delimited JSON sensor telemetry;
- HAL-independent sensor conversion logic with native host tests;
- CI cross-compilation and checksummed deployment packages;
- verified programming of a CI-built artifact through ST-LINK/SWD.

This proves the basic sensor-to-firmware and build-to-device paths. It does not yet
prove configurable telemetry, gateway recovery, MQTT transport, persistent storage,
alerting, wireless operation, measurement accuracy, or production suitability.

## Engineering Decisions Still Open

| Decision | Options to compare | Selection criteria |
|---|---|---|
| Temperature sensor | Analog TMP36, digital I2C sensor, other calibrated part | Accuracy, calibration, cost, availability, board area, operating range |
| Device transport | USB gateway, Bluetooth, Wi-Fi, LTE | Installation model, coverage, power, recurring cost, certification, Israeli sourcing |
| Power source | USB, regulated DC input, battery | Availability, runtime, safety, service interval, enclosure constraints |
| Timestamp authority | Gateway clock, RTC, network-synchronized device clock | Offline behavior, accuracy, cost, complexity, audit requirements |
| Telemetry protocol | MQTT, HTTPS | Fleet scale, reliability, firewall constraints, backend integration |
| Production MCU | Retain STM32F446 class or right-size | Peripheral needs, memory, security, lifecycle, unit cost, tooling |

## Risks and Honest Limitations

- The current conversion assumes a nominal 3.3 V ADC reference; it is not yet a
  calibrated temperature instrument.
- TMP36 accuracy and total measurement error must be budgeted before committing to
  excursion limits.
- The laptop gateway is appropriate for a pilot but may not match the customer's
  installation or reliability requirements.
- Wireless transport selection can materially change power, antenna, enclosure,
  certification, carrier, and recurring-cost assumptions.
- RF-capable component sourcing and Israeli import/regulatory constraints must be
  checked before final part selection.
- A first custom PCB requires power protection, ESD, debug/test access, DFM, and a
  documented bring-up plan; a working breadboard does not prove production readiness.

## Customer Discovery Questions

1. What asset or process is being protected, and what is the cost of a missed
   excursion?
2. What temperature range, accuracy, resolution, and response time are actually
   required?
3. How often must readings arrive, and how long may the system be offline?
4. Where will devices operate: indoors, outdoors, refrigerated, mobile, or near
   electrical noise and moisture?
5. What power and network access exist at each installation?
6. How many pilot and production units are expected, and at what target unit cost?
7. Who consumes the data, and what alert, API, dashboard, or audit workflow is
   required?
8. What security, data residency, retention, and regulatory requirements apply?
9. How will devices be installed, identified, serviced, calibrated, and replaced?
10. What would make the pilot an unambiguous technical and commercial success?

## Demo Story

1. Show the device identity, firmware provenance, and live baseline readings.
2. Cover the light sensor and warm the temperature sensor; show the expected trends.
3. Send a malformed command, then a valid command, proving recovery without reboot.
4. Disconnect and reconnect the gateway; show offline detection and resumed data.
5. Trace one dashboard measurement back through storage, MQTT, gateway input, and
   the exact firmware artifact.
6. Close with the open decisions for a custom PCB and explain how customer answers
   determine power, sensor, transport, enclosure, and cost choices.

## Pilot Exit Criteria

The pilot is successful when all P-series requirements pass repeatably, the customer
accepts the dashboard and alert workflow, measurement error is understood relative
to the use case, and enough installation evidence exists to choose a production
power and communications architecture. Custom-PCB work should begin from those
validated requirements rather than from the development board schematic alone.
