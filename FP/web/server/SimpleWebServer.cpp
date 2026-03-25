#include "SimpleWebServer.h"
#include <iostream>
#include <sstream>
#include <fstream>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

SimpleWebServer::SimpleWebServer(Exchange* exchange, int port)
    : exchange_(exchange), port_(port), running_(false)
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
    }
#endif
}

SimpleWebServer::~SimpleWebServer()
{
    stop();
#ifdef _WIN32
    WSACleanup();
#endif
}

void SimpleWebServer::start()
{
    running_ = true;
    int serverSocket;
    struct sockaddr_in serverAddr, clientAddr;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Failed to create socket" << std::endl;
        return;
    }

    // Allow socket reuse
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port_);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind socket" << std::endl;
        closesocket(serverSocket);
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on socket" << std::endl;
        closesocket(serverSocket);
        return;
    }

    std::cout << "Web server started on http://localhost:" << port_ << std::endl;
    std::cout << "Press Ctrl+C to stop the server" << std::endl;

    while (running_) {
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

        if (clientSocket == INVALID_SOCKET) continue;

        // Read request
        char buffer[4096] = {0};
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            std::string request(buffer);
            std::string response = handleRequest(request);

            send(clientSocket, response.c_str(), response.length(), 0);
        }

        closesocket(clientSocket);
    }

    closesocket(serverSocket);
}

void SimpleWebServer::stop()
{
    running_ = false;
}

bool SimpleWebServer::isRunning() const
{
    return running_;
}

std::string SimpleWebServer::handleRequest(const std::string& request)
{
    std::istringstream iss(request);
    std::string method, path, version;
    iss >> method >> path >> version;

    // Handle CORS preflight
    if (method == "OPTIONS") {
        return "HTTP/1.1 200 OK\r\n"
               "Access-Control-Allow-Origin: *\r\n"
               "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
               "Access-Control-Allow-Headers: Content-Type\r\n"
               "Content-Length: 0\r\n"
               "\r\n";
    }

    // Handle API requests
    if (path.find("/api/") == 0) {
        size_t bodyPos = request.find("\r\n\r\n");
        std::string body = (bodyPos != std::string::npos) ? request.substr(bodyPos + 4) : "";

        std::string jsonResponse = handleAPI(path, method, body);

        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\n"
                 << "Content-Type: application/json\r\n"
                 << "Access-Control-Allow-Origin: *\r\n"
                 << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n"
                 << "Access-Control-Allow-Headers: Content-Type\r\n"
                 << "Content-Length: " << jsonResponse.length() << "\r\n"
                 << "\r\n"
                 << jsonResponse;
        return response.str();
    }

    // Handle static files
    if (path == "/") path = "/index.html";

    std::string filepath = "web/client" + path;
    std::string content = readFile(filepath);

    if (content.empty()) {
        return "HTTP/1.1 404 Not Found\r\n"
               "Content-Type: text/html\r\n"
               "Content-Length: 47\r\n"
               "\r\n"
               "<html><body><h1>404 Not Found</h1></body></html>";
    }

    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: " << getContentType(filepath) << "\r\n"
             << "Content-Length: " << content.length() << "\r\n"
             << "\r\n"
             << content;
    return response.str();
}

std::string SimpleWebServer::getContentType(const std::string& filename)
{
    if (filename.find(".html") != std::string::npos) return "text/html; charset=utf-8";
    if (filename.find(".css") != std::string::npos) return "text/css";
    if (filename.find(".js") != std::string::npos) return "application/javascript";
    if (filename.find(".json") != std::string::npos) return "application/json";
    return "text/plain";
}

std::string SimpleWebServer::readFile(const std::string& filepath)
{
    std::ifstream file(filepath.c_str());
    if (!file.is_open()) return "";

    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

std::string SimpleWebServer::handleAPI(const std::string& endpoint, const std::string& method, const std::string& body)
{
    if (endpoint == "/api/orders" && method == "POST") {
        return handleSubmitOrder(body);
    }
    else if (endpoint.find("/api/orderbook/") == 0 && method == "GET") {
        std::string instrument = endpoint.substr(15); // Remove "/api/orderbook/"
        return handleGetOrderBook(instrument);
    }
    else if (endpoint == "/api/reports" && method == "GET") {
        return handleGetReports();
    }
    else if (endpoint == "/api/reset" && method == "POST") {
        return handleReset();
    }

    return "{\"error\": \"Endpoint not found\"}";
}

std::string SimpleWebServer::handleSubmitOrder(const std::string& body)
{
    // Parse JSON body (simplified)
    RawOrder rawOrder;

    // Extract fields from JSON (basic parsing)
    rawOrder.clientOrderId = extractFormData(body, "clientOrderId");
    rawOrder.instrument = extractFormData(body, "instrument");
    rawOrder.side = extractFormData(body, "side");
    rawOrder.price = extractFormData(body, "price");
    rawOrder.quantity = extractFormData(body, "quantity");

    // Process order through exchange
    std::vector<ExecutionReport> reports = exchange_->processOrder(rawOrder);

    // Store reports for later retrieval
    for (const auto& report : reports) {
        allReports_.push_back(report);
    }

    // Convert to JSON
    std::ostringstream json;
    json << "{\"reports\": [";
    for (size_t i = 0; i < reports.size(); ++i) {
        if (i > 0) json << ",";
        json << "{"
             << "\"clientOrderId\": \"" << reports[i].getClientOrderId() << "\","
             << "\"orderId\": \"" << reports[i].getOrderId() << "\","
             << "\"instrument\": \"" << reports[i].getInstrument() << "\","
             << "\"side\": " << reports[i].getSide() << ","
             << "\"price\": " << reports[i].getPrice() << ","
             << "\"quantity\": " << reports[i].getQuantity() << ","
             << "\"status\": " << reports[i].getStatusInt() << ","
             << "\"reason\": \"" << reports[i].getReason() << "\","
             << "\"transactionTime\": \"" << reports[i].getTransactionTime() << "\""
             << "}";
    }
    json << "]}";

    return json.str();
}

std::string SimpleWebServer::handleGetOrderBook(const std::string& instrument)
{
    // Simplified order book response
    // In a full implementation, you would need access to OrderBook internals
    return "{\"buys\": [], \"sells\": [], \"instrument\": \"" + instrument + "\"}";
}

std::string SimpleWebServer::handleGetReports()
{
    std::ostringstream json;
    json << "{\"reports\": [";
    for (size_t i = 0; i < allReports_.size(); ++i) {
        if (i > 0) json << ",";
        const ExecutionReport& report = allReports_[i];
        json << "{"
             << "\"clientOrderId\": \"" << report.getClientOrderId() << "\","
             << "\"orderId\": \"" << report.getOrderId() << "\","
             << "\"instrument\": \"" << report.getInstrument() << "\","
             << "\"side\": " << report.getSide() << ","
             << "\"price\": " << report.getPrice() << ","
             << "\"quantity\": " << report.getQuantity() << ","
             << "\"status\": " << report.getStatusInt() << ","
             << "\"reason\": \"" << report.getReason() << "\","
             << "\"transactionTime\": \"" << report.getTransactionTime() << "\""
             << "}";
    }
    json << "]}";
    return json.str();
}

std::string SimpleWebServer::handleReset()
{
    // Clear stored reports
    allReports_.clear();

    // Create new exchange instance
    *exchange_ = Exchange();

    return "{\"message\": \"Exchange reset successfully\", \"reports\": 0}";
}

std::string SimpleWebServer::extractFormData(const std::string& body, const std::string& field)
{
    // Simple JSON field extraction
    std::string searchFor = "\"" + field + "\":\"";
    size_t pos = body.find(searchFor);
    if (pos == std::string::npos) {
        // Try without quotes for numbers
        searchFor = "\"" + field + "\":";
        pos = body.find(searchFor);
        if (pos == std::string::npos) return "";
        pos += searchFor.length();
        size_t endPos = body.find_first_of(",}", pos);
        std::string value = body.substr(pos, endPos - pos);
        // Remove any quotes
        if (value.front() == '"' && value.back() == '"') {
            value = value.substr(1, value.length() - 2);
        }
        return value;
    }

    pos += searchFor.length();
    size_t endPos = body.find("\"", pos);
    return body.substr(pos, endPos - pos);
}