// Header files
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <random>

// Constants
const int POPULATION_SIZE = 100;
const int NUM_GENERATIONS = 1000;
const float MUTATION_RATE = 0.1;
const float CROSSOVER_RATE = 0.8;

// City class
class City {
public:
    int x, y;

    City(int x, int y) : x(x), y(y) {}

    // Overloading the == operator to compare cities
    bool operator==(const City& other) const {
        return x == other.x && y == other.y;
    }
};

// Route class
class Route {
public:
    std::vector<City> cities;
    double fitness;

    Route(const std::vector<City>& cities) : cities(cities) {
        // Calculate fitness of the route
        calculateFitness();
    }

    // Function to calculate fitness of the route
    void calculateFitness() {
        double totalDistance = 0.0;
        for (size_t i = 1; i < cities.size(); ++i) {
            totalDistance += std::hypot(cities[i].x - cities[i - 1].x,
                                        cities[i].y - cities[i - 1].y);
        }
        totalDistance += std::hypot(cities.front().x - cities.back().x,
                                    cities.front().y - cities.back().y);
        fitness = 1.0 / totalDistance;
    }
};

// Function to initialize the population of routes
std::vector<Route> initializePopulation(const std::vector<City>& cities) {
    std::vector<Route> population;
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        // Shuffle the cities randomly
        std::vector<City> shuffledCities = cities;
        std::shuffle(shuffledCities.begin(), shuffledCities.end(), std::default_random_engine{std::random_device{}()});
        // Add the shuffled cities to the population as a new route
        population.emplace_back(shuffledCities);
    }
    return population;
}

// Function to perform tournament selection of routes
Route tournamentSelection(const std::vector<Route>& population) {
    // Select two random routes from the population
    int index1 = rand() % population.size();
    int index2 = rand() % population.size();
    // Return the fittest of the two routes
    return population[index1].fitness > population[index2].fitness ? population[index1] : population[index2];
}

Route crossover(const Route& parent1, const Route& parent2) {
    // Create a child route that is a copy of parent1
    std::vector<City> childCities = parent1.cities;
    // With a certain probability, perform crossover between parent1 and parent2
    if (rand() / static_cast<double>(RAND_MAX) < CROSSOVER_RATE) {
        // Choose a random start and end point for the crossover
        int startPos = rand() % childCities.size();
        int endPos = rand() % childCities.size();
        if (startPos > endPos) std::swap(startPos, endPos);
        // Swap the cities between the start and end points in the child route with those in parent2
        for (int i = startPos; i <= endPos; ++i) {
            auto it = std::find(childCities.begin(), childCities.end(), parent2.cities[i]);
            std::swap(*it, childCities[i]);
        }
    }
    // Return the resulting child route
    return Route(childCities);
}


void mutate(Route& route) {
    // For each city in the route, with a certain probability, swap it with another random city
    for (size_t i = 0; i < route.cities.size(); ++i) {
        if (rand() / static_cast<double>(RAND_MAX) < MUTATION_RATE) {
            int index = rand() % route.cities.size();
            std::swap(route.cities[i], route.cities[index]);
        }
    }
    // Recalculate the fitness of the route after mutation
    route.calculateFitness();
}


int main() {

    // Starting the timer
    auto start = std::chrono::high_resolution_clock::now(); // get the current time

    srand(time(0)); // seed the random number generator

    // Define a vector of City objects with their x and y coordinates
    std::vector<City> cities = {
            {60,  200},
            {180, 200},
            {80,  180},
            {140, 180},
            {20,  160},
            {100, 160},
            {200, 160},
            {140, 140},
            {40,  120},
            {100, 120},
            {20,  100},
            {60,  100},
            {120, 100},
            {160, 100},
            {200, 100},
            {20,  80},
            {60,  80},
            {120, 80},
            {160, 80},
            {200, 80},
            {20,  60},
            {60,  60},
            {120, 60},
            {160, 60},
            {200, 60},
            {20,  40},
            {60,  40},
            {120, 40},
            {160, 40},
            {200, 40},
            {20,  20},
            {60,  20},
            {120, 20},
            {160, 20},
            {200, 20},
            {40,  140},
            {80,  140},
            {120, 140},
            {160, 140},
            {40,  120},
            {80,  120},
            {120, 120},
            {160, 120},
            {200, 10},
            {140, 50},
            {160, 50},
            {50, 120},
            {10, 120},
            {40, 10},
            {160, 10},
    };
    // Initialize a vector of Route objects with the initial population
    std::vector<Route> population = initializePopulation(cities);

    // Loop through a set number of generations
    for (int generation = 0; generation < NUM_GENERATIONS; ++generation) {
        // Initialize a new population vector
        std::vector<Route> newPopulation;
        // Loop through the current population
        for (int i = 0; i < POPULATION_SIZE; ++i) {
            // Select two parents using tournament selection
            Route parent1 = tournamentSelection(population);
            Route parent2 = tournamentSelection(population);
            // Create a new child by crossing over the two parents
            Route child = crossover(parent1, parent2);
            // Mutate the child
            mutate(child);
            // Add the child to the new population
            newPopulation.push_back(child);
        }
        // Replace the old population with the new one
        population = newPopulation;
    }

    // Find the best route in the final population
    Route bestRoute = population[0];
    for (const Route& route : population) {
        if (route.fitness > bestRoute.fitness) {
            bestRoute = route;
        }
    }

    // Print the best route and its total distance
    std::cout << "Best route: ";
    for (const City& city : bestRoute.cities) {
        std::cout << '(' << city.x << ", " << city.y << ") -> ";
    }
    std::cout << '(' << bestRoute.cities.front().x << ", " << bestRoute.cities.front().y << ")\n";
    std::cout << "Total distance: " << 1.0 / bestRoute.fitness << std::endl;

    // Ending the timer
    auto end = std::chrono::high_resolution_clock::now(); // get the current time

    // Calculating elapsed time in milliseconds
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Print the elapsed time
    std::cout << "Execution time: " << duration << " ms" << std::endl; // print the elapsed time

    return 0;
}