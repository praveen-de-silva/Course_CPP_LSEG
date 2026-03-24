#ifndef CSV_READER_H
#define CSV_READER_H

#include "Exchange.h"  // For RawOrder
#include <string>
#include <vector>

/**
 * CSVReader Class - Reads orders from CSV file
 *
 * Demonstrates OOP principles:
 * - Encapsulation: Private parsing logic
 * - Single Responsibility: Only handles CSV reading
 */
class CSVReader
{
private:
    std::string filename_;

    // Private helper method
    RawOrder parseLine(const std::string& line) const;

public:
    // Constructor
    explicit CSVReader(const std::string& filename);

    // Copy constructor and assignment
    CSVReader(const CSVReader& other);
    CSVReader& operator=(const CSVReader& other);

    // Destructor
    ~CSVReader();

    // ==================== GETTERS ====================
    std::string getFilename() const;

    // ==================== MAIN BUSINESS METHOD ====================
    std::vector<RawOrder> readAll() const;
};

#endif // CSV_READER_H
