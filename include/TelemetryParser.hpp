#ifndef TELEMETRY_PARSER_HPP
#define TELEMETRY_PARSER_HPP

#include <cstdint>
#include "TelemetryPacket.hpp"

namespace parser {
    enum class ParseResult {
        Ok,
        CrcMismatch,
        ShortRead
    };

    // Decode a 31-byte payload buffer (little-endian) into Telemetry.
    // Returns true on success. Caller already performed CRC check.
    bool decode_payload_le(const uint8_t* buf31, pkt::Telemetry& out);

}

#endif