#pragma once

#include <algorithm>
#include <cstddef>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <limits>

namespace Tesix {

namespace Bitset {

template<typename T> constexpr size_t countBits() {
    return sizeof(T) * 8;
}

struct Range {
    size_t i;
    size_t len;
};

template<typename T> inline bool isValidBitRange(const Range range) {
    return countBits<T>() >= range.i + range.len;
}

template<typename T> inline bool isValidBitIndex(const size_t i) {
    return countBits<T>() > i;
}

template<typename T> inline size_t remainingBits(const size_t bits) {
    return countBits<T>() - bits;
}

template<typename T = size_t> constexpr inline T setMask() {
    return std::numeric_limits<T>::max();
}

constexpr inline size_t unsetMask() {
    return 0;
}

constexpr inline size_t bitMask(const size_t i) {
    assert(isValidBitIndex<size_t>(i));

    return 0b1 << i;
}

/**
 * @brief creates a bit mask that has the first n bits set
 **/
inline size_t firstBitsMask(const size_t n) {
    assert(countBits<size_t>() >= n);

    return (n == countBits<size_t>()) ? setMask() : ((size_t)0b1 << n) - 1;
}

/**
 * @brief shifts to the left, but allows to shift out an entire number
 */
inline size_t lShiftExactOverflow(const size_t bits, const size_t shift) {
    assert(isValidBitRange<size_t>({ .i = 0, .len = shift}));

    return (shift == countBits<size_t>()) ? unsetMask() : (bits << shift);
}

/**
 * @brief shifts to the right, but allows to shift out an entire number
 */
inline size_t rShiftExactOverflow(const size_t bits, const size_t shift) {
    assert(isValidBitRange<size_t>({ .i = 0, .len = shift}));

    return (shift == countBits<size_t>()) ? unsetMask() : (bits >> shift);
}


inline size_t leftCutoffMask(const size_t lbits) {
    assert(countBits<size_t>() >= lbits);

    return firstBitsMask(remainingBits<size_t>(lbits));
}

inline size_t cutoff(const size_t bits, const size_t lbits, const size_t rbits) {
    assert(countBits<size_t>() >= lbits + rbits);

    return rShiftExactOverflow(lShiftExactOverflow(bits, lbits), lbits + rbits);
}

inline size_t cutoffLeft(const size_t bits, const size_t lbits) {
    assert(countBits<size_t>() >= lbits);

    return bits & leftCutoffMask(lbits); 
}

inline size_t cutoffRight(const size_t bits, const size_t rbits) {
    assert(countBits<size_t>() >= rbits);

    return rShiftExactOverflow(bits, rbits);
}

inline size_t truncate(const size_t  bits, const size_t count) {
    assert(countBits<size_t>() >= count);

    return bits & firstBitsMask(count);
}

template<typename T = size_t> inline T patternMask(const size_t pattern, const size_t pattern_len,  size_t len = std::numeric_limits<size_t>::max()) {
    len = std::min(len, countBits<T>());

    const size_t full_repeats = len / pattern_len;

    T ret = 0;

    for(size_t i = 0; i < full_repeats; i++) {
        ret |= pattern << i * pattern_len;
    }

    ret |= truncate(pattern, len - (full_repeats * pattern_len)) << full_repeats * pattern_len;

    return ret;
}

inline size_t rangeMask(const Range range) {
    assert(isValidBitRange<size_t>(range));

    size_t lbits = countBits<size_t>() - (range.i + range.len);
    size_t rbits = range.i;

    return cutoff(setMask(), lbits, rbits) << range.i;
}

/**
* @brief Takes the first range.len bits and moves them to the range
*/
inline size_t valueMask(const size_t value, const Range range) {
    size_t lbits = countBits<size_t>() - (range.i + range.len);

    return cutoff(value, lbits, 0) << range.i;
}

inline bool getBit(const size_t bits, const size_t i) {
    assert(isValidBitIndex<size_t>(i));

    const size_t lbits = countBits<size_t>() - (i + 1);
    const size_t rbits = i;

    return cutoff(bits, lbits, rbits);
}

inline size_t getBitRange(const size_t bits, const Range range) {
    assert(isValidBitRange<size_t>(range));

    const size_t lbits = countBits<size_t>() - (range.i + range.len); 
    const size_t rbits = range.i;

    return cutoff(bits, lbits, rbits);
}

template<typename T> inline void setBit(T& bits, const size_t i) {
    assert(isValidBitIndex<T>(i));

    bits |= bitMask(i);
}

template<typename T> inline void unsetBit(T& bits, const size_t i){
    assert(isValidBitIndex<T>(i));

    bits &= ~bitMask(i);
}

template<typename T> inline void setBitRange(T& bits, const Range range) {
    assert(isValidBitRange<T>(range));

    bits |= rangeMask(range);
}

template<typename T> inline void unsetBitRange(T& bits, const Range range) {
    assert(isValidBitRange<T>(range));

    bits &= ~rangeMask(range);
}

/**
 * @brief sets the bits after the bit with the index of 'offset' to the given value
 * 
 * @param bits
 * @param put    The value the bits will be set to
 * @param offset
 */
template<typename T> inline void putBits(T& bits, const size_t put, const size_t offset) {
    assert(countBits<T>() >= offset);

    bits = truncate(bits, offset);
    bits |= lShiftExactOverflow(put, offset);
}

template<typename T> inline void setBitRangeTo(T& bits, const size_t to, const Range range) {
    assert(isValidBitRange<T>(range));

    unsetBitRange(bits, range);
    bits |= valueMask(to, range);
}

}

// TODO: Implement
namespace ArrBitset {

using namespace Bitset;

inline size_t countChunks(const size_t byte_c) {
    return byte_c / sizeof(size_t); 
}

inline size_t& getChunk(uint8_t* bytes, const size_t byte_c, const size_t i) {
    assert(i <= countChunks(byte_c));

    return reinterpret_cast<size_t&>(bytes[i * sizeof(size_t)]);
}

inline size_t getByteBitIndex(const size_t byte_i) {
    return byte_i * countBits<uint8_t>();
}

inline size_t getByteIndex(const size_t i) {
    return i / countBits<uint8_t>();
}

inline size_t getChunkIndex(const size_t i) {
    return i / countBits<size_t>();
}

/**
 * @brief gets the bit index relative to the byte index
 * @param byte_i the global index of the byte
 * @param bit_i  the global index of the bit
 */
constexpr size_t getRelativeBitIndex(const size_t byte_i, const size_t bit_i) {
    assert(bit_i >= getByteBitIndex(byte_i));

    return bit_i - getByteBitIndex(byte_i);
}

/**
 * @brief gets a range of bits inside a byte array without chunks
 * @param bytes The byte buffer to extract from
 * @param byte_c The lenght of the buffer
 * @param range The range of bits to get
 */
size_t getBitsSmallChunk(const uint8_t* bytes, const size_t byte_c, const Range range) {
    const size_t end = std::min(range.i + range.len, range.i + countBits<size_t>());
    assert(byte_c * 8 > end);


    const size_t start_i = getByteIndex(range.i);
    const size_t end_i   = getByteIndex(range.i + range.len);


    size_t i = 0;

    size_t ret = 0;

    size_t rel_start_bit = getRelativeBitIndex(start_i, range.i); 
    putBits(ret, cutoffRight(bytes[start_i], rel_start_bit), 0);

    i += remainingBits<uint8_t>(rel_start_bit); 

    for(size_t byte_i = start_i + 1; byte_i < end_i; byte_i++) {
        putBits(ret, bytes[byte_i], i);

        i += 8;

        if(i + 8 > countBits<size_t>()) {
            break;
        }
    }

    const size_t end_bit_count = std::min(range.len, countBits<size_t>()) - i;
    putBits(ret, cutoffLeft(bytes[end_i], remainingBits<size_t>(end_bit_count)), i);

    return ret;
}

/**
 * @brief get a range of bits out of two chunks
 */
size_t getBitsChunk(const uint8_t* bytes, const Range range) {
    return 0;
}

size_t getBitsArr(const uint8_t* bytes, const Range range) {
    return 0;
}

}

}
