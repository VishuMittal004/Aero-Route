# Flight Simulator Project

A comprehensive flight simulation system consisting of a ticket booking interface and a visual flight path simulator.

## Project Overview

This project combines a terminal-based flight booking system with an SFML-powered flight path visualization tool. Users can book flights between airports and then watch the selected flight path being simulated with real-time weather conditions and potential rerouting.

## Components

### 1. Booking System (`booking_system.cpp`)

The booking system provides a user-friendly terminal interface to:
- Select departure and arrival airports
- View flight options for 5 days with dynamic pricing
- Enter passenger details
- Confirm bookings
- Launch the flight simulator with the selected airports

### 2. Flight Simulator (`flight_simulator.cpp`)

The SFML-based flight simulator visualizes:
- Flight paths between airports
- Dynamic weather conditions
- Rerouting based on conditions
- Animated aircraft movement
- Map-based visualization

## Dependencies

- [SFML](https://www.sfml-dev.org/) (Simple and Fast Multimedia Library) for graphics
- C++ compiler supporting C++11 or later
- Windows environment (for current build)

## Setup and Installation

1. Ensure all project files are in the same directory
2. The required SFML DLLs are included in the project folder
3. Make sure the `assets` and `resources` folders are present with their contents

## How to Compile

Use the included batch file to compile the project:

```
compile.bat
```

Alternatively, you can compile manually:

```
g++ -c booking_system.cpp
g++ booking_system.o -o booking_system.exe
g++ -c flight_simulator.cpp
g++ flight_simulator.o -o flight_simulator.exe -lsfml-graphics -lsfml-window -lsfml-system
```

## How to Run

1. **Full Experience (Recommended)**: 
   - Run the booking system:
   ```
   booking_system.exe
   ```
   - Complete the booking process and the flight simulator will launch automatically

2. **Flight Simulator Only**:
   - Run the flight simulator directly with source and destination airports:
   ```
   flight_simulator.exe [source_airport] [destination_airport]
   ```

## Project Structure

- `booking_system.cpp` - Source code for the booking system
- `flight_simulator.cpp` - Source code for the flight simulator
- `compile.bat` - Batch file for compiling the project
- `assets/` - Contains graphical assets like the map
- `resources/` - Contains fonts and other resources
- `*.dll` - SFML library dependencies

## Features

- Dynamic flight pricing based on day selection
- Weather condition simulation affecting flight paths
- Visual representation of the flight journey
- Smooth animation for aircraft movement
- Comprehensive booking workflow
- Integration between booking and simulation

## License

This project is created for educational purposes.

## Author

[Your Name]
