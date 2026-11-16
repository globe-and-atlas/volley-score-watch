# Decisions

## Initial Decision

- Template profile: `workflow-python`
- Deploy target: `local-only`
- Runtime: `pebble-sdk`

## 2026-10-24 — On-watch scoring and history

- Use a native C Emery watchapp so scoring works without a phone connection.
- Keep active score and a rolling 80-record history in watch persistent storage. The alternative of a phone-only archive would make the calendar unavailable offline. Ask before replacing the oldest record when full.
- Apply edited rules only when starting a new match, so changing settings cannot reinterpret an active score.
- Ask the paired phone for one GPS fix only after the user saves a completed match. Save the result first so denied or unavailable location cannot discard it.
- Show coordinate pins in a relative on-watch plot. A hosted street map requires a separate companion surface and remains a later product decision.

## 2026-10-24 — Match presets and team colors

- Treat the user's 3-set formats as best of 3; the match ends at two set wins.
- Provide three neutral next-match presets: 3 sets to 25/25/15, 3 sets to 25/25/25, and 5 sets to 25/25/25/25/25. Preserve custom point targets for other leagues.
- Default US to blue and THEM to red. Keep team colors distinct so calendar wins and losses remain identifiable; apply color preference to historical result views as a display choice.

## 2026-10-24 — Touch input

- Enable touch by default in this Emery watchapp, with a persisted on-watch switch. Keep the physical button handlers active regardless of that switch.
- Accept score taps only inside the two large team panels during an active set. Ignore unarmed contacts and score-panel swipes to reduce accidental points. Give successful touch points a vibration tick.
- Use touch for direct menu selection and calendar/history browsing. Subscribe to the touch service only while the watchapp window is visible and the app's touch preference is on.

## 2026-10-24 — Set result screens

- Draw Set Won and Set Lost from the persisted match state rather than adding a separate result record. The final point totals identify the winning team while the existing phase distinguishes a completed set from a completed match.
- Use native Pebble shapes for the court and team-color accents. This keeps the graphic aligned with color preferences without new image resources.
- Route a tap on the result action bar through the existing Select handler so touch and button paths share Next Set, Save Match, and menu behavior.
- The initial boxed black mesh looked like a fence at 200×228. Use a light field, centered score, perspective court lines, navy posts, and a sparse lighter net to preserve the approved concept's hierarchy at watch scale.
