#pragma once

#include <cstddef>
#include <cstdint>

#include "bitset.hpp"

namespace Tesix {

namespace Style {

// Style bit distribution:
// 24B fg color
// 24B bg color
// 4B  font
// 12B modifiers
//
//   | Modifiers  | |Font| | Background             | | Foreground             |
// 0b|000000000000| |0000| |000000000000000000000000| |000000000000000000000000|
//
// Colors:
// | B      | | G      | | R      |
// |00000000| |00000000| |00000000|
//
// Modifiers:
// In order from the least significant bit
//
// 1. bold (overrides faint)
// 2. faint
// 3. italic
// 4. underlined
// 5. slow blink
// 6. reversed
// 7. strikethrough
// 8.
// 9.
// 10.
// 11.
// 12.

constexpr size_t COLOR_LEN = 24;
constexpr size_t COLOR_COMP_LEN = 8;
constexpr size_t FONT_LEN = 4;

constexpr size_t FG_INDEX = 0;
constexpr size_t BG_INDEX = 24;

constexpr size_t R_OFFSET = 0;
constexpr size_t G_OFFSET = 8;
constexpr size_t B_OFFSET = 16;

inline uint8_t getR(const uint32_t color) {
    return Bitset::getBitRange(color, { .i = R_OFFSET, .len = COLOR_COMP_LEN });
}

inline uint8_t getG(const uint32_t color) {
    return Bitset::getBitRange(color, { .i = G_OFFSET, .len = COLOR_COMP_LEN });
}

inline uint8_t getB(const uint32_t color) {
    return Bitset::getBitRange(color, { .i = B_OFFSET, .len = COLOR_COMP_LEN });
}

inline uint8_t getFG(const uint64_t style) {
    return Bitset::getBitRange(style, { .i = FG_INDEX, .len = COLOR_LEN });
}

inline uint32_t getBG(const uint64_t style) {
    return Bitset::getBitRange(style, { .i = BG_INDEX, .len = COLOR_LEN });
}

inline void setFG(uint64_t& style, const uint32_t val) {
    Bitset::setBitRangeTo(style, val, { .i = 0, .len = 24 });
}

inline void setBG(uint64_t& style, const uint32_t val) {
    Bitset::setBitRangeTo(style, val, { .i = 24, .len = 24 });
}

struct alignas(4) Color {
    uint8_t _r = 0;
    uint8_t _g = 0;
    uint8_t _b = 0;

    Color() = default;

    Color(const uint32_t color) {
        _r = getR(color);
        _g = getG(color);
        _b = getB(color);
    }

    Color(const uint8_t r, const uint8_t g, const uint8_t b) {
        _r = r;
        _g = g;
        _b = b;
    }

    uint32_t raw() {
        return *reinterpret_cast<uint32_t*>(this);
    }
};

constexpr size_t FONT_INDEX = 48;

inline uint8_t getFont(const uint64_t style) {
    return Bitset::getBitRange(style, { .i = FONT_INDEX, .len = FONT_LEN });
}

inline void setFont(uint64_t& style, uint8_t font) {
    Bitset::setBitRangeTo(style, font, { .i = FONT_INDEX, .len = FONT_LEN });
}

enum Modifier {
    BOLD = 0,
    FAINT = 1,
    ITALIC = 2,
    UNDERLINED = 3,
    BLINKING = 4,
    REVERSE = 5,
    STRIKETHROUGH = 6,
};

constexpr size_t MOD_INDEX = 52;

inline bool getMod(const uint64_t style, Modifier mod) {
    return Bitset::getBit(style, MOD_INDEX + mod);
}

inline void setMod(uint64_t& style, Modifier mod) {
    Bitset::setBit(style, MOD_INDEX + mod);
}

inline void unsetMod(uint64_t& style, Modifier mod) {
    Bitset::unsetBit(style, MOD_INDEX + mod);
}

const uint64_t DEFAULT_STYLE = 0b0000000000000000000000000000000000000000111111111111111111111111;

}

}
