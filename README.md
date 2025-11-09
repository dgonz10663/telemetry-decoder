# Telemetry Decoder: Modular C++ System for Drone Telemetry

A modular C++ telemetry decoding system that reads fixed-length binary packets from an ISR drone, 
validates each with a CRC-8 checksum, parses the decoded fields (timestamp, GPS, altitude, temperature, flags), 
and outputs the data to both the console and a CSV log file. Designed for aerospace-style simulation scenarios.

---

# Features (planned by milestones)
- Binary packet parsing (little-endian, 32 bytes)
- CRC-8 validation (polynomial 0x07)
- Console + CSV output with automatic headers
- CLI flags: `--input`, `--output`, `--verbose`, 
- Modular, testable architecture

# Future implementation
Real time serial or UDP input
JSON or SQLite logging
Cross-platform GUI for live telemetry

---

# Build and Run (MSYS2 MinGW64 / Linux / WSL)
Requirements: `g++`, `make`

```bash
make clean           
make 

# Usage
./bin/telemetry-decoder --gen-sample data/sample_packets.bin 10 # generate N packets
./bin/telemetry-decoder --input data/sample_packets.bin --output outputs/decoded_log.csv --verbose # Decode binary file and write CSV
make test # Run unit tests

