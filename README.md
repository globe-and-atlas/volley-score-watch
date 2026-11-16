# VolleyKeeper

[Pebble appstore listing](https://apps.repebble.com/7c886ac0ac594f5a95755c8a) · Pebble Time 2

A volleyball scorer for Pebble Time 2 with touch and button controls. It runs offline on the watch and saves completed matches by date. A calendar colors win and loss days; a places screen plots the coordinates of matches saved with a phone GPS fix.

After each set, a Set Won or Set Lost screen shows the final point score and match tally. Select or tap its action bar to start the next set or save a completed match; hold Select to undo the deciding point before saving.

Emery emulator captures: [touch scoring](docs/screenshots/score-emery.png), [Set Won](docs/screenshots/set-won-emery.png), [Set Lost](docs/screenshots/set-lost-emery.png), [match formats](docs/screenshots/settings-emery.png), [touch setting](docs/screenshots/input-emery.png), [3-set final set](docs/screenshots/three-all-25-third-emery.png), [calendar](docs/screenshots/calendar-emery.png), [saved match](docs/screenshots/history-emery.png), [places](docs/screenshots/places-emery.png). Physical watch readability remains to be checked.

## Watch controls

- **Touch:** tap the US or THEM score panel to add one point and feel a short vibration. Tap a menu or settings row to activate it. Swipe horizontally in the calendar to change months or in saved match details to change records. Tap a marked calendar date to open its saved match. A score-panel swipe does not add points.
- **Touch switch:** Match Settings → Colors / Input → Touch. Touch starts on and can be turned off on the watch. A button press or wrist wake arms touch after the watch has been idle.
- **Up / Down:** add a point to US / THEM.
- **Select:** open the menu; after a set, start the next set; after a match, save it.
- **Hold Select:** undo the last point, including a set-winning or match-winning point before saving.
- **Back:** return to the score or leave the app.
- **Menu:** undo the last point, inspect the calendar and match places, change future match rules, or start a new match.

The default is a best-of-3 match with sets to 25, 25, and 15. Match Settings offers **3 sets: 25/25/15**, **3 sets: 25/25/25**, and **5 sets: 25/25/25/25/25**. A 3-set match ends at two set wins; a 5-set match ends at three. Custom rules allow best of 3 or 5, regular sets to 15 or 25, and deciding sets to 15 or 25. Every set requires a two-point lead. Rule changes apply when a new match starts.

Colors / Input shows a live color swatch beside each team choice. Select a team row to cycle its color; the swatch changes immediately. The defaults are **blue for US** and **red for THEM**. Color changes apply immediately to the score, calendar, saved match details, and places plot. Buttons work with touch either on or off. The current match auto-saves after each point and survives app exit.

Completed matches save on the watch when Select is pressed. A phone GPS fix is requested at that moment if **Save GPS** is on. The result saves even when the phone is disconnected or location is denied. The match detail distinguishes coordinates from no GPS. Match Places is a relative point plot without street-map tiles.

The watch retains the latest **80 matches** in its 4 kB app storage allocation. At the limit, saving a new match asks whether to replace the oldest dated record. Uninstalling the app removes those records. This first version has no phone archive or export.

## Build and test

```bash
cc -std=c11 -Wall -Wextra -Werror -Iwatchapp/src/c tests/model_test.c watchapp/src/c/model.c -o .tmp/model_test
.tmp/model_test
python3 execution/build.py
```

The clean build copies the Emery PBW to `dist/volleykeeper.pbw`. Install locally with `pebble install --emulator emery dist/volleykeeper.pbw` from the `watchapp/` directory, using an absolute PBW path if needed.

The generated project scaffold used the `workflow-python` profile because the workspace template has no Pebble profile. The product code is native C and phone-side PebbleKit JS; Python only runs the deterministic build.
