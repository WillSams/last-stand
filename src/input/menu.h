#pragma once

// Title-menu input rule, pure and SDL-free so a spec can pin it. The state
// samples the keyboard and the pad and hands the already-boolean result here;
// this decides what that means.

namespace last_stand {

enum class MenuAction { None, Start, Quit };

struct MenuInput {
    bool enter = false;     // Enter or Space on the keyboard
    bool padStart = false;  // gamepad Start, edge-triggered
    bool padA = false;      // gamepad A, edge-triggered
    bool esc = false;       // Esc or the window close box
};

// Quit wins over Start: a player holding the pad's Start while tapping Esc
// means quit, not a race between the two. Any start key starts.
inline MenuAction MenuActionFor(const MenuInput &in) {
    if (in.esc)
        return MenuAction::Quit;
    if (in.enter || in.padStart || in.padA)
        return MenuAction::Start;
    return MenuAction::None;
}

} // namespace last_stand
