---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-10-24T12:11:30-05:00"
---
# VolleyKeeper

## Objective

Build an offline Pebble Time 2 volleyball scorer with configurable match rules and a dated record of wins and losses.

## Contract

- Target directive: `directives/build_watchapp.md` for repeatable clean builds.
- Intended execution script: `execution/build.py` for clean production builds.
- Output artifact: `dist/volleykeeper.pbw`.
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

## 2026-10-24 Match format presets

### Validation Contract

- A 3-set preset uses 25 points for sets one and two.
- The 3-set preset uses 15 points for set three.
- The 3-set 25/25/25 preset uses 25 points for set three.
- The 5-set 25/25/25/25/25 preset uses 25 points for set five.
- A 3-set match ends at two set wins.
- A 5-set match ends at three set wins.
- Custom point targets remain editable.
- A preset selection applies to the next match.
- US defaults to blue when no color preference is saved.
- THEM defaults to red when no color preference is saved.
- The settings allow changing the US color.
- The settings allow changing the THEM color.
- The settings prevent both teams using the same color.
- Saved match colors follow the current team color preferences.
- Calendar colors follow the current team color preferences.
- Places plot colors follow the current team color preferences.
- `make build` succeeds.
- The Emery match-format menu is legible at native resolution.
- A fresh verifier checks the final rules and selection flow.

### Checklist

- [x] Add preset rules and tests.
- [x] Add match-format menu.
- [x] Add team-color settings.
- [x] Build and inspect Emery screen.
- [x] Run fresh verification.

### Result

- 2026-10-24: Host model tests and clean Emery PBW build passed.
- 2026-10-24: Emery replay showed the 3-set 25/25/25 final set targeting 25 points; format, color, score, and calendar screens were inspected at native resolution.
- 2026-10-24: Independent read-only verifier found no concrete defect in the preset and color changes.

## 2026-10-24 Touch controls

### Validation Contract

- Touch controls default to on when no preference exists.
- A saved off preference survives app restart.
- The input settings allow touch controls to be turned off.
- The input settings allow touch controls to be turned on.
- An off preference prevents touch scoring.
- An off preference prevents touch menu navigation.
- Up adds a US point with touch off.
- Down adds a THEM point with touch off.
- Select navigates with touch off.
- Back navigates with touch off.
- A tap inside the US score panel adds one US point during play.
- A tap inside the THEM score panel adds one THEM point during play.
- A scoring tap produces one vibration pulse.
- A swipe across the score panels adds no point.
- An unarmed touch adds no point.
- Tapping a list row activates that row.
- A horizontal calendar swipe changes the displayed month.
- Tapping a date with saved matches opens a saved match from that date.
- The October 2026 calendar places October 24 in the Saturday column.
- A horizontal history swipe changes the displayed record when another exists.
- The Emery PBW builds successfully.
- An Emery emulator replay exercises touch scoring.
- An Emery emulator replay exercises the off setting.
- A fresh verifier reviews the touch path.

### Checklist

- [x] Implement persisted touch setting.
- [x] Implement touch scoring and navigation.
- [x] Run host tests and build Emery PBW.
- [x] Replay touch and button paths in Emery.
- [x] Run fresh verification.

### Result

- 2026-10-24: `make build` passed host tests and created the Emery PBW.
- 2026-10-24: Emulator taps scored US and THEM; a cross-panel swipe scored neither.
- 2026-10-24: Touch off blocked tap input after restart; Up still added a point.
- 2026-10-24: Touch menu taps, calendar swipes, date taps, and history swipes were replayed in Emery.
- 2026-10-24: A fresh verifier found two issues; both were fixed and re-reviewed with no remaining concrete defect.

## 2026-10-24 Neutral match format names

### Validation Contract

- Match Settings lists a 3-set 25/25/15 preset.
- Match Settings lists a 3-set 25/25/25 preset.
- Match Settings lists a 5-set 25/25/25/25/25 preset.
- Match Settings contains no school-level labels.
- Preset selection retains the existing match rules.
- `make build` succeeds.

### Checklist

- [x] Rename the preset items.
- [x] Update related source names and documentation.
- [x] Build the Emery PBW.

### Result

- 2026-10-24: Host model tests and clean Emery PBW build passed.
- 2026-10-24: The Emery match-settings screen displayed all three format labels without clipping.

## 2026-10-24 VolleyKeeper store release

### Validation Contract

- The PBW metadata names the watchapp VolleyKeeper.
- The PBW retains UUID `476a8033-c6b6-45a8-9e0c-f5f94f02b809`.
- The PBW targets Emery.
- `make build` passes.
- The store package includes five 200×228 Emery screenshots.
- The store listing uses the name VolleyKeeper.
- The store listing describes saved matches accurately.
- A fresh verifier checks the release package before submission.
- The submitted release has a public appstore link.

### Checklist

- [x] Update watchapp identity and build artifact.
- [x] Prepare listing text, icons, and screenshots.
- [x] Build and inspect the PBW.
- [x] Run fresh release verification.
- [x] Publish to the Pebble appstore.
- [x] Verify the public listing.

### Result

- 2026-10-24: Published VolleyKeeper 0.1.0 at https://apps.repebble.com/7c886ac0ac594f5a95755c8a.
- 2026-10-24: The public page displays the PBW download and five Emery screenshots.
- 2026-10-24: Physical watch and paired iPhone GPS validation remain open.

## 2026-10-24 Set result graphics

### Validation Contract

- A set won by US displays `SET WON`.
- A set won by THEM displays `SET LOST`.
- A set result displays the final US point total.
- A set result displays the final THEM point total.
- A set result displays the updated set tally.
- A set result uses the current US color.
- A set result uses the current THEM color.
- A nonfinal set result offers Next Set.
- A final set result offers Save Match.
- Tapping a set-result action with touch on performs its displayed action.
- Holding Select on an unsaved set result restores the previous point state.
- Scoring input adds no point while a set result is displayed.
- The Emery emulator shows both result variants at 200×228.
- `make build` passes.
- A fresh verifier checks the result flow.
- The appstore serves version 0.1.1.

### Checklist

- [x] Draw the set-result screen.
- [x] Verify both outcomes and Undo in Emery.
- [x] Build the 0.1.1 PBW.
- [x] Run fresh verification.
- [x] Publish the updated release.
- [x] Verify the public listing.

### Result

- 2026-10-24: Emery showed Set Won at 25–23 and Set Lost at 23–25.
- 2026-10-24: Holding Select restored a completed set to its prior live score.
- 2026-10-24: Tapping Next Set advanced to the following set.
- 2026-10-24: Saving the deciding match retained the GPS status message.
- 2026-10-24: `make build` passed and a fresh verifier found no concrete release blocker.
- 2026-10-24: The public appstore page serves VolleyKeeper 0.1.1 with five screenshots.

## 2026-10-24 Set result visual revision

### Validation Contract

- The result screen uses a light background.
- The result screen centers the final point score as one focal element.
- The court has visible perspective sidelines.
- The net has visible end posts.
- The net remains distinct from the court floor at 200×228.
- The screen retains the Set Won or Set Lost label.
- The screen retains the updated set tally.
- The screen retains the context-appropriate Select action.
- The screen retains the Hold Select Undo cue before saving.
- A 25–23 Set Won screenshot is legible in Emery.
- A 23–25 Set Lost screenshot is legible in Emery.
- `make build` passes.
- A fresh verifier checks the revised screens.
- The appstore serves version 0.1.2.

### Checklist

- [x] Redraw the result screen and court.
- [x] Inspect both result variants at 200×228.
- [x] Verify Next Set, Undo, and Save Match still work.
- [x] Build the 0.1.2 PBW.
- [x] Run fresh verification.
- [x] Publish the revised release.
- [x] Verify the public listing.

### Result

- 2026-10-24: Emery showed the revised Set Won screen at 25–23.
- 2026-10-24: Emery showed the revised Set Lost screen at 23–25.
- 2026-10-24: Hold Select restored the prior point state.
- 2026-10-24: Touch Next Set advanced to the deciding set.
- 2026-10-24: Save Match retained the GPS status message.
- 2026-10-24: `make build` passed and a fresh visual verifier found no release blocker.
- 2026-10-24: The refreshed public listing showed VolleyKeeper 0.1.2 with five screenshots.
