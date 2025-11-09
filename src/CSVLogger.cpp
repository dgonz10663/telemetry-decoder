#include "CSVLogger.hpp"
#include <iomanip>
#include <filesystem>

/*
    CSVLogger.cpp
    Handles creation and appending of decoded telemetry records to a CSV file.
    Automatically manages headers and enforces consistent formatting.
*/

CSVLogger::CSVLogger(const std::string& path, bool append) {
    using std::ios;
    auto openmode = ios::binary | ios::out | (append ? ios::app : ios::trunc);
    this->out.open(path, openmode);

    if (!this->out) return;

    bool need_header = true;
    if (append) {
        // If appending, only write header if file is empty
        std::error_code ec;
        auto sz = std::filesystem::file_size(path, ec);
        if (!ec && sz > 0) need_header = false;
    }

    if (need_header) this->write_header_if_needed();
}

bool CSVLogger::ok() const {return this->out.good();}

void CSVLogger::write_header_if_needed() {
    if (this->wrote_header || !this->out) return;
    this->out << "timestamp_ms,latitude,longitude,altitude_m,temperature_c,"
            "sensor_id,battery_pct,status_flags\n";
    this->out.flush();
    this->wrote_header = true;
}

void CSVLogger::write_row(const pkt::Telemetry& t) {
    if (!this->out) return;
    // Fixed, human-friendly formatting for floats
    this->out << t.timestamp_ms << ','
         << std::fixed << std::setprecision(6) << t.latitude  << ','
         << std::fixed << std::setprecision(6) << t.longitude << ','
         << std::fixed << std::setprecision(2) << t.altitude_m << ','
         << std::fixed << std::setprecision(2) << t.temperature_c << ','
         << t.sensor_id << ','
         << static_cast<int>(t.battery_pct) << ','
         << static_cast<int>(t.status_flags)
         << '\n';
    this->out.flush(); // keep file current; safe for small logs
}