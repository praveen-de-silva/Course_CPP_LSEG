@echo off
REM Flower Exchange Build Script (Updated)
REM C++ Workshop Series for UoM | LSEG Technology

echo === FLOWER EXCHANGE BUILD SYSTEM ===
echo.

REM Check if g++ is available
g++ --version > nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: g++ not found. Please install MinGW or GCC.
    pause
    exit /b 1
)

echo Building Flower Exchange...
echo.

REM Build CLI version (original)
echo [1/2] Building CLI version...
g++ -std=c++11 -Wall -O2 -o flower_exchange_cli.exe ^
    main.cpp ^
    CSVReader.cpp ^
    CSVWriter.cpp ^
    Exchange.cpp ^
    ExecutionReport.cpp ^
    Order.cpp ^
    OrderBook.cpp

if %ERRORLEVEL% EQU 0 (
    echo ✓ CLI version built successfully: flower_exchange_cli.exe
) else (
    echo ✗ CLI build failed
    pause
    exit /b 1
)

echo.

REM Build Web UI version (simple, compatible with older compilers)
echo [2/2] Building Web UI version...
g++ -std=c++11 -Wall -O2 -o flower_exchange_web.exe ^
    main_simple_web.cpp ^
    CSVReader.cpp ^
    CSVWriter.cpp ^
    Exchange.cpp ^
    ExecutionReport.cpp ^
    Order.cpp ^
    OrderBook.cpp ^
    web/server/SimpleWebServer.cpp ^
    -lws2_32

if %ERRORLEVEL% EQU 0 (
    echo ✓ Web UI version built successfully: flower_exchange_web.exe
) else (
    echo ✗ Web UI build failed
    pause
    exit /b 1
)

echo.
echo === BUILD COMPLETE ===
echo.
echo Available executables:
echo   flower_exchange_cli.exe  - CLI mode (processes orders.csv)
echo   flower_exchange_web.exe  - Web UI mode (starts server on port 8080)
echo.
echo Usage:
echo   flower_exchange_cli.exe                - Run CLI mode
echo   flower_exchange_web.exe --web-ui       - Start web server
echo   flower_exchange_web.exe --help         - Show help
echo.
echo Web UI Access:
echo   http://localhost:8080                  - Trading interface
echo   http://localhost:8080/api/reports      - API endpoint example
echo.
pause