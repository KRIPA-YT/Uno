#pragma once
#include <vector>
#include "Card.h"

class Player {
public:
	void addCard(Card) noexcept;
	std::vector<Card>& getHand() noexcept;
	Card playCard(size_t index) noexcept;
private:
	std::vector<Card> hand;
};