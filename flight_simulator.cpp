#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <cmath>
#include <algorithm>
#include <string>
#include <cctype>
#include <iomanip>
#include <ctime>
#include <random>
#include <chrono>
#include <sstream>

struct Airport 
{
    std::string code;
    sf::Vector2f position;  
};

struct WeatherCondition 
{
    bool isBad;
    std::string description;
};

struct FlightGraph 
{
    std::vector<Airport> airports;
    std::vector<std::vector<std::pair<int, double>>> adj;  
    std::vector<std::vector<bool>> pathAvailable; 
    std::vector<std::vector<WeatherCondition>> pathWeather;  

    void addAirport(const std::string& code, float x, float y) {
        airports.push_back({code, {x, y}});
        adj.emplace_back();

        for (auto& row : pathAvailable) row.push_back(false);
        pathAvailable.push_back(std::vector<bool>(airports.size(), false));
        
        for (auto& row : pathWeather) row.push_back({false, "Clear"});
        pathWeather.push_back(std::vector<WeatherCondition>(airports.size(), {false, "Clear"}));
    }

    void addEdge(int u, int v, double dist) 
    {
        adj[u].emplace_back(v, dist);
        adj[v].emplace_back(u, dist);
        pathAvailable[u][v] = true;
        pathAvailable[v][u] = true;
        pathWeather[u][v] = {false, "Clear"};
        pathWeather[v][u] = {false, "Clear"};
    }
    
    void updateWeather(int u, int v, bool isBad, const std::string& description) 
    {
        pathWeather[u][v] = {isBad, description};
        pathWeather[v][u] = {isBad, description};
        pathAvailable[u][v] = !isBad;
        pathAvailable[v][u] = !isBad;
    }
    
    bool hasBadWeather(const std::vector<int>& path) const 
    {
        for (size_t i = 0; i < path.size() - 1; ++i) 
        {
            int u = path[i];
            int v = path[i + 1];
            if (pathWeather[u][v].isBad) 
            {
                return true;
            }
        }
        return false;
    }
    std::vector<std::pair<std::string, std::string>> getPathWeatherInfo(const std::vector<int>& path) const 
    {
        std::vector<std::pair<std::string, std::string>> result;
        for (size_t i = 0; i < path.size() - 1; ++i) 
        {
            int u = path[i];
            int v = path[i + 1];
            if (pathWeather[u][v].isBad) 
            {
                result.push_back({airports[u].code + "-" + airports[v].code, pathWeather[u][v].description});
            }
        }
        return result;
    }

    std::vector<int> dijkstra(int src, int dst) const 
    {
        int n = adj.size();
        std::vector<double> dist(n, std::numeric_limits<double>::infinity());
        std::vector<int> prev(n, -1);
        dist[src] = 0;
    
        using PDI = std::pair<double, int>;
        std::priority_queue<PDI, std::vector<PDI>, std::greater<>> pq;
        pq.push({0, src});
    
        while (!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();
            if (u == dst) break;
            
            for (auto [v, w] : adj[u]) 
            {
                if (!pathAvailable[u][v]) continue;  
    
                double alt = d + w;
                if (alt < dist[v]) 
                {
                    dist[v] = alt;
                    prev[v] = u;
                    pq.push({alt, v});
                }
            }
        }
    
        std::vector<int> path;
        for (int at = dst; at != -1; at = prev[at])
            path.push_back(at);
        std::reverse(path.begin(), path.end());
    
        if (path.empty() || path.front() != src)
            return {};
        return path;
    }
    
    std::vector<int> findRouteWithWeatherRerouting(int src, int dst, bool& rerouted) 
    {
        std::vector<int> originalPath = dijkstra(src, dst);
        
        if (originalPath.empty() || !hasBadWeather(originalPath)) 
        {
            rerouted = false;
            return originalPath;
        }
        
        rerouted = true;

        FlightGraph tempGraph = *this;

        for (size_t i = 0; i < originalPath.size() - 1; ++i) 
        {
            int u = originalPath[i];
            int v = originalPath[i + 1];
            if (pathWeather[u][v].isBad) {
                
                tempGraph.pathAvailable[u][v] = false;
                tempGraph.pathAvailable[v][u] = false;
            }
        }
        
        return tempGraph.dijkstra(src, dst);
    }
};

struct FlightTicket 
{
    int departureAirportIndex;
    int arrivalAirportIndex;
    std::string departureDate;
    std::string departureTime;
    std::string arrivalTime;
    double price;
    std::string seatNumber;
    std::string passengerName;
    std::string bookingReference;
    bool isBooked;
};


struct Date
{
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
    
    static Date fromString(const std::string& dateStr)
    {
        Date date;
        std::stringstream ss(dateStr);
        char delimiter;
        ss >> date.day >> delimiter >> date.month >> delimiter >> date.year;
        return date;
    }
    
    Date addDays(int days) const 
    {
        Date newDate = *this;
        newDate.day += days;
    
        int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

        if (newDate.year % 4 == 0) 
        {
            daysInMonth[2] = 29;
        }
        
        while (newDate.day > daysInMonth[newDate.month]) 
        {
            newDate.day -= daysInMonth[newDate.month];
            newDate.month++;
            
            if (newDate.month > 12) 
            {
                newDate.month = 1;
                newDate.year++;
    
                if (newDate.year % 4 == 0) 
                {
                    daysInMonth[2] = 29;
                } 
                else 
                {
                    daysInMonth[2] = 28;
                }
            }
        }
        
        return newDate;
    }
    
    static Date getCurrentDate() 
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm* now_tm = std::localtime(&now_time);
        
        Date currentDate;
        currentDate.day = now_tm->tm_mday;
        currentDate.month = now_tm->tm_mon + 1; 
        currentDate.year = now_tm->tm_year + 1900; 
        
        return currentDate;
    }
};

void printLine(char c = '-', int length = 50) 
{
    for (int i = 0; i < length; i++) 
    {
        std::cout << c;
    }
    std::cout << std::endl;
}

int resolveAirportIndex(const std::string& input, const std::vector<Airport>& airports) 
{
    if (std::isdigit(input[0])) return std::stoi(input);
    for (size_t i = 0; i < airports.size(); ++i)
        if (airports[i].code == input)
            return static_cast<int>(i);
    return -1;
}

void visualizeGraph(const FlightGraph& graph, const std::vector<int>& path, bool rerouted = false) 
{
    sf::RenderWindow window(sf::VideoMode(900, 650), "Flight Path Visualization");
    window.setFramerateLimit(60); 

    sf::Font font;
    if (!font.loadFromFile("default.ttf"))
    {
        std::cerr << "Error loading font 'default.ttf'. Make sure it's available.\n";
        return;
    }

    sf::Color availableColor = sf::Color(100, 255, 100);
    sf::Color unavailableColor = sf::Color(255, 80, 80);
    sf::Color airportColor = sf::Color(50, 120, 250);
    sf::Color pathColor = sf::Color::Yellow;
    sf::Color reroutedPathColor = sf::Color(255, 165, 0); 
    sf::Color planeColor = sf::Color::White;
    sf::Color waypointColor = sf::Color::Magenta; 
    sf::Color badWeatherColor = sf::Color(255, 0, 0, 128); 

    std::cout << "\nVisualizing path: ";
    for (int idx : path) 
    {
        std::cout << graph.airports[idx].code << " ";
    }
    std::cout << "\n";

    std::vector<sf::CircleShape> airportShapes;
    std::vector<sf::Text> airportLabels;

    for (size_t i = 0; i < graph.airports.size(); ++i) 
    {
        const auto& airport = graph.airports[i];
        sf::CircleShape shape(8);
        shape.setFillColor(airportColor);
        shape.setPosition(airport.position);
        airportShapes.push_back(shape);

        sf::Text label;
        label.setFont(font);
        label.setString(airport.code);
        label.setCharacterSize(14);
        label.setFillColor(sf::Color::White);
        label.setPosition(airport.position.x + 12, airport.position.y - 5);
        airportLabels.push_back(label);
    }

    std::vector<sf::CircleShape> pathAirportShapes;
    if (!path.empty()) 
    {
        for (size_t i = 0; i < path.size(); ++i) {
            const auto& airport = graph.airports[path[i]];
            sf::CircleShape shape(10); 
            shape.setFillColor(waypointColor);
            shape.setPosition(airport.position.x - 2, airport.position.y - 2); 
            pathAirportShapes.push_back(shape);
        }
    }

    std::vector<sf::VertexArray> connectionLines;
    std::vector<sf::VertexArray> badWeatherLines; 

    std::vector<std::pair<int, int>> badWeatherPaths;

    for (int i = 0; i < graph.airports.size(); ++i) 
    {
        for (int j = i + 1; j < graph.airports.size(); ++j) 
        {
            sf::Color color;
            bool isBadWeather = graph.pathWeather[i][j].isBad;
            
            if (isBadWeather) 
            {    
                sf::VertexArray badWeatherLine(sf::Lines, 2);
                badWeatherLine[0] = sf::Vertex(graph.airports[i].position, badWeatherColor);
                badWeatherLine[1] = sf::Vertex(graph.airports[j].position, badWeatherColor);
                badWeatherLines.push_back(badWeatherLine);
        
                badWeatherPaths.push_back({i, j});
                badWeatherPaths.push_back({j, i}); 
                
                color = unavailableColor;
            } 
            else if (!graph.pathAvailable[i][j]) 
            {
                color = sf::Color(150, 150, 150); 
            }
            else 
            {
                color = availableColor; 
            }
            
            sf::VertexArray line(sf::Lines, 2);
            line[0] = sf::Vertex(graph.airports[i].position, color);
            line[1] = sf::Vertex(graph.airports[j].position, color);
            connectionLines.push_back(line);
        }
    }

    sf::Text statusText;
    statusText.setFont(font);
    if (rerouted) 
    {
        statusText.setString("FLIGHT REROUTED DUE TO WEATHER CONDITIONS");
        statusText.setFillColor(sf::Color::Red);
    } 
    else if (path.empty()) 
    {
        statusText.setString("NO PATH AVAILABLE");
        statusText.setFillColor(sf::Color::Red);
    } 
    else 
    {
        statusText.setString("ORIGINAL FLIGHT PATH");
        statusText.setFillColor(sf::Color::Green);
    }
    statusText.setCharacterSize(18);
    statusText.setPosition(20, 20);
    
    sf::Text pathText;
    pathText.setFont(font);
    std::string pathStr = "Path: ";
    for (size_t i = 0; i < path.size(); ++i) 
    {
        pathStr += graph.airports[path[i]].code;
        if (i < path.size() - 1) pathStr += " → ";
    }

    pathText.setString(pathStr);
    pathText.setCharacterSize(16);
    pathText.setFillColor(sf::Color::White);
    pathText.setPosition(20, 50);
    
    std::vector<sf::VertexArray> pathSegments;
    std::vector<float> pathLengths;
    std::vector<std::pair<int, int>> pathPairs;
    float totalPathLength = 0.0f;
    
    if (!path.empty()) 
    {
        for (size_t i = 1; i < path.size(); ++i) 
        {
            int fromIdx = path[i-1];
            int toIdx = path[i];
            sf::Vector2f start = graph.airports[fromIdx].position;
            sf::Vector2f end = graph.airports[toIdx].position;
            
            bool hasBadWeather = graph.pathWeather[fromIdx][toIdx].isBad;
            
            float dx = end.x - start.x;
            float dy = end.y - start.y;
            float length = std::sqrt(dx*dx + dy*dy);
            pathLengths.push_back(length);
            totalPathLength += length;
            
            pathPairs.push_back({fromIdx, toIdx});
            
            sf::VertexArray segment(sf::Lines, 2);
            sf::Color actualPathColor;
            
            if (hasBadWeather) 
            {
                std::cout << "WARNING: Path includes bad weather segment: " 
                        << graph.airports[fromIdx].code << " -> " 
                        << graph.airports[toIdx].code << "\n";
                actualPathColor = sf::Color::Red; 
            } else if (rerouted) {
                actualPathColor = reroutedPathColor; 
            } else {
                actualPathColor = pathColor;
            }
            
            segment[0] = sf::Vertex(start, actualPathColor);
            segment[1] = sf::Vertex(end, actualPathColor);
            pathSegments.push_back(segment);

            std::cout << "Created path segment: " << graph.airports[fromIdx].code << " -> " 
                    << graph.airports[toIdx].code << " (length: " << length << ")";
            
            if (hasBadWeather) 
            {
                std::cout << " [BAD WEATHER!]";
            }
            
            std::cout << "\n";
        }
    }
    
    sf::Clock clock;
    float animationProgress = 0.0f;
    const float animationSpeed = 0.3f; 
    
    sf::CircleShape airplane(6, 3); 
    airplane.setFillColor(planeColor);
    airplane.setOrigin(6, 6); 
    
    while (window.isOpen()) 
    {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();
        
        float deltaTime = clock.restart().asSeconds();
        if (!path.empty() && path.size() > 1) 
        {
            animationProgress += deltaTime * animationSpeed;
            if (animationProgress > 1.0f) 
            {
                animationProgress = 0.0f; 
            }
        }
        
        window.clear();

        for (const auto& line : connectionLines)
            window.draw(line);
            
        for (const auto& line : badWeatherLines)
            window.draw(line);

        for (const auto& shape : airportShapes)
            window.draw(shape);

        for (const auto& shape : pathAirportShapes)
            window.draw(shape);
            
        for (const auto& label : airportLabels)
            window.draw(label);
        if (!path.empty() && path.size() > 1) {
            float distanceCovered = animationProgress * totalPathLength;
            float accumulatedLength = 0.0f;
            int currentSegment = -1;
            float segmentProgress = 0.0f;
            
            for (size_t i = 0; i < pathLengths.size(); ++i) 
            {
                if (distanceCovered <= accumulatedLength + pathLengths[i]) 
                {
                    currentSegment = i;
                    segmentProgress = (distanceCovered - accumulatedLength) / pathLengths[i];
                    break;
                }
                accumulatedLength += pathLengths[i];
            }
            
            if (currentSegment == -1) {
                currentSegment = pathLengths.size() - 1;
                segmentProgress = 1.0f;
            }

            for (int i = 0; i < currentSegment; ++i) 
            {
                window.draw(pathSegments[i]);
            }

            if (currentSegment < pathSegments.size()) 
            {
                sf::VertexArray partialSegment = pathSegments[currentSegment];
                sf::Vector2f start = partialSegment[0].position;
                sf::Vector2f end = partialSegment[1].position;
                sf::Vector2f partialEnd = start + (end - start) * segmentProgress;
                
                partialSegment[1].position = partialEnd;
                window.draw(partialSegment);

                airplane.setPosition(partialEnd);
                float angle = std::atan2(end.y - start.y, end.x - start.x) * 180 / 3.14159f;
                airplane.setRotation(angle + 90);
            }

            window.draw(airplane);
        }

        window.draw(statusText);
        window.draw(pathText);

        sf::Text animText;
        animText.setFont(font);
        animText.setString("Press ESC to exit");
        animText.setCharacterSize(14);
        animText.setFillColor(sf::Color::White);
        animText.setPosition(20, 80);
        window.draw(animText);
        
        window.display();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();
    }
}


int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(true); 
    
    int src = -1, dst = -1;
    bool useCommandLineArgs = false;
    
    if (argc == 3) 
    {
        try {
            src = std::stoi(argv[1]);
            dst = std::stoi(argv[2]);
            useCommandLineArgs = true;
            std::cout << "Using command-line arguments: src=" << src << ", dst=" << dst << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing command-line arguments: " << e.what() << std::endl;
            useCommandLineArgs = false;
        }
    }
    
    FlightGraph graph;

    graph.addAirport("JFK", 150, 100);
    graph.addAirport("LAX", 50, 500);
    graph.addAirport("ORD", 350, 150);
    graph.addAirport("DFW", 450, 350);
    graph.addAirport("ATL", 300, 300);
    graph.addAirport("SFO", 100, 450);
    graph.addAirport("MIA", 250, 550);
    graph.addAirport("SEA", 50, 50);
    graph.addAirport("DEN", 300, 200);
    graph.addAirport("BOS", 200, 80);
    graph.addAirport("LAS", 150, 450);
    graph.addAirport("PHX", 350, 450);
    graph.addAirport("IAH", 500, 400);
    graph.addAirport("EWR", 180, 90);
    graph.addAirport("CLT", 330, 280);

    int n = graph.airports.size();
    for (int i = 0; i < n; ++i) 
    {
        for (int j = i + 1; j < n; ++j) 
        {
            const auto& pi = graph.airports[i].position;
            const auto& pj = graph.airports[j].position;
            double dx = pi.x - pj.x;
            double dy = pi.y - pj.y;
            double dist = std::sqrt(dx * dx + dy * dy);
            graph.addEdge(i, j, dist);
        }
    }

    printLine('=');
    std::cout << "WELCOME TO FLIGHT SIMULATOR" << std::endl;
    printLine('=');

    if (!useCommandLineArgs) {
        std::cout << "Available Airports (index: code):" << std::endl;
        for (int i = 0; i < n; ++i) 
        {
            std::cout << "  " << i << ": " << graph.airports[i].code << std::endl;
        }
        printLine();

        std::string input;
        std::cout << "FLIGHT SELECTION" << std::endl;
        printLine();

        do {
            std::cout << "Enter departure index or code: ";
            std::cout.flush();
            std::cin >> input;
            src = resolveAirportIndex(input, graph.airports);
            
            if (src < 0 || src >= n) {
                std::cout << "Invalid airport. Please try again." << std::endl;
                std::cout.flush();
            }
        } while (src < 0 || src >= n);

        do {
            std::cout << "Enter arrival index or code:   ";
            std::cout.flush(); 
            std::cin >> input;
            dst = resolveAirportIndex(input, graph.airports);
            
            if (dst < 0 || dst >= n) {
                std::cout << "Invalid airport. Please try again." << std::endl;
                std::cout.flush();
            }
        } while (dst < 0 || dst >= n);
    }

    if (useCommandLineArgs) 
    {
        if (src < 0 || src >= n || dst < 0 || dst >= n) {
            std::cerr << "Invalid airport indices provided via command line." << std::endl;
            std::cerr << "Valid range is 0 to " << (n-1) << std::endl;
            return 1;
        }
    }
    
    std::cout << "Selected route: " << graph.airports[src].code << " to "   << graph.airports[dst].code << std::endl;

    printLine('=');
    std::cout << "WEATHER CONDITIONS UPDATE" << std::endl;
    printLine('=');
    
    std::cout << "Would you like to update weather conditions? (y/n): ";
    std::cout.flush();
    std::string updateWeather;
    std::cin >> updateWeather;

    if (updateWeather == "y" || updateWeather == "Y") 
    {
        std::cout << "Enter number of weather updates: ";
        std::cout.flush();
        int numUpdates;
        std::cin >> numUpdates;

        for (int i = 0; i < numUpdates; ++i) 
        {
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
            } 
            else 
            {
                description = "Clear skies";
            }
            
            graph.updateWeather(a1, a2, condition == 1, description);
            
            std::cout << "Weather updated: " 
            << graph.airports[a1].code << " to " 
            << graph.airports[a2].code << " - " 
            << (condition == 1 ? "Bad weather (" + description + ")" : "Clear skies") 
            << std::endl;
        }
    }

    FlightGraph directGraph = graph;
    
    for (int i = 0; i < directGraph.airports.size(); ++i) 
    {
        for (int j = 0; j < directGraph.airports.size(); ++j) 
        {
            if (i != j) 
            {
                directGraph.pathAvailable[i][j] = true;
            }
        }
    }
    
    std::vector<int> directPath = directGraph.dijkstra(src, dst);
    
    FlightGraph weatherGraph = graph;

    for (int i = 0; i < weatherGraph.airports.size(); ++i) 
    {
        for (int j = 0; j < weatherGraph.airports.size(); ++j) 
        {
            if (weatherGraph.pathWeather[i][j].isBad) 
            {
                weatherGraph.pathAvailable[i][j] = false;
            }
        }
    }
    

    std::vector<int> weatherAwarePath = weatherGraph.dijkstra(src, dst);
    
    bool hasDirectPathBadWeather = false;
    std::vector<std::pair<std::string, std::string>> badWeatherSegments;
    
    if (!directPath.empty())
    {
        badWeatherSegments = graph.getPathWeatherInfo(directPath);
        hasDirectPathBadWeather = !badWeatherSegments.empty();
    }
    
    bool rerouted = false;
    std::vector<int> finalPath;
    
    if (weatherAwarePath.empty()) 
    {
        std::cout << "No path found between the selected airports due to weather conditions.\n";
        finalPath = weatherAwarePath;
    } 
    else if (hasDirectPathBadWeather) 
    {
        std::cout << "\nBAD WEATHER DETECTED on the direct route!\n";
        std::cout << "Affected segments:\n";
        for (const auto& segment : badWeatherSegments) {
            std::cout << "  " << segment.first << ": " << segment.second << "\n";
        }
        

        bool pathsAreDifferent = directPath.size() != weatherAwarePath.size() || 
                                !std::equal(directPath.begin(), directPath.end(), weatherAwarePath.begin());
        
        if (pathsAreDifferent && !weatherAwarePath.empty()) 
        {
            std::cout << "Automatically rerouted to avoid bad weather.\n";
            rerouted = true;
            finalPath = weatherAwarePath;

            bool finalPathHasBadWeather = false;
            for (size_t i = 1; i < finalPath.size(); ++i)
            {
                int u = finalPath[i-1];
                int v = finalPath[i];
                if (graph.pathWeather[u][v].isBad) 
                {
                    finalPathHasBadWeather = true;
                    std::cout << "WARNING: Rerouted path still contains bad weather segment: " 
                            << graph.airports[u].code << " -> " << graph.airports[v].code << "\n";
                }
            }
            
            if (finalPathHasBadWeather) {
                std::cout << "Fixing rerouted path to completely avoid bad weather...\n";
                FlightGraph fixedGraph = graph;
                for (int i = 0; i < fixedGraph.airports.size(); ++i) 
                {
                    for (int j = 0; j < fixedGraph.airports.size(); ++j) 
                    {
                        if (fixedGraph.pathWeather[i][j].isBad) {
                            fixedGraph.pathAvailable[i][j] = false;
                        }
                    }
                }
                finalPath = fixedGraph.dijkstra(src, dst);
                if (finalPath.empty()) {
                    std::cout << "No completely safe path found. Using best available route.\n";
                    finalPath = weatherAwarePath;
                }
            }
            
            std::cout << "New route: ";
            for (int idx : finalPath)
                std::cout << graph.airports[idx].code << " ";
            std::cout << "\n";
        }
        else
        {
            std::cout << "Attempting to find alternative route...\n";

            FlightGraph rerouteGraph = graph;

            for (int i = 0; i < rerouteGraph.airports.size(); ++i) {
                for (int j = 0; j < rerouteGraph.airports.size(); ++j) {
                    if (rerouteGraph.pathWeather[i][j].isBad) {
                        rerouteGraph.pathAvailable[i][j] = false;
                    }
                }
            }

            std::vector<int> alternativePath = rerouteGraph.dijkstra(src, dst);
            
            if (alternativePath.empty()) {
                std::cout << "No alternative route found. All possible paths are affected by bad weather.\n";
                finalPath = weatherAwarePath; 
            } else {
                std::cout << "REROUTED SUCCESSFULLY!\n";
                rerouted = true;
                finalPath = alternativePath;
                
                std::cout << "New route: ";
                for (int idx : finalPath)
                    std::cout << graph.airports[idx].code << " ";
                std::cout << "\n";
            }
        }
    }
     else 
     {
        std::cout << "Route has good weather conditions.\n";
        finalPath = weatherAwarePath;
        
        std::cout << "Shortest path: ";
        for (int idx : finalPath)
            std::cout << graph.airports[idx].code << " ";
        std::cout << "\n";
    }

    visualizeGraph(graph, finalPath, rerouted);
    return 0;
}
