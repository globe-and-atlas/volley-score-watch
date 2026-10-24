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
- Provide 25/25/15, JV 25/25/25, and Varsity five sets to 25 as next-match presets. Preserve custom point targets for other leagues.
- Default US to blue and THEM to red. Keep team colors distinct so calendar wins and losses remain identifiable; apply color preference to historical result views as a display choice.
