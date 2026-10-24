---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-10-24T12:11:30-05:00"
---
# Volley Score Watch

## Objective

Build an offline Pebble Time 2 volleyball scorer with configurable match rules and a dated record of wins and losses.

## Contract

- Target directive: `directives/build_watchapp.md` for repeatable clean builds.
- Intended execution script: `execution/build.py` for clean production builds.
- Output artifact: `dist/volley-score-watch.pbw`.
- Safety: never read or commit `.env` or `.lock-waf*`.
- Safety: do not publish to the app store without a separate instruction.

## Validation Contract

- Up adds one point to US.
- Down adds one point to THEM.
- Undo restores the previous point state.
- A set ends only when its target is reached with a two point lead.
- A match ends when one side wins the required number of sets.
- Match setup allows best of 3.
- Match setup allows best of 5.
- Match setup allows a 15 point regular set target.
- Match setup allows a 25 point regular set target.
- Match setup allows a 15 point deciding set target.
- Match setup allows a 25 point deciding set target.
- The active match survives app exit.
- Saving a completed match records its calendar date.
- Saving a completed match succeeds when phone location is unavailable.
- A phone location fix can attach latitude and longitude to a saved match.
- Match history distinguishes records with coordinates from records without coordinates.
- Saving at full history capacity requires explicit replacement confirmation.
- The calendar distinguishes days with wins from days with losses.
- The calendar shows a monthly win and loss count.
- A places view plots saved GPS results by location.
- The Pebble SDK build succeeds for Emery.
- Host tests cover scoring boundaries and match completion.
- A fresh verifier reviews the implementation against this contract.

## Checklist

- [x] Create project from workspace template.
- [x] Implement rules and persistence.
- [x] Implement live score and calendar screens.
- [x] Run host tests.
- [x] Build PBW.
- [x] Inspect emulator UI.
- [x] Run fresh verification.

## Progress

- 2026-10-24 12:11 CDT: Project scaffold created; rules and display contract defined.
- 2026-10-24 12:23 CDT: Model tests pass; clean Emery PBW built; emulator match save, calendar result, GPS result, and active-match resume inspected.
- 2026-10-24 12:42 CDT: Fresh verifier found save and undo edge cases; fixes landed and independent reinspection found no remaining concrete local defect. Physical watch/iPhone behavior remains a release gate.
