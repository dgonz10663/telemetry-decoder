#include "TelemetryParser.hpp"
#include <cstring>   // memcpy, standard safe way to parse binary in cpp

/* 
    TelemetryParser.cpp
    Converts 31-byte binary payloads into structured Telemetry objects.
    All reads assume little-endian byte order for multi-byte fields.
*/


namespace parser {
    static void read_u64_le(const uint8_t* p, uint64_t& v) { std::memcpy(&v, p, 8); }
    static void read_u16_le(const uint8_t* p, uint16_t& v) { std::memcpy(&v, p, 2); }
    static void read_f32_le(const uint8_t* p, float& v)    { std::memcpy(&v, p, 4); }

    // Assumes little-endian host (MSYS2 on Windows is little-endian). For a big-endian
    // host, you would byte-swap after memcpy. We keep it simple here.
    bool decode_payload_le(const uint8_t* b, pkt::Telemetry& out) {
        std::size_t o = 0;

        read_u64_le(b + o, out.timestamp_ms); o += 8;
        read_f32_le(b + o, out.latitude);     o += 4;
        read_f32_le(b + o, out.longitude);    o += 4;
        read_f32_le(b + o, out.altitude_m);   o += 4;
        read_f32_le(b + o, out.temperature_c);o += 4;
        read_u16_le(b + o, out.sensor_id);    o += 2;
        out.battery_pct  = *(b + o);          o += 1;
        out.status_flags = *(b + o);          o += 1;
        out.reserved     = *(b + o);          o += 1;
        // o should be 31
        return true;
    }

}