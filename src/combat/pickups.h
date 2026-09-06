#pragma once
// Pickup kinds + weapon stats. Pure, no SDL.
namespace last_stand {
enum class PickupKind { Gun, Machine, Silencer, Health, Shield, Speed };
struct WeaponStats { float interval; int damage; float spread; const char *pose; };
inline WeaponStats StatsFor(PickupKind k) {
    switch (k) {
        case PickupKind::Machine: return {0.08f, 1, 0.06f, "player_machine"};
        case PickupKind::Silencer: return {0.25f, 2, 0.0f, "player_silencer"};
        default: return {0.15f, 1, 0.0f, "player"};
    }
}
}  // namespace last_stand
