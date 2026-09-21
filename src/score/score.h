#pragma once

#include <cstdio>
#include <string>
#include <utility>

// Score rules: the run clock, the persisted best, and the one-line file that
// keeps it. Pure and SDL-free, so a spec drives all of it with no renderer.
//
// The score IS the survival time. There is no kill counter and no combo: the
// design's whole point is that the only currency is seconds survived, and the
// best run is the longest one.

namespace last_stand {

// The persisted best. One number: the longest run, in whole seconds.
struct ScoreBoard {
    int bestSeconds = 0;
};

// Submits a finished run. Returns true when it beats the stored best and
// updates the board in that case. A tie is not a new best.
inline bool Submit(ScoreBoard &board, int seconds) {
    if (seconds > board.bestSeconds) {
        board.bestSeconds = seconds;
        return true;
    }
    return false;
}

// "2:03" from 123 seconds, the HUD and the death screen share this one format.
// Negative clamps to "0:00"; minutes do not wrap at 60, so a 70-minute run
// reads "70:11" rather than rolling over.
inline std::string FormatClock(int seconds) {
    if (seconds < 0) seconds = 0;
    char buf[16];
    std::snprintf(buf, sizeof(buf), "%d:%02d", seconds / 60, seconds % 60);
    return std::string(buf);
}

// One integer in a text file. A missing or unreadable file is a zero board,
// not an error: the first run has no save. Save is best-effort and returns
// false when the path cannot be written (a read-only install dir), which the
// caller may ignore.
class ScoreStore {
public:
    explicit ScoreStore(std::string path) : path_(std::move(path)) {}

    ScoreBoard Load() const {
        ScoreBoard board;
        if (std::FILE *f = std::fopen(path_.c_str(), "r")) {
            int best = 0;
            if (std::fscanf(f, "%d", &best) == 1 && best > 0)
                board.bestSeconds = best;
            std::fclose(f);
        }
        return board;
    }

    bool Save(const ScoreBoard &board) const {
        std::FILE *f = std::fopen(path_.c_str(), "w");
        if (!f) return false;
        const int written = std::fprintf(f, "%d\n", board.bestSeconds);
        std::fclose(f);
        return written > 0;
    }

private:
    std::string path_;
};

} // namespace last_stand
