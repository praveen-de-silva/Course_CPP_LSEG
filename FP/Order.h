#ifndef ORDER_H
#define ORDER_H

#include <string>

/**
 * Order Class - Represents a trading order
 *
 * Demonstrates OOP principles:
 * - Encapsulation: Private data members with public getters/setters
 * - Constructor overloading
 * - Const correctness
 */
class Order
{
private:
    // Private data members (encapsulation)
    std::string clientOrderId_;  // Alpha-numeric, max 7 chars
    std::string instrument_;     // Rose, Lavender, Lotus, Tulip, Orchid
    int side_;                   // 1 = Buy, 2 = Sell
    double price_;               // Must be > 0.0
    int quantity_;               // 10-1000, multiple of 10
    std::string orderId_;        // System-generated unique ID
    int sequenceNumber_;         // For time priority
    int remainingQuantity_;      // Tracks unfilled quantity

public:
    // Default constructor
    Order();

    // Parameterized constructor
    Order(const std::string& clientOrderId,
          const std::string& instrument,
          int side,
          double price,
          int quantity,
          const std::string& orderId,
          int sequenceNumber);

    // Copy constructor (Rule of Three)
    Order(const Order& other);

    // Assignment operator (Rule of Three)
    Order& operator=(const Order& other);

    // Destructor
    ~Order();

    // ==================== GETTERS (const methods) ====================
    std::string getClientOrderId() const;
    std::string getInstrument() const;
    int getSide() const;
    double getPrice() const;
    int getQuantity() const;
    std::string getOrderId() const;
    int getSequenceNumber() const;
    int getRemainingQuantity() const;

    // ==================== SETTERS ====================
    void setClientOrderId(const std::string& clientOrderId);
    void setInstrument(const std::string& instrument);
    void setSide(int side);
    void setPrice(double price);
    void setQuantity(int quantity);
    void setOrderId(const std::string& orderId);
    void setSequenceNumber(int sequenceNumber);
    void setRemainingQuantity(int remainingQuantity);

    // ==================== BUSINESS METHODS ====================
    // Check if this is a buy order
    bool isBuyOrder() const;

    // Check if this is a sell order
    bool isSellOrder() const;

    // Reduce remaining quantity after a match
    void reduceRemainingQuantity(int amount);

    // Check if order is completely filled
    bool isFilled() const;
};

#endif // ORDER_H
