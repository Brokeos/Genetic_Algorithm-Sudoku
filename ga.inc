#ifndef GENETIC_GENERATOR_STRUCT_ // Not TODO (only for moodle coderunner)
#define GENETIC_GENERATOR_STRUCT_

struct _GeneticGenerator {
    unsigned int size;
    unsigned int *cardinalities;
};

#endif // GENETIC_GENERATOR_STRUCT_

#ifndef POPULATION_STRUCT_ // Not TODO (only for moodle coderunner)
#define POPULATION_STRUCT_

struct _Population {
    unsigned int size;
    GeneticGenerator *genetic_generator;
    Individual **individuals;
};

#endif // POPULATION_STRUCT_

#ifndef INDIVIDUAL_STRUCT_ // Not TODO (only for moodle coderunner)
#define INDIVIDUAL_STRUCT_

struct _Individual {
    unsigned int index;
    unsigned int *genome;
    unsigned int size;
};

#endif // INDIVIDUAL_STRUCT_

#ifndef FORTUNE_RANK_STRUCT_ // Not TODO (only for moodle coderunner)
#define FORTUNE_RANK_STRUCT_

struct _Fortune_Rank {
    Individual *individual;
    unsigned int note;
};

#endif // FORTUNE_RANK_STRUCT_