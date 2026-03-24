#include "Exchange.h"

// ==================== CONSTRUCTORS & DESTRUCTOR ====================

Exchange::Exchange()
    : orderIdCounter_(0), sequenceCounter_(0)
{
    // Initialize valid instruments
    validInstruments_ = {"Rose", "Lavender", "Lotus", "Tulip", "Orchid"};
}

Exchange::Exchange(const Exchange &other)
    : orderBooks_(other.orderBooks_), validInstruments_(other.validInstruments_), orderIdCounter_(other.orderIdCounter_), sequenceCounter_(other.sequenceCounter_)
{
}

Exchange &Exchange::operator=(const Exchange &other)
{
    if (this != &other)
    {
        orderBooks_ = other.orderBooks_;
        validInstruments_ = other.validInstruments_;
        orderIdCounter_ = other.orderIdCounter_;
        sequenceCounter_ = other.sequenceCounter_;
    }
    return *this;
}

Exchange::~Exchange()
{
}

// ==================== GETTERS ====================

int Exchange::getOrderIdCounter() const
{
    return orderIdCounter_;
}

int Exchange::getSequenceCounter() const
{
    return sequenceCounter_;
}

int Exchange::getInstrumentCount() const
{
    return static_cast<int>(validInstruments_.size());
}

// ==================== PRIVATE HELPER METHODS ====================

std::string Exchange::generateOrderId()
{
    return "ord" + std::to_string(++orderIdCounter_);
}

bool Exchange::validateOrder(const RawOrder &raw, std::string &reason) const
{
    // Check client order ID
    if (raw.clientOrderId.empty())
    {
        reason = "Invalid client order ID";
        return false;
    }

    // Check instrument
    if (validInstruments_.find(raw.instrument) == validInstruments_.end())
    {
        reason = "Invalid instrument";
        return false;
    }

    // Check side
    if (raw.side != "1" && raw.side != "2")
    {
        reason = "Invalid side";
        return false;
    }

    // Check price
    try
    {
        double price = std::stod(raw.price);
        if (price <= 0.0)
        {
            reason = "Invalid price";
            return false;
        }
    }
    catch (...)
    {
        reason = "Invalid price";
        return false;
    }

    // Check quantity
    try
    {
        int qty = std::stoi(raw.quantity);
        if (qty < 10 || qty > 1000)
        {
            reason = "Invalid size";
            return false;
        }
        if (qty % 10 != 0)
        {
            reason = "Invalid size";
            return false;
        }
    }
    catch (...)
    {
        reason = "Invalid size";
        return false;
    }

    return true;
}

// ==================== PUBLIC METHODS ====================

bool Exchange::isValidInstrument(const std::string &instrument) const
{
    return validInstruments_.find(instrument) != validInstruments_.end();
}

std::vector<ExecutionReport> Exchange::processOrder(const RawOrder &rawOrder)
{
    std::vector<ExecutionReport> reports;

    // Generate order ID
    std::string orderId = generateOrderId();
    int seqNum = ++sequenceCounter_;

    // Validate order
    std::string rejectReason;
    if (!validateOrder(rawOrder, rejectReason))
    {
        reports.push_back(ExecutionReport(
            rawOrder.clientOrderId,
            orderId,
            rawOrder.instrument,
            0,
            0.0,
            0,
            Status::Rejected,
            rejectReason));
        return reports;
    }

    // Create Order object
    Order order(
        rawOrder.clientOrderId,
        rawOrder.instrument,
        std::stoi(rawOrder.side),
        std::stod(rawOrder.price),
        std::stoi(rawOrder.quantity),
        orderId,
        seqNum);

    // Route to appropriate order book
    OrderBook &book = orderBooks_[order.getInstrument()];
    reports = book.processOrder(order);

    return reports;
}
