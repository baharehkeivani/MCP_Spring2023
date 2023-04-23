//
// Created by zahra on 4/23/23.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <omp.h>
#include <chrono>

const int POPULATION_SIZE = 100;
const int NUM_GENERATIONS = 1000;
const int NUM_CITIES = 10;
const float MUTATION_RATE = 0.1;
const float CROSSOVER_RATE = 0.8;

class City {
public:
    int x, y;

    City(int x, int y) : x(x), y(y) {}

    bool operator==(const City& other) const {
        return x == other.x && y == other.y;
    }
};

class Route {
public:
    std::vector<City> cities;
    double fitness;

    Route(const std::vector<City>& cities) : cities(cities) {
        calculateFitness();
    }

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

// Since the function does not involve any data dependencies or complex computations that could be parallelized,
// there would be little benefit to parallelize the for loop. (unlesss the city vector is veryy large)
std::vector<Route> initializePopulation(const std::vector<City>& cities) {
    std::vector<Route> population;
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        std::vector<City> shuffledCities = cities;
        std::random_shuffle(shuffledCities.begin(), shuffledCities.end());
        population.emplace_back(shuffledCities);
    }
    return population;
}

Route tournamentSelection(const std::vector<Route>& population) {
    int index1 = rand() % population.size();
    int index2 = rand() % population.size();
    return population[index1].fitness > population[index2].fitness ? population[index1] : population[index2];
}

// not parallelized, same reason as initializePopulation function
Route crossover(const Route& parent1, const Route& parent2) {
    std::vector<City> childCities = parent1.cities;
    if (rand() / static_cast<double>(RAND_MAX) < CROSSOVER_RATE) {
        int startPos = rand() % childCities.size();
        int endPos = rand() % childCities.size();
        if (startPos > endPos) std::swap(startPos, endPos);

        for (int i = startPos; i <= endPos; ++i) {
            auto it = std::find(childCities.begin(), childCities.end(), parent2.cities[i]);
            std::swap(*it, childCities[i]);
        }
    }
    return Route(childCities);
}

void mutate(Route& route) {
    for (size_t i = 0; i < route.cities.size(); ++i) {
        if (rand() / static_cast<double>(RAND_MAX) < MUTATION_RATE) {
            int index = rand() % route.cities.size();
            std::swap(route.cities[i], route.cities[index]);
        }
    }
    route.calculateFitness();
}

int main() {

    // Starting the timer
    auto start = std::chrono::high_resolution_clock::now();

    srand(time(0));

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

    std::vector<Route> population = initializePopulation(cities);

    for (int generation = 0; generation < NUM_GENERATIONS; ++generation) {
        std::vector<Route> newPopulation;
        for (int i = 0; i < POPULATION_SIZE; ++i) {
            Route parent1 = tournamentSelection(population);
            Route parent2 = tournamentSelection(population);
            Route child = crossover(parent1, parent2);
            mutate(child);
            newPopulation.push_back(child);
        }
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
    auto end = std::chrono::high_resolution_clock::now();

    // Calculating elapsed time in milliseconds
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    // Print the elapsed time
    std::cout << "Execution time: " << duration << " ms" << std::endl;

    return 0;
}