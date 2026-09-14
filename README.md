# Embedded DevOps Codex Handoff

This folder is the handoff from a ChatGPT planning conversation into Codex.

Start here:

1. `AGENTS.md`
2. `PROJECT_CONTEXT.md`
3. `WHERE_WE_LEFT_OFF.md`
4. `HARDWARE_INVENTORY.md`
5. `PROJECT_IDEAS_AND_ROADMAP.md`

Then use the prompt in:

- `CODEX_START_PROMPT.md`

## Intended Repository Shape

As implementation begins, evolve toward:

```text
EmbeddedDevOps/
|
+-- AGENTS.md
+-- PROJECT_CONTEXT.md
+-- HARDWARE_INVENTORY.md
+-- WHERE_WE_LEFT_OFF.md
+-- PROJECT_IDEAS_AND_ROADMAP.md
+-- README.md
|
+-- firmware/
+-- gateway/
+-- server/
+-- infra/
+-- scripts/
+-- tests/
+-- docs/
```

Do not create empty complexity prematurely. Add folders when the relevant implementation begins.

## Firmware CI artifacts

GitHub Actions builds the Release firmware for every push to `main`, every pull
request, and every manual workflow run. A successful run publishes a directly
downloadable artifact named like:

```text
stm32-telemetry-20260914-153045-utc.zip
```

The timestamp is UTC. The package contains ELF, BIN, Intel HEX, linker map,
memory-usage, build-provenance, and SHA-256 checksum files.

### Flash the newest downloaded artifact on Windows

Connect the NUCLEO-F446RE by USB and download the workflow artifact into the
standard Windows Downloads folder. From the repository root, run exactly one
command:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\flash-firmware.ps1
```

The helper finds the newest `stm32-telemetry-*-utc.zip` in Downloads, extracts
it to a uniquely named temporary directory, verifies the Intel HEX SHA-256,
flashes it with STM32CubeProgrammer over ST-LINK/SWD, verifies the write, resets
the MCU, and removes only that temporary directory.

To flash a specific downloaded package instead, pass its absolute path:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\tools\flash-firmware.ps1 `
  -ArtifactPath 'C:\Users\Owner\Downloads\stm32-telemetry-YYYYMMDD-HHMMSS-utc.zip'
```
