# Errors

Record deterministic errors, root causes, and fixes here.

## 2026-10-24 — Stale emulator app after successful install

- Symptom: `pebble install --emulator emery` returned success while the screenshot still showed Rainy Bookshop.
- Cause: the shared emulator retained the previous app/error screen.
- Fix: `pebble kill`, restart the emulator with `pebble install`, then press Back to dismiss the stale error screen and install the PBW again. Verify the app name and pixels before accepting screenshots.
- Graduated to: the workspace `_PEBBLE` skill already documents the general stale emulator behavior.

## 2026-10-24 — Calendar weekday shifted in Emery

- Symptom: an October 24, 2026 saved result appeared in the Friday column; October 24 is a Saturday.
- Cause: the app used Pebble's `mktime` result to position the first weekday, which produced a one-column shift in this emulator replay. The precise firmware cause was not isolated.
- Fix: calculate the first weekday with a deterministic Gregorian integer formula shared by calendar rendering and touch hit testing. Host tests cover October 2026 and leap-year dates.
- Graduated to: `watchapp/src/c/model.c` provides `weekday_of_month()` for future calendar features.
