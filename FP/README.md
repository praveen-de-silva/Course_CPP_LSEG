# 🌸 FLOWER EXCHANGE - Order Matching Engine

**C++ Workshop Series for UoM | LSEG Technology**

A comprehensive trading exchange system with both command-line and modern web interfaces, demonstrating professional C++ development practices and SOLID design principles.

## 🚀 Features

- **Dual Interface Support**: Traditional CSV processing + Modern Web UI
- **Real-time Order Matching**: Price-time priority matching engine
- **5 Flower Instruments**: Rose, Lavender, Lotus, Tulip, Orchid
- **Order Types**: Market orders with validation and execution reporting
- **Professional Architecture**: SOLID principles, clean code, extensive documentation

## 📁 Project Structure

```
FP/
├── 📄 Core C++ Engine
│   ├── main.cpp              # Unified entry point (CLI + Web modes)
│   ├── Exchange.{h,cpp}      # Main exchange engine
│   ├── OrderBook.{h,cpp}     # Order matching logic
│   ├── Order.{h,cpp}         # Order representation
│   ├── ExecutionReport.{h,cpp} # Trade reporting
│   ├── CSVReader.{h,cpp}     # CSV input processing
│   └── CSVWriter.{h,cpp}     # CSV output generation
│
├── 🌐 Web Interface
│   ├── web/
│   │   ├── server/
│   │   │   ├── WebServer.{h,cpp}  # HTTP server & REST API
│   │   │   └── .gitkeep
│   │   └── client/
│   │       ├── index.html         # Main UI page
│   │       └── assets/
│   │           ├── style.css      # Light mode styling
│   │           └── script.js      # Frontend logic
│
├── 📋 Data & Configuration
│   ├── orders.csv            # Input orders file
│   ├── execution_rep.csv     # Output execution reports
│   └── Project-Flower-Exchange.{txt,pdf} # Requirements
│
├── 🔧 Build System
│   ├── Makefile              # Cross-platform build system
│   └── README.md             # This documentation
│
└── 📦 Build Artifacts
    ├── flower_exchange.exe      # Full version (Web UI + CLI)
    ├── flower_exchange_cli.exe  # Lightweight CLI-only version
    └── *.o                      # Object files
```

## 🛠️ Building the Project

### Prerequisites
- **C++ Compiler**: GCC 7+ or Clang 5+ (C++17 support)
- **Make**: GNU Make or compatible
- **Web Browser**: For web interface (Chrome, Firefox, Safari, Edge)

### Build Options

```bash
# Build full version with web UI (recommended)
make web                    # or just 'make'

# Build lightweight CLI-only version
make cli

# Build both versions
make both

# Development build with debug symbols
make debug

# Clean build artifacts
make clean

# Show all available targets
make help
```

### Platform-Specific Notes

**Windows (MinGW/MSYS2):**
```bash
# Install dependencies
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make

# Build (will link ws2_32 automatically)
make web
```

**Linux/macOS:**
```bash
# Install GCC/Clang if needed
sudo apt install build-essential  # Ubuntu/Debian
brew install gcc                  # macOS

# Build
make web
```

## 🎯 Usage

### Command Line Interface

```bash
# Default: CSV processing mode
./flower_exchange.exe

# Explicit CSV mode
./flower_exchange.exe --csv

# Web UI mode (default port 8080)
./flower_exchange.exe --web-ui

# Web UI mode (custom port)
./flower_exchange.exe --web-ui 9000

# Show help
./flower_exchange.exe --help
```

### Web Interface

1. **Start the server:**
   ```bash
   ./flower_exchange.exe --web-ui
   ```

2. **Open browser:** Navigate to `http://localhost:8080`

3. **Features available:**
   - **Order Entry**: Submit buy/sell orders with real-time validation
   - **Order Book View**: Live visualization of market depth
   - **Execution Reports**: Real-time trade confirmations
   - **CSV Import/Export**: Batch processing capabilities
   - **Multi-Instrument Support**: Switch between flower types
   - **Light Mode Theme**: Clean, professional interface

### CSV Format

**Input (orders.csv):**
```csv
ClientOrderID,Instrument,Side,Quantity,Price
ord001,Rose,1,100,55.50
ord002,Rose,2,50,56.00
```

**Output (execution_rep.csv):**
```csv
Client Order ID,Order ID,Instrument,Side,Exec Status,Quantity,Price,Reason,Transaction Time
ord001,ord1,Rose,1,New,100,55.50,,20260325-143022.123
```

## ⚡ Quick Start

```bash
# 1. Create sample data
make sample

# 2. Test CLI version
make test-cli

# 3. Test web version
make test-web
# Then open http://localhost:8080
```

## 🏗️ Architecture & Design

### Core Principles

- **Single Responsibility**: Each class has one clear purpose
- **Open/Closed**: Extensible design for new instruments/order types
- **Liskov Substitution**: Proper inheritance hierarchies
- **Interface Segregation**: Minimal, focused interfaces
- **Dependency Inversion**: Abstract dependencies, concrete implementations

### Key Components

**Exchange Engine:**
- Validates incoming orders
- Routes to appropriate OrderBook
- Generates unique order IDs and sequence numbers

**OrderBook:**
- Price-time priority matching
- Separate buy/sell sides with optimal sorting
- Efficient O(log n) insertion using std::multiset

**WebServer:**
- RESTful API endpoints
- Static file serving
- Cross-platform socket implementation

**Frontend:**
- Vanilla JavaScript (no framework dependencies)
- Real-time order book visualization
- Responsive design with light theme

## 📊 Performance Characteristics

- **Order Processing**: O(log n) per order using balanced trees
- **Memory Usage**: Efficient with pre-allocated vectors
- **Network**: HTTP/1.1 with connection reuse
- **Concurrency**: Single-threaded (suitable for educational purposes)

## 🧪 Testing

### Automated Testing
```bash
# Quick CLI test with sample data
make test-cli

# Web server functionality test
make test-web
```

### Manual Testing Scenarios

1. **Order Validation**: Submit invalid orders (wrong quantities, prices)
2. **Matching Logic**: Test buy orders against existing sells
3. **Partial Fills**: Large orders consuming multiple levels
4. **Multi-Instrument**: Orders across different flower types
5. **CSV Import**: Batch processing of order files

## 🔧 Development

### Adding New Features

1. **New Instrument**: Add to `validInstruments_` set in Exchange
2. **Order Types**: Extend Order class with new fields
3. **Matching Rules**: Modify OrderBook matching logic
4. **UI Components**: Update frontend JavaScript and CSS

### Debugging

```bash
# Build with debug symbols
make debug

# Use GDB for debugging
gdb ./flower_exchange.exe
```

### Code Style

- **Naming**: CamelCase for classes, snake_case for variables
- **Comments**: Doxygen-style documentation
- **Headers**: Include guards and forward declarations
- **Memory**: RAII principles, smart pointers where appropriate

## 🚨 Known Limitations

- **Single-threaded**: Not suitable for high-frequency trading
- **In-memory**: Order book state not persisted
- **Basic Authentication**: No user authentication in web UI
- **Limited Protocols**: HTTP only (no WebSocket for real-time updates)

## 📚 Educational Value

This project demonstrates:

- **Modern C++**: C++17 features, STL containers, smart pointers
- **Design Patterns**: Factory, Strategy, Observer patterns
- **Software Architecture**: Layered architecture, separation of concerns
- **Web Development**: HTTP servers, REST APIs, responsive UI
- **Build Systems**: Makefiles, cross-platform compilation
- **Documentation**: Professional README, code comments

## 🤝 Contributing

This is an educational project for the LSEG C++ Workshop. Improvements welcome:

1. Fork the repository
2. Create feature branch
3. Implement changes with tests
4. Submit pull request

## 📄 License

Educational use only. Created for the UoM C++ Workshop Series by LSEG Technology.

---

**Built with ❤️ for learning C++ and financial systems development**

For questions or support, please refer to the workshop materials or contact the LSEG Technology team.