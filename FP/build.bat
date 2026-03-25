@echo off
REM Flower Exchange - Build and Run Script for Windows
REM C++ Workshop Series for UoM | LSEG Technology

setlocal

echo.
echo ================================================================
echo   FLOWER EXCHANGE - Windows Build and Run Script
echo   C++ Workshop Series for UoM ^| LSEG Technology
echo ================================================================
echo.

if "%1"=="" goto :menu
if "%1"=="help" goto :help
if "%1"=="cli" goto :build_cli
if "%1"=="web" goto :build_web
if "%1"=="both" goto :build_both
if "%1"=="clean" goto :clean
if "%1"=="sample" goto :sample
if "%1"=="test-cli" goto :test_cli
if "%1"=="test-web" goto :test_web
if "%1"=="run-cli" goto :run_cli
if "%1"=="run-web" goto :run_web

:menu
echo Available Commands:
echo.
echo   cli           - Build CLI-only version
echo   web           - Build web UI version (default)
echo   both          - Build both versions
echo   clean         - Clean build artifacts
echo   sample        - Create sample orders.csv
echo   test-cli      - Build and test CLI version
echo   test-web      - Build and start web server
echo   run-cli       - Run existing CLI version
echo   run-web       - Run existing web version
echo   help          - Show detailed help
echo.
set /p choice="Enter command (or press Enter for web): "
if "%choice%"=="" set choice=web
goto :build_%choice%

:help
echo.
echo FLOWER EXCHANGE - Detailed Help
echo ==============================
echo.
echo This script helps you build and run the Flower Exchange system.
echo.
echo BUILD COMMANDS:
echo   build.bat web      Build full version with web UI
echo   build.bat cli      Build lightweight CLI-only version
echo   build.bat both     Build both versions
echo   build.bat clean    Remove all build artifacts
echo.
echo TEST COMMANDS:
echo   build.bat sample   Create sample orders.csv file
echo   build.bat test-cli Build and test CLI with sample data
echo   build.bat test-web Build and start web server
echo.
echo RUN COMMANDS:
echo   build.bat run-cli  Run existing CLI version
echo   build.bat run-web  Run existing web version
echo.
echo The unified executable supports both modes:
echo   flower_exchange.exe --csv      (CSV processing)
echo   flower_exchange.exe --web-ui   (Web interface)
echo.
goto :end

:build_cli
echo Building CLI-only version...
echo.
REM Check if g++ is available
g++ --version > nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: g++ not found. Please install MinGW or GCC.
    goto :end
)

g++ -std=c++17 -Wall -Wextra -O2 -DCLI_ONLY -o flower_exchange_cli.exe ^
    main.cpp ^
    CSVReader.cpp ^
    CSVWriter.cpp ^
    Exchange.cpp ^
    ExecutionReport.cpp ^
    Order.cpp ^
    OrderBook.cpp

if %errorlevel% == 0 (
    echo ✓ CLI build successful: flower_exchange_cli.exe
    echo   Usage: flower_exchange_cli.exe
) else (
    echo ✗ CLI build failed
)
goto :end

:build_web
echo Building web version...
echo.
REM Check if g++ is available
g++ --version > nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo Error: g++ not found. Please install MinGW or GCC.
    goto :end
)

g++ -std=c++17 -Wall -Wextra -O2 -lws2_32 -o flower_exchange.exe ^
    main.cpp ^
    CSVReader.cpp ^
    CSVWriter.cpp ^
    Exchange.cpp ^
    ExecutionReport.cpp ^
    Order.cpp ^
    OrderBook.cpp ^
    web/server/WebServer.cpp

if %errorlevel% == 0 (
    echo ✓ Web build successful: flower_exchange.exe
    echo   Usage:
    echo     flower_exchange.exe --csv      (CSV mode)
    echo     flower_exchange.exe --web-ui   (Web UI mode)
) else (
    echo ✗ Web build failed
)
goto :end

:build_both
echo Building both versions...
echo.
call :build_cli
if %errorlevel% NEQ 0 goto :end
echo.
call :build_web
goto :end

:clean
echo Cleaning build artifacts...
if exist "*.exe" del /Q *.exe
if exist "*.o" del /Q *.o
if exist "web\server\*.o" del /Q web\server\*.o
echo ✓ Clean completed
goto :end

:sample
echo Creating sample orders.csv...
echo ClientOrderID,Instrument,Side,Quantity,Price > orders.csv
echo ord001,Rose,1,100,55.50 >> orders.csv
echo ord002,Rose,2,50,56.00 >> orders.csv
echo ord003,Lavender,1,200,45.25 >> orders.csv
echo ord004,Tulip,2,150,32.75 >> orders.csv
echo ord005,Lotus,1,80,42.00 >> orders.csv
echo ✓ Sample file created with 5 test orders
echo Contents:
type orders.csv
goto :end

:test_cli
call :build_cli
if %errorlevel% NEQ 0 goto :end
echo.
call :sample
echo.
echo Testing CLI version...
if exist "flower_exchange_cli.exe" (
    flower_exchange_cli.exe
) else if exist "flower_exchange.exe" (
    flower_exchange.exe --csv
) else (
    echo ✗ No executable found
)
goto :end

:test_web
call :build_web
if %errorlevel% NEQ 0 goto :end
echo.
echo Starting web server on http://localhost:8080
echo Open your browser and navigate to: http://localhost:8080
echo Press Ctrl+C to stop the server
echo.
flower_exchange.exe --web-ui
goto :end

:run_cli
if exist "flower_exchange_cli.exe" (
    echo Running CLI version...
    flower_exchange_cli.exe
) else if exist "flower_exchange.exe" (
    echo Running unified version in CSV mode...
    flower_exchange.exe --csv
) else (
    echo ✗ No executable found. Run 'build.bat cli' or 'build.bat web' first.
    echo   Available commands:
    echo     build.bat cli    - Build CLI version
    echo     build.bat web    - Build web version
)
goto :end

:run_web
if exist "flower_exchange.exe" (
    echo Starting web server on http://localhost:8080
    echo Open your browser and navigate to: http://localhost:8080
    echo Press Ctrl+C to stop the server
    echo.
    flower_exchange.exe --web-ui
) else (
    echo ✗ flower_exchange.exe not found. Run 'build.bat web' first.
)
goto :end

:end
echo.
pause