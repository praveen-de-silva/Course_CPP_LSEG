#include "CSVWriter.h"
#include <fstream>
#include <stdexcept>

// ==================== CONSTRUCTORS & DESTRUCTOR ====================

CSVWriter::CSVWriter(const std::string &filename)
    : filename_(filename)
{
}

CSVWriter::CSVWriter(const CSVWriter &other)
    : filename_(other.filename_)
{
}

CSVWriter &CSVWriter::operator=(const CSVWriter &other)
{
    if (this != &other)
    {
        filename_ = other.filename_;
    }
    return *this;
}

CSVWriter::~CSVWriter()
{
}

// ==================== GETTERS ====================

std::string CSVWriter::getFilename() const
{
    return filename_;
}

// ==================== PUBLIC METHODS ====================

void CSVWriter::writeAll(const std::vector<ExecutionReport> &reports) const
{
    std::ofstream file(filename_);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file for writing: " + filename_);
    }

    // Write header
    file << "Client Order ID,Order ID,Instrument,Side,Price,Quantity,Status,Reason,Transaction Time\n";

    // Write each report
    for (const auto &report : reports)
    {
        file << report.toCSVLine() << "\n";
    }

    file.close();
}
