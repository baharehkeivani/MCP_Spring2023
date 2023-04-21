#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>

using namespace std;

const int POPULATION_SIZE = 100;
const int MAX_GENERATIONS = 1000;
const double MUTATION_RATE = 0.1;
const double CROSSOVER_RATE = 0.6;

class City
{
public:
    double x;
    double y;

    City(double x_, double y_) : x(x_), y(y_) {}
};

class Tour
{
public:
    vector<int> order;
    double fitness;

    Tour()
    {
        for (int i = 0; i < cities.size(); ++i)
        {
            order.push_back(i);
        }
        random_shuffle(order.begin() + 1, order.end());
        calculateFitness();
    }

    Tour(const vector<int> &order_) : order(order_)
    {
        calculateFitness();
    }

    void calculateFitness()
    {
        fitness = 0;
        for (int i = 0; i < cities.size() - 1; ++i)
        {
            City &city1 = cities[order[i]];
            City &city2 = cities[order[i + 1]];
            fitness += distance(city1, city2);
        }
        fitness += distance(cities[order[0]], cities[order[cities.size() - 1]]);
    }

    void mutate()
    {
        for (int i = 1; i < cities.size() - 1; ++i)
        {
            if ((double)rand() / RAND_MAX < MUTATION_RATE)
            {
                int j = rand() % (cities.size() - 1) + 1;
                swap(order[i], order[j]);
            }
        }
        calculateFitness();
    }

    friend Tour crossover(const Tour &a, const Tour &b)
    {
        int start = rand() % (cities.size() - 1) + 1;
        // int end = rand() % (cities.size() - start - 1) + start;
        int range = cities.size() - start - 1;
        if (range <= 0)
        {
            range = 1; // or some other value to ensure non-zero division
        }
        int end = rand() % range + start;
        vector<int> order(cities.size());
        for (int i = 0; i < order.size(); ++i)
        {
            if (i < start || i > end)
            {
                order[i] = a.order[i];
            }
        }
        int j = 1;
        for (int i = 1; i < cities.size(); ++i)
        {
            if (find(order.begin(), order.end(), b.order[i]) == order.end())
            {
                while (find(order.begin(), order.end(), a.order[j]) != order.end())
                {
                    ++j;
                }
                order[i] = a.order[j];
            }
        }
        return Tour(order);
    }

    static double distance(const City &city1, const City &city2)
    {
        cout << "city 1 in distance: ";
        cout << city1.x << " ";
        cout << city1.y << "\n";
        cout << "city 2 in distance: ";
        cout << city2.x << " ";
        cout << city2.y << "\n";
        double dx = city1.x - city2.x;
        double dy = city1.y - city2.y;
        double dist = dx * dx + dy * dy;
        if (dist == 0)
        {
            dist = -dist;
        }
        return sqrt(dist);
    }

    static vector<City> cities;
};

vector<City> Tour::cities;

class Population
{
public:
    vector<Tour> tours;

    Population()
    {
        for (int i = 0; i < POPULATION_SIZE; ++i)
        {
            Tour tour;
            tours.push_back(tour);
        }
        sort();
    }

    Tour &getFittest()
    {
        return tours[0];
    }

    void sort()
    {
        std::sort(tours.begin(), tours.end(), [](const Tour &a, const Tour &b)
                  { return a.fitness < b.fitness; });
    }

    void evolve()
    {
        vector<Tour> newTours(POPULATION_SIZE);
        for (int i = 0; i < POPULATION_SIZE; ++i)
        {
            Tour parent1 = selectParent();
            Tour parent2 = selectParent();
            Tour child = crossover(parent1, parent2);
            child.mutate();
            newTours[i] = child;
        }
        tours = newTours;
        sort();
    }

    Tour selectParent()
    {
        // int index = pow((double) rand() / RAND_MAX, 2) * POPULATION_SIZE;
        int index = rand() % POPULATION_SIZE;
        return tours[index];
    }
};

int main()
{
    srand(time(NULL));
    vector<City> cities = {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}, {8, 8}, {9, 9}, {10, 10}};
    Tour::cities = cities;
    Population population;

    for (int i = 0; i < MAX_GENERATIONS; ++i)
    {
        cout << "Generation: " << i << " Fittest: " << population.getFittest().fitness << endl;
        population.evolve();
    }

    Tour fittest = population.getFittest();
    cout << "Solution: ";
    for (int i = 0; i < fittest.order.size(); ++i)
    {
        cout << fittest.order[i] << " ";
    }
    cout << endl;

    return 0;
}
