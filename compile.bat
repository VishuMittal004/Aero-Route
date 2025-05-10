@echo off
echo Compiling Flight Simulator...
g++ -o flight_simulator flight_simulator.cpp -lsfml-graphics -lsfml-window -lsfml-system
if %ERRORLEVEL% NEQ 0 (
    echo Error compiling flight_simulator.cpp
    pause
    exit /b 1
)

echo Compiling Booking System...
g++ -o booking_system booking_system.cpp
if %ERRORLEVEL% NEQ 0 (
    echo Error compiling booking_system.cpp
    pause
    exit /b 1
)

echo Compilation successful!
echo.
echo Run the booking system with: booking_system.exe
echo.
pause
