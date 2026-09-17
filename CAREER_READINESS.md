# Solutions Engineer / Field Applications Engineer Readiness

## Target Role

The target is a customer-facing technical role in which the engineer can discover
a customer's requirements, select an embedded/IoT architecture, build or adapt a
prototype, debug it with the customer, and explain the technical and commercial
tradeoffs clearly.

The strongest positioning for this project is not "firmware developer who can
present." It is:

> Solutions engineer who can take a connected-device opportunity from customer
> requirements through working embedded hardware, firmware, telemetry, and a
> credible production path.

## Evidence Scale

- **Proven**: implemented and verified with authoritative evidence.
- **In progress**: implementation has begun but its exit criteria are incomplete.
- **Planned**: required for the target role but no project evidence exists yet.

## Current Readiness Matrix

| Capability | Why it matters in the role | Current evidence | Status | Next proof required |
|---|---|---|---|---|
| Customer discovery and presentation | Converts a vague problem into a compelling solution | Prior professional experience plus `CUSTOMER_SOLUTION_BRIEF.md`, which separates assumed needs from verified requirements | In progress | Use the discovery questions to refine the brief, then deliver a five-minute demo using measured results |
| Embedded C fundamentals | Required to understand, modify, and debug customer firmware | STM32 HAL application, integer conversion module, native C tests, explicit ADC lifecycle handling, and bounded JSON serialization | Proven at an introductory project level | Add stateful UART receive, interrupts, and watchdog behavior |
| GPIO and board bring-up | First-line FAE work often begins with power, reset, pins, and observable signals | LD2, active-low B1, debounce, ST-LINK flashing, UART smoke test | Proven | Produce a repeatable bring-up checklist with expected measurements and failure hypotheses |
| Analog acquisition | Connects real sensors to an MCU and exposes reference, range, and noise tradeoffs | Two-channel ADC with TMP36 and photoresistor; physical stimulus tests | Proven at a basic level | Quantify accuracy, reference-voltage error, sampling tradeoffs, and filtering |
| Robust peripheral handling | Customer prototypes must fail diagnosably rather than silently | ADC start, rank-specific poll, and stop diagnostics preserve cleanup; clean builds and normal hardware operation verified | Proven at a basic level | Separate acquisition control from HAL calls so error paths can be injected and unit-tested safely |
| UART protocol design | Enables configuration, diagnostics, gateways, and manufacturing fixtures | Versioned newline-delimited JSON telemetry plus button and diagnostic output | In progress | Validate schema 1 on the host, then add framed receive commands, timeouts, malformed-input recovery, and tests |
| I2C | Common sensor/configuration bus and frequent customer-debugging topic | No implemented driver | Planned | Bring up LIS3DH over I2C; explain addressing, register access, pull-ups, ACK/NACK, and bus diagnosis |
| SPI | Common high-throughput peripheral bus with board-level timing concerns | No implemented driver | Planned | Drive a real peripheral over SPI; verify chip select, CPOL/CPHA, transactions, and failure diagnosis |
| Firmware architecture and testability | Demonstrates maintainable application/peripheral boundaries | HAL-independent sensor conversion module with host tests and a versioned telemetry contract | Proven at a basic level | Add host schema tests, then separate acquisition, domain data, serialization, and transport interfaces |
| Debugging method | Core FAE differentiator during customer escalations | Physical stimulus checks, serial observations, build/flash verification | In progress | Record expectation, observation, hypotheses, isolation experiment, root cause, and fix for real faults |
| Build and release engineering | Makes examples and customer deliverables reproducible | GitHub Actions cross-build, tests, checksums, packaged artifacts, verified flashing | Proven | Add static analysis, size limits, tagged releases, provenance/SBOM, and signing |
| IoT and gateway integration | Connects the embedded design to business-visible data | Architecture documented only | Planned | Build serial-to-MQTT gateway, local broker, schema, reconnect behavior, timestamps, and observability |
| Backend and fleet observability | Supports demos, pilots, and post-deployment diagnosis | Existing DevOps background; no project implementation | Planned | Ingest telemetry into PostgreSQL and show device health and measurements in a dashboard |
| Schematic and component selection | Required for custom solutions and design reviews | Sensor wiring and voltage-safety decisions only | Planned | Write requirements, select MCU/sensor/power/protection parts, and justify each choice |
| PCB layout and design for test | Turns a prototype into reviewable custom hardware | No custom PCB yet | Planned | Complete schematic/ERC, layout/DRC, SWD and test points, fabrication outputs, and review checklist |
| Power, signal integrity, and EMC awareness | Prevents common prototype and field failures | No quantified project evidence | Planned | Build power budget; design decoupling/protection; review return paths, interfaces, and EMC risks |
| RF/wireless architecture | Important for connected-product feasibility and customer expectation setting | Replaceable-transport constraint documented | Planned | Compare gateway, Bluetooth, Wi-Fi, and LTE against range, power, cost, certification, and Israeli sourcing |
| Production and commercial tradeoffs | Distinguishes an FAE/solutions engineer from a hobbyist | No formal project evidence | Planned | Produce BOM cost, sourcing risks, test strategy, certification risks, and prototype-to-production plan |

## Portfolio System and Learning Sequence

Each stage must produce four things: working engineering evidence, a concise design
decision, a repeatable test, and a customer-facing explanation.

1. **Harden the existing STM32 sensor node.** Finish ADC diagnostics, then define
   structured telemetry. Explain what a customer can diagnose from each field.
2. **Make the device configurable.** Add a small framed UART command protocol with
   malformed-message recovery and host tests. Demonstrate changing reporting
   behavior without reflashing firmware.
3. **Connect it locally.** Build a Python serial-to-MQTT gateway and local
   containerized broker. Demonstrate disconnect/reconnect and timestamp strategy.
4. **Add digital buses.** Use the LIS3DH for I2C first and SPI later where
   practical. Capture and explain a real bus-debugging case.
5. **Build the customer-visible system.** Add ingestion, PostgreSQL, dashboards,
   device health, and observability before moving the proven stack to Hetzner.
6. **Design the custom PCB.** Start from written requirements and a block diagram;
   then perform component selection, schematic review, layout, manufacturing-file
   checks, and a documented bring-up plan.
7. **Add production credibility.** Include HIL tests, watchdog/fault handling,
   static analysis, firmware-size gates, signed releases, BOM/sourcing risk, and a
   prototype-to-production assessment.

## Required Customer-Facing Artifacts

By the end of the project, the portfolio should include:

- a one-page customer problem and requirements brief (`CUSTOMER_SOLUTION_BRIEF.md`
  now provides the first project-specific version);
- a system block diagram and interface specification;
- a decision record comparing at least three architectures;
- a short schematic/PCB design-review package;
- a bring-up and troubleshooting report with measurements;
- a five-minute live demo with a failure-and-recovery scenario;
- a concise deployment/security model;
- a production-readiness page covering BOM, sourcing, test, certification, and
  remaining risks.

These artifacts provide interview evidence that the user can do more than build a
prototype: they can guide a customer from requirements through a defensible
technical solution.

## Immediate Technical Gate: Host Telemetry Validation

The next increment is complete only when a host-side parser:

- accepts a representative schema-1 telemetry record;
- rejects malformed JSON;
- rejects a missing required field;
- rejects an unsupported schema version;
- rejects incorrect field types, including JSON booleans where integers are
  required;
- preserves the raw MCU uptime and sensor values without silently coercing them.

The validated parser will become the input boundary for the serial-to-MQTT gateway.
