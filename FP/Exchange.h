#ifndef EXCHANGE_H
#define EXCHANGE_H

#include "Order.h"
#include "OrderBook.h"
#include "ExecutionReport.h"
#include <map>
#include <set>
#include <string>
#include <vector>

/**
 * RawOrder - Represents unvalidated order data from CSV
 * Kept as struct since it's just a data container
 */
struct RawOrder
{
    std::string clientOrderId;
    std::string instrument;
    std::string side;
    std::string price;
    std::string quantity;
};

/**
 * Exchange Class - Main trading exchange
 *
 * Demonstrates OOP principles:
 * - Encapsulation: Private state and validation logic
 * - Composition: Contains multiple OrderBook objects
 * - Single Responsibility: Handles validation and routing
 */
class Exchange
{
private:
    std::map<std::string, OrderBook> orderBooks_;
    std::set<std::string> validInstruments_;
    int orderIdCounter_;
    int sequenceCounter_;

    // Private helper methods
    std::string generateOrderId();
    bool validateOrder(const RawOrder& rawOrder, std::string& reason) const;

public:
    // Constructor
    Exchange();

    // Copy constructor and assignment
    Exchange(const Exchange& other);
    Exchange& operator=(const Exchange& other);

    // Destructor
    ~Exchange();

    // ==================== GETTERS ====================
    int getOrderIdCounter() const;
    int getSequenceCounter() const;
    int getInstrumentCount() const;

    // ==================== MAIN BUSINESS METHOD ====================
    // Process a raw order and return execution reports
    std::vector<ExecutionReport> processOrder(const RawOrder& rawOrder);

    // Check if instrument is valid
    bool isValidInstrument(const std::string& instrument) const;
};

#endif // EXCHANGE_H
