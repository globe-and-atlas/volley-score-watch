---
generated_by: "OpenAI Codex (GPT-6; exact variant not exposed)"
timestamp: "2026-10-24T13:07:00-05:00"
---
# Pebble touch input

- Time 2 (`emery`) supports touch in watchapps through `TouchService`; watchfaces cannot subscribe. The user can disable touch system-wide. Source: [Pebble touch guide](https://developer.repebble.com/guides/events-and-services/touch/).
- `TouchEvent.non_navigational` is set for contact without an active interaction session. Ignore that gesture before awarding points; a button press or wrist wake can arm the session.
- A raw touch subscription powers the sensor while active. Subscribe only while the app window is visible and its touch preference is on; unsubscribe when either condition ends.
- The system touch bridge can map touches to buttons. This app handles raw coordinates and keeps the bridge disabled on its custom window, so one tap cannot also cause a bridged button click.
- The QEMU VNC framebuffer reports 208x228 while the app canvas is 200x228. Touch coordinates received by the app are scaled from that framebuffer; use broad tap regions and inspect actual emulator pixels.
