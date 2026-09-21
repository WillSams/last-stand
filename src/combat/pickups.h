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

// One roll decides both whether a pickup drops and which one, so the drop
// table is spec'd without a registry. ~1 in 8 deaths drops. The gate reads the
// low bits and the kind the high bits: selecting with `roll % 4` would always
// name Gun, because a drop is `roll % 8 == 0`. Gun common, machine and
// silencer rarer, health rarest.
//
// Health is in the table on purpose: the old inline selector was `roll % 3`,
// which can never name PickupKind::Health (3), so the full-heal branch in
// ApplyContacts was dead code.
inline bool RollDrop(unsigned roll, PickupKind &out) {
    if (roll % 8u != 0u)
        return false;
    switch ((roll >> 8) % 8u) {
        case 3: case 4: out = PickupKind::Machine; break;
        case 5: case 6: out = PickupKind::Silencer; break;
        case 7: out = PickupKind::Health; break;
        default: out = PickupKind::Gun; break;
    }
    return true;
}
}  // namespace last_stand
