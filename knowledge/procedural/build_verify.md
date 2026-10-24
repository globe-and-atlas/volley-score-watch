---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-10-24T12:22:00-05:00"
---
# Build and verify

1. Run `cc -std=c11 -Wall -Wextra -Werror -Iwatchapp/src/c tests/model_test.c watchapp/src/c/model.c -o .tmp/model_test` from the project root.
2. Run `.tmp/model_test`.
3. Run `python3 execution/build.py`; it cleans first, builds Emery, and copies the PBW to `dist/volley-score-watch.pbw`.
4. Install the absolute PBW path with `pebble install --emulator emery /Users/danielbally/Git/volley-score-watch/dist/volley-score-watch.pbw` from `watchapp/`.
5. Capture exact display pixels with `pebble screenshot --no-open --no-correction /Users/danielbally/Git/volley-score-watch/.tmp/<name>.png`.
6. If a prior watchface remains visible, follow the stale emulator remedy in `knowledge/ERRORS.md` before interpreting screenshots.
