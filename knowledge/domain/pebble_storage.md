---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-10-24T12:22:00-05:00"
---
# Pebble storage and location constraints

- Pebble persistent storage allows 4 kB per app and a maximum of 256 bytes per key. Source: [Pebble persistent storage guide](https://developer.repebble.com/guides/events-and-services/persistent-storage/).
- A `MatchRecord` is 28 bytes in the host C layout. Eight records per persisted chunk use 224 bytes. Ten chunks retain 80 records plus the active match and metadata.
- PebbleKit JS can call `navigator.geolocation.getCurrentPosition` on the paired phone when the app declares the `location` capability. Source: [PebbleKit JS guide](https://developer.repebble.com/guides/communication/using-pebblekit-js/).
- Location permission may be denied. The result is committed before the GPS request; the record then carries `has_location = 0` until a valid response is written.
- Persistent storage is removed on uninstall. The current product does not have a phone archive or export.
- A saved record carries a unique match ID. On reload, the active match reconciles against the latest history record so a restart between history commit and active-state commit does not duplicate the result.
- At 80 records, replacement writes a 228-byte backup of the affected chunk before changing it. A generation byte in history metadata lets startup restore the old chunk after an interrupted rollover, or discard the backup after a committed rollover.
