#include "CSVReader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

// ==================== CONSTRUCTORS & DESTRUCTOR ====================

CSVReader::CSVReader(const std::string &filename)
    : filename_(filename)
{
}

CSVReader::CSVReader(const CSVReader &other)
    : filename_(other.filename_)
{
}

CSVReader &CSVReader::operator=(const CSVReader &other)
{
    if (this != &other)
    {
        filename_ = other.filename_;
    }
    return *this;
}

CSVReader::~CSVReader()
{
}

// ==================== GETTERS ====================

std::string CSVReader::getFilename() const
{
    return filename_;
}

// ==================== PRIVATE HELPER METHODS ====================

RawOrder CSVReader::parseLine(const std::string &line) const
{
    RawOrder order;
    std::vector<std::string> fields;
    std::stringstream ss(line);
    std::string field;

    // Split by comma
    while (std::getline(ss, field, ','))
    {
        // Trim whitespace
        size_t start = field.find_first_not_of(" \t\r\n");
        size_t end = field.find_last_not_of(" \t\r\n");
        if (start != std::string::npos)
        {
            field = field.substr(start, end - start + 1);
        }
        else
        {
            field = "";
        }
        fields.push_back(field);
    }

    // Assign fields
    if (fields.size() > 0)
        order.clientOrderId = fields[0];
    if (fields.size() > 1)
        order.instrument = fields[1];
    if (fields.size() > 2)
        order.side = fields[2];
    if (fields.size() > 3)
        order.price = fields[3];
    if (fields.size() > 4)
        order.quantity = fields[4];

    return order;
}

// ==================== PUBLIC METHODS ====================

std::vector<RawOrder> CSVReader::readAll() const
{
    std::vector<RawOrder> orders;
    std::ifstream file(filename_);

    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file: " + filename_);
    }

    std::string line;
    bool isHeader = true;

    while (std::getline(file, line))
    {
        if (isHeader)
        {
            isHeader = false;
            continue;
        }

        if (line.empty())
        {
            continue;
        }

        RawOrder order = parseLine(line);
        orders.push_back(order);
    }

    file.close();
    return orders;
}
