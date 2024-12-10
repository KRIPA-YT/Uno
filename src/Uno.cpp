#include "Uno.h"
#include <format>

int main() {
	for (int i = 0; i < Card::maxCards; i++) {
		Card card = Card::nthCard(i);
		std::cout << std::format("id: {:x}, c: {:c}, s: {:b}, v: {:d}, #{:d}, b: {:b}\n", card.id, Card::colorToString(card.getColor()), card.isSpecial(), card.getValue(), card.isFirst(), card.isBlack());
	}
	return 0;
}