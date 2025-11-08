#ifndef CRC8_HPP
#define CRC8_HPP
#include <cstdint>

namespace crc8{
    // CRC-8, polynomial 0x07, init 0x00, no final XOR, no reflection
    // This matches common "CRC-8-ATM/CRC-8-CCITT" style parameters (non-reflected).
    uint8_t compute(const uint8_t* data, std::size_t len);
}

#endif 