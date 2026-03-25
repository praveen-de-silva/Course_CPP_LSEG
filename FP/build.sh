#!/bin/bash
# Flower Exchange - Build and Run Script for Linux/macOS
# C++ Workshop Series for UoM | LSEG Technology

set -e

echo ""
echo "================================================================"
echo "  FLOWER EXCHANGE - Linux/macOS Build and Run Script"
echo "  C++ Workshop Series for UoM | LSEG Technology"
echo "================================================================"
echo ""

# Function declarations
show_help() {
    echo ""
    echo "FLOWER EXCHANGE - Detailed Help"
    echo "==============================="
    echo ""
    echo "This script helps you build and run the Flower Exchange system."
    echo ""
    echo "BUILD COMMANDS:"
    echo "  ./build.sh web         Build full version with web UI (default)"
    echo "  ./build.sh cli         Build lightweight CLI-only version"
    echo "  ./build.sh both        Build both versions"
    echo "  ./build.sh clean       Remove all build artifacts"
    echo ""
    echo "TEST COMMANDS:"
    echo "  ./build.sh sample      Create sample orders.csv file"
    echo "  ./build.sh test-cli    Build and test CLI with sample data"
    echo "  ./build.sh test-web    Build and start web server"
    echo ""
    echo "RUN COMMANDS:"
    echo "  ./build.sh run-cli     Run existing CLI version"
    echo "  ./build.sh run-web     Run existing web version"
    echo ""
    echo "EXAMPLES:"
    echo "  ./build.sh             # Build web version (default)"
    echo "  ./build.sh test-cli    # Quick CLI test"
    echo "  ./build.sh test-web    # Start web server"
    echo ""
    echo "REQUIREMENTS:"
    echo "  - GCC 7+ or Clang 5+ with C++17 support"
    echo "  - Make (GNU Make)"
    echo ""
}

check_compiler() {
    if ! command -v g++ &> /dev/null; then
        echo "Error: g++ not found. Please install GCC or Clang."
        echo ""
        echo "Installation instructions:"
        echo "  Ubuntu/Debian: sudo apt install build-essential"
        echo "  CentOS/RHEL:   sudo yum groupinstall 'Development Tools'"
        echo "  macOS:         brew install gcc"
        echo ""
        exit 1
    fi
}

build_cli() {
    echo "Building CLI-only version..."
    check_compiler
    make clean > /dev/null 2>&1 || true
    make cli

    if [ -f "flower_exchange_cli" ] || [ -f "flower_exchange_cli.exe" ]; then
        echo "✓ CLI build successful"
        echo "  Usage: ./flower_exchange_cli"
    else
        echo "✗ CLI build failed"
        exit 1
    fi
}

build_web() {
    echo "Building web version..."
    check_compiler
    make clean > /dev/null 2>&1 || true
    make web

    if [ -f "flower_exchange" ] || [ -f "flower_exchange.exe" ]; then
        echo "✓ Web build successful"
        echo "  Usage:"
        echo "    ./flower_exchange --csv      (CSV mode)"
        echo "    ./flower_exchange --web-ui   (Web UI mode)"
    else
        echo "✗ Web build failed"
        exit 1
    fi
}

build_both() {
    echo "Building both versions..."
    build_cli
    echo ""
    build_web
}

clean_build() {
    echo "Cleaning build artifacts..."
    make clean
    echo "✓ Clean completed"
}

create_sample() {
    echo "Creating sample orders.csv..."
    make sample
    echo "✓ Sample file created"
    echo "Contents:"
    cat orders.csv
}

test_cli() {
    build_cli
    echo ""
    create_sample
    echo ""
    echo "Testing CLI version..."

    if [ -f "flower_exchange_cli" ]; then
        ./flower_exchange_cli
    elif [ -f "flower_exchange" ]; then
        ./flower_exchange --csv
    else
        echo "✗ No executable found"
        exit 1
    fi
}

test_web() {
    build_web
    echo ""
    echo "Starting web server on http://localhost:8080"
    echo "Open your browser and navigate to: http://localhost:8080"
    echo "Press Ctrl+C to stop the server"
    echo ""

    if [ -f "flower_exchange" ]; then
        ./flower_exchange --web-ui
    elif [ -f "flower_exchange.exe" ]; then
        ./flower_exchange.exe --web-ui
    else
        echo "✗ No executable found"
        exit 1
    fi
}

run_cli() {
    if [ -f "flower_exchange_cli" ]; then
        echo "Running CLI version..."
        ./flower_exchange_cli
    elif [ -f "flower_exchange" ]; then
        echo "Running unified version in CSV mode..."
        ./flower_exchange --csv
    else
        echo "✗ No executable found. Run './build.sh cli' or './build.sh web' first."
        echo "  Available commands:"
        echo "    ./build.sh cli    - Build CLI version"
        echo "    ./build.sh web    - Build web version"
        exit 1
    fi
}

run_web() {
    if [ -f "flower_exchange" ]; then
        echo "Starting web server on http://localhost:8080"
        echo "Open your browser and navigate to: http://localhost:8080"
        echo "Press Ctrl+C to stop the server"
        echo ""
        ./flower_exchange --web-ui
    else
        echo "✗ flower_exchange not found. Run './build.sh web' first."
        exit 1
    fi
}

show_menu() {
    echo "Available Commands:"
    echo ""
    echo "  cli           - Build CLI-only version"
    echo "  web           - Build web UI version (default)"
    echo "  both          - Build both versions"
    echo "  clean         - Clean build artifacts"
    echo "  sample        - Create sample orders.csv"
    echo "  test-cli      - Build and test CLI version"
    echo "  test-web      - Build and start web server"
    echo "  run-cli       - Run existing CLI version"
    echo "  run-web       - Run existing web version"
    echo "  help          - Show detailed help"
    echo ""
    read -p "Enter command (or press Enter for web): " choice
    choice=${choice:-web}

    case "$choice" in
        cli) build_cli ;;
        web) build_web ;;
        both) build_both ;;
        clean) clean_build ;;
        sample) create_sample ;;
        test-cli) test_cli ;;
        test-web) test_web ;;
        run-cli) run_cli ;;
        run-web) run_web ;;
        help) show_help ;;
        *) echo "Unknown command: $choice" ; show_help ; exit 1 ;;
    esac
}

# Main script logic
case "${1:-}" in
    cli) build_cli ;;
    web) build_web ;;
    both) build_both ;;
    clean) clean_build ;;
    sample) create_sample ;;
    test-cli) test_cli ;;
    test-web) test_web ;;
    run-cli) run_cli ;;
    run-web) run_web ;;
    help) show_help ;;
    "") show_menu ;;
    *) echo "Unknown command: $1" ; show_help ; exit 1 ;;
esac

echo ""