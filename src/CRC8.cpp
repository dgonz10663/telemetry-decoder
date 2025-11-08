#include "CRC8.hpp"


namespace crc8 {

    uint8_t compute(const uint8_t* data, std::size_t len) {
        uint8_t crc = 0x00;        // Initial value
        const uint8_t poly = 0x07;   // x^8 + x^2 + x + 1

        for (std::size_t i = 0; i < len; ++i) {
            crc ^= data[i];
            for (int b = 0; b < 8; ++b) {
                if (crc & 0x80) {
                    crc = static_cast<uint8_t>((crc << 1) ^ poly);
                }
                else {
                    crc <<= 1;
                }
            }
        }
        return crc;
    }

} // namespace crc8