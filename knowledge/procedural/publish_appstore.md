---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-10-24T21:26:01-05:00"
---
# Publish VolleyKeeper to the Pebble appstore

## First release used on 2026-10-24

1. Run `make build` and inspect `dist/volleykeeper.pbw` for the VolleyKeeper name, UUID, version, and Emery platform.
2. Use `docs/store/listing.md` for the description, category, release notes, and screenshot order.
3. Copy five native 200×228 screenshots to `.tmp/store-submission/` with `emery_` filename prefixes. The CLI requires this prefix for local screenshot files.
4. Run `pebble publish --non-interactive --is-published` from `watchapp/` with the name, description, category key `health`, release notes, two icon paths, and five screenshot paths. The account must already be signed in with `pebble login`.
5. Open the returned app page and verify the public title, version, PBW download, category, and five screenshots.

The installed CLI defaults `--is-published` to false. Include it when the release should be publicly installable. A new app created through this CLI starts as Listed. An existing app uses its UUID for lookup and requires a new version for each release. For an existing app with five screenshots already present, use `--replace-screenshots` alongside five new `emery_` files to update the gallery; the CLI cannot undo this replacement.

## Current release

- App: https://apps.repebble.com/7c886ac0ac594f5a95755c8a
- Version: 0.1.2
- UUID: `476a8033-c6b6-45a8-9e0c-f5f94f02b809`
- Category: Health & Fitness
- Platform: Emery
- The 0.1.2 gallery contains the live score, revised Set Won, revised Set Lost, match formats, and calendar.

The public release has not yet been validated on a physical Time 2 with a paired iPhone.
