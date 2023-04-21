#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

const int POPULATION_SIZE = 100;
const int GENOME_LENGTH = 10;
const int MAX_GENERATIONS = 1000;
const double MUTATION_RATE = 0.1;
const double CROSSOVER_RATE = 0.6;

class Genome {
public:
    vector<bool> genes;

    Genome() {
        for (int i = 0; i < GENOME_LENGTH; ++i) {
            genes.push_back(rand() % 2);
        }
    }

    double getFitness() const {
        double fitness = 0;
        for (int i = 0; i < GENOME_LENGTH; ++i) {
            if (genes[i]) {
                fitness += 1;
            }
        }
        return fitness;
    }

    void mutate() {
        for (int i = 0; i < GENOME_LENGTH; ++i) {
            if ((double) rand() / RAND_MAX < MUTATION_RATE) {
                genes[i] = !genes[i];
            }
        }
    }

    friend Genome crossover(const Genome& a, const Genome& b) {
        Genome child;
        for (int i = 0; i < GENOME_LENGTH; ++i) {
            if ((double) rand() / RAND_MAX < CROSSOVER_RATE) {
                child.genes.push_back(a.genes[i]);
            } else {
                child.genes.push_back(b.genes[i]);
            }
        }
        return child;
    }
};

class Population {
public:
    vector<Genome> genomes;

    Population() {
        for (int i = 0; i < POPULATION_SIZE; ++i) {
            genomes.push_back(Genome());
        }
    }

    void sort() {
        std::sort(genomes.begin(), genomes.end(), [](const Genome& a, const Genome& b) {
            return a.getFitness() > b.getFitness();
        });
    }

    void evolve() {
        sort();

        vector<Genome> newGenomes;
        for (int i = 0; i < POPULATION_SIZE; ++i) {
            int parentA = rand() % (POPULATION_SIZE / 2);
            int parentB = rand() % (POPULATION_SIZE / 2);
            newGenomes.push_back(crossover(genomes[parentA], genomes[parentB]));
            newGenomes.back().mutate();
        }
        genomes = newGenomes;
    }

    double getBestFitness() const {
        return genomes[0].getFitness();
    }

    friend ostream& operator<<(ostream& os, const Population& p) {
        for (int i = 0; i < POPULATION_SIZE; ++i) {
            for (int j = 0; j < GENOME_LENGTH; ++j) {
                os << p.genomes[i].genes[j];
            }
            os << " (" << p.genomes[i].getFitness() << ")\n";
        }
        return os;
    }
};

int main() {
    srand(time(nullptr));

    Population population;

    for (int i = 0; i < MAX_GENERATIONS; ++i) {
        population.evolve();
        cout << "Generation " << i + 1 << ":\n" << population << endl;
    }

    cout << "Best fitness: " << population.getBestFitness() << endl;

    return 0;
}
