#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <cstdlib>

#include "CRC8.hpp"
#include "TelemetryPacket.hpp"
#include "TelemetryParser.hpp"
#include "CSVLogger.hpp"

// Prints program usage instructions
static void print_help() {
    std::cout <<
    R"(Telemetry Decoder Usage:
        telemetry_decoder --help
        telemetry_decoder --crc-selftest
        telemetry_decoder --gen-sample <path> <N>
        telemetry_decoder --input <file> [--output <csv>] [--append] [--verbose]

    Notes:
        --gen-sample writes N example packets to <path>.
        --input reads first packet, verifies CRC, and decodes to console.
    )";
}
static bool run_crc_selftest() {
    // Standard check vectory for CRC-8 (poly 0x07, init 0x00): "123456789" -> 0xF4
    const char* s = "123456789";
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(s);
    uint8_t crc = crc8::compute(bytes, 9);
    std::cout << "CRC-8(\"123456789\") = 0x" << std::hex << (int)crc << std::dec << "\n";
    return crc == 0xF4;
}
static bool write_sample_packets(const std::string& path, int N) {
    if (N <= 0) return 0;

    std::ofstream out(path, std::ios::binary);
    if(!out) {
        std::cerr << "Error: cannot open " << path << " for writing\n";
        return false;
    }

    for (int i = 0; i < N; ++i) {
        // Build 31-byte payload
        std::vector<uint8_t> pay(pkt::kPayloadLen);
        std::size_t o = 0;

        // Deterministic sample values
        uint64_t ts   = 1700000000000ULL + static_cast<uint64_t>(i)*100; // ms
        float lat     = 37.400f + 0.001f * i;
        float lon     = -122.000f - 0.001f * i;
        float alt     = 120.0f + i;
        float temp    = 22.0f  + 0.1f * i;
        uint16_t sid  = static_cast<uint16_t>(1 + (i % 10));
        uint8_t batt  = static_cast<uint8_t>(100 - i % 50);
        uint8_t flags = 0x03; // SYSTEM_OK + GPS_LOCK
        uint8_t rsv   = 0x00;

        std::memcpy(pay.data() + o, &ts,   8); o += 8;
        std::memcpy(pay.data() + o, &lat,  4); o += 4;
        std::memcpy(pay.data() + o, &lon,  4); o += 4;
        std::memcpy(pay.data() + o, &alt,  4); o += 4;
        std::memcpy(pay.data() + o, &temp, 4); o += 4;
        std::memcpy(pay.data() + o, &sid,  2); o += 2;
        pay[o++] = batt;
        pay[o++] = flags;
        pay[o++] = rsv;

        // CRC over 31 bytes
        uint8_t c = crc8::compute(pay.data(), pkt::kPayloadLen);

        // Write payload + CRC
        out.write(reinterpret_cast<const char*>(pay.data()), pkt::kPayloadLen);
        out.put(static_cast<char>(c));
    }

    std::cout << "Wrote " << N << " packet(s) to " << path << "\n";
    return true;
}

static int decode_all_packets(const std::string& inPath,
                              const std::string& outCSV,
                              bool append_csv,          
                              bool verbose) 
{
    std::ifstream in(inPath, std::ios::binary);
    if (!in) { std::cerr << "Error: cannot open " << inPath << " for reading\n"; return 2; }

    CSVLogger csv(outCSV, append_csv);
    if (!csv.ok()) { std::cerr << "Error: cannot open CSV " << outCSV << "\n"; return 2; }

    std::vector<uint8_t> buf(pkt::kPacketLen);
    std::size_t total = 0, good = 0, bad_crc = 0, short_reads = 0;

    while (true) {
        in.read(reinterpret_cast<char*>(buf.data()), pkt::kPacketLen);
        std::size_t got = static_cast<std::size_t>(in.gcount());
        if (got == 0) break; // EOF
        if (got < pkt::kPacketLen) {
            std::cerr << "Short packet at tail: " << got << " bytes\n";
            ++short_reads;
            break;
        }

        ++total;

        // Split payload and CRC
        const uint8_t* payload = buf.data();
        uint8_t stored_crc = buf[pkt::kPayloadLen];
        uint8_t calc_crc   = crc8::compute(payload, pkt::kPayloadLen);

        if (verbose) {
            std::cout << "[pkt " << total << "] stored=0x" << std::hex << (int)stored_crc
                << " calc=0x" << (int)calc_crc << std::dec << "\n"; 
        }
        
        if (stored_crc != calc_crc) {
            ++bad_crc;
            std::cerr << "CRC mismatch, skipping\n";
            continue;
        }

        pkt::Telemetry t{};
        if (!parser::decode_payload_le(payload, t)) {
            if (verbose) std::cout << "Decode failed, skipping\n";
            continue;
        }
        
        // Console one-line summary
        std::cout << "ts=" << t.timestamp_ms
                  << " lat=" << t.latitude
                  << " lon=" << t.longitude
                  << " alt=" << t.altitude_m
                  << " temp=" << t.temperature_c
                  << " batt=" << (int)t.battery_pct
                  << " flags=" << (int)t.status_flags
                  << "\n";

        // CSV row
        csv.write_row(t);
        ++good;
    }

    std::cout << "Summary: total=" << total
              << " good=" << good
              << " bad_crc=" << bad_crc
              << " short_reads=" << short_reads
              << "\n";

    // Exit code: 0 if we decoded at least one good packet, else non-zero
    return (good > 0) ? 0 : 1;

}

int main(int argc, char** argv) {
    // If no arguments, call help
    if (argc == 1) { print_help(); return 0;}

    std::string arg1 = argv[1];
    if (arg1 == "--help" || arg1 == "-h") {print_help(); return 0;}

    else if (arg1 == "--crc-selftest") {
        bool test = run_crc_selftest();
        std::cout << (test ? "CRC self-test: PASS\n" : "CRC self-test: FAIL\n");
        return test ? 0 : 1;
    }

    if (arg1 == "--gen-sample") {
        if (argc < 4) { std::cerr << "Usage: --gen-sample <path> <N>\n"; return 1; }
        std::string outPath = argv[2];
        int N = std::atoi(argv[3]);
        return write_sample_packets(outPath, N) ? 0 : 1;
    }


    if (arg1 == "--input") {
        if (argc < 3) { std::cerr << "Usage: --input <file> [--output <csv>] [--append] [--verbose]\n"; return 1; }
        std::string inPath = argv[2];
        std::string outCSV = "outputs/decoded_log.csv";
        bool verbose = false;
        bool append = false;

        for (int i = 3; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--verbose") verbose = true;
            else if (arg == "--append") append = true;
            else if (arg == "--output" && i + 1 < argc) { outCSV = argv[++i]; }
            else { std::cerr << "Unknown or incomplete option: " << arg << "\n"; return 1; }
        }
        return decode_all_packets(inPath, outCSV, append, verbose);
    }

    return 0;
}