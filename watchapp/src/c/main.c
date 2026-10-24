#include <pebble.h>
#include <stdio.h>
#include <string.h>

#include "model.h"

#define KEY_MATCH 1
#define KEY_RULES 2
#define KEY_HISTORY_META 3
#define KEY_LOCATION_ENABLED 5
#define KEY_ROLLOVER_BACKUP 6
#define KEY_US_COLOR 7
#define KEY_THEM_COLOR 8
#define KEY_RECORD_BASE 100
#define RECORD_CAPACITY 80
#define RECORDS_PER_CHUNK 8
#define RECORD_CHUNKS (RECORD_CAPACITY / RECORDS_PER_CHUNK)
#define TEAM_COLOR_COUNT 6

typedef struct {
  uint8_t version;
  uint8_t count;
  uint8_t next;
  uint8_t generation;
} HistoryMeta;

typedef struct {
  uint8_t chunk;
  uint8_t next_before;
  uint8_t generation_before;
  uint8_t reserved;
  MatchRecord records[RECORDS_PER_CHUNK];
} RolloverBackup;

typedef enum {
  SCREEN_SCORE,
  SCREEN_MENU,
  SCREEN_FORMATS,
  SCREEN_SETTINGS,
  SCREEN_COLORS,
  SCREEN_CONFIRM_NEW,
  SCREEN_CONFIRM_REPLACE,
  SCREEN_CALENDAR,
  SCREEN_HISTORY,
  SCREEN_PLACES
} Screen;

static Window *s_window;
static Layer *s_canvas;
static Match s_match;
static Rules s_defaults;
static uint8_t s_color_choice[2];
static MatchRecord s_records[RECORD_CAPACITY];
static uint8_t s_count;
static uint8_t s_next;
static uint8_t s_generation;
static bool s_location_enabled;
static Screen s_screen;
static uint8_t s_cursor;
static int16_t s_month_offset;
static uint8_t s_history_cursor;
static char s_notice[28];

static void redraw(void) { layer_mark_dirty(s_canvas); }

static void notice(const char *message) {
  snprintf(s_notice, sizeof(s_notice), "%s", message);
  redraw();
}

static bool save_match_state(void) {
  return persist_write_data(KEY_MATCH, &s_match, sizeof(s_match)) == sizeof(s_match);
}

static void save_defaults(void) {
  persist_write_data(KEY_RULES, &s_defaults, sizeof(s_defaults));
  persist_write_bool(KEY_LOCATION_ENABLED, s_location_enabled);
}

static void save_colors(void) {
  persist_write_int(KEY_US_COLOR, s_color_choice[0]);
  persist_write_int(KEY_THEM_COLOR, s_color_choice[1]);
  persist_write_bool(KEY_LOCATION_ENABLED, s_location_enabled);
}

static MatchRecord *record_at(uint8_t chronological_index) {
  if (chronological_index >= s_count) return NULL;
  uint8_t slot = (s_next + RECORD_CAPACITY - s_count + chronological_index) % RECORD_CAPACITY;
  return &s_records[slot];
}

static bool write_record_slot(uint8_t slot) {
  uint8_t chunk = slot / RECORDS_PER_CHUNK;
  int written = persist_write_data(KEY_RECORD_BASE + chunk,
                                   &s_records[chunk * RECORDS_PER_CHUNK],
                                   RECORDS_PER_CHUNK * sizeof(MatchRecord));
  return written == (int)(RECORDS_PER_CHUNK * sizeof(MatchRecord));
}

static void load_data(void) {
  s_defaults = rules_default();
  if (persist_exists(KEY_RULES)) {
    Rules loaded;
    if (persist_read_data(KEY_RULES, &loaded, sizeof(loaded)) == sizeof(loaded) &&
        rules_valid(loaded)) s_defaults = loaded;
  }
  s_location_enabled = !persist_exists(KEY_LOCATION_ENABLED) ||
                       persist_read_bool(KEY_LOCATION_ENABLED);
  s_color_choice[0] = 0;  // blue
  s_color_choice[1] = 1;  // red
  if (persist_exists(KEY_US_COLOR)) {
    int color = persist_read_int(KEY_US_COLOR);
    if (color >= 0 && color < TEAM_COLOR_COUNT) s_color_choice[0] = color;
  }
  if (persist_exists(KEY_THEM_COLOR)) {
    int color = persist_read_int(KEY_THEM_COLOR);
    if (color >= 0 && color < TEAM_COLOR_COUNT) s_color_choice[1] = color;
  }
  if (s_color_choice[0] == s_color_choice[1])
    s_color_choice[1] = s_color_choice[0] == 1 ? 0 : 1;
  match_start(&s_match, s_defaults);
  if (persist_exists(KEY_MATCH)) {
    Match loaded;
    if (persist_read_data(KEY_MATCH, &loaded, sizeof(loaded)) == sizeof(loaded) &&
        loaded.version == 2 && rules_valid(loaded.rules) &&
        loaded.score.phase <= PHASE_MATCH_WON) s_match = loaded;
  }
  if (s_match.match_id == 0) s_match.match_id = 1;
  HistoryMeta meta;
  if (persist_exists(KEY_HISTORY_META) &&
      persist_get_size(KEY_HISTORY_META) == sizeof(meta) &&
      persist_read_data(KEY_HISTORY_META, &meta, sizeof(meta)) == sizeof(meta) &&
      meta.version == 1 && meta.count <= RECORD_CAPACITY && meta.next < RECORD_CAPACITY) {
    s_count = meta.count;
    s_next = meta.next;
    s_generation = meta.generation;
  }
  for (uint8_t chunk = 0; chunk < RECORD_CHUNKS; chunk++) {
    if (persist_exists(KEY_RECORD_BASE + chunk)) {
      persist_read_data(KEY_RECORD_BASE + chunk,
                        &s_records[chunk * RECORDS_PER_CHUNK],
                        RECORDS_PER_CHUNK * sizeof(MatchRecord));
    }
  }
  if (persist_exists(KEY_ROLLOVER_BACKUP) &&
      persist_get_size(KEY_ROLLOVER_BACKUP) == sizeof(RolloverBackup)) {
    RolloverBackup backup;
    if (persist_read_data(KEY_ROLLOVER_BACKUP, &backup, sizeof(backup)) == sizeof(backup)) {
      if (backup.chunk < RECORD_CHUNKS && s_count == RECORD_CAPACITY &&
          s_next == backup.next_before && s_generation == backup.generation_before) {
        memcpy(&s_records[backup.chunk * RECORDS_PER_CHUNK], backup.records,
               sizeof(backup.records));
        if (write_record_slot(backup.chunk * RECORDS_PER_CHUNK))
          persist_delete(KEY_ROLLOVER_BACKUP);
      } else persist_delete(KEY_ROLLOVER_BACKUP);
    }
  }
  if (s_count && s_match.score.phase == PHASE_MATCH_WON && !s_match.saved) {
    MatchRecord *latest = record_at(s_count - 1);
    if (latest && latest->match_id == s_match.match_id) {
      s_match.saved = 1;
      save_match_state();
    }
  }
}

static void request_location(uint8_t slot) {
  if (!s_location_enabled) return;
  DictionaryIterator *out;
  if (app_message_outbox_begin(&out) != APP_MSG_OK) {
    notice("Saved without GPS");
    return;
  }
  dict_write_uint8(out, MESSAGE_KEY_LOC_REQUEST, 1);
  dict_write_uint8(out, MESSAGE_KEY_LOC_SLOT, slot);
  if (app_message_outbox_send() != APP_MSG_OK) notice("Saved without GPS");
}

static void save_finished_match(void) {
  // Commit the winning score before history. If the final saved flag later fails,
  // startup can reconcile this match ID against the latest history record.
  if (!save_match_state()) {
    notice("Save failed; retry");
    return;
  }
  time_t now = time(NULL);
  struct tm *today = localtime(&now);
  MatchRecord record;
  if (!today || !match_record(&s_match, today->tm_year + 1900,
                              today->tm_mon + 1, today->tm_mday, &record)) {
    notice("Cannot save match");
    return;
  }
  uint8_t slot = s_next;
  MatchRecord old = s_records[slot];
  bool replacing = s_count == RECORD_CAPACITY;
  if (replacing) {
    RolloverBackup backup = {.chunk = slot / RECORDS_PER_CHUNK,
                             .next_before = s_next,
                             .generation_before = s_generation};
    memcpy(backup.records, &s_records[backup.chunk * RECORDS_PER_CHUNK],
           sizeof(backup.records));
    if (persist_write_data(KEY_ROLLOVER_BACKUP, &backup, sizeof(backup)) != sizeof(backup)) {
      notice("Save failed; retry");
      return;
    }
  }
  s_records[slot] = record;
  if (!write_record_slot(slot)) {
    s_records[slot] = old;
    if (replacing && write_record_slot(slot)) persist_delete(KEY_ROLLOVER_BACKUP);
    notice("Storage full");
    return;
  }
  uint8_t next = (s_next + 1) % RECORD_CAPACITY;
  uint8_t count = s_count < RECORD_CAPACITY ? s_count + 1 : s_count;
  HistoryMeta meta = {.version = 1, .count = count, .next = next,
                      .generation = (uint8_t)(s_generation + 1)};
  if (persist_write_data(KEY_HISTORY_META, &meta, sizeof(meta)) != sizeof(meta)) {
    s_records[slot] = old;
    if (write_record_slot(slot) && replacing) persist_delete(KEY_ROLLOVER_BACKUP);
    notice("Save failed; retry");
    return;
  }
  s_next = next;
  s_count = count;
  s_generation = meta.generation;
  if (replacing) persist_delete(KEY_ROLLOVER_BACKUP);
  s_match.saved = 1;
  bool state_written = save_match_state();
  // If this write fails, startup reconciles the already committed result by ID.
  notice(!state_written ? "Saved; sync pending" :
         (s_location_enabled ? "Saved; getting GPS" : "Match saved"));
  request_location(slot);
}

static void inbox_received(DictionaryIterator *iter, void *context) {
  Tuple *slot_tuple = dict_find(iter, MESSAGE_KEY_LOC_SLOT);
  Tuple *status = dict_find(iter, MESSAGE_KEY_LOC_STATUS);
  if (!slot_tuple || !status) return;
  uint8_t slot = slot_tuple->value->uint8;
  if (slot >= RECORD_CAPACITY || !s_match.saved ||
      slot != (uint8_t)((s_next + RECORD_CAPACITY - 1) % RECORD_CAPACITY)) return;
  if (status->value->uint8 != 1) {
    notice("Saved without GPS");
    return;
  }
  Tuple *lat = dict_find(iter, MESSAGE_KEY_LOC_LAT_E6);
  Tuple *lon = dict_find(iter, MESSAGE_KEY_LOC_LON_E6);
  Tuple *accuracy = dict_find(iter, MESSAGE_KEY_LOC_ACCURACY_M);
  if (!lat || !lon || !accuracy) return;
  int32_t latitude = lat->value->int32;
  int32_t longitude = lon->value->int32;
  if (latitude < -90000000 || latitude > 90000000 ||
      longitude < -180000000 || longitude > 180000000) return;
  MatchRecord previous = s_records[slot];
  s_records[slot].latitude_e6 = latitude;
  s_records[slot].longitude_e6 = longitude;
  s_records[slot].accuracy_m = accuracy->value->uint16;
  s_records[slot].has_location = 1;
  if (write_record_slot(slot)) notice("Saved with GPS");
  else {
    s_records[slot] = previous;
    notice("Saved without GPS");
  }
}

static void fill(GContext *ctx, GColor color, GRect rect) {
  graphics_context_set_fill_color(ctx, color);
  graphics_fill_rect(ctx, rect, 0, GCornerNone);
}

static GColor team_color(uint8_t team) {
  switch (s_color_choice[team]) {
    case 0: return GColorBlue;
    case 1: return GColorRed;
    case 2: return GColorFromRGB(30, 79, 100);
    case 3: return GColorFromRGB(205, 111, 91);
    case 4: return GColorPurple;
    default: return GColorDarkGreen;
  }
}

static const char *team_color_name(uint8_t team) {
  static const char *names[] = {"BLUE", "RED", "TEAL", "CORAL", "PURPLE", "GREEN"};
  return names[s_color_choice[team]];
}

static void cycle_team_color(uint8_t team) {
  do {
    s_color_choice[team] = (s_color_choice[team] + 1) % TEAM_COLOR_COUNT;
  } while (s_color_choice[team] == s_color_choice[1 - team]);
}

static void label(GContext *ctx, const char *value, const char *font,
                  GColor color, GRect rect, GTextAlignment alignment) {
  graphics_context_set_text_color(ctx, color);
  graphics_draw_text(ctx, value, fonts_get_system_font(font), rect,
                     GTextOverflowModeTrailingEllipsis, alignment, NULL);
}

static void score_screen(GContext *ctx) {
  char text[40];
  fill(ctx, GColorWhite, GRect(0, 0, 200, 228));
  int sets_played = s_match.score.sets[0] + s_match.score.sets[1];
  int shown_set = sets_played + (s_match.score.phase == PHASE_PLAYING ? 1 : 0);
  snprintf(text, sizeof(text), "SET %d / %d", shown_set, s_match.rules.best_of);
  label(ctx, text, FONT_KEY_GOTHIC_18_BOLD, GColorBlack,
        GRect(8, 5, 105, 25), GTextAlignmentLeft);
  snprintf(text, sizeof(text), "SETS %d-%d", s_match.score.sets[0], s_match.score.sets[1]);
  label(ctx, text, FONT_KEY_GOTHIC_18_BOLD, GColorBlack,
        GRect(102, 5, 90, 25), GTextAlignmentRight);
  fill(ctx, team_color(0), GRect(7, 39, 90, 126));
  fill(ctx, team_color(1), GRect(103, 39, 90, 126));
  label(ctx, "US", FONT_KEY_GOTHIC_24_BOLD, GColorWhite,
        GRect(10, 43, 84, 32), GTextAlignmentCenter);
  label(ctx, "THEM", FONT_KEY_GOTHIC_24_BOLD, GColorWhite,
        GRect(105, 43, 84, 32), GTextAlignmentCenter);
  snprintf(text, sizeof(text), "%u", s_match.score.points[0]);
  label(ctx, text, FONT_KEY_BITHAM_42_BOLD, GColorWhite,
        GRect(8, 82, 88, 62), GTextAlignmentCenter);
  snprintf(text, sizeof(text), "%u", s_match.score.points[1]);
  label(ctx, text, FONT_KEY_BITHAM_42_BOLD, GColorWhite,
        GRect(104, 82, 88, 62), GTextAlignmentCenter);
  if (s_match.score.server < 2)
    fill(ctx, GColorBlack, GRect(s_match.score.server == 0 ? 36 : 136, 157, 27, 4));
  snprintf(text, sizeof(text), "PLAY TO %u  /  WIN BY 2", match_target(&s_match));
  label(ctx, text, FONT_KEY_GOTHIC_18, GColorBlack,
        GRect(4, 171, 192, 25), GTextAlignmentCenter);
  if (s_match.score.phase == PHASE_PLAYING) {
    label(ctx, "UP +US  |  DOWN +THEM", FONT_KEY_GOTHIC_18_BOLD, GColorBlack,
          GRect(4, 197, 192, 24), GTextAlignmentCenter);
  } else if (s_match.score.phase == PHASE_SET_WON) {
    label(ctx, "SELECT NEXT | HOLD UNDO", FONT_KEY_GOTHIC_18_BOLD, GColorBlack,
          GRect(3, 197, 194, 24), GTextAlignmentCenter);
  } else {
    label(ctx, s_match.saved ? "SAVED  |  SELECT: MENU" :
          "SELECT SAVE | HOLD UNDO", FONT_KEY_GOTHIC_18_BOLD, GColorBlack,
          GRect(2, 197, 196, 24), GTextAlignmentCenter);
  }
  if (s_notice[0]) {
    fill(ctx, GColorBlack, GRect(10, 169, 180, 28));
    label(ctx, s_notice, FONT_KEY_GOTHIC_18_BOLD, GColorWhite,
          GRect(12, 170, 176, 25), GTextAlignmentCenter);
  }
}

static const char *menu_items[] = {"Undo point", "Calendar", "Match places", "Match settings", "New match"};

static void list_screen(GContext *ctx, const char *title, const char **items,
                        uint8_t length, uint8_t selected) {
  fill(ctx, GColorWhite, GRect(0, 0, 200, 228));
  fill(ctx, GColorBlack, GRect(0, 0, 200, 37));
  label(ctx, title, FONT_KEY_GOTHIC_24_BOLD, GColorWhite,
        GRect(7, 3, 186, 31), GTextAlignmentLeft);
  for (uint8_t i = 0; i < length; i++) {
    int y = 39 + i * 37;
    if (i == selected) fill(ctx, GColorBlue, GRect(7, y, 186, 35));
    label(ctx, items[i], FONT_KEY_GOTHIC_24_BOLD,
          i == selected ? GColorWhite : GColorBlack,
          GRect(14, y + 1, 172, 32), GTextAlignmentLeft);
  }
}

static void settings_screen(GContext *ctx) {
  char row0[30], row1[30], row2[30];
  snprintf(row0, sizeof(row0), "Best of %u", s_defaults.best_of);
  snprintf(row1, sizeof(row1), "Regular: %u", s_defaults.regular_target);
  snprintf(row2, sizeof(row2), "Decider: %u", s_defaults.deciding_target);
  const char *items[] = {row0, row1, row2, "Done"};
  list_screen(ctx, "CUSTOM RULES", items, 4, s_cursor);
}

static void colors_screen(GContext *ctx) {
  char us[28], them[28];
  snprintf(us, sizeof(us), "US: %s", team_color_name(0));
  snprintf(them, sizeof(them), "THEM: %s", team_color_name(1));
  const char *items[] = {us, them,
                         s_location_enabled ? "Save GPS: on" : "Save GPS: off", "Done"};
  list_screen(ctx, "TEAM COLORS", items, 4, s_cursor);
}

static uint8_t selected_format(void) {
  for (uint8_t i = 0; i < PRESET_COUNT; i++) {
    Rules preset = rules_preset((RulesPreset)i);
    if (s_defaults.best_of == preset.best_of &&
        s_defaults.regular_target == preset.regular_target &&
        s_defaults.deciding_target == preset.deciding_target) return i;
  }
  return PRESET_COUNT;
}

static void formats_screen(GContext *ctx) {
  const char *items[] = {"3 SETS: 25/25/15", "JV: 3 SETS TO 25",
                         "VARSITY: 5 TO 25", "CUSTOM RULES", "COLORS / GPS"};
  list_screen(ctx, "MATCH SETTINGS", items, 5, s_cursor);
}

static void current_month(int16_t offset, int *year, int *month) {
  time_t now = time(NULL);
  struct tm *today = localtime(&now);
  int index = (today ? today->tm_year + 1900 : 2026) * 12 +
              (today ? today->tm_mon : 9) + offset;
  *year = index / 12;
  *month = index % 12 + 1;
}

static int days_in_month(int year, int month) {
  static const uint8_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month == 2 && year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) return 29;
  return days[month - 1];
}

static void calendar_screen(GContext *ctx) {
  static const char *months[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN",
                                 "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
  int year, month;
  current_month(s_month_offset, &year, &month);
  uint8_t day_state[32] = {0};
  uint16_t wins = 0, losses = 0;
  for (uint8_t i = 0; i < s_count; i++) {
    MatchRecord *r = record_at(i);
    if (r->year == year && r->month == month && r->day >= 1 && r->day <= 31) {
      day_state[r->day] |= r->winner == 0 ? 1 : 2;
      if (r->winner == 0) wins++; else losses++;
    }
  }
  fill(ctx, GColorWhite, GRect(0, 0, 200, 228));
  fill(ctx, GColorBlack, GRect(0, 0, 200, 47));
  char text[40];
  snprintf(text, sizeof(text), "%s %d", months[month - 1], year);
  label(ctx, text, FONT_KEY_GOTHIC_28_BOLD, GColorWhite,
        GRect(7, 1, 117, 37), GTextAlignmentLeft);
  snprintf(text, sizeof(text), "%uW %uL", wins, losses);
  label(ctx, text, FONT_KEY_GOTHIC_18_BOLD, GColorWhite,
        GRect(121, 8, 73, 28), GTextAlignmentRight);
  const char *weekdays[] = {"S", "M", "T", "W", "T", "F", "S"};
  for (int i = 0; i < 7; i++)
    label(ctx, weekdays[i], FONT_KEY_GOTHIC_18_BOLD, GColorBlack,
          GRect(6 + i * 27, 48, 25, 21), GTextAlignmentCenter);
  struct tm first = {.tm_year = year - 1900, .tm_mon = month - 1, .tm_mday = 1};
  mktime(&first);
  int first_weekday = first.tm_wday;
  for (int day = 1; day <= days_in_month(year, month); day++) {
    int cell = first_weekday + day - 1;
    int x = 7 + (cell % 7) * 27;
    int y = 72 + (cell / 7) * 22;
    if (day_state[day] != 0) {
      GColor color = day_state[day] == 1 ? team_color(0) :
                     day_state[day] == 2 ? team_color(1) : GColorBlack;
      fill(ctx, color, GRect(x, y, 24, 20));
    }
    snprintf(text, sizeof(text), "%d", day);
    label(ctx, text, FONT_KEY_GOTHIC_18_BOLD,
          day_state[day] ? GColorWhite : GColorBlack,
          GRect(x, y - 2, 24, 21), GTextAlignmentCenter);
  }
  fill(ctx, team_color(0), GRect(13, 207, 10, 10));
  label(ctx, "W", FONT_KEY_GOTHIC_14_BOLD, GColorBlack,
        GRect(27, 201, 25, 20), GTextAlignmentLeft);
  fill(ctx, team_color(1), GRect(62, 207, 10, 10));
  label(ctx, "L", FONT_KEY_GOTHIC_14_BOLD, GColorBlack,
        GRect(77, 201, 20, 20), GTextAlignmentLeft);
  fill(ctx, GColorBlack, GRect(108, 207, 10, 10));
  label(ctx, "BOTH", FONT_KEY_GOTHIC_14_BOLD, GColorBlack,
        GRect(122, 201, 65, 20), GTextAlignmentLeft);
}

static uint8_t month_match_count(void) {
  int year, month;
  current_month(s_month_offset, &year, &month);
  uint8_t count = 0;
  for (uint8_t i = 0; i < s_count; i++) {
    MatchRecord *r = record_at(i);
    if (r->year == year && r->month == month) count++;
  }
  return count;
}

static MatchRecord *month_record_at(uint8_t reversed_index) {
  int year, month;
  current_month(s_month_offset, &year, &month);
  uint8_t found = 0;
  for (int i = s_count - 1; i >= 0; i--) {
    MatchRecord *r = record_at(i);
    if (r->year == year && r->month == month) {
      if (found == reversed_index) return r;
      found++;
    }
  }
  return NULL;
}

static void history_screen(GContext *ctx) {
  fill(ctx, GColorWhite, GRect(0, 0, 200, 228));
  fill(ctx, GColorBlack, GRect(0, 0, 200, 38));
  label(ctx, "SAVED MATCHES", FONT_KEY_GOTHIC_24_BOLD, GColorWhite,
        GRect(7, 2, 185, 32), GTextAlignmentLeft);
  uint8_t count = month_match_count();
  if (!count) {
    label(ctx, "No matches this month", FONT_KEY_GOTHIC_24, GColorBlack,
          GRect(4, 85, 192, 40), GTextAlignmentCenter);
    return;
  }
  if (s_history_cursor >= count) s_history_cursor = count - 1;
  MatchRecord *r = month_record_at(s_history_cursor);
  if (!r) return;
  char text[64];
  snprintf(text, sizeof(text), "%02u/%02u/%04u", r->month, r->day, r->year);
  label(ctx, text, FONT_KEY_GOTHIC_24_BOLD, GColorBlack,
        GRect(4, 45, 192, 35), GTextAlignmentCenter);
  fill(ctx, team_color(r->winner), GRect(15, 84, 170, 59));
  snprintf(text, sizeof(text), "%s  %u - %u", r->winner == 0 ? "WIN" : "LOSS",
           r->us_sets, r->them_sets);
  label(ctx, text, FONT_KEY_GOTHIC_28_BOLD, GColorWhite,
        GRect(18, 92, 164, 42), GTextAlignmentCenter);
  snprintf(text, sizeof(text), "BEST OF %u  /  %u-%u PTS", r->best_of,
           r->regular_target, r->deciding_target);
  label(ctx, text, FONT_KEY_GOTHIC_18, GColorBlack,
        GRect(3, 147, 194, 25), GTextAlignmentCenter);
  if (r->has_location) {
    int32_t lat_abs = r->latitude_e6 < 0 ? -r->latitude_e6 : r->latitude_e6;
    int32_t lon_abs = r->longitude_e6 < 0 ? -r->longitude_e6 : r->longitude_e6;
    snprintf(text, sizeof(text), "GPS %s%ld.%06ld %s%ld.%06ld",
             r->latitude_e6 < 0 ? "-" : "", (long)(lat_abs / 1000000),
             (long)(lat_abs % 1000000), r->longitude_e6 < 0 ? "-" : "",
             (long)(lon_abs / 1000000), (long)(lon_abs % 1000000));
  } else snprintf(text, sizeof(text), "NO GPS SAVED");
  label(ctx, text, FONT_KEY_GOTHIC_14, GColorBlack,
        GRect(2, 174, 196, 24), GTextAlignmentCenter);
  snprintf(text, sizeof(text), "%u OF %u THIS MONTH", s_history_cursor + 1, count);
  label(ctx, text, FONT_KEY_GOTHIC_18_BOLD, GColorBlack,
        GRect(3, 200, 194, 25), GTextAlignmentCenter);
}

static void places_screen(GContext *ctx) {
  fill(ctx, GColorWhite, GRect(0, 0, 200, 228));
  fill(ctx, GColorBlack, GRect(0, 0, 200, 38));
  label(ctx, "MATCH PLACES", FONT_KEY_GOTHIC_24_BOLD, GColorWhite,
        GRect(7, 2, 185, 32), GTextAlignmentLeft);
  int32_t min_lat = 90000000, max_lat = -90000000;
  int32_t min_lon = 180000000, max_lon = -180000000;
  uint8_t plotted = 0;
  for (uint8_t i = 0; i < s_count; i++) {
    MatchRecord *r = record_at(i);
    if (!r->has_location) continue;
    if (r->latitude_e6 < min_lat) min_lat = r->latitude_e6;
    if (r->latitude_e6 > max_lat) max_lat = r->latitude_e6;
    if (r->longitude_e6 < min_lon) min_lon = r->longitude_e6;
    if (r->longitude_e6 > max_lon) max_lon = r->longitude_e6;
    plotted++;
  }
  if (!plotted) {
    label(ctx, "No GPS matches yet", FONT_KEY_GOTHIC_24, GColorBlack,
          GRect(4, 88, 192, 40), GTextAlignmentCenter);
    return;
  }
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_rect(ctx, GRect(18, 48, 164, 145));
  label(ctx, "N", FONT_KEY_GOTHIC_14_BOLD, GColorBlack,
        GRect(92, 40, 16, 19), GTextAlignmentCenter);
  int64_t lat_span = (int64_t)max_lat - min_lat;
  int64_t lon_span = (int64_t)max_lon - min_lon;
  for (uint8_t i = 0; i < s_count; i++) {
    MatchRecord *r = record_at(i);
    if (!r->has_location) continue;
    int x = lon_span ? 23 + (int)(((int64_t)r->longitude_e6 - min_lon) * 152 / lon_span) : 99;
    int y = lat_span ? 53 + (int)(((int64_t)max_lat - r->latitude_e6) * 135 / lat_span) : 120;
    fill(ctx, team_color(r->winner), GRect(x - 3, y - 3, 7, 7));
  }
  char text[40];
  snprintf(text, sizeof(text), "%u PIN%s  US=W  THEM=L", plotted,
           plotted == 1 ? "" : "S");
  label(ctx, text, FONT_KEY_GOTHIC_14, GColorBlack,
        GRect(2, 198, 196, 22), GTextAlignmentCenter);
}

static void update_proc(Layer *layer, GContext *ctx) {
  switch (s_screen) {
    case SCREEN_SCORE: score_screen(ctx); break;
    case SCREEN_MENU: list_screen(ctx, "MATCH MENU", menu_items, 5, s_cursor); break;
    case SCREEN_FORMATS: formats_screen(ctx); break;
    case SCREEN_SETTINGS: settings_screen(ctx); break;
    case SCREEN_COLORS: colors_screen(ctx); break;
    case SCREEN_CONFIRM_NEW:
      fill(ctx, GColorWhite, GRect(0, 0, 200, 228));
      label(ctx, "NEW MATCH?", FONT_KEY_GOTHIC_28_BOLD, GColorBlack,
            GRect(5, 46, 190, 40), GTextAlignmentCenter);
      label(ctx, "Current score will reset", FONT_KEY_GOTHIC_24, GColorBlack,
            GRect(5, 94, 190, 38), GTextAlignmentCenter);
      label(ctx, "SELECT: YES  BACK: NO", FONT_KEY_GOTHIC_18_BOLD, GColorBlack,
            GRect(5, 176, 190, 28), GTextAlignmentCenter);
      break;
    case SCREEN_CONFIRM_REPLACE: {
      MatchRecord *oldest = record_at(0);
      char text[40];
      fill(ctx, GColorWhite, GRect(0, 0, 200, 228));
      label(ctx, "HISTORY FULL", FONT_KEY_GOTHIC_28_BOLD, GColorBlack,
            GRect(5, 32, 190, 42), GTextAlignmentCenter);
      snprintf(text, sizeof(text), "Replace oldest: %02u/%02u/%04u?",
               oldest ? oldest->month : 0, oldest ? oldest->day : 0,
               oldest ? oldest->year : 0);
      label(ctx, text, FONT_KEY_GOTHIC_18, GColorBlack,
            GRect(4, 90, 192, 30), GTextAlignmentCenter);
      label(ctx, "SELECT REPLACE", FONT_KEY_GOTHIC_24_BOLD, GColorBlack,
            GRect(5, 151, 190, 35), GTextAlignmentCenter);
      label(ctx, "BACK KEEP OLD RECORD", FONT_KEY_GOTHIC_18_BOLD, GColorBlack,
            GRect(5, 188, 190, 27), GTextAlignmentCenter);
      break;
    }
    case SCREEN_CALENDAR: calendar_screen(ctx); break;
    case SCREEN_HISTORY: history_screen(ctx); break;
    case SCREEN_PLACES: places_screen(ctx); break;
  }
}

static void up_click(ClickRecognizerRef recognizer, void *context) {
  s_notice[0] = '\0';
  if (s_screen == SCREEN_SCORE && match_point(&s_match, 0)) save_match_state();
  else if (s_screen == SCREEN_MENU) s_cursor = (s_cursor + 4) % 5;
  else if (s_screen == SCREEN_FORMATS) s_cursor = (s_cursor + 4) % 5;
  else if (s_screen == SCREEN_SETTINGS || s_screen == SCREEN_COLORS)
    s_cursor = (s_cursor + 3) % 4;
  else if (s_screen == SCREEN_CALENDAR && s_month_offset > -120) s_month_offset--;
  else if (s_screen == SCREEN_HISTORY && s_history_cursor > 0) s_history_cursor--;
  redraw();
}

static void down_click(ClickRecognizerRef recognizer, void *context) {
  s_notice[0] = '\0';
  if (s_screen == SCREEN_SCORE && match_point(&s_match, 1)) save_match_state();
  else if (s_screen == SCREEN_MENU) s_cursor = (s_cursor + 1) % 5;
  else if (s_screen == SCREEN_FORMATS) s_cursor = (s_cursor + 1) % 5;
  else if (s_screen == SCREEN_SETTINGS || s_screen == SCREEN_COLORS)
    s_cursor = (s_cursor + 1) % 4;
  else if (s_screen == SCREEN_CALENDAR && s_month_offset < 0) s_month_offset++;
  else if (s_screen == SCREEN_HISTORY && s_history_cursor + 1 < month_match_count())
    s_history_cursor++;
  redraw();
}

static void select_click(ClickRecognizerRef recognizer, void *context) {
  s_notice[0] = '\0';
  if (s_screen == SCREEN_SCORE) {
    if (s_match.score.phase == PHASE_SET_WON) {
      match_next_set(&s_match);
      save_match_state();
    } else if (s_match.score.phase == PHASE_MATCH_WON && !s_match.saved) {
      if (s_count == RECORD_CAPACITY) s_screen = SCREEN_CONFIRM_REPLACE;
      else save_finished_match();
    } else {
      s_screen = SCREEN_MENU;
      s_cursor = 0;
    }
  } else if (s_screen == SCREEN_MENU) {
    if (s_cursor == 0) {
      if (match_undo(&s_match)) { save_match_state(); notice("Point undone"); }
      else notice("Nothing to undo");
      s_screen = SCREEN_SCORE;
    } else if (s_cursor == 1) {
      s_month_offset = 0;
      s_screen = SCREEN_CALENDAR;
    } else if (s_cursor == 2) {
      s_screen = SCREEN_PLACES;
    } else if (s_cursor == 3) {
      s_screen = SCREEN_FORMATS;
      s_cursor = selected_format();
    } else s_screen = SCREEN_CONFIRM_NEW;
  } else if (s_screen == SCREEN_FORMATS) {
    if (s_cursor < PRESET_COUNT) {
      s_defaults = rules_preset((RulesPreset)s_cursor);
      save_defaults();
      s_screen = SCREEN_MENU;
      s_cursor = 3;
    } else if (s_cursor == 3) {
      s_screen = SCREEN_SETTINGS;
      s_cursor = 0;
    } else {
      s_screen = SCREEN_COLORS;
      s_cursor = 0;
    }
  } else if (s_screen == SCREEN_SETTINGS) {
    if (s_cursor == 0) s_defaults.best_of = s_defaults.best_of == 3 ? 5 : 3;
    else if (s_cursor == 1) s_defaults.regular_target = s_defaults.regular_target == 25 ? 15 : 25;
    else if (s_cursor == 2) s_defaults.deciding_target = s_defaults.deciding_target == 15 ? 25 : 15;
    else { save_defaults(); s_screen = SCREEN_FORMATS; s_cursor = 3; }
  } else if (s_screen == SCREEN_COLORS) {
    if (s_cursor == 0 || s_cursor == 1) {
      cycle_team_color(s_cursor);
      save_colors();
    } else if (s_cursor == 2) {
      s_location_enabled = !s_location_enabled;
      save_colors();
    } else { s_screen = SCREEN_FORMATS; s_cursor = 4; }
  } else if (s_screen == SCREEN_CONFIRM_NEW) {
    uint32_t next_id = s_match.match_id == UINT32_MAX ? 1 : s_match.match_id + 1;
    match_start(&s_match, s_defaults);
    s_match.match_id = next_id;
    save_match_state();
    s_screen = SCREEN_SCORE;
  } else if (s_screen == SCREEN_CONFIRM_REPLACE) {
    s_screen = SCREEN_SCORE;
    save_finished_match();
  } else if (s_screen == SCREEN_CALENDAR) {
    s_history_cursor = 0;
    s_screen = SCREEN_HISTORY;
  }
  redraw();
}

static void back_click(ClickRecognizerRef recognizer, void *context) {
  if (s_screen == SCREEN_SETTINGS) { save_defaults(); s_screen = SCREEN_FORMATS; s_cursor = 3; }
  else if (s_screen == SCREEN_COLORS) { save_colors(); s_screen = SCREEN_FORMATS; s_cursor = 4; }
  else if (s_screen == SCREEN_FORMATS) { s_screen = SCREEN_MENU; s_cursor = 3; }
  else if (s_screen == SCREEN_HISTORY) s_screen = SCREEN_CALENDAR;
  else if (s_screen == SCREEN_MENU || s_screen == SCREEN_CALENDAR || s_screen == SCREEN_PLACES ||
           s_screen == SCREEN_CONFIRM_NEW || s_screen == SCREEN_CONFIRM_REPLACE)
    s_screen = SCREEN_SCORE;
  else { window_stack_pop(true); return; }
  redraw();
}

static void select_long_click(ClickRecognizerRef recognizer, void *context) {
  if (s_screen != SCREEN_SCORE) return;
  if (match_undo(&s_match)) {
    save_match_state();
    notice("Point undone");
  } else notice("Nothing to undo");
}

static void click_config(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, up_click);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click);
  window_long_click_subscribe(BUTTON_ID_SELECT, 700, select_long_click, NULL);
  window_single_click_subscribe(BUTTON_ID_BACK, back_click);
}

static void init(void) {
  load_data();
  s_window = window_create();
  window_set_background_color(s_window, GColorWhite);
  window_set_click_config_provider(s_window, click_config);
  Layer *root = window_get_root_layer(s_window);
  s_canvas = layer_create(layer_get_bounds(root));
  layer_set_update_proc(s_canvas, update_proc);
  layer_add_child(root, s_canvas);
  window_stack_push(s_window, true);
  app_message_register_inbox_received(inbox_received);
  app_message_open(128, 64);
}

static void deinit(void) {
  save_match_state();
  layer_destroy(s_canvas);
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
