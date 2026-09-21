#include <igloo/igloo_alt.h>

#include <cstdio>
#include <string>

#include "../src/score/score.h"

using namespace igloo;
using last_stand::FormatClock;
using last_stand::ScoreBoard;
using last_stand::ScoreStore;
using last_stand::Submit;

Describe(ScoreSpec) {

  It(accepts_the_first_run_as_the_best) {
    ScoreBoard board;
    Assert::That(Submit(board, 42), IsTrue());
    Assert::That(board.bestSeconds, Equals(42));
  };

  It(keeps_the_longer_run) {
    ScoreBoard board{100};
    Assert::That(Submit(board, 90), IsFalse());
    Assert::That(board.bestSeconds, Equals(100));
  };

  It(treats_a_tie_as_not_a_new_best) {
    ScoreBoard board{100};
    Assert::That(Submit(board, 100), IsFalse());
    Assert::That(board.bestSeconds, Equals(100));
  };

  It(overwrites_the_best_with_a_longer_run) {
    ScoreBoard board{100};
    Assert::That(Submit(board, 140), IsTrue());
    Assert::That(board.bestSeconds, Equals(140));
  };

  It(formats_minutes_and_pads_the_seconds) {
    Assert::That(FormatClock(123), Equals(std::string("2:03")));
    Assert::That(FormatClock(5), Equals(std::string("0:05")));
  };

  It(does_not_wrap_minutes_at_sixty) {
    Assert::That(FormatClock(3600), Equals(std::string("60:00")));
  };

  It(clamps_a_negative_clock_to_zero) {
    Assert::That(FormatClock(-1), Equals(std::string("0:00")));
  };
};

Describe(ScoreStoreSpec) {

  const std::string path = "build/specs/score-round-trip.sav";

  It(round_trips_a_board_through_the_file) {
    ScoreStore store(path);
    Assert::That(store.Save(ScoreBoard{321}), IsTrue());
    Assert::That(store.Load().bestSeconds, Equals(321));
    std::remove(path.c_str());
  };

  It(reads_a_missing_file_as_an_empty_board) {
    const std::string missing = "build/specs/score-absent.sav";
    std::remove(missing.c_str());
    Assert::That(ScoreStore(missing).Load().bestSeconds, Equals(0));
  };

  It(reads_a_garbage_file_as_an_empty_board) {
    const std::string garbage = "build/specs/score-garbage.sav";
    if (std::FILE *f = std::fopen(garbage.c_str(), "w")) {
      std::fputs("not-a-number\n", f);
      std::fclose(f);
    }
    Assert::That(ScoreStore(garbage).Load().bestSeconds, Equals(0));
    std::remove(garbage.c_str());
  };

  It(writes_an_empty_board_as_zero_not_absent) {
    const std::string zero = "build/specs/score-zero.sav";
    ScoreStore store(zero);
    Assert::That(store.Save(ScoreBoard{}), IsTrue());
    Assert::That(store.Load().bestSeconds, Equals(0));
    std::remove(zero.c_str());
  };
};
