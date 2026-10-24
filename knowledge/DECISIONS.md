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
