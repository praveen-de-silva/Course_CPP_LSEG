#ifndef CSV_WRITER_H
#define CSV_WRITER_H

#include "ExecutionReport.h"
#include <string>
#include <vector>

/**
 * CSVWriter Class - Writes execution reports to CSV
 *
 * Demonstrates OOP principles:
 * - Encapsulation: Private file handling
 * - Single Responsibility: Only handles CSV writing
 */
class CSVWriter
{
private:
    std::string filename_;

public:
    // Constructor
    explicit CSVWriter(const std::string& filename);

    // Copy constructor and assignment
    CSVWriter(const CSVWriter& other);
    CSVWriter& operator=(const CSVWriter& other);

    // Destructor
    ~CSVWriter();

    // ==================== GETTERS ====================
    std::string getFilename() const;

    // ==================== MAIN BUSINESS METHOD ====================
    void writeAll(const std::vector<ExecutionReport>& reports) const;
};

#endif // CSV_WRITER_H
