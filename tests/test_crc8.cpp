#include <cassert>
#include <cstdint>
#include <iostream>
#include "CRC8.hpp"

int main() {
    const char* s = "123456789";
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(s);
    uint8_t crc = crc8::compute(bytes, 9);
    std::cout << "Test vector CRC = 0x" << std::hex << (int)crc << std::dec << "\n";
    assert(crc == 0xF4 && "CRC-8 check vector failed");
    std::cout << "All CRC tests passed.\n";
    return 0;
}
