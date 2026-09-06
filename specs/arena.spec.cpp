#include <igloo/igloo_alt.h>

#include "../src/arena/arena.h"

using namespace igloo;

namespace {

// A 5x4 arena: solid border, one solid interior tile at (2,2). The map is in
// the editor format so the Arena constructor needs no SDL/PNG — TileMapLoader
// parses srcX/srcY and the collider flag straight from the lines.
constexpr const char *kMap = "./specs/fixtures/arena-test.map";
constexpr int kTile = 64;

} // namespace

Describe(ArenaSpec) {

  It(reports_the_pixel_size_from_the_grid_dimensions) {
    const Arena arena(kMap, kTile);
    Assert::That(arena.PixelSize().x, Equals(5.0f * kTile));  // 320
    Assert::That(arena.PixelSize().y, Equals(4.0f * kTile));  // 256
  };

  It(marks_the_border_as_solid) {
    const Arena arena(kMap, kTile);
    Assert::That(arena.IsSolid(0, 0), Is().True());
    Assert::That(arena.IsSolid(4, 3), Is().True());
    Assert::That(arena.IsSolid(2, 0), Is().True());  // top edge
    Assert::That(arena.IsSolid(0, 2), Is().True());  // left edge
  };

  It(marks_the_interior_wall_as_solid) {
    const Arena arena(kMap, kTile);
    Assert::That(arena.IsSolid(2, 2), Is().True());
  };

  It(leaves_open_floor_as_not_solid) {
    const Arena arena(kMap, kTile);
    Assert::That(arena.IsSolid(1, 1), Is().False());
    Assert::That(arena.IsSolid(3, 2), Is().False());
  };

  It(treats_out_of_range_as_solid_so_bodies_cannot_escape) {
    const Arena arena(kMap, kTile);
    Assert::That(arena.IsSolid(-1, 0), Is().True());
    Assert::That(arena.IsSolid(0, -1), Is().True());
    Assert::That(arena.IsSolid(5, 0), Is().True());
    Assert::That(arena.IsSolid(0, 4), Is().True());
  };
};