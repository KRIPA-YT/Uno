#include "Uno.h"
#include <format>
#include "Game.h"

int main() {
	Game game(4);
	while (game.round());
	std::cout << std::format("Winner: {:d}\n", game.getWinner());

	return 0;
}