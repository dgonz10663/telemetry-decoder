#include <iostream>
#include <vector>
#include <cassert>
#include "../include/CRC8.hpp"
#include "../include/TelemetryParser.hpp"
#include "../include/TelemetryPacket.hpp"
#include "../include/CSVLogger.hpp"

using namespace std;

static void test_crc8() {
    const uint8_t data1[] = {0x00, 0x01, 0x02, 0x03};
    uint8_t crc = crc8::compute(data1, sizeof(data1));
    cout << "[CRC8] " << (crc == 0x48 ? "Pass" : "Fail") << "  computed=" << (int)crc << "\n";
}

static void test_parser() {
    pkt::Telemetry t{};
    vector<uint8_t> b(pkt::kPayloadLen, 0);
    uint64_t ts = 1700000000000ULL;
    memcpy(b.data(), &ts, 8);
    parser::decode_payload_le(b.data(), t);
    cout << "[Parser] " << (t.timestamp_ms == ts ? "Pass" : "Fail") << "\n";
}

static void test_csv() {
    CSVLogger csv("outputs/test_log.csv", false);
    pkt::Telemetry t{};
    t.timestamp_ms = 1;
    t.latitude = 2.2f;
    t.longitude = 3.3f;
    t.altitude_m = 4.4f;
    t.temperature_c = 5.5f;
    t.sensor_id = 6;
    t.battery_pct = 7;
    t.status_flags = 3;
    csv.write_row(t);
    cout << "[CSVLogger] " << (csv.ok() ? "Pass" : "Fail") << "\n";
}

int main() {
    cout << "Running unit tests...\n";
    test_crc8();
    test_parser();
    test_csv();
    cout << "All tests complete.\n";
    return 0;
}
