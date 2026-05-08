#include "game.h"

using namespace std;

int main() {
  Game game;
  string testName = "tests/Example4.txt";
  bool reset = false;

  game.init(testName);
  game.start();

  if (reset) {
    game.reset();
  }

  game.exit();
}
