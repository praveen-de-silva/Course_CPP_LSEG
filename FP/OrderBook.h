#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include "Order.h"
#include "ExecutionReport.h"
#include <set>
#include <vector>
#include <string>

/**
 * BuyComparator - Functor for sorting buy orders
 *
 * Demonstrates: Function Objects (Functors)
 * Higher price = higher priority, then earlier time
 */
class BuyComparator
{
public:
    bool operator()(const Order& a, const Order& b) const;
};

/**
 * SellComparator - Functor for sorting sell orders
 *
 * Lower price = higher priority, then earlier time
 */
class SellComparator
{
public:
    bool operator()(const Order& a, const Order& b) const;
};

/**
 * OrderBook Class - Manages buy and sell orders for one instrument
 *
 * Demonstrates OOP principles:
 * - Encapsulation: Private order containers
 * - Composition: Contains Order objects
 * - Abstraction: Hides matching logic complexity
 */
class OrderBook
{
private:
    std::string instrument_;
    std::set<Order, BuyComparator> buyOrders_;
    std::set<Order, SellComparator> sellOrders_;

    // Private helper methods
    void matchBuyOrder(Order& incoming, std::vector<ExecutionReport>& reports);
    void matchSellOrder(Order& incoming, std::vector<ExecutionReport>& reports);

public:
    // Constructor
    OrderBook();
    explicit OrderBook(const std::string& instrument);

    // Copy constructor and assignment
    OrderBook(const OrderBook& other);
    OrderBook& operator=(const OrderBook& other);

    // Destructor
    ~OrderBook();

    // ==================== GETTERS ====================
    std::string getInstrument() const;
    int getBuyOrderCount() const;
    int getSellOrderCount() const;

    // ==================== MAIN BUSINESS METHOD ====================
    // Process an incoming order and return execution reports
    std::vector<ExecutionReport> processOrder(Order& incomingOrder);
};

#endif // ORDER_BOOK_H
