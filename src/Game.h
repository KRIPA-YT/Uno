#pragma once
#include <stdint.h>
#include <vector>
#include <bitset>
#include "Card.h"
#include "Player.h"

class Deck {
private:
	std::bitset<Card::maxCards> deck;

	static size_t randint(size_t bottom, size_t top) noexcept;
public:
	Deck() noexcept;

	Card dealCard() noexcept;
};

class Game {
public:
	Game(size_t amountPlayers, size_t startCards = 5) noexcept;

	bool round() noexcept;

	const size_t getWinner() noexcept;

private:
	enum DrawCardHandleState {
		NO_DRAW, NEED_PLUS_2, NEED_PLUS_4
	};

	std::vector<Player> players;
	size_t currentPlayer = -1;
	uint8_t drawCardStack = 0;
	bool clockwise = true;
	Deck pickupPile = Deck();
	Card currentDiscard = Card(0x00);

	DrawCardHandleState handleDrawCards(Player& player) noexcept;

	void forceDrawCardStack(Player& player) noexcept;

	bool hasPlus2(Player& player) noexcept;
	bool hasPlus4(Player& player) noexcept;

	void nextPlayer() noexcept;

	void processCard() noexcept;

	void processBlack() noexcept;

	void playCardOrDraw(Player& player) noexcept;

	void printCurrentPlayerHand(Player& player) noexcept;
};