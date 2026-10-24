#include "model.h"

#include <string.h>

Rules rules_default(void) {
  return rules_preset(PRESET_THREE_DECIDER_15);
}

Rules rules_preset(RulesPreset preset) {
  switch (preset) {
    case PRESET_JV_THREE_25:
      return (Rules){.best_of = 3, .regular_target = 25, .deciding_target = 25};
    case PRESET_VARSITY_FIVE_25:
      return (Rules){.best_of = 5, .regular_target = 25, .deciding_target = 25};
    case PRESET_THREE_DECIDER_15:
    default:
      return (Rules){.best_of = 3, .regular_target = 25, .deciding_target = 15};
  }
}

bool rules_valid(Rules rules) {
  return (rules.best_of == 3 || rules.best_of == 5) &&
         (rules.regular_target == 15 || rules.regular_target == 25) &&
         (rules.deciding_target == 15 || rules.deciding_target == 25);
}

void match_start(Match *match, Rules rules) {
  memset(match, 0, sizeof(*match));
  match->version = 2;
  match->rules = rules_valid(rules) ? rules : rules_default();
  match->score.server = 2;
}

uint8_t match_target(const Match *match) {
  const uint8_t sets_played = match->score.sets[0] + match->score.sets[1];
  const uint8_t shown_set = sets_played +
                            (match->score.phase == PHASE_PLAYING ? 1 : 0);
  return shown_set == match->rules.best_of
             ? match->rules.deciding_target
             : match->rules.regular_target;
}

bool match_point(Match *match, uint8_t team) {
  if (team > 1 || match->score.phase != PHASE_PLAYING ||
      match->score.points[team] == UINT16_MAX) {
    return false;
  }
  match->undo_score = match->score;
  match->undo_available = 1;
  match->score.points[team]++;
  match->score.server = team;
  const uint16_t ours = match->score.points[team];
  const uint16_t theirs = match->score.points[1 - team];
  if (ours >= match_target(match) && (uint32_t)ours >= (uint32_t)theirs + 2) {
    match->score.sets[team]++;
    const uint8_t sets_needed = (match->rules.best_of / 2) + 1;
    match->score.phase = match->score.sets[team] >= sets_needed
                             ? PHASE_MATCH_WON
                             : PHASE_SET_WON;
  }
  return true;
}

bool match_undo(Match *match) {
  if (!match->undo_available || match->saved) {
    return false;
  }
  match->score = match->undo_score;
  match->undo_available = 0;
  return true;
}

bool match_next_set(Match *match) {
  if (match->score.phase != PHASE_SET_WON) {
    return false;
  }
  match->score.points[0] = 0;
  match->score.points[1] = 0;
  match->score.server = 2;
  match->score.phase = PHASE_PLAYING;
  match->undo_available = 0;
  return true;
}

bool match_record(const Match *match, uint16_t year, uint8_t month, uint8_t day,
                  MatchRecord *record) {
  if (match->score.phase != PHASE_MATCH_WON || match->saved || year < 2000 ||
      month < 1 || month > 12 || day < 1 || day > 31 || !record) {
    return false;
  }
  *record = (MatchRecord){.match_id = match->match_id,
                          .year = year,
                          .month = month,
                          .day = day,
                          .winner = match->score.sets[0] > match->score.sets[1] ? 0 : 1,
                          .us_sets = match->score.sets[0],
                          .them_sets = match->score.sets[1],
                          .best_of = match->rules.best_of,
                          .regular_target = match->rules.regular_target,
                          .deciding_target = match->rules.deciding_target};
  return true;
}
