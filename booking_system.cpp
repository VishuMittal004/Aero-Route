#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <ctime>
#include <random>
#include <chrono>
#include <sstream>
using namespace std;

struct Airport {
    std::string code;
    std::string name;
};

struct Date {
    int day;
    int month;
    int year;
    
    std::string toString() const {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << day << "/"
        << std::setfill('0') << std::setw(2) << month << "/"
        << year;
        return ss.str();
    }
    
    static Date getCurrentDate() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time);
        
        Date currentDate;
        currentDate.day = now_tm->tm_mday;
        currentDate.month = now_tm->tm_mon + 1;
        currentDate.year = now_tm->tm_year + 1900;
        
        return currentDate;
    }
    
    Date addDays(int days) const {
        Date newDate = *this;
        newDate.day += days;
        
        if (newDate.day > 28) {
            newDate.day = newDate.day % 28;
            newDate.month += 1;
            if (newDate.month > 12) {
                newDate.month = 1;
                newDate.year += 1;
            }
        }
        
        return newDate;
    }
};

struct FlightTicket {
    std::string departureAirport;
    std::string arrivalAirport;
    std::string departureDate;
    std::string departureTime;
    std::string arrivalTime;
    double price;
    std::string passengerName;
    std::string seatNumber;
    std::string bookingReference;
    bool isBooked;
};

std::string generateRandomTime() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> hour_dist(0, 23);
    std::uniform_int_distribution<> minute_dist(0, 59);
    
    int hour = hour_dist(gen);
    int minute = minute_dist(gen);
    
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << hour << ":"
       << std::setfill('0') << std::setw(2) << minute;
    
    return ss.str();
}

std::pair<std::string, std::string> generateFlightTimes(double distance) {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    std::string departureTime = generateRandomTime();
    
    int durationHours = static_cast<int>(distance / 100) + 1;
    std::uniform_int_distribution<> minute_dist(0, 59);
    int durationMinutes = minute_dist(gen);
    
    int depHour, depMinute;
    sscanf(departureTime.c_str(), "%d:%d", &depHour, &depMinute);
    
    int arrHour = depHour + durationHours;
    int arrMinute = depMinute + durationMinutes;
   
    if (arrMinute >= 60) {
        arrHour += 1;
        arrMinute -= 60;
    }
    
    if (arrHour >= 24) {
        arrHour -= 24;
    }
    
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << arrHour << ":"
       << std::setfill('0') << std::setw(2) << arrMinute;
    
    return {departureTime, ss.str()};
}

double generateRandomPrice(double distance) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0.8, 1.2); 
    
    double basePrice = distance * 0.5; 
    return basePrice * dist(gen);
}

std::string generateSeatNumber() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> row_dist(1, 30);
    std::uniform_int_distribution<> col_dist(0, 5); 
    
    int row = row_dist(gen);
    char col = 'A' + col_dist(gen);
    
    std::stringstream ss;
    ss << row << col;
    
    return ss.str();
}

std::string generateBookingReference() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> char_dist(0, 25); 
    std::uniform_int_distribution<> num_dist(0, 9);   
    
    std::stringstream ss;
    
    for (int i = 0; i < 2; ++i) {
        ss << static_cast<char>('A' + char_dist(gen));
    }
    
    for (int i = 0; i < 4; ++i) {
        ss << num_dist(gen);
    }
    
    return ss.str();
}

void printLine(char c = '-', int length = 50) {
    for (int i = 0; i < length; i++) {
        std::cout << c;
    }
    std::cout << std::endl;
}

FlightTicket bookFlight(const std::vector<Airport>& airports, int src, int dst) {

    double distance = 500 + (src * 100) + (dst * 50); 
    
    Date currentDate = Date::getCurrentDate();
   
    std::vector<FlightTicket> flightOptions;
   
    printLine('=');
    std::cout << "AVAILABLE FLIGHTS" << std::endl;
    printLine('=');
    std::cout << "From: " << airports[src].code << " To: " << airports[dst].code << std::endl << std::endl;
    std::cout << std::left << std::setw(5) << "No." 
              << std::setw(12) << "Date" 
              << std::setw(12) << "Departure" 
              << std::setw(12) << "Arrival" 
              << std::setw(10) << "Price" 
              << std::endl;
    printLine();
    
    for (int i = 0; i < 5; ++i) {
        Date flightDate = currentDate.addDays(i);
        auto [departureTime, arrivalTime] = generateFlightTimes(distance);
        double price = generateRandomPrice(distance);
        
        FlightTicket ticket;
        ticket.departureAirport = airports[src].code;
        ticket.arrivalAirport = airports[dst].code;
        ticket.departureDate = flightDate.toString();
        ticket.departureTime = departureTime;
        ticket.arrivalTime = arrivalTime;
        ticket.price = price;
        ticket.isBooked = false;
        
        flightOptions.push_back(ticket);
        
        std::cout << std::left << std::setw(5) << (i + 1) 
                  << std::setw(12) << flightDate.toString() 
                  << std::setw(12) << departureTime 
                  << std::setw(12) << arrivalTime 
                  << "$" << std::fixed << std::setprecision(2) << price;
        std::cout << std::endl;
        std::cout.flush();
    }
    printLine();
    
    int selection;
    do {
        std::cout << "Select a flight (1-5): ";
        std::cout.flush();
        std::cin >> selection;
        
        if (selection < 1 || selection > 5) {
            std::cout << "Invalid selection. Please enter a number between 1 and 5." << std::endl;
            std::cout.flush();
        }
    } while (selection < 1 || selection > 5);
    
    FlightTicket selectedTicket = flightOptions[selection - 1];
    
    std::string passengerName;
    std::cout << "Enter passenger name: ";
    std::cout.flush();
    std::cin.ignore(); 
    std::getline(std::cin, passengerName);
    
    std::cout << "Processing your booking..." << std::endl;
    std::cout.flush();
    
    selectedTicket.passengerName = passengerName;
    selectedTicket.seatNumber = generateSeatNumber();
    selectedTicket.bookingReference = generateBookingReference();
    selectedTicket.isBooked = true;
    
    printLine('*');
    std::cout << "BOOKING CONFIRMATION" << std::endl;
    printLine('*');
    std::cout << "Booking Reference: " << selectedTicket.bookingReference << std::endl;
    std::cout << "Passenger: " << selectedTicket.passengerName << std::endl;
    std::cout << "Flight: " << selectedTicket.departureAirport << " to " << selectedTicket.arrivalAirport << std::endl;
    std::cout << "Date: " << selectedTicket.departureDate << std::endl;
    std::cout << "Time: " << selectedTicket.departureTime << " - " << selectedTicket.arrivalTime << std::endl;
    std::cout << "Seat: " << selectedTicket.seatNumber << std::endl;
    std::cout << "Price: $" << std::fixed << std::setprecision(2) << selectedTicket.price << std::endl;
    printLine('*');
    
    std::cout << "Ticket booked successfully!" << std::endl;
    std::cout << "Now checking weather conditions for your flight..." << std::endl;
    std::cout.flush();
    
    return selectedTicket;
}

int resolveAirportIndex(const std::string& input, const std::vector<Airport>& airports) {
    bool isNumber = true;
    for (char c : input) {
        if (!std::isdigit(c)) {
            isNumber = false;
            break;
        }
    }
    
    if (isNumber) {
        int index = std::stoi(input);
        if (index >= 0 && index < airports.size()) {
            return index;
        }
    } else 
    {
        for (int i = 0; i < airports.size(); ++i) {
            if (airports[i].code == input) {
                return i;
            }
        }
    }
    
    return -1; 
}

int main() {
    std::ios_base::sync_with_stdio(true); 
    std::vector<Airport> airports = {
        {"JFK", "John F. Kennedy International Airport"},
        {"LAX", "Los Angeles International Airport"},
        {"ORD", "O'Hare International Airport"},
        {"DFW", "Dallas/Fort Worth International Airport"},
        {"ATL", "Hartsfield-Jackson Atlanta International Airport"},
        {"SFO", "San Francisco International Airport"},
        {"MIA", "Miami International Airport"},
        {"SEA", "Seattle-Tacoma International Airport"},
        {"DEN", "Denver International Airport"},
        {"BOS", "Boston Logan International Airport"},
        {"LAS", "Harry Reid International Airport"},
        {"PHX", "Phoenix Sky Harbor International Airport"},
        {"IAH", "George Bush Intercontinental Airport"},
        {"EWR", "Newark Liberty International Airport"},
        {"CLT", "Charlotte Douglas International Airport"}
    };

    printLine('=');
    std::cout << "WELCOME TO FLIGHT BOOKING SYSTEM" << std::endl;
    printLine('=');
    std::cout << "Available Airports (index: code):" << std::endl;
    for (int i = 0; i < airports.size(); ++i) {
        std::cout << "  " << i << ": " << airports[i].code << " - " << airports[i].name << std::endl;
    }
    printLine();

    std::string input;
    int src = -1, dst = -1;

    std::cout << "FLIGHT BOOKING" << std::endl;
    printLine();
    
    do {
        std::cout << "Enter departure index or code: ";
        std::cout.flush();
        std::cin >> input;
        src = resolveAirportIndex(input, airports);
        
        if (src < 0 || src >= airports.size()) {
            std::cout << "Invalid airport. Please try again." << std::endl;
            std::cout.flush();
        }
    } while (src < 0 || src >= airports.size());

    do {
        std::cout << "Enter arrival index or code:   ";
        std::cout.flush();
        std::cin >> input;
        dst = resolveAirportIndex(input, airports);
        
        if (dst < 0 || dst >= airports.size()) {
            std::cout << "Invalid airport. Please try again." << std::endl;
            std::cout.flush();
        }
    } while (dst < 0 || dst >= airports.size());

    FlightTicket ticket = bookFlight(airports, src, dst);
    
    printLine('=');
    std::cout << "WEATHER CONDITIONS UPDATE" << std::endl;
    printLine('=');
    
    std::cout << "Would you like to update weather conditions? (y/n): ";
    std::cout.flush();
    std::string updateWeather;
    std::cin >> updateWeather;

    if (updateWeather == "y" || updateWeather == "Y") {
        std::cout << "Enter number of weather updates: ";
        std::cout.flush();
        int numUpdates;
        std::cin >> numUpdates;

        for (int i = 0; i < numUpdates; ++i) {
            printLine('-');
            std::cout << "Update " << (i + 1) << ":" << std::endl;
            
            std::cout << "Enter airport 1 index: ";
            std::cout.flush();
            int a1;
            std::cin >> a1;

            std::cout << "Enter airport 2 index: ";
            std::cout.flush();
            int a2;
            std::cin >> a2;

            std::cout << "Enter weather condition (0 for good, 1 for bad): ";
            std::cout.flush();
            int condition;
            std::cin >> condition;

            std::string description;
            if (condition == 1) {
                std::cout << "Enter weather description: ";
                std::cout.flush();
                std::cin.ignore();
                std::getline(std::cin, description);
            } else {
                description = "Clear skies";
            }
            
            std::cout << "Weather updated: " 
            << airports[a1].code << " to " 
            << airports[a2].code << " - " 
            << (condition == 1 ? "Bad weather (" + description + ")" : "Clear skies") 
            << std::endl;
        }
    }

    std::cout << std::endl;
    printLine('=');
    std::cout << "LAUNCHING FLIGHT SIMULATOR" << std::endl;
    printLine('=');
    std::cout << "Starting flight simulation for " << ticket.departureAirport 
            << " to " << ticket.arrivalAirport << "..." << std::endl;

    std::string command = "flight_simulator.exe " + std::to_string(src) + " " + std::to_string(dst);
    std::cout << "Running: " << command << std::endl;
    
    int result = system(command.c_str());
    
    if (result != 0) {
        std::cout << "\nERROR: Could not launch flight simulator." << std::endl;
        std::cout << "This could be due to missing SFML libraries or other dependencies." << std::endl;
        std::cout << "\nFlight Summary:" << std::endl;
        printLine('-');
        std::cout << "Departure: " << ticket.departureAirport << std::endl;
        std::cout << "Arrival: " << ticket.arrivalAirport << std::endl;
        std::cout << "Date: " << ticket.departureDate << std::endl;
        std::cout << "Time: " << ticket.departureTime << " - " << ticket.arrivalTime << std::endl;
        std::cout << "\nThank you for using our booking system!" << std::endl;
    }
    
    std::cout << "\nPress Enter to exit...";
    std::cin.ignore(1);
    std::cin.get();
    
    return 0;
}
