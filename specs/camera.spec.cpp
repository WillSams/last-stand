#include <igloo/igloo_alt.h>

#include "../src/arena/camera.h"

using namespace igloo;

Describe(CameraTopLeftSpec) {

  It(centres_the_view_on_the_target) {
    const glm::ivec2 topLeft = CameraTopLeft({400, 300}, 800, 600,
                                             {2368, 1792});
    Assert::That(topLeft.x, Equals(0));
    Assert::That(topLeft.y, Equals(0));
  };

  It(clamps_to_zero_at_the_top_left_corner) {
    const glm::ivec2 topLeft = CameraTopLeft({0, 0}, 800, 600, {2368, 1792});
    Assert::That(topLeft.x, Equals(0));
    Assert::That(topLeft.y, Equals(0));
  };

  It(clamps_to_the_world_edge_at_the_bottom_right) {
    const glm::ivec2 topLeft = CameraTopLeft({2368, 1792}, 800, 600,
                                             {2368, 1792});
    Assert::That(topLeft.x, Equals(1568));  // 2368 - 800
    Assert::That(topLeft.y, Equals(1192));  // 1792 - 600
  };

  It(pins_to_zero_when_the_world_is_smaller_than_the_view) {
    const glm::ivec2 topLeft = CameraTopLeft({0, 0}, 800, 600, {400, 300});
    Assert::That(topLeft.x, Equals(0));
    Assert::That(topLeft.y, Equals(0));
  };

  It(tracks_a_target_partway_across_the_world) {
    const glm::ivec2 topLeft = CameraTopLeft({800, 600}, 800, 600,
                                             {2368, 1792});
    Assert::That(topLeft.x, Equals(400));
    Assert::That(topLeft.y, Equals(300));
  };
};