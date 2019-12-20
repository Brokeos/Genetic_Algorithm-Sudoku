#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "./ga.h"
#include "./ga.inc"

void *(*ga_malloc)(size_t size) = malloc;
void *(*ga_realloc)(void *ptr, size_t size) = realloc;
void (*ga_free)(void *ptr) = free;

#define MIN(a,b) (((a)<(b))?(a):(b))

static int counter = 0;
static int gen_counter = 1;
static int low_score = 999999;
static Individual *low_individual;

/**
 * Initializes the library
 * @return a boolean if success
 */
bool ga_init(void) {
    if (!counter++) {
        srand(time(NULL));
        assert(printf("GA initialised\n"));
    }
    return true;
}

/**
 * Closes the library
 * @return a boolean if success or fail.
 */
bool ga_finish(void) {
    if (counter) {
        if (!--counter) {
            assert(printf("GA finished\n"));
        }
        return true;
    } else {
        return false;
    }
}

/**
 * Creates a genetic generator of a certain number of chromosomes given in parameter.
 * @param size The wanted number of chromosomes
 * @return The generator.
 */
GeneticGenerator *genetic_generator_create(const unsigned int size) {
    GeneticGenerator *generator = ga_malloc(sizeof *generator);
    if (generator) {
        generator->size = size;
        if (size) {
            generator->cardinalities = ga_malloc(sizeof(unsigned int) * size);
            if (!generator->cardinalities) {
                ga_free(generator);
                generator = NULL;
            } else {
                memset(generator->cardinalities, 0, sizeof(unsigned int) * size);
            }
        } else {
            generator->cardinalities = NULL;
        }
    }
    return generator;
}

/**
 * Frees the allocated memory for a generator and destroys it.
 * @param generator the generator.
 */
void genetic_generator_destroy(GeneticGenerator *generator) {
    ga_free(generator->cardinalities);
    ga_free(generator);
}

/**
 * Defines the max cardinality of a chromosome.
 * @param generator The generator whitch cardinality is to be defined.
 * @param index position of the chromosome in the genetic sequence.
 * @param cardinality the maximum cardinality.
 * @return the modified generator.
 */
GeneticGenerator *genetic_generator_set_cardinality(GeneticGenerator *generator, const unsigned int index,
                                                    const unsigned int cardinality) {
    assert(index < generator->size);
    generator->cardinalities[index] = cardinality;
    return generator;
}

/**
 * Gets the cardinality of a chromosome in a genetic sequence.
 * @param generator The generator
 * @param index the position of the chromosome.
 * @return the cardinality of the chromosome.
 */
unsigned int genetic_generator_get_cardinality(const GeneticGenerator *generator, const unsigned int index) {
    assert(index < generator->size);
    return generator->cardinalities[index];
}

/**
 * Gets the size of the generator
 * @param generator the generator
 * @return the size
 */
unsigned int genetic_generator_get_size(const GeneticGenerator *generator) {
    return generator->size;
}

/**
 * Clones a generator.
 * @param generator the genator
 * @return the clone
 */
GeneticGenerator *genetic_generator_clone(const GeneticGenerator *generator) {
    GeneticGenerator *clone = genetic_generator_create(generator->size);
    if (clone) {
        memcpy(clone->cardinalities, generator->cardinalities, generator->size * sizeof(unsigned int));
        return clone;
    } else {
        return NULL;
    }
}


/**
 * Copies a generator and Overwrites the previous generator at the destination.
 * @param dest the destination to be overwritten.
 * @param src the source generator to copy.
 * @return the position of the copied generator.
 */
GeneticGenerator *genetic_generator_copy(GeneticGenerator *dest, const GeneticGenerator *src) {
    unsigned int *cardinalities = ga_realloc(dest->cardinalities, src->size * sizeof(unsigned int));
    if (cardinalities) {
        dest->size = src->size;
        dest->cardinalities = cardinalities;
        memcpy(dest->cardinalities, src->cardinalities, src->size * sizeof(unsigned int));
        return dest;
    } else {
        return NULL;
    }
}

/**
 * Writes a generator to a file given in parameter.
 * @param generator the generator to write in a file
 * @param stream the destination file.
 * @return the generator that has been written in the file.
 */
GeneticGenerator *genetic_generator_fwrite(const GeneticGenerator *generator, FILE *stream) {
    if (fwrite(&generator->size, sizeof(generator->size), 1, stream) == 1 &&
        fwrite(generator->cardinalities, sizeof(unsigned int), generator->size, stream) == generator->size) {
        return (GeneticGenerator *)generator;
    } else {
        return NULL;
    }
}

/**
 * Reads a generator in a file.
 * @param generator the generator to read
 * @param stream the file in whitch the generator is supposed to be.
 * @return the generator if found.
 */
GeneticGenerator *genetic_generator_fread(GeneticGenerator *generator, FILE *stream) {
    unsigned int size;
    unsigned int *cardinalities;
    if (fread(&size, sizeof(size), 1, stream) == 1) {
        cardinalities = ga_malloc(sizeof(unsigned int) * size);
        if (cardinalities) {
            if (fread(cardinalities, sizeof(unsigned int), size, stream) == size) {
                ga_free(generator->cardinalities);
                generator->size = size;
                generator->cardinalities = cardinalities;
                return generator;
            } else {
                ga_free(cardinalities);
                return NULL;
            }
        } else {
            return NULL;
        }
    } else {
        return NULL;
    }
}

/**
 * Is used by the tostring function to add a character in a string.
 * @param pstring the string that we want do add the character in.
 * @param plength the length of the string.
 * @param add the character.
 * @return a boolean.
 */
static bool _add_string(char **pstring, unsigned int *plength, const char *add) {
    if (add) {
        unsigned int inc_length = strlen(add);
        char *string = ga_realloc(*pstring, *plength + inc_length + 1);
        if (string) {
            *pstring = string;
            strncpy(*pstring + *plength, add, inc_length + 1);
            *plength += inc_length;
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

/**
 * Converts a generator to a string
 * @param generator the generator.
 * @return the obtained string
 */
const char *genetic_generator_to_string(const GeneticGenerator *generator) {
    static char *string = NULL;
    unsigned int length;

    length = 0;

    if (!_add_string(&string, &length, "[")) {
        return NULL;
    }
    for (unsigned int index = 0; index < generator->size; index++) {
        if (index) {
            if (!_add_string(&string, &length, ",")) {
                return NULL;
            }
        }
        char element[100];
        snprintf(element, 100, "%u", generator->cardinalities[index]);
        if (!_add_string(&string, &length, element)) {
            return NULL;
        }
    }
    if (!_add_string(&string, &length, "]")) {
        return NULL;
    }
    return string;
}

/**
 * Generates an individual with a number of chromosomes.
 * @param generator the generator
 * @return the generated individual.
 */
unsigned int* genetic_generator_individual(const GeneticGenerator* generator){
    unsigned int *individual = malloc(sizeof(unsigned int) * generator->size);
    for(int i = 0; i < generator->size; i++){
        int max = (int)genetic_generator_get_cardinality(generator, i);
        individual[i] = random_number(1, max);
    }
    return individual;
}

/**
 * Creates a population of individuals.
 * @param generator the generator.
 * @param size the size of the population.
 * @return the population.
 */
Population* ga_population_create(const GeneticGenerator* generator, unsigned int size){
    Population *population = ga_malloc(sizeof(Population));
    population->genetic_generator = genetic_generator_clone(generator);;
    population->size = size;
    if (size && (size % 2 == 0)){
        population->individuals = ga_malloc(sizeof(Individual) * size);
        if (!population->individuals){
            ga_population_destroy(population);
        } else {
            for(int i = 0; i < size; i++){
                Individual *individual = ga_malloc(sizeof(Individual));
                unsigned int *genome = genetic_generator_individual(population->genetic_generator);
                individual->genome = genome;
                individual->index = -1;
                individual->size = population->genetic_generator->size;
                population->individuals[i] = individual;
            }
        }
    } else {
        ga_population_destroy(population);
    }
    return population;
}
/**
 * Frees the memory taken by a population.
 * @param population the population to destroy.
 */
void ga_population_destroy(Population* population){
    for(int i = 0; i < population->size; i++){
        ga_individual_destroy(population->individuals[i]);
    }
    ga_free(population->individuals);
    genetic_generator_destroy(population->genetic_generator);
    ga_free(population);
}

/**
 * Generates a new population
 * @param population
 * @param cross_over
 * @param mutation
 * @param evaluate
 * @param problem
 * @return
 */
Population* ga_population_next(Population* population, const float cross_over,const float mutation,unsigned int (*evaluate)(unsigned int *, const void*),const void *problem){
    printf("Current generation : %d\n", gen_counter);
    Population *new_population = ga_population_clone(population);
    Fortune_Rank *ranks = ga_malloc(sizeof(Fortune_Rank) * population->size);
    unsigned int sum_of_fitness = 0;
    for(int i = 0; i < population->size; i++){
        population->individuals[i]->index = i;
        ranks[i].individual = population->individuals[i];
        ranks[i].note = evaluate(population->individuals[i]->genome, problem);
        sum_of_fitness += ranks[i].note;
        low_score = MIN(ranks[i].note, low_score);
        if (low_score == ranks[i].note){
            low_individual = ga_individual_clone(ranks[i].individual);
        }
    }
    for(int i = 0; i < new_population->size; i+=2){
        Individual *mom = get_random_individual(ranks, (int)population->size, (int)sum_of_fitness);
        Individual *dad;
        do{
            dad = get_random_individual(ranks, (int)population->size, (int)sum_of_fitness);
        } while (mom->index == dad->index);
        Individual *sister = ga_individual_clone(mom);
        Individual *brother = ga_individual_clone(dad);
        for(int y = 0; y < population->genetic_generator->size; y++){
            int random_crossover = random_number(0, 100);
            if ((int)(cross_over * 100) >= random_crossover){
                unsigned int first_individual_chromosome = mom->genome[y];
                unsigned int second_individual_chromosome = dad->genome[y];
                sister->genome[y] = second_individual_chromosome;
                brother->genome[y] = first_individual_chromosome;
            }
            int random_mutation = random_number(0, 100);
            if ((int)(mutation * 100) >= random_mutation){
                sister->genome[y] = random_number(1, 9);
            }
            random_mutation = random_number(0, 100);
            if ((int)(mutation * 100) >= random_mutation){
                brother->genome[y] = random_number(1, 9);
            }
        }
        new_population->individuals[i] = sister;
        new_population->individuals[i+1] = brother;
    }
    gen_counter++;
    printf("Best score : %d\n", low_score);
    free(ranks);
    ga_population_destroy(population);
    return new_population;
}

/**
 * The biased fortune wheel that will randomly select an individual (the higher the rating of the individual the higher the chance it will be selected).
 * @param fortune_rank the association of the individuals and their rating.
 * @param size the size of the wheel
 * @param sum_of_fitness the sum of every rating of every individual.
 * @return the selected individual.
 */
Individual *get_random_individual(Fortune_Rank *ranks, int size, int sum_of_fitness){
    float T = 0;
    float *p_slect_array = malloc(sizeof(float) * size);
    float *val_esp_array = malloc(sizeof(float) * size);
    float f_sum = (float)sum_of_fitness;
    float f_pop = (float)size;
    for(int i = 0; i < size; i++){
        float f_note = (float)ranks[i].note;
        p_slect_array[i] = (1 - (f_note / f_sum));
        val_esp_array[i] = (p_slect_array[i] * f_pop);
        T += val_esp_array[i];
    }
    float r_number = random_float(0, T);
    int individual_i = -1;
    float sum=0;
    for (int i = 0; i < size; i++) {
        sum += val_esp_array[i];
        if (sum >= r_number) {
            individual_i = i;
            break;
        }
    }
    Individual *individual = ranks[individual_i].individual;
    free(p_slect_array);
    free(val_esp_array);
    return individual;
}

/**
 * clones a population.
 * @param population the population
 * @return the cloned population.
 */
Population *ga_population_clone(const Population *population){
    Population *clone = ga_population_create(population->genetic_generator, population->size);
    if (clone) {
        memcpy(clone->individuals, population->individuals, population->size * sizeof(Individual));
        for(int i = 0; i < clone->size; i++){
            clone->individuals[i] = ga_individual_clone(population->individuals[i]);
        }
        return clone;
    } else {
        return NULL;
    }
}

/**
 * clones a individual.
 * @param individual the individual to clone
 * @return the cloned individual.
 */
Individual* ga_individual_clone(const Individual *individual){
    Individual *clone = malloc(sizeof(Individual));
    clone->genome = malloc(sizeof(unsigned int) * individual->size);
    clone->size = individual->size;
    memcpy(clone->genome, individual->genome, individual->size * sizeof(unsigned int));
    return clone;
}


/**
 * Frees the memory taken by an individual.
 * @param individual the individual to destroy.
 */
void ga_individual_destroy(Individual *individual){
    ga_free(individual->genome);
    ga_free(individual);
}

/**
 * Returns the best score.
 * @return the score.
 */
int get_best_score(){
    return low_score;
}
/**
 * Returns the best individual based on its result
 * @return the best individual.
 */
Individual *get_best_individual(){
    return low_individual;
}
/**
 * Returns a random int number in a given interval.
 * @param min_num the lowest number of the interval
 * @param max_num the highest number of the interval
 * @return the random number
 */
int random_number(int min_num, int max_num)
{
    int result = 0, low_num = 0, hi_num = 0;
    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1;
    } else {
        low_num = max_num + 1;
        hi_num = min_num;
    }
    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}

/**
 * Returns a random float number in a given interval
 * @param min the lowest number of the interval
 * @param max the highest number of the interval
 * @return the random float.
 */
float random_float(const float min, const float max)
{
    if (max == min) return min;
    else if (min < max) return (max - min) * ((float)rand() / RAND_MAX) + min;
    return 0;
}