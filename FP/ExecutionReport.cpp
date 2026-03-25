#include "ExecutionReport.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

// ==================== STATIC METHOD ====================

std::string ExecutionReport::generateTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    std::tm *tm_ptr = std::localtime(&time_t_now);
    std::tm tm_now = *tm_ptr;

    std::ostringstream oss;
    oss << std::put_time(&tm_now, "%Y%m%d-%H%M%S")
        << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

// ==================== CONSTRUCTORS & DESTRUCTOR ====================

ExecutionReport::ExecutionReport(const std::string &clientOrderId,
                                 const std::string &orderId,
                                 const std::string &instrument,
                                 int side,
                                 double price,
                                 int quantity,
                                 Status status,
                                 const std::string &reason)
    : clientOrderId_(clientOrderId), orderId_(orderId), instrument_(instrument), side_(side), price_(price), quantity_(quantity), status_(status), reason_(reason), transactionTime_(generateTimestamp()) {
}

ExecutionReport::ExecutionReport(const ExecutionReport &other)
    : clientOrderId_(other.clientOrderId_), orderId_(other.orderId_), instrument_(other.instrument_), side_(other.side_), price_(other.price_), quantity_(other.quantity_), status_(other.status_), reason_(other.reason_), transactionTime_(other.transactionTime_) {
}

ExecutionReport &ExecutionReport::operator=(const ExecutionReport &other) {
    if (this != &other)
    {
        clientOrderId_ = other.clientOrderId_;
        orderId_ = other.orderId_;
        instrument_ = other.instrument_;
        side_ = other.side_;
        price_ = other.price_;
        quantity_ = other.quantity_;
        status_ = other.status_;
        reason_ = other.reason_;
        transactionTime_ = other.transactionTime_;
    }
    return *this;
}

ExecutionReport::~ExecutionReport() {
}

// ==================== GETTERS ====================

std::string ExecutionReport::getClientOrderId() const { return clientOrderId_; }
std::string ExecutionReport::getOrderId() const { return orderId_; }
std::string ExecutionReport::getInstrument() const { return instrument_; }
int ExecutionReport::getSide() const { return side_; }
double ExecutionReport::getPrice() const { return price_; }
int ExecutionReport::getQuantity() const { return quantity_; }
Status ExecutionReport::getStatus() const { return status_; }
std::string ExecutionReport::getReason() const { return reason_; }
std::string ExecutionReport::getTransactionTime() const { return transactionTime_; }

// ==================== STATUS HELPERS ====================

int ExecutionReport::getStatusInt() const {
    return static_cast<int>(status_);
}

std::string ExecutionReport::getStatusString() const {
    switch (status_)
    {
    case Status::New:
        return "New";
    case Status::Rejected:
        return "Rejected";
    case Status::Fill:
        return "Fill";
    case Status::PFill:
        return "PFill";
    default:
        return "Unknown";
    }
}

// ==================== UTILITY METHODS ====================

std::string ExecutionReport::toCSVLine() const {
    std::ostringstream oss;
    oss << clientOrderId_ << ","
        << orderId_ << ","
        << instrument_ << ","
        << side_ << ","
        << std::fixed << std::setprecision(2) << price_ << ","
        << quantity_ << ","
        << getStatusInt() << ","
        << reason_ << ","
        << transactionTime_;
    return oss.str();
}
