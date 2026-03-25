/*
 * Flower Exchange - Order Matching Engine
 * C++ Workshop Series for UoM | LSEG Technology
 *
 * This program supports two modes:
 *   1. CSV Mode (default): Reads orders.csv → Processes → Writes execution_rep.csv
 *   2. Web UI Mode: Starts HTTP server with web interface for live trading
 *
 * Usage:
 *   ./main              - CSV processing mode (default)
 *   ./main --csv        - CSV processing mode (explicit)
 *   ./main --web-ui     - Web UI mode on port 8080
 *   ./main --web-ui 9000 - Web UI mode on custom port
 *
 * Architecture follows SOLID principles:
 *   - CSVReader/Writer: File I/O (Single Responsibility)
 *   - Exchange:         Validates orders and routes to OrderBooks
 *   - OrderBook:        Handles order matching logic
 *   - WebServer:        HTTP server for web interface
 */

#include "CSVReader.h"
#include "Exchange.h"
#include "CSVWriter.h"
#include "web/server/WebServer.h"
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>

// Function declarations
void runCSVMode();
void runWebUIMode(int port = 8080);
void printUsage(const char* programName);

int main(int argc, char* argv[])
{
    // Performance optimization for I/O
    std::ios::sync_with_stdio(false);

    try
    {
        // Parse command line arguments
        if (argc == 1) {
            // Default: CSV mode
            runCSVMode();
        }
        else if (argc == 2) {
            std::string arg(argv[1]);
            if (arg == "--csv") {
                runCSVMode();
            }
            else if (arg == "--web-ui") {
                runWebUIMode();
            }
            else if (arg == "--help" || arg == "-h") {
                printUsage(argv[0]);
                return 0;
            }
            else {
                std::cerr << "Unknown argument: " << arg << std::endl;
                printUsage(argv[0]);
                return 1;
            }
        }
        else if (argc == 3) {
            std::string arg(argv[1]);
            if (arg == "--web-ui") {
                int port = std::stoi(argv[2]);
                if (port < 1024 || port > 65535) {
                    std::cerr << "Port must be between 1024-65535" << std::endl;
                    return 1;
                }
                runWebUIMode(port);
            }
            else {
                std::cerr << "Invalid arguments" << std::endl;
                printUsage(argv[0]);
                return 1;
            }
        }
        else {
            std::cerr << "Too many arguments" << std::endl;
            printUsage(argv[0]);
            return 1;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

void runCSVMode()
{
    std::cout << "=== FLOWER EXCHANGE - CSV PROCESSING MODE ===" << std::endl;

    // Step 1: Read orders from input file
    std::cout << "Reading orders from orders.csv..." << std::endl;
    CSVReader reader("orders.csv");
    std::vector<RawOrder> rawOrders = reader.readAll();
    std::cout << "Read " << rawOrders.size() << " orders." << std::endl;

    // Step 2: Process each order through the exchange
    std::cout << "Processing orders..." << std::endl;
    Exchange exchange;
    std::vector<ExecutionReport> allReports;

    // Reserve space for efficiency (estimate ~2 reports per order average)
    allReports.reserve(rawOrders.size() * 2);

    for (const auto& rawOrder : rawOrders)
    {
        // Process order and get execution reports
        std::vector<ExecutionReport> reports = exchange.processOrder(rawOrder);

        // Add reports to our collection
        allReports.insert(allReports.end(), reports.begin(), reports.end());
    }

    std::cout << "Generated " << allReports.size() << " execution reports." << std::endl;

    // Step 3: Write execution reports to output file
    std::cout << "Writing execution reports to execution_rep.csv..." << std::endl;
    CSVWriter writer("execution_rep.csv");
    writer.writeAll(allReports);

    std::cout << "Done! Check execution_rep.csv for results." << std::endl;
}

void runWebUIMode(int port)
{
    std::cout << "=== FLOWER EXCHANGE - WEB UI MODE ===" << std::endl;

    // Create exchange instance
    Exchange exchange;

    // Create and start web server
    std::cout << "Starting web server..." << std::endl;
    WebServer server(&exchange, port);

    server.start();

    std::cout << "Web UI available at: http://localhost:" << port << std::endl;
    std::cout << "Press Ctrl+C to stop the server..." << std::endl;

    // Keep the server running
    // In a production environment, you'd want proper signal handling
    while (server.isRunning()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    server.stop();
    std::cout << "Web server stopped." << std::endl;
}

void printUsage(const char* programName)
{
    std::cout << "FLOWER EXCHANGE - Order Matching Engine" << std::endl;
    std::cout << "C++ Workshop Series for UoM | LSEG Technology" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << programName << "              - CSV processing mode (default)" << std::endl;
    std::cout << "  " << programName << " --csv        - CSV processing mode (explicit)" << std::endl;
    std::cout << "  " << programName << " --web-ui     - Web UI mode on port 8080" << std::endl;
    std::cout << "  " << programName << " --web-ui PORT - Web UI mode on custom port" << std::endl;
    std::cout << "  " << programName << " --help       - Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "CSV Mode:" << std::endl;
    std::cout << "  Reads orders from orders.csv, processes them, and writes" << std::endl;
    std::cout << "  execution reports to execution_rep.csv" << std::endl;
    std::cout << std::endl;
    std::cout << "Web UI Mode:" << std::endl;
    std::cout << "  Starts HTTP server with live trading interface." << std::endl;
    std::cout << "  Access via web browser at http://localhost:PORT" << std::endl;
}
