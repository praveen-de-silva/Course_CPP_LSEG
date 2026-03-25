#include "WebServer.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

WebServer::WebServer(Exchange* exchange, int port)
    : exchange_(exchange), port_(port), running_(false)
{
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
}

WebServer::~WebServer()
{
    stop();
#ifdef _WIN32
    WSACleanup();
#endif
}

void WebServer::start()
{
    running_ = true;
    serverThread_ = std::thread([this]() {
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

        while (running_) {
            socklen_t clientAddrLen = sizeof(clientAddr);
            int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

            if (clientSocket == INVALID_SOCKET) continue;

            // Read request
            char buffer[4096] = {0};
            recv(clientSocket, buffer, sizeof(buffer), 0);

            std::string request(buffer);
            std::string response = handleRequest(request);

            send(clientSocket, response.c_str(), response.length(), 0);
            closesocket(clientSocket);
        }

        closesocket(serverSocket);
    });
}

void WebServer::stop()
{
    running_ = false;
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
}

bool WebServer::isRunning() const
{
    return running_;
}

std::string WebServer::handleRequest(const std::string& request)
{
    std::istringstream iss(request);
    std::string method, path, version;
    iss >> method >> path >> version;

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
        return "HTTP/1.1 404 Not Found\r\nContent-Length: 13\r\n\r\n404 Not Found";
    }

    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: " << getContentType(filepath) << "\r\n"
             << "Content-Length: " << content.length() << "\r\n"
             << "\r\n"
             << content;
    return response.str();
}

std::string WebServer::getContentType(const std::string& filename)
{
    if (filename.find(".html") != std::string::npos) return "text/html";
    if (filename.find(".css") != std::string::npos) return "text/css";
    if (filename.find(".js") != std::string::npos) return "application/javascript";
    if (filename.find(".json") != std::string::npos) return "application/json";
    return "text/plain";
}

std::string WebServer::readFile(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open()) return "";

    std::ostringstream content;
    content << file.rdbuf();
    return content.str();
}

std::string WebServer::handleAPI(const std::string& endpoint, const std::string& method, const std::string& body)
{
    if (method == "OPTIONS") {
        return "{}"; // CORS preflight
    }

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
    else if (endpoint == "/api/import" && method == "POST") {
        return handleImportCSV(body);
    }
    else if (endpoint == "/api/export" && method == "GET") {
        return handleExportCSV();
    }
    else if (endpoint == "/api/reset" && method == "POST") {
        return handleReset();
    }

    return "{\"error\": \"Endpoint not found\"}";
}

std::string WebServer::handleSubmitOrder(const std::string& body)
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

std::string WebServer::handleGetOrderBook(const std::string& instrument)
{
    // Note: This is a simplified implementation
    // Real implementation would need access to OrderBook internals
    return "{\"buys\": [], \"sells\": [], \"instrument\": \"" + instrument + "\"}";
}

std::string WebServer::handleGetReports()
{
    return "{\"reports\": []}"; // Placeholder - would need to store reports
}

std::string WebServer::handleImportCSV(const std::string& body)
{
    return "{\"message\": \"CSV import functionality to be implemented\"}";
}

std::string WebServer::handleExportCSV()
{
    return "{\"message\": \"CSV export functionality to be implemented\"}";
}

std::string WebServer::handleReset()
{
    // Reset exchange (would need to implement reset method)
    return "{\"message\": \"Exchange reset\"}";
}

std::string WebServer::extractFormData(const std::string& body, const std::string& field)
{
    std::string searchFor = "\"" + field + "\":\"";
    size_t pos = body.find(searchFor);
    if (pos == std::string::npos) {
        // Try without quotes for numbers
        searchFor = "\"" + field + "\":";
        pos = body.find(searchFor);
        if (pos == std::string::npos) return "";
        pos += searchFor.length();
        size_t endPos = body.find_first_of(",}", pos);
        return body.substr(pos, endPos - pos);
    }

    pos += searchFor.length();
    size_t endPos = body.find("\"", pos);
    return body.substr(pos, endPos - pos);
}