# 01 - Onboard LED

## Goal

Use the onboard LED to learn GPIO, non-blocking timing, and serial commands on
the Nano 33 BLE Sense Rev2, with only the board and USB cable.

## Sketch location

The developer-written sketch is `01_blink.ino`.

## Setup and verification

- Board package/version: pending
- Selected board: pending
- COM port: pending
- Compile result: successful, reported by developer on October 26, 2026
- Upload result: successful, reported by developer on October 26, 2026
- Expected behavior: toggle every 500 ms; `s` stops and forces OFF; `r` resumes
  with a fresh 500 ms interval. Serial processing continues while stopped.
- Observed behavior: developer reports working blink, non-blocking timing,
  serial response, and stop/resume after fixes on October 26, 2026.
- Review: saved source checked after the final brace and state fixes. No new
  automated build or independent hardware test was run by the assistant.
- Timing observation: Verify about 17 seconds; Upload about 20 seconds, as
  reported by developer. No build log was captured to attribute the time.

## What I learned

- GPIO output configuration and physical LED state.
- Blocking `delay()` versus elapsed-time checks using `millis()`.
- Persistent state, `bool`, and `unsigned long` timestamps.
- Unsigned current-minus-previous subtraction and resetting the time reference.
- Non-blocking serial availability checks and single-character commands.
- Keeping software state consistent with hardware output.

## Bugs and debugging

- Compile error from calling `digitalWrite()` at global scope: moved the call
  into `setup()` after pin initialization.
- Reversed elapsed-time subtraction and missing timestamp update: corrected to
  current minus previous and updated the reference after each toggle.
- Stop initially forced the output OFF without updating the state variable:
  synchronized both so resume starts from the expected OFF state.
- Misplaced brace attached `else` to the timer condition: corrected nesting so
  both toggle branches execute only after the interval expires.
- Arduino saved an extra folder and doubled `.ino` extension: corrected the
  path after closing the sketch in the IDE.
