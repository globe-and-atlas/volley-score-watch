# Errors

Record deterministic errors, root causes, and fixes here.

## 2026-10-24 — Stale emulator app after successful install

- Symptom: `pebble install --emulator emery` returned success while the screenshot still showed Rainy Bookshop.
- Cause: the shared emulator retained the previous app/error screen.
- Fix: `pebble kill`, restart the emulator with `pebble install`, then press Back to dismiss the stale error screen and install the PBW again. Verify the app name and pixels before accepting screenshots.
- Graduated to: the workspace `_PEBBLE` skill already documents the general stale emulator behavior.
