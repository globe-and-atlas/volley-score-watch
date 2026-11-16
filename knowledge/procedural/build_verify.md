---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-10-24T12:22:00-05:00"
---
# Build and verify

1. Run `cc -std=c11 -Wall -Wextra -Werror -Iwatchapp/src/c tests/model_test.c watchapp/src/c/model.c -o .tmp/model_test` from the project root.
2. Run `.tmp/model_test`.
3. Run `python3 execution/build.py`; it cleans first, builds Emery, and copies the PBW to `dist/volleykeeper.pbw`.
4. Install the absolute PBW path with `pebble install --emulator emery /Users/danielbally/Git/volley-score-watch/dist/volleykeeper.pbw` from `watchapp/`.
5. Capture exact display pixels with `pebble screenshot --no-open --no-correction /Users/danielbally/Git/volley-score-watch/.tmp/<name>.png`.
6. If a prior watchface remains visible, follow the stale emulator remedy in `knowledge/ERRORS.md` before interpreting screenshots.

## Touch replay on Emery

1. Use `pebble install --emulator emery --vnc <absolute-pbw-path>` for both the boot install and the second install. Omitting `--vnc` on the second call starts a different emulator without a VNC listener.
2. The VNC listener used port 5901 in this run. Query the actual listener before sending test input.
3. A minimal RFB client must advertise QEMU's pointer type change encoding (`-257`) before sending pointer events. Without it, taps reached the app as `(0,0)` rather than the requested coordinates.
4. A pointer down then up at a team panel was observed to add one point; a cross-panel swipe added none. Use `pebble screenshot --emulator emery --vnc --no-open --no-correction <path>` after each material interaction.
5. The emulator button control remains `pebble emu-button --emulator emery --vnc click <up|down|select|back>` for the fallback path.
