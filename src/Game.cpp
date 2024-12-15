#include "Game.h"
#include "Player.h"
#include <random>
#include <iostream>
#include <format>
#include <string>
#include <cctype>
#include <unordered_map>
#include <ranges>

Game::Game(size_t amountPlayers, size_t startCards) noexcept {
	players.reserve(amountPlayers);
	for (int i = 0; i < amountPlayers; i++) {
		Player player;
		for (int j = 0; j < startCards; j++) {
			player.addCard(pickupPile.dealCard());
		}
		players.push_back(player);
	}
	currentDiscard = pickupPile.dealCard();
}

bool Game::round() noexcept {
	nextPlayer();
	Player& player = players[currentPlayer];
	DrawCardHandleState handleState = handleDrawCards(player);
	printCurrentPlayerHand(player);
	playCardOrDraw(player);

	switch (handleState) {
	case NEED_PLUS_2:
		if (currentDiscard.getType() == 0x12) break;
		[[fallthrough]];
	case NEED_PLUS_4:
		if (currentDiscard.isBlack() && (currentDiscard.getType() & 0x1E) == 0x02) break;
		forceDrawCardStack(player);
	}
	return player.getHand().size() != 0;
}

const size_t Game::getWinner() noexcept {
	for (size_t i = 0; i < players.size(); i++) {
		if (players.at(currentPlayer).getHand().size() == 0) {
			return i;
		}
	}
	return -1;
}

Game::DrawCardHandleState Game::handleDrawCards(Player& player) noexcept {
	bool plus2 = currentDiscard.getType() == 0x12;
	bool plus4 = currentDiscard.isBlack() && (currentDiscard.getValue() >> 1 == 0x05);
	if (!plus2 && !plus4) {
		return NO_DRAW;
	}

	if (plus2 && (hasPlus2(player) || hasPlus4(player))) {
		drawCardStack += 2;
		return NEED_PLUS_2;
	}

	if (plus4 && hasPlus4(player)) {
		drawCardStack += 4;
		return NEED_PLUS_4;
	}
	drawCardStack += plus2 ? 2 : 4;
	forceDrawCardStack(player);
	return NO_DRAW;
}

void Game::forceDrawCardStack(Player& player) noexcept {
	for (int i = 0; i < drawCardStack; i++) {
		player.addCard(pickupPile.dealCard());
	}
	drawCardStack = 0;
}

bool Game::hasPlus2(Player& player) noexcept {
	auto isPlus2PlayerHand = player.getHand()
		| std::views::filter(&Card::isSpecial)
		| std::views::transform(&Card::getValue);
	return std::find(isPlus2PlayerHand.begin(), isPlus2PlayerHand.end(), 0x02) != isPlus2PlayerHand.end(); // 2nd special card is +2
}

bool Game::hasPlus4(Player& player) noexcept {
	auto isPlus4PlayerHand = player.getHand()
		| std::views::filter(&Card::isBlack)
		| std::views::transform(&Card::getValue)
		| std::views::transform([](const uint8_t& value) { return value >> 1; }); // 1st black card is +4
	return std::find(isPlus4PlayerHand.begin(), isPlus4PlayerHand.end(), 0x01) != isPlus4PlayerHand.end();
}

void Game::nextPlayer() noexcept {
	currentPlayer = (currentPlayer + (clockwise ? 1 : -1)) % players.size();
}

void Game::processCard() noexcept {
	if (currentDiscard.isBlack()) {
		processBlack();
		return;
	}
	if (currentDiscard.getType() == 0x11) {
		clockwise = !clockwise;
	}
	if (currentDiscard.getType() == 0x10) {
		nextPlayer();
	}
}

void Game::processBlack() noexcept {
	while (true) {
		std::cout << "Input the next color [R/Y/G/B]:\n>";
		std::string input;
		std::cin >> input;
		if (input.empty()) {
			continue;
		}
		const std::unordered_map<char, int> colorMap = {{'R', 0},{'Y', 1},{'G', 2},{'B', 3}};
		char inputChar = std::toupper(input.front());
		if (colorMap.count(inputChar)) {
			currentDiscard.setColor(colorMap.at(inputChar));
			return;
		}
	}
}

void Game::playCardOrDraw(Player& player) noexcept { // TODO: Refactor this giant mess
	std::vector<Card>& hand = player.getHand();
	Card playedCard = Card(Card::defaultInvalid);
	size_t index;
	std::string input;

	std::cout << "\n" << std::format("Which card do you want to play? [1-{:d} | D]\n>", hand.size());
	goto first;

	do {
		std::cout << std::format("Played card has to be able to stack on top of current discard!\n>");
	first:
		std::cin >> input;
		if (std::toupper(input.front()) == 'D') {
			Card drawnCard = pickupPile.dealCard();
			std::cout << std::format("Drawing card: {:s}\n", (std::string) drawnCard);
			if (!drawnCard.canStack(currentDiscard)) {
				std::cout << std::format("Card cannot be played!\n");
				player.addCard(drawnCard);
				return;
			}

			bool validInput = false;
			do {
				std::cout << "Do you want to play it? [Y/N]\n>";
				std::string playInput;
				std::cin >> playInput;
				switch (std::toupper(playInput.front())) {
				case 'Y':
					validInput = true;
					index = hand.size(); // size before pushing is last index after pushing
					hand.push_back(drawnCard);
					playedCard = drawnCard;
					break;
				case 'N':
					validInput = true;
					hand.push_back(drawnCard);
					return;
				}
			} while (!validInput);
			break;
		}

		if (input.empty() || std::find_if(input.begin(), input.end(), [](unsigned char c) { return !std::isdigit(c); }) != input.end()) {
			std::cout << std::format("Please enter a valid card number [1-{:d} | D]!\n>", player.getHand().size());
			goto first;
		}
		index = std::stoi(input) - 1;

		if (index < 0 || index >= player.getHand().size()) {
			std::cout << std::format("Please enter a valid card number [1-{:d} | D]!\n>", player.getHand().size());
			goto first;
		}

		playedCard = hand[index];
	} while (!playedCard.canStack(currentDiscard));
	hand.erase(hand.begin() + index);

	std::cout << std::format("Played card #{:d}, {:s}\n", index + 1, (std::string) playedCard);
	currentDiscard = playedCard;
	processCard();
}

void Game::printCurrentPlayerHand(Player& player) noexcept {
	const std::vector<Card>& hand = player.getHand();
	std::cout << std::format("----------------- Current Player: #{:d} -----------------\n", currentPlayer);
	for (int j = 0; j < hand.size(); j++) {
		const Card& card = hand[j];
		std::cout << std::format("Card #{:d}: {:s}\n", j + 1, static_cast<std::string>(card));
	}
	std::cout << std::format("Current discard: {:s}\n", static_cast<std::string>(currentDiscard));
}


Card Deck::dealCard() noexcept {
	uint8_t dealtIndex = (uint8_t) randint(0, deck.count());

	uint8_t setBits = 0;
	for (uint8_t i = 0; i < deck.size(); i++) {
		setBits += deck[i];

		if (setBits > dealtIndex) {
			deck.reset(i);
			return Card::nthCard(i);
		}
	} // TODO: Shuffle
}

static std::random_device rd;
static std::mt19937 rng(rd());

size_t Deck::randint(size_t bottom, size_t top) noexcept {
	std::uniform_int_distribution<size_t> dist(bottom, top - 1); // distribution in range [bottom, top)
	return dist(rng);
}

Deck::Deck() noexcept {
	deck.set();
#ifndef NDEBUG
	rng.seed(69420);
#endif
}
