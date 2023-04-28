// Header files
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <omp.h>
#include <chrono>

// Constants for genetic algorithm
const int POPULATION_SIZE = 100;
const int NUM_GENERATIONS = 1000;
const float MUTATION_RATE = 0.1;
const float CROSSOVER_RATE = 0.8;

// Class to represent a city with its x and y coordinates
class City {
public:
    int x, y;

    City(int x, int y) : x(x), y(y) {
        // Check if this is a null pointer
        if (!this) {
            std::cerr << "Error: null pointer passed to City constructor." << std::endl;
            exit(1);
        }
    }

    // Overload the equality operator for comparison
    bool operator==(const City &other) const {
        return x == other.x && y == other.y;
    }
};

// Class to represent a route consisting of cities and its fitness score
class Route {
public:
    std::vector<City> cities;
    double fitness;

    // Constructor to initialize the cities and calculate the fitness score
    Route(const std::vector<City> &cities) : cities(cities) {
        calculateFitness();
    }

    // Method to calculate the fitness score of the route
    void calculateFitness() {
        double totalDistance = 0.0;
        #pragma omp parallel for reduction(+:totalDistance)
        for (size_t i = 1; i < cities.size(); ++i) {
            totalDistance += std::hypot(cities[i].x - cities[i - 1].x,
                                        cities[i].y - cities[i - 1].y);
        }
        totalDistance += std::hypot(cities.front().x - cities.back().x,
                                    cities.front().y - cities.back().y);
        fitness = 1.0 / totalDistance;
    }
};

// Function to initialize a population of random routes
std::vector<Route> initializePopulation(const std::vector<City> &cities) {
    std::vector<Route> population;
    population.reserve(POPULATION_SIZE); // Optional: reserve memory for the vector
    #pragma omp parallel for
    // Loop to create a population of random routes
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        // Declare a private copy of shuffledCities for each thread
        std::vector<City> shuffledCities = cities;
        // Randomly shuffle the cities
        std::shuffle(shuffledCities.begin(), shuffledCities.end(), std::mt19937_64(std::random_device()()));
        // Add the shuffled cities to the population
        population.emplace_back(Route(shuffledCities));
    }
    return population;
}

// Function to perform tournament selection on a population
Route tournamentSelection(const std::vector<Route> &population) {
    // Randomly select two routes from the population
    int index1 = rand() % population.size();
    int index2 = rand() % population.size();
    // Return the fittest route of the two selected
    return population[index1].fitness > population[index2].fitness ? population[index1] : population[index2];
}

// parallelizing this function created additional overhead, based on input's size and number of threads its best to
// leave it serial
// Function to perform crossover between two routes
Route crossover(const Route &parent1, const Route &parent2) {
    std::vector<City> childCities = parent1.cities;
    // Perform crossover with a probability of CROSSOVER_RATE
    if (rand() / static_cast<double>(RAND_MAX) < CROSSOVER_RATE) {
        // Randomly select a start and end position for the crossover
        int startPos = rand() % childCities.size();
        int endPos = rand() % childCities.size();
        if (startPos > endPos) std::swap(startPos, endPos);
        // Perform the crossover between parent1 and parent2
        for (int i = startPos; i <= endPos; ++i) {
            auto it = std::find(childCities.begin(), childCities.end(), parent2.cities[i]);
            std::swap(*it, childCities[i]);
        }
    }
    // Create a new route from the child cities and return it
    return Route(childCities);
}


// This function mutates a route in the population by swapping two cities at random with a given mutation rate.
void mutate(Route &route) {
    // Initialize random seed once
    unsigned seed = static_cast<unsigned>(time(nullptr));

    #pragma omp parallel
    {
        // Calculate the starting index for the thread
        size_t start = omp_get_thread_num() * route.cities.size() / omp_get_num_threads();
        // Calculate the ending index for the thread
        size_t end = (omp_get_thread_num() + 1) * route.cities.size() / omp_get_num_threads();
        // Loop over each city in the route for this thread
        for (size_t i = start; i < end; ++i) {
            // If a random number is less than the mutation rate, mutate the city
            if (rand_r(&seed) / static_cast<double>(RAND_MAX) < MUTATION_RATE) {
                // Select a random city to swap with
                int index = rand_r(&seed) % route.cities.size();
                // Swap the two cities
                std::swap(route.cities[i], route.cities[index]);
            }
        }
    }
    // Recalculate fitness after mutation
    route.calculateFitness();
}


int main() {

    // Set the number of threads to be used in the next parallel region
    omp_set_num_threads(7);

    // Starting the timer
    auto start = std::chrono::high_resolution_clock::now();

    // Seed the random number generator
    srand(time(0));

    // Create a vector of City objects representing the coordinates of the cities to visit
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
    };

    // Create an initial population of routes
    std::vector<Route> population = initializePopulation(cities);

    // Run the genetic algorithm for a fixed number of generations
    for (int generation = 0; generation < NUM_GENERATIONS; ++generation) {
        // Create a new population of routes
        std::vector<Route> newPopulation;
        newPopulation.reserve(POPULATION_SIZE); // reserve space for new routes
        #pragma omp parallel
        {
            // Each thread creates its own local population
            std::vector<Route> localPopulation;
            localPopulation.reserve(POPULATION_SIZE / omp_get_num_threads()); // reserve space for each thread
            // Each thread generates a number of children through selection, crossover, and mutation
            #pragma omp for
            for (int i = 0; i < POPULATION_SIZE; ++i) {
                Route parent1 = tournamentSelection(population);
                Route parent2 = tournamentSelection(population);
                Route child = crossover(parent1, parent2);
                mutate(child);
                localPopulation.push_back(child);
            }
            // Combine the local populations from all threads into the new population
            #pragma omp critical
            {
                newPopulation.insert(newPopulation.end(), localPopulation.begin(), localPopulation.end());
            }
        }
        // Replace the old population with the new one
        population = std::move(newPopulation);
    }

    // Find the best route in the final population
    Route bestRoute = population[0];
    for (const Route &route: population) {
        if (route.fitness > bestRoute.fitness) {
            bestRoute = route;
        }
    }

    // Print the best route and its total distance
    std::cout << "Best route: ";
    for (const City &city: bestRoute.cities) {
        std::cout << '(' << city.x << ", " << city.y << ") -> ";
    }
    std::cout << '(' << bestRoute.cities.front().x << ", " << bestRoute.cities.front().y << ")\n";
    std::cout << "Total distance: " << 1.0 / bestRoute.fitness << std::endl;

    // Ending the timer
    auto end = std::chrono::high_resolution_clock::now();

    // Calculating elapsed time in milliseconds
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Print the elapsed time
    std::cout << "Execution time: " << duration << " ms" << std::endl;

    return 0;
}
