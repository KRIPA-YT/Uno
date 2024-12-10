#pragma once
#include <stdint.h>
#include <string>
#include <assert.h>

class Card {
public:
	constexpr static inline uint8_t cardsPerColor = 25; // 1x 0's, 2x 1-9's, 2x Block, 2x Reverse, 2x +2
	constexpr static inline uint8_t colors = 4;
	constexpr static inline uint8_t maxCards = cardsPerColor * colors + 2 * 4;
	uint8_t id;
	
	constexpr inline Card(uint8_t id) : id(id) {}

	constexpr inline const uint8_t getColor() const noexcept {
		return (0xC0 & id) >> 6;
	}

	constexpr inline const bool isSpecial() const noexcept {
		return (0x20 & id) >> 5;
	}

	constexpr inline const uint8_t getValue() const noexcept {
		return (0x1E & id) >> 1;
	}

	constexpr inline const bool isBlack() const noexcept {
		return (0xF0 & id) == 0xF0;
	}

	constexpr inline const bool isFirst() const noexcept {
		return (0x01 & id);
	}

	constexpr inline void setColor(uint8_t color) noexcept {
		id |= (color & 0x03) << 6;
	}

	constexpr inline void setSpecial(bool special) noexcept {
		id |= special << 5;
	}

	constexpr inline void setValue(uint8_t value) noexcept {
		id |= (value & 0x0F) << 1;
	}

	constexpr inline void setFirst(bool first) noexcept {
		id |= first << 0;
	}

	constexpr inline const bool operator<(const Card& other) const noexcept {
		return id < other.id;
	}

	constexpr inline const bool operator==(const Card& other) const noexcept {
		return (other.id & (0xFC | !other.isBlack() << 1)) == (id & (0xFC | !isBlack() << 1));
	}

	constexpr inline const bool canStack(const Card& stackedOn) const noexcept {
		return (stackedOn.getColor() == getColor()) || (stackedOn.id & (0x3C | !stackedOn.isBlack() << 1)) == (id & (0x3C | !isBlack() << 1));
	}

	static constexpr inline const char colorToString(const uint8_t color) noexcept {
		constexpr char colorKeys[] = {'r', 'y', 'g', 'b'};
		return colorKeys[color & 0x03];
	}

	static constexpr inline const Card nthCard(const uint8_t n) noexcept {
		assert(n < maxCards && "n cannot be bigger than the maximum available cards");
		if (n < cardsPerColor * colors) {
			uint8_t cardColor = n / cardsPerColor;
			uint8_t cardIndex = n % cardsPerColor;
			uint8_t zeroShiftedIndex = cardIndex == 0 ? 0 : cardIndex + 1;
			uint8_t specialShiftedIndex = zeroShiftedIndex <= 0x13 ? zeroShiftedIndex : zeroShiftedIndex + 0xC;
			return Card(specialShiftedIndex | cardColor << 6);
		}
		uint8_t nthBlack = n - 0x34;
		return Card(nthBlack | 0xE0);
	}
};