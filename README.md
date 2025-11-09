# Telemetry Decoder: Modular C++ System for Drone Telemetry

A modular C++ telemetry decoding system that reads fixed-length binary packets from an ISR drone, 
validates each with a CRC-8 checksum, parses the decoded fields (timestamp, GPS, altitude, temperature, flags), 
and outputs the data to both the console and a CSV log file.

---

# Features (planned by milestones)
- Binary packet parsing (little-endian, 32 bytes)
- CRC-8 validation (polynomial 0x07)
- Console + CSV output
- CLI flags: `--input`, `--output`, `--verbose`
- Unit tests for parsing and CRC (later)

---

# Build and Run (MSYS2 MinGW64 / Linux / WSL)
Requirements: `g++`, `make`

```bash
make            # build executable
make run        # quick --help check
./bin/telemetry-decoder --help
./bin/telemetry-decoder --input data/sample_packets.bin --output outputs/decoded_log.csv

