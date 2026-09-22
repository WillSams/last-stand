#include <igloo/igloo_alt.h>

#include "../src/input/menu.h"

using namespace igloo;
using last_stand::MenuAction;
using last_stand::MenuActionFor;
using last_stand::MenuInput;

Describe(MenuInputSpec) {

  It(does_nothing_on_an_empty_frame) {
    Assert::That(MenuActionFor(MenuInput{}), Equals(MenuAction::None));
  };

  It(starts_on_enter) {
    MenuInput in;
    in.enter = true;
    Assert::That(MenuActionFor(in), Equals(MenuAction::Start));
  };

  It(starts_on_the_gamepad_start_button) {
    MenuInput in;
    in.padStart = true;
    Assert::That(MenuActionFor(in), Equals(MenuAction::Start));
  };

  It(starts_on_the_gamepad_a_button) {
    MenuInput in;
    in.padA = true;
    Assert::That(MenuActionFor(in), Equals(MenuAction::Start));
  };

  It(quits_on_escape) {
    MenuInput in;
    in.esc = true;
    Assert::That(MenuActionFor(in), Equals(MenuAction::Quit));
  };

  It(prefers_quit_when_start_and_escape_arrive_together) {
    MenuInput in;
    in.enter = true;
    in.esc = true;
    Assert::That(MenuActionFor(in), Equals(MenuAction::Quit));
  };
};
