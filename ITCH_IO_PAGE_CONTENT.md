# Last Stand — Itch.io Page Content

## Basic Info
- **Game Title**: Last Stand
- **Short Description**: Top-down arena survival. Kite the horde, shoot what closes in, trade retreat for time while the spawn ramp keeps climbing until you die.
- **Tagline**: Kite the horde. Shoot what closes in. Trade retreat for time.
- **Genre**: Action, Survival, Twin-Stick Shooter, Arcade
- **Status**: Draft — ready to publish (Free)
- **Platforms**: Windows, Linux
- **Price**: Free / "Name your own price" (recommended: $0+)
- **Language**: English

## Tags (Itch.io Categories)
```
twin-stick-shooter, survival, arena-shooter, top-down, action, arcade, horror, zombie, roguelike-elements, free, open-source, cpp, sdl2
```

## Thumbnail / Cover Image Requirements
- **Cover**: 616×353 px (16:9) — Use screenshot.png cropped/resized
- **Thumbnail**: 300×300 px — Center on survivor sprite with vignette glow
- **Screenshots**: 5-8 images, 1280×720 px minimum
  1. Gameplay mid-run (horde visible, minimap showing)
  2. Weapon pickup moment (sprite swap flash)
  3. Minimap + HUD close-up
  4. Death screen / final stats
  5. Title screen / menu
  6. Enemy variety (zombie, robot, old man together)
  7. Weapon comparison (gun / machine / silencer)
  8. Spawn ramp visualization (graphic or late-game chaos)

## Description (Markdown — Paste into Itch.io Description Field)

---

# Last Stand

**A top-down arena survival game built on Storm! Engine v2.**

Kite the horde. Shoot what closes in. Trade retreat for time while the spawn ramp keeps climbing until you die.

---

## Core Loop

> **Move** with WASD → **Aim** with mouse → **Click** to fire → **Get hit** → **Retreat** to regen while the arena fills behind you

Every second you survive, the spawn rate increases. Every second you spend regenerating health, the horde grows thicker. There is no winning — only a high score to chase.

---

## Key Features

<div class="feature-grid">

<div class="feature-card">
<div class="feature-icon">🎯</div>
<div class="feature-title">Continuous Spawn Ramp</div>
<div class="feature-desc">Spawn rate climbs the entire run. Robots join at 2 minutes. Old men later still. The pressure never stops.</div>
</div>

<div class="feature-card">
<div class="feature-icon">❤️</div>
<div class="feature-title">Regen with a Price</div>
<div class="feature-desc">Health regenerates after 3 untouched seconds — but every second waiting is a second the ramp spends spawning.</div>
</div>

<div class="feature-card">
<div class="feature-icon">🧟</div>
<div class="feature-title">Melee-Only Enemies</div>
<div class="feature-title">Zombie, Robot, Old Man</div>
<div class="feature-desc">Each chases, faces target, and pushes apart so crowds read as crowds — not stacks.</div>
</div>

<div class="feature-card">
<div class="feature-icon">🔫</div>
<div class="feature-title">Weapon Pickups</div>
<div class="feature-desc">Gun (balanced), Machine (fast, spread), Silencer (slow, 2× damage). Each swaps fire rate AND survivor sprite.</div>
</div>

<div class="feature-card">
<div class="feature-icon">🗺️</div>
<div class="feature-title">Minimap & HUD</div>
<div class="feature-desc">Health bar, arena outline, player/enemy/pickup dots — drawn after vignette so UI stays bright.</div>
</div>

<div class="feature-card">
<div class="feature-icon">⚡</div>
<div class="feature-title">Pooled Bullets</div>
<div class="feature-desc">128 bullets from a free-list. Inactive ones parked in broadphase cells — never pair with each other.</div>
</div>

</div>

---

## Weapons

<div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(180px, 1fr)); gap: 16px; margin: 24px 0;">

<div class="weapon-card">
<div class="weapon-name">GUN</div>
<div class="weapon-stats">Balanced • 7 RPS • 1 DMG</div>
</div>

<div class="weapon-card">
<div class="weapon-name">MACHINE</div>
<div class="weapon-stats">Fast • 12 RPS • Spread • 1 DMG</div>
</div>

<div class="weapon-card">
<div class="weapon-name">SILENCER</div>
<div class="weapon-stats">Slow • 4 RPS • 2 DMG • Precise</div>
</div>

</div>

---

## Enemies

<div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(180px, 1fr)); gap: 16px; margin: 24px 0;">

<div class="enemy-card">
<div class="enemy-name">ZOMBIE</div>
<div class="weapon-stats">Basic • Spawns from start • Melee</div>
</div>

<div class="enemy-card">
<div class="enemy-name">ROBOT</div>
<div class="weapon-stats">Tougher • Faster • Spawns at 2:00</div>
</div>

<div class="enemy-card">
<div class="enemy-name">OLD MAN</div>
<div class="weapon-stats">Frail • Hits hardest • Spawns late</div>
</div>

</div>

---

## Controls

<table class="controls-table">
<thead>
<tr><th>Input</th><th>Action</th></tr>
</thead>
<tbody>
<tr><td>W / A / S / D</td><td>Move</td></tr>
<tr><td>Mouse</td><td>Aim (survivor faces cursor)</td></tr>
<tr><td>Left Click</td><td>Fire</td></tr>
<tr><td>R</td><td>Restart after death</td></tr>
<tr><td>Esc</td><td>Quit</td></tr>
</tbody>
</table>

**Gamepad (Xbox-style)**: Left stick moves, right stick aims, Right Trigger / A fires, Start / A restarts, D-pad moves when left stick is dead.

---

## Technical Highlights

| Aspect | Detail |
|--------|--------|
| **Language** | C++17 |
| **Engine** | Storm! Engine v2 (SDL2-based ECS) |
| **Architecture** | Pure-function game logic + engine systems |
| **Testing** | 47 BDD specs via igloo (aim, regen, spawn curve, walls, pool, minimap) |
| **Build** | `make` (Linux) / `make -f Makefile.win` (Windows cross-compile) |
| **License** | Public domain (do what you want) |

### Why This Game Exists

*Last Stand* is a load-bearing example for **Storm! Engine v2**. It showcases four engine pieces used as designed:

1. **`ContactSystem::SetPairFilter`** — Rejects bullet-vs-bullet pairs before manifold creation
2. **Uniform-grid broadphase** — Sustained entity churn with ID recycling
3. **Camera-clamped `RenderSystem`** — View drawn through engine render pipeline
4. **`LightingOverlay` key light** — Single light on player at screen centre

Everything else (aim, regen, spawn curve, wall resolution, pool, minimap transform) is pure, testable code with no engine dependency — which is what keeps a horde game honest.

---

## Downloads

| Platform | File | Size |
|----------|------|------|
| **Windows (x64)** | `last-stand-win64.zip` | — |
| **Linux (x64)** | `last-stand-linux.tar.gz` | — |
| **Source** | Included in both / GitHub | — |

> **Linux**: Requires Storm! Engine v2 installed (`sudo make -f Makefile.debian install` in engine repo). Run from game root: `make && make run`
>
> **Windows**: Extract ZIP and run `last-stand.exe`. Includes all DLLs.

---

## Links

- **Source Code**: [GitHub Repository](https://github.com/WillSams/last-stand)
- **Engine**: [Storm! Engine v2](https://github.com/SamsWebs/storm-engine-v2)
- **Issues / Feedback**: [GitHub Issues](https://github.com/WillSams/last-stand/issues)

---

## Credits

- **Engine**: Storm! Engine v2 by WillSams
- **Art**: Kenney.nl asset pack (modified)
- **Font**: Kenney Future (`kenvector_future.ttf`)
- **Testing**: igloo BDD framework

---

*Totally free. Do what you want. Don't blame me if it breaks.*

---

## SEO / Metadata

- **Meta Title**: Last Stand — Free Top-Down Arena Survival (Twin-Stick Shooter)
- **Meta Description**: Kite the horde in Last Stand, a free top-down arena survival game. Continuous spawn ramp, weapon pickups, health regen with a cost. Built on Storm! Engine v2. Windows/Linux.
- **OG Image**: Custom 1200×630 cover with title, tagline, and gameplay screenshot
- **Twitter Card**: summary_large_image

---

## Launch Checklist

- [ ] Upload Windows build (`last-stand-win64.zip`)
- [ ] Upload Linux build (`last-stand-linux.tar.gz`)
- [ ] Set cover image (616×353)
- [ ] Add 5+ screenshots (1280×720+)
- [ ] Paste description (Markdown above)
- [ ] Add tags from list above
- [ ] Set price: "Name your own price" (minimum $0)
- [ ] Enable "Allow downloads" for both files
- [ ] Add custom CSS theme (from ITCH_IO_THEME.md)
- [ ] Publish!

---

## Post-Launch

- [ ] Submit to relevant collections (Twin-Stick Shooters, Free Games, C++ Games)
- [ ] Share on social with #indiedev #gamdev #twinstickshooter
- [ ] Add link to GitHub repo in description
- [ ] Respond to comments / feedback within 24h
- [ ] Consider adding a devlog post about the spawn ramp design