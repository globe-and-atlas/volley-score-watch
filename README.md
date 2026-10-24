# Volley Score Watch

A button-first volleyball scorer for Pebble Time 2. It runs offline on the watch and saves completed matches by date. A calendar colors win and loss days; a places screen plots the coordinates of matches saved with a phone GPS fix.

Emery emulator captures: [score](docs/screenshots/score-emery.png), [match settings](docs/screenshots/settings-emery.png), [calendar](docs/screenshots/calendar-emery.png), [places](docs/screenshots/places-emery.png). Physical watch readability remains to be checked.

## Watch controls

- **Up / Down:** add a point to US / THEM.
- **Select:** open the menu; after a set, start the next set; after a match, save it.
- **Hold Select:** undo the last point, including a set-winning or match-winning point before saving.
- **Back:** return to the score or leave the app.
- **Menu:** undo the last point, inspect the calendar and match places, change future match rules, or start a new match.

The default is best of 3, regular sets to 25, deciding set to 15, always win by 2. The on-watch settings offer best of 3 or 5, regular sets to 15 or 25, and deciding sets to 15 or 25. Settings apply when a new match starts. The current match auto-saves after each point and survives app exit.

Completed matches save on the watch when Select is pressed. A phone GPS fix is requested at that moment if **Save GPS** is on. The result saves even when the phone is disconnected or location is denied. The match detail distinguishes coordinates from no GPS. Match Places is a relative point plot without street-map tiles.

The watch retains the latest **80 matches** in its 4 kB app storage allocation. At the limit, saving a new match asks whether to replace the oldest dated record. Uninstalling the app removes those records. This first version has no phone archive or export.

## Build and test

```bash
cc -std=c11 -Wall -Wextra -Werror -Iwatchapp/src/c tests/model_test.c watchapp/src/c/model.c -o .tmp/model_test
.tmp/model_test
python3 execution/build.py
```

The clean build copies the Emery PBW to `dist/volley-score-watch.pbw`. Install locally with `pebble install --emulator emery dist/volley-score-watch.pbw` from the `watchapp/` directory, using an absolute PBW path if needed.

The generated project scaffold used the `workflow-python` profile because the workspace template has no Pebble profile. The product code is native C and phone-side PebbleKit JS; Python only runs the deterministic build.
