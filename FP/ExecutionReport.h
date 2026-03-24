#ifndef EXECUTION_REPORT_H
#define EXECUTION_REPORT_H

#include <string>

/**
 * Status Enum - Represents execution status
 * Using enum class for type safety (C++11)
 */
enum class Status
{
    New = 0,      // Order entered the book with no match
    Rejected = 1, // Order failed validation
    Fill = 2,     // Order fully matched and executed
    PFill = 3     // Partial fill (some quantity remaining)
};

/**
 * ExecutionReport Class - Represents an execution report
 *
 * Demonstrates OOP principles:
 * - Encapsulation: Private data with public interface
 * - Static methods for utility functions
 * - Const correctness
 */
class ExecutionReport
{
private:
    std::string clientOrderId_;
    std::string orderId_;
    std::string instrument_;
    int side_;
    double price_;
    int quantity_;
    Status status_;
    std::string reason_;
    std::string transactionTime_;

    // Private static method for timestamp generation
    static std::string generateTimestamp();

public:
    // Constructor
    ExecutionReport(const std::string& clientOrderId,
                    const std::string& orderId,
                    const std::string& instrument,
                    int side,
                    double price,
                    int quantity,
                    Status status,
                    const std::string& reason = "");

    // Copy constructor
    ExecutionReport(const ExecutionReport& other);

    // Assignment operator
    ExecutionReport& operator=(const ExecutionReport& other);

    // Destructor
    ~ExecutionReport();

    // ==================== GETTERS ====================
    std::string getClientOrderId() const;
    std::string getOrderId() const;
    std::string getInstrument() const;
    int getSide() const;
    double getPrice() const;
    int getQuantity() const;
    Status getStatus() const;
    std::string getReason() const;
    std::string getTransactionTime() const;

    // ==================== STATUS HELPERS ====================
    int getStatusInt() const;
    std::string getStatusString() const;

    // ==================== UTILITY METHODS ====================
    // Format report as CSV line
    std::string toCSVLine() const;
};

#endif // EXECUTION_REPORT_H
