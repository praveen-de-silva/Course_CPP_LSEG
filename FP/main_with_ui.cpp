/*
 * Flower Exchange - Order Matching Engine
 * C++ Workshop Series for UoM | LSEG Technology
 *
 * This program supports two modes:
 *   CLI Mode:  Processes orders.csv → execution_rep.csv (original functionality)
 *   Web Mode:  Starts web server for real-time UI interaction
 *
 * Usage:
 *   ./main           - Run CLI mode (default)
 *   ./main --web-ui  - Start web server on http://localhost:8080
 *   ./main --help    - Show help message
 *
 * Architecture follows SOLID principles:
 *   - CSVReader:   Reads input (Single Responsibility)
 *   - Exchange:    Validates orders and routes to OrderBooks
 *   - OrderBook:   Handles order matching logic
 *   - CSVWriter:   Writes output
 *   - WebServer:   Serves UI and provides REST API
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

void showHelp()
{
    std::cout << "\nFlower Exchange - Order Matching Engine\n"
              << "C++ Workshop Series for UoM | LSEG Technology\n\n"
              << "Usage:\n"
              << "  ./main           - Run CLI mode (process orders.csv)\n"
              << "  ./main --web-ui  - Start web server for UI\n"
              << "  ./main --help    - Show this help message\n\n"
              << "CLI Mode:\n"
              << "  Reads orders from orders.csv, processes them through the\n"
              << "  exchange, and writes execution reports to execution_rep.csv\n\n"
              << "Web Mode:\n"
              << "  Starts a web server on http://localhost:8080\n"
              << "  Open your browser to interact with the trading interface\n"
              << "  Press Ctrl+C to stop the server\n\n";
}

int runCLIMode()
{
    std::cout << "=== FLOWER EXCHANGE - CLI MODE ===\n" << std::endl;

    try
    {
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

        for (const auto &rawOrder : rawOrders)
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

        std::cout << "\n=== PROCESSING COMPLETE ===\n"
                  << "✓ Input:  orders.csv (" << rawOrders.size() << " orders)\n"
                  << "✓ Output: execution_rep.csv (" << allReports.size() << " reports)\n"
                  << "✓ Check execution_rep.csv for detailed results\n" << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

int runWebMode()
{
    std::cout << "=== FLOWER EXCHANGE - WEB UI MODE ===\n" << std::endl;

    try
    {
        // Create exchange instance
        Exchange exchange;

        // Create and start web server
        WebServer server(&exchange, 8080);

        std::cout << "Starting web server..." << std::endl;
        server.start();

        std::cout << "\n=== SERVER STARTED ===\n"
                  << "🌐 Web UI: http://localhost:8080\n"
                  << "📡 API:    http://localhost:8080/api/\n\n"
                  << "Available endpoints:\n"
                  << "  POST /api/orders      - Submit new order\n"
                  << "  GET  /api/orderbook/  - Get order book for instrument\n"
                  << "  GET  /api/reports     - Get execution reports\n"
                  << "  POST /api/import      - Import CSV orders\n"
                  << "  GET  /api/export      - Export execution reports\n"
                  << "  POST /api/reset       - Reset exchange state\n\n"
                  << "Press Ctrl+C to stop the server...\n" << std::endl;

        // Keep server running
        while (server.isRunning())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        std::cout << "Server stopped." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

int main(int argc, char* argv[])
{
    // Performance optimization for I/O
    std::ios::sync_with_stdio(false);

    // Parse command line arguments
    bool webMode = false;
    bool showHelpFlag = false;

    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];
        if (arg == "--web-ui" || arg == "-w")
        {
            webMode = true;
        }
        else if (arg == "--help" || arg == "-h")
        {
            showHelpFlag = true;
        }
        else
        {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::cerr << "Use --help for usage information." << std::endl;
            return 1;
        }
    }

    // Show help if requested
    if (showHelpFlag)
    {
        showHelp();
        return 0;
    }

    // Run in appropriate mode
    if (webMode)
    {
        return runWebMode();
    }
    else
    {
        return runCLIMode();
    }
}