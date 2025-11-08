#ifndef TELEMETRY_PACKET_HPP
#define TELEMETRY_PACKET_HPP

#include <cstdint>
#include <cstddef>
#include <cstring>

namespace pkt {
    constexpr std::size_t kPayloadLen = 31; // bytes before CRC
    constexpr std::size_t kPacketLen = 32;  // total bytes

    // Status flags bit layout (1 = true)
    // bit0: SYSTEM_OK, bit1: GPS_LOCK, bit2: PAYLOAD_ACTIVE, bits3-7 reserved
    inline bool system_ok(uint8_t flags)     { return flags & 0x01; }
    inline bool gps_lock(uint8_t flags)      { return flags & 0x02; }
    inline bool payload_active(uint8_t flags){ return flags & 0x04; }

    struct Telemetry {
        uint64_t timestamp_ms;  // little-endian
        float    latitude;      // deg
        float    longitude;     // deg
        float    altitude_m;    // m
        float    temperature_c; // C
        uint16_t sensor_id;
        uint8_t  battery_pct;   // 0..100
        uint8_t  status_flags;  // bitfield
        uint8_t  reserved;      // padding/future
    };
}

#endif