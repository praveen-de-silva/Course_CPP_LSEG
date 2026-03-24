/*
 * Flower Exchange - Order Matching Engine
 * C++ Workshop Series for UoM | LSEG Technology
 *
 * This program:
 *   1. Reads orders from orders.csv
 *   2. Validates and processes each order through the exchange
 *   3. Writes execution reports to execution_rep.csv
 *
 * Architecture follows SOLID principles:
 *   - CSVReader:   Reads input (Single Responsibility)
 *   - Exchange:    Validates orders and routes to OrderBooks
 *   - OrderBook:   Handles order matching logic
 *   - CSVWriter:   Writes output
 */

#include "CSVReader.h"
#include "Exchange.h"
#include "CSVWriter.h"
#include <iostream>
#include <vector>

int main()
{
    // Performance optimization for I/O
    std::ios::sync_with_stdio(false);

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

        std::cout << "Done! Check execution_rep.csv for results." << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
