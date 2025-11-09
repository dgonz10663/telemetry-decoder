#ifndef CSV_LOGGER_HPP
#define CSV_LOGGER_HPP

#include <fstream>
#include <string>
#include "TelemetryPacket.hpp"

class CSVLogger {
    public:
        // If append == false, overwrite file and write header
        // If append == true, append rows; writes header only if file is empty  
        explicit CSVLogger(const std::string& path, bool append = false);

        // True if stream is open and healthy
        bool ok() const;

        // Write one telemetry row
        void write_row(const pkt::Telemetry& t);

    private:
        std::ofstream out;
        bool wrote_header = false;
        void write_header_if_needed();
};

#endif