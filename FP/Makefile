# Flower Exchange - Makefile
# C++ Workshop Series for UoM | LSEG Technology
# Updated to support unified main.cpp with CLI and Web UI modes

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS =

# Windows specific libraries
ifeq ($(OS),Windows_NT)
    LDFLAGS += -lws2_32
endif

# Source files
CORE_SOURCES = CSVReader.cpp CSVWriter.cpp Exchange.cpp ExecutionReport.cpp Order.cpp OrderBook.cpp main.cpp
WEB_SOURCES = web/server/WebServer.cpp

# Object files
CORE_OBJECTS = $(CORE_SOURCES:.cpp=.o)
WEB_OBJECTS = $(WEB_SOURCES:.cpp=.o)

# Targets
.PHONY: all clean cli web both help sample test-cli test-web debug

# Default target
all: web

# Build CLI-only version (excludes web server to reduce dependencies)
cli: flower_exchange_cli.exe

# Build full version with web UI support
web: flower_exchange.exe

# Build both versions
both: cli web

help:
	@echo "Flower Exchange Build System"
	@echo "============================"
	@echo ""
	@echo "Targets:"
	@echo "  web        - Build full version with web UI support (default)"
	@echo "  cli        - Build lightweight CLI-only version"
	@echo "  both       - Build both versions"
	@echo "  clean      - Clean object files and executables"
	@echo "  sample     - Create sample orders.csv file"
	@echo "  test-cli   - Build and test CLI version"
	@echo "  test-web   - Build and run web version"
	@echo "  debug      - Build with debug symbols"
	@echo "  help       - Show this help"
	@echo ""
	@echo "Usage Examples:"
	@echo "  make                    # Build web version (default)"
	@echo "  make cli               # Build CLI-only version"
	@echo "  make test-cli          # Quick CLI test"
	@echo "  make clean web         # Clean and rebuild"
	@echo ""
	@echo "Running the Program:"
	@echo "  ./flower_exchange.exe           # CSV mode (default)"
	@echo "  ./flower_exchange.exe --csv     # CSV mode (explicit)"
	@echo "  ./flower_exchange.exe --web-ui  # Web UI on port 8080"
	@echo "  ./flower_exchange.exe --web-ui 9000  # Web UI on port 9000"
	@echo "  ./flower_exchange.exe --help    # Show program help"

# CLI version (lightweight, no web dependencies)
flower_exchange_cli.exe: CXXFLAGS += -DCLI_ONLY
flower_exchange_cli.exe: $(CORE_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Full version with web UI
flower_exchange.exe: $(CORE_OBJECTS) $(WEB_OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Object file compilation
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Create sample orders.csv for testing
sample:
	@echo "ClientOrderID,Instrument,Side,Quantity,Price" > orders.csv
	@echo "ord001,Rose,1,100,55.50" >> orders.csv
	@echo "ord002,Rose,2,50,56.00" >> orders.csv
	@echo "ord003,Lavender,1,200,45.25" >> orders.csv
	@echo "ord004,Tulip,2,150,32.75" >> orders.csv
	@echo "ord005,Lotus,1,80,42.00" >> orders.csv
	@echo "Sample orders.csv created with 5 test orders"

# Test CLI version
test-cli: cli sample
	@echo "Testing CLI version..."
	./flower_exchange_cli.exe

# Test web version (starts server)
test-web: web
	@echo "Starting web server on http://localhost:8080"
	@echo "Press Ctrl+C to stop..."
	./flower_exchange.exe --web-ui

# Build with debug symbols
debug: CXXFLAGS += -g -DDEBUG
debug: web

# Clean up
clean:
	rm -f *.o web/server/*.o *.exe

# Dependencies
CSVReader.o: CSVReader.cpp CSVReader.h Order.h
CSVWriter.o: CSVWriter.cpp CSVWriter.h ExecutionReport.h
Exchange.o: Exchange.cpp Exchange.h Order.h OrderBook.h ExecutionReport.h
ExecutionReport.o: ExecutionReport.cpp ExecutionReport.h
Order.o: Order.cpp Order.h
OrderBook.o: OrderBook.cpp OrderBook.h Order.h ExecutionReport.h
main.o: main.cpp CSVReader.h Exchange.h CSVWriter.h web/server/WebServer.h
web/server/WebServer.o: web/server/WebServer.cpp web/server/WebServer.h Exchange.h