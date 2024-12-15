#include "Player.h"
#include <algorithm>

void Player::addCard(Card card) noexcept {
	hand.push_back(card);
	std::sort(hand.begin(), hand.end());
}

std::vector<Card>& Player::getHand() noexcept { // TODO: This is unsafe, make it safe
	return hand;
}

Card Player::playCard(size_t index) noexcept {
	return *hand.erase(hand.begin() + index);
}

