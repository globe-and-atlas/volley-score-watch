#ifndef VOLLEY_MODEL_H
#define VOLLEY_MODEL_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  PHASE_PLAYING = 0,
  PHASE_SET_WON = 1,
  PHASE_MATCH_WON = 2
} Phase;

typedef struct {
  uint8_t best_of;
  uint8_t regular_target;
  uint8_t deciding_target;
} Rules;

typedef enum {
  PRESET_THREE_DECIDER_15 = 0,
  PRESET_JV_THREE_25 = 1,
  PRESET_VARSITY_FIVE_25 = 2,
  PRESET_COUNT = 3
} RulesPreset;

typedef struct {
  uint16_t points[2];
  uint8_t sets[2];
  uint8_t server;
  uint8_t phase;
} Score;

typedef struct {
  uint8_t version;
  uint32_t match_id;
  Rules rules;
  Score score;
  Score undo_score;
  uint8_t undo_available;
  uint8_t saved;
} Match;

typedef struct {
  uint32_t match_id;
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t winner;
  uint8_t us_sets;
  uint8_t them_sets;
  uint8_t best_of;
  uint8_t regular_target;
  uint8_t deciding_target;
  int32_t latitude_e6;
  int32_t longitude_e6;
  uint16_t accuracy_m;
  uint8_t has_location;
} MatchRecord;

Rules rules_default(void);
Rules rules_preset(RulesPreset preset);
bool rules_valid(Rules rules);
void match_start(Match *match, Rules rules);
uint8_t match_target(const Match *match);
bool match_point(Match *match, uint8_t team);
bool match_undo(Match *match);
bool match_next_set(Match *match);
bool match_record(const Match *match, uint16_t year, uint8_t month, uint8_t day,
                  MatchRecord *record);

#endif
