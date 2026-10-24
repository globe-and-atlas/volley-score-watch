#include <assert.h>
#include <stdio.h>

#include "model.h"

static void score_to(Match *match, unsigned us, unsigned them) {
  for (unsigned i = 0; i < us; i++) assert(match_point(match, 0));
  for (unsigned i = 0; i < them; i++) assert(match_point(match, 1));
}

int main(void) {
  Match m;
  Rules three = rules_preset(PRESET_THREE_DECIDER_15);
  Rules jv = rules_preset(PRESET_JV_THREE_25);
  Rules varsity = rules_preset(PRESET_VARSITY_FIVE_25);
  assert(three.best_of == 3 && three.regular_target == 25 && three.deciding_target == 15);
  assert(jv.best_of == 3 && jv.regular_target == 25 && jv.deciding_target == 25);
  assert(varsity.best_of == 5 && varsity.regular_target == 25 && varsity.deciding_target == 25);
  Rules rules = {3, 25, 15};
  assert(rules_valid(rules));
  assert(!rules_valid((Rules){1, 25, 15}));
  match_start(&m, rules);
  m.match_id = 42;
  assert(match_target(&m) == 25);
  score_to(&m, 24, 24);
  assert(m.score.phase == PHASE_PLAYING);
  assert(match_point(&m, 0));
  assert(m.score.phase == PHASE_PLAYING);
  assert(match_point(&m, 1));
  assert(match_point(&m, 0));
  assert(match_point(&m, 0));
  assert(m.score.phase == PHASE_SET_WON);
  assert(m.score.sets[0] == 1);
  assert(match_undo(&m));
  assert(m.score.phase == PHASE_PLAYING);
  assert(m.score.sets[0] == 0);
  assert(!match_undo(&m));
  assert(match_point(&m, 0));
  assert(match_next_set(&m));
  assert(m.score.points[0] == 0);
  assert(m.score.points[1] == 0);
  assert(!match_undo(&m));
  score_to(&m, 0, 25);
  assert(m.score.phase == PHASE_SET_WON);
  assert(match_next_set(&m));
  assert(match_target(&m) == 15);
  score_to(&m, 15, 0);
  assert(m.score.phase == PHASE_MATCH_WON);
  assert(match_target(&m) == 15);
  assert(m.score.sets[0] == 2);
  MatchRecord record;
  assert(match_record(&m, 2026, 10, 24, &record));
  assert(record.winner == 0);
  assert(record.match_id == 42);
  assert(record.us_sets == 2 && record.them_sets == 1);
  assert(!match_point(&m, 0));
  m.saved = 1;
  assert(!match_record(&m, 2026, 10, 24, &record));

  match_start(&m, (Rules){5, 15, 25});
  score_to(&m, 15, 0); assert(match_next_set(&m));
  score_to(&m, 0, 15); assert(match_next_set(&m));
  score_to(&m, 15, 0); assert(match_next_set(&m));
  score_to(&m, 0, 15); assert(match_next_set(&m));
  assert(match_target(&m) == 25);
  score_to(&m, 0, 25);
  assert(m.score.phase == PHASE_MATCH_WON);
  assert(match_target(&m) == 25);
  assert(m.score.sets[1] == 3);
  assert(match_record(&m, 2026, 10, 24, &record));
  assert(record.winner == 1);

  match_start(&m, jv);
  score_to(&m, 0, 25); assert(match_next_set(&m));
  score_to(&m, 25, 0); assert(match_next_set(&m));
  assert(match_target(&m) == 25);
  score_to(&m, 25, 0);
  assert(m.score.phase == PHASE_MATCH_WON);
  assert(m.score.sets[0] == 2);

  match_start(&m, varsity);
  for (int set = 0; set < 4; set++) {
    score_to(&m, set % 2 == 0 ? 25 : 0, set % 2 == 0 ? 0 : 25);
    assert(match_next_set(&m));
  }
  assert(match_target(&m) == 25);
  score_to(&m, 25, 0);
  assert(m.score.phase == PHASE_MATCH_WON);
  assert(m.score.sets[0] == 3);

  match_start(&m, three);
  score_to(&m, 25, 0); assert(match_next_set(&m));
  score_to(&m, 25, 0);
  assert(m.score.phase == PHASE_MATCH_WON);
  assert(m.score.sets[0] == 2);
  assert(!match_next_set(&m));
  puts("model tests passed");
  return 0;
}
