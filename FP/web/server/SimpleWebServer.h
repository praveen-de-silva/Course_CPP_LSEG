#ifndef SIMPLE_WEB_SERVER_H
#define SIMPLE_WEB_SERVER_H

#include "../../Exchange.h"
#include <string>
#include <vector>

/**
 * SimpleWebServer Class - Basic HTTP server for the Flower Exchange UI
 * Compatible with older MinGW compilers (no threading required)
 */
class SimpleWebServer
{
private:
    Exchange* exchange_;
    int port_;
    bool running_;
    std::vector<ExecutionReport> allReports_; // Store all reports for API access

    // HTTP handling methods
    std::string handleRequest(const std::string& request);
    std::string getContentType(const std::string& filename);
    std::string readFile(const std::string& filepath);
    std::string handleAPI(const std::string& endpoint, const std::string& method, const std::string& body);

    // API endpoint handlers
    std::string handleSubmitOrder(const std::string& body);
    std::string handleGetOrderBook(const std::string& instrument);
    std::string handleGetReports();
    std::string handleReset();

    // JSON helpers
    std::string extractFormData(const std::string& body, const std::string& field);

public:
    // Constructor
    SimpleWebServer(Exchange* exchange, int port = 8080);

    // Destructor
    ~SimpleWebServer();

    // Server control
    void start();
    void stop();
    bool isRunning() const;
};

#endif // SIMPLE_WEB_SERVER_H