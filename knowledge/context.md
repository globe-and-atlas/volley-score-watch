# Project Context

## Identity

- Name: `volley-score-watch`
- Profile: `workflow-python`
- Deploy: `local-only`
- Runtime: `pebble-sdk`

## Purpose

Let a volleyball player or scorekeeper record points without handling a phone, then see match wins and losses on a dated calendar. Capture the paired phone's coordinates when a match is saved so results can be plotted by place.

## Constraints

- Pebble Time 2 / Emery first.
- The watch has no GPS; PebbleKit JS requests phone location only after a completed match is saved.
- Results are stored locally in Pebble persistent storage, which has a 4 kB per-app allocation.
- A record without a GPS fix remains a valid saved result.
- The watch keeps a rolling window of 80 match records.

## Out of Scope

- A hosted map with street tiles.
- Cloud backup or account synchronization.
- Official referee rotation and substitution tracking.
