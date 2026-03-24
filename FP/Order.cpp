#include "Order.h"

// ==================== CONSTRUCTORS & DESTRUCTOR ====================

// Default constructor
Order::Order()
    : clientOrderId_(""), instrument_(""), side_(0), price_(0.0), quantity_(0), orderId_(""), sequenceNumber_(0), remainingQuantity_(0)
{
}

// Parameterized constructor
Order::Order(const std::string &clientOrderId,
             const std::string &instrument,
             int side,
             double price,
             int quantity,
             const std::string &orderId,
             int sequenceNumber)
    : clientOrderId_(clientOrderId), instrument_(instrument), side_(side), price_(price), quantity_(quantity), orderId_(orderId), sequenceNumber_(sequenceNumber), remainingQuantity_(quantity) // Initially, remaining = total
{
}

// Copy constructor
Order::Order(const Order &other)
    : clientOrderId_(other.clientOrderId_), instrument_(other.instrument_), side_(other.side_), price_(other.price_), quantity_(other.quantity_), orderId_(other.orderId_), sequenceNumber_(other.sequenceNumber_), remainingQuantity_(other.remainingQuantity_)
{
}

// Assignment operator
Order &Order::operator=(const Order &other)
{
    if (this != &other)
    {
        clientOrderId_ = other.clientOrderId_;
        instrument_ = other.instrument_;
        side_ = other.side_;
        price_ = other.price_;
        quantity_ = other.quantity_;
        orderId_ = other.orderId_;
        sequenceNumber_ = other.sequenceNumber_;
        remainingQuantity_ = other.remainingQuantity_;
    }
    return *this;
}

// Destructor
Order::~Order()
{
    // No dynamic memory to clean up
}

// ==================== GETTERS ====================

std::string Order::getClientOrderId() const { return clientOrderId_; }
std::string Order::getInstrument() const { return instrument_; }
int Order::getSide() const { return side_; }
double Order::getPrice() const { return price_; }
int Order::getQuantity() const { return quantity_; }
std::string Order::getOrderId() const { return orderId_; }
int Order::getSequenceNumber() const { return sequenceNumber_; }
int Order::getRemainingQuantity() const { return remainingQuantity_; }

// ==================== SETTERS ====================

void Order::setClientOrderId(const std::string &clientOrderId) { clientOrderId_ = clientOrderId; }
void Order::setInstrument(const std::string &instrument) { instrument_ = instrument; }
void Order::setSide(int side) { side_ = side; }
void Order::setPrice(double price) { price_ = price; }
void Order::setQuantity(int quantity) { quantity_ = quantity; }
void Order::setOrderId(const std::string &orderId) { orderId_ = orderId; }
void Order::setSequenceNumber(int sequenceNumber) { sequenceNumber_ = sequenceNumber; }
void Order::setRemainingQuantity(int remainingQuantity) { remainingQuantity_ = remainingQuantity; }

// ==================== BUSINESS METHODS ====================

bool Order::isBuyOrder() const
{
    return side_ == 1;
}

bool Order::isSellOrder() const
{
    return side_ == 2;
}

void Order::reduceRemainingQuantity(int amount)
{
    remainingQuantity_ -= amount;
    if (remainingQuantity_ < 0)
    {
        remainingQuantity_ = 0;
    }
}

bool Order::isFilled() const
{
    return remainingQuantity_ == 0;
}
