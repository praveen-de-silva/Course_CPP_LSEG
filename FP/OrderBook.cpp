#include "OrderBook.h"
#include <algorithm>

// ==================== COMPARATORS ====================

bool BuyComparator::operator()(const Order &a, const Order &b) const
{
    // Higher price has priority
    if (a.getPrice() != b.getPrice())
    {
        return a.getPrice() > b.getPrice();
    }
    // Same price: earlier order has priority
    return a.getSequenceNumber() < b.getSequenceNumber();
}

bool SellComparator::operator()(const Order &a, const Order &b) const
{
    // Lower price has priority
    if (a.getPrice() != b.getPrice())
    {
        return a.getPrice() < b.getPrice();
    }
    // Same price: earlier order has priority
    return a.getSequenceNumber() < b.getSequenceNumber();
}

// ==================== CONSTRUCTORS & DESTRUCTOR ====================

OrderBook::OrderBook()
    : instrument_("")
{
}

OrderBook::OrderBook(const std::string &instrument)
    : instrument_(instrument)
{
}

OrderBook::OrderBook(const OrderBook &other)
    : instrument_(other.instrument_), buyOrders_(other.buyOrders_), sellOrders_(other.sellOrders_)
{
}

OrderBook &OrderBook::operator=(const OrderBook &other)
{
    if (this != &other)
    {
        instrument_ = other.instrument_;
        buyOrders_ = other.buyOrders_;
        sellOrders_ = other.sellOrders_;
    }
    return *this;
}

OrderBook::~OrderBook()
{
}

// ==================== GETTERS ====================

std::string OrderBook::getInstrument() const
{
    return instrument_;
}

int OrderBook::getBuyOrderCount() const
{
    return static_cast<int>(buyOrders_.size());
}

int OrderBook::getSellOrderCount() const
{
    return static_cast<int>(sellOrders_.size());
}

// ==================== MAIN BUSINESS METHOD ====================

std::vector<ExecutionReport> OrderBook::processOrder(Order &incomingOrder)
{
    std::vector<ExecutionReport> reports;

    if (incomingOrder.isBuyOrder())
    {
        matchBuyOrder(incomingOrder, reports);
    }
    else
    {
        matchSellOrder(incomingOrder, reports);
    }

    return reports;
}

// ==================== PRIVATE MATCHING METHODS ====================

void OrderBook::matchBuyOrder(Order &incoming, std::vector<ExecutionReport> &reports)
{
    // Emit "New" report for incoming order
    reports.push_back(ExecutionReport(
        incoming.getClientOrderId(),
        incoming.getOrderId(),
        incoming.getInstrument(),
        incoming.getSide(),
        incoming.getPrice(),
        incoming.getQuantity(),
        Status::New));

    // Match against sell orders (lowest price first)
    auto it = sellOrders_.begin();
    while (it != sellOrders_.end() && incoming.getRemainingQuantity() > 0)
    {
        Order passiveOrder = *it;

        // Check if prices cross
        if (incoming.getPrice() < passiveOrder.getPrice())
        {
            break;
        }

        // Calculate match quantity
        int matchQty = std::min(incoming.getRemainingQuantity(),
                                passiveOrder.getRemainingQuantity());

        // Execution price is passive order's price
        double execPrice = passiveOrder.getPrice();

        // Update quantities
        incoming.reduceRemainingQuantity(matchQty);
        passiveOrder.reduceRemainingQuantity(matchQty);

        // Emit report for incoming order
        Status incomingStatus = incoming.isFilled() ? Status::Fill : Status::PFill;
        reports.push_back(ExecutionReport(
            incoming.getClientOrderId(),
            incoming.getOrderId(),
            incoming.getInstrument(),
            incoming.getSide(),
            execPrice,
            matchQty,
            incomingStatus));

        // Emit report for passive order
        Status passiveStatus = passiveOrder.isFilled() ? Status::Fill : Status::PFill;
        reports.push_back(ExecutionReport(
            passiveOrder.getClientOrderId(),
            passiveOrder.getOrderId(),
            passiveOrder.getInstrument(),
            passiveOrder.getSide(),
            execPrice,
            matchQty,
            passiveStatus));

        // Remove passive order from book
        it = sellOrders_.erase(it);

        // Re-add if partially filled
        if (!passiveOrder.isFilled())
        {
            sellOrders_.insert(passiveOrder);
            break;
        }
    }

    // Add incoming to book if not fully filled
    if (!incoming.isFilled())
    {
        buyOrders_.insert(incoming);
    }
}

void OrderBook::matchSellOrder(Order &incoming, std::vector<ExecutionReport> &reports)
{
    // Emit "New" report for incoming order
    reports.push_back(ExecutionReport(
        incoming.getClientOrderId(),
        incoming.getOrderId(),
        incoming.getInstrument(),
        incoming.getSide(),
        incoming.getPrice(),
        incoming.getQuantity(),
        Status::New));

    // Match against buy orders (highest price first)
    auto it = buyOrders_.begin();
    while (it != buyOrders_.end() && incoming.getRemainingQuantity() > 0)
    {
        Order passiveOrder = *it;

        // Check if prices cross
        if (incoming.getPrice() > passiveOrder.getPrice())
        {
            break;
        }

        // Calculate match quantity
        int matchQty = std::min(incoming.getRemainingQuantity(),
                                passiveOrder.getRemainingQuantity());

        // Execution price is passive order's price
        double execPrice = passiveOrder.getPrice();

        // Update quantities
        incoming.reduceRemainingQuantity(matchQty);
        passiveOrder.reduceRemainingQuantity(matchQty);

        // Emit report for incoming order
        Status incomingStatus = incoming.isFilled() ? Status::Fill : Status::PFill;
        reports.push_back(ExecutionReport(
            incoming.getClientOrderId(),
            incoming.getOrderId(),
            incoming.getInstrument(),
            incoming.getSide(),
            execPrice,
            matchQty,
            incomingStatus));

        // Emit report for passive order
        Status passiveStatus = passiveOrder.isFilled() ? Status::Fill : Status::PFill;
        reports.push_back(ExecutionReport(
            passiveOrder.getClientOrderId(),
            passiveOrder.getOrderId(),
            passiveOrder.getInstrument(),
            passiveOrder.getSide(),
            execPrice,
            matchQty,
            passiveStatus));

        // Remove passive order from book
        it = buyOrders_.erase(it);

        // Re-add if partially filled
        if (!passiveOrder.isFilled())
        {
            buyOrders_.insert(passiveOrder);
            break;
        }
    }

    // Add incoming to book if not fully filled
    if (!incoming.isFilled())
    {
        sellOrders_.insert(incoming);
    }
}
