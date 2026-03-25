#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "../../Exchange.h"
#include <string>
#include <thread>

/**
 * WebServer Class - HTTP server for the Flower Exchange UI
 *
 * Provides REST API endpoints and serves static files
 */
class WebServer
{
private:
    Exchange* exchange_;
    int port_;
    bool running_;
    std::thread serverThread_;

    // HTTP handling methods
    std::string handleRequest(const std::string& request);
    std::string getContentType(const std::string& filename);
    std::string readFile(const std::string& filepath);
    std::string handleAPI(const std::string& endpoint, const std::string& method, const std::string& body);

    // API endpoint handlers
    std::string handleSubmitOrder(const std::string& body);
    std::string handleGetOrderBook(const std::string& instrument);
    std::string handleGetReports();
    std::string handleImportCSV(const std::string& body);
    std::string handleExportCSV();
    std::string handleReset();

    // JSON helpers
    std::string orderBookToJSON(const std::string& instrument);
    std::string reportsToJSON();
    std::string extractFormData(const std::string& body, const std::string& field);

public:
    // Constructor
    WebServer(Exchange* exchange, int port = 8080);

    // Destructor
    ~WebServer();

    // Server control
    void start();
    void stop();
    bool isRunning() const;

    // Static file serving
    void setStaticPath(const std::string& path);
};

#endif // WEB_SERVER_H