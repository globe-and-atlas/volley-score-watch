---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-10-24T12:22:00-05:00"
---
# Build the Pebble watchapp

## Goal

Produce a clean Emery PBW after the scoring model passes its host tests.

## Inputs

- `watchapp/package.json`
- `watchapp/src/c/`
- `watchapp/src/pkjs/`

## Execution

1. Run the host C test command in `knowledge/procedural/build_verify.md`.
2. Run `python3 execution/build.py` from the project root.
3. Confirm `dist/volley-score-watch.pbw` exists.

## Edge cases

- A successful SDK build does not prove the watch UI is usable. Install in Emery and inspect native-resolution screenshots.
- The shared emulator can show a stale prior app after install; follow `knowledge/ERRORS.md` before accepting a screenshot.
- Phone GPS requires a paired phone runtime and permission. Emulator GPS does not prove physical iPhone behavior.

## Output

`dist/volley-score-watch.pbw`, kept out of Git by `.gitignore`.
