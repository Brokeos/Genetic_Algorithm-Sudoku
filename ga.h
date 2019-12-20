#ifndef GA_H_
#define GA_H_

#include <stdbool.h>
#include <stdio.h>

typedef struct _GeneticGenerator GeneticGenerator;
typedef struct _Population Population;
typedef struct _Individual Individual;
typedef struct _Fortune_Rank Fortune_Rank;

extern void *(*ga_malloc)(size_t size);
extern void *(*ga_realloc)(void *ptr, size_t size);
extern void (*ga_free)(void *ptr);

extern bool ga_init(void);
extern bool ga_finish(void);

extern GeneticGenerator *genetic_generator_create(const unsigned int size);
extern void genetic_generator_destroy(GeneticGenerator *generator);

extern GeneticGenerator *genetic_generator_set_cardinality(GeneticGenerator *generator, const unsigned int index,
                                                           const unsigned int cardinality);
extern unsigned int genetic_generator_get_cardinality(const GeneticGenerator *generator, const unsigned int index);
extern unsigned int genetic_generator_get_size(const GeneticGenerator *generator);

extern GeneticGenerator *genetic_generator_clone(const GeneticGenerator *genetic_generator);
extern GeneticGenerator *genetic_generator_copy(GeneticGenerator *dest, const GeneticGenerator *src);

extern GeneticGenerator *genetic_generator_fwrite(const GeneticGenerator *generator, FILE *stream);
extern GeneticGenerator *genetic_generator_fread(GeneticGenerator *generator, FILE *stream);

extern const char *genetic_generator_to_string(const GeneticGenerator *generator);

extern unsigned int* genetic_generator_individual(const GeneticGenerator* generator);
extern Population* ga_population_create(const GeneticGenerator* generator,unsigned int size);
extern void ga_population_destroy(Population* population);
extern Population* ga_population_next(Population* population,const float cross_over,const float mutation,unsigned int (*evaluate)(unsigned int *, const void*),const void *problem);
extern Individual *get_random_individual(Fortune_Rank *ranks, int size, int sum_of_fitness);
extern void ga_individual_destroy(Individual* individual);
extern Population* ga_population_clone(const Population *population);
extern Individual* ga_individual_clone(const Individual *individual);
extern int get_best_score();
extern Individual* get_best_individual();

extern int random_number(int min, int max);
extern float random_float(float min, float max);

#endif /* GA_H_ */
