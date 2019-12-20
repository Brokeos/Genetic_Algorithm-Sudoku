#include<stdio.h>
#include <string.h>
#include<stdlib.h>
#include <yaml.h>
#include "ga.h"
#include "ga.inc"

/**
 * Gets the wanted line in the sudoku
 * @param sd The Sudoku
 * @param row_number line to get
 * @return The wanted line
 */
unsigned int *get_row(const unsigned int *sd, int row_number){

    unsigned int *values = malloc(sizeof(unsigned int) * 9);

    for (int i = 0; i < 9; i++){

        values[i] = sd[row_number*9 + i];

    }

    return values;

}

/**
 * Gets an entire column in the sudoku
 * @param sd The Sudoku
 * @param col_number The number of the wanted column
 * @return The wanted column
 */
unsigned int *get_column(const unsigned int *sd, int col_number){

    unsigned int *values = malloc(sizeof(unsigned int) * 9);

    for (int i = 0; i < 9; i++){

        values[i] = sd[col_number + i*9];

    }

    return values;

}

/**
 * Gets a 3x3 block of the sudoku
 * @param sd the Sudoku
 * @param block_number The position of the wanted block
 * @return The values of the wanted block
 */
unsigned int *get_block(const unsigned int *sd, int block_number){

    unsigned int *values = malloc(sizeof(unsigned int) * 9);
    int block_start = (int)(block_number/3)*9*3+(block_number-(int)(block_number/3)*3)*3;
    int index = 0;

    for(int i = 0; i < 3; i++){

        for(int y = 0; y < 3; y++){

            values[index] = sd[block_start+i*9+y];

            index++;

        }

    }

    return values;

}

/**
 * Counts the number of times a specific number x is present in the sudoku
 * @param sd The sudoku
 * @param n the size of the sudoku (number of boxes)
 * @param x the number we are searching for
 * @return the amount of times the number is present
 */
int count_occurrences(const unsigned int *sd, int n, int x)
{
    int res = 0;
    for (int i=0; i<n; i++)
        if (x == sd[i])
            res++;
    return res;
}

/**
 * Tests a solution given by an individual and gives a rating based on the solution compared to the problem.
 * @param solution an attempt at a solved sudoku given by an individual
 * @param problem the initial sudoku given by the user
 * @return the rating of the individual
 */
unsigned int fitness(unsigned int *solution, const void *problem){

    int note = 0;

    unsigned int *sudoku = (unsigned int *)problem;

    for(int i = 0; i < 9; i++){

        unsigned int *row = get_row(solution, i);
        unsigned int *column = get_column(solution, i);
        unsigned int *block = get_block(solution, i);

        for(int t = 1; t < 10; t++){

            if (count_occurrences(row, 9, t) > 1) {

                note += count_occurrences(row, 9, t) - 1;

            }

            if (count_occurrences(column, 9, t) > 1) {

                note += count_occurrences(column, 9, t) - 1;

            }

            if (count_occurrences(block, 9, t) > 1) {

                note += count_occurrences(block, 9, t) - 1;

            }

        }

        free(row);
        free(column);
        free(block);

    }

    for(int i = 0; i < 81; i++){

        if (solution[i]!=sudoku[i] && sudoku[i]!=0){

            note += 2;

        }

    }

    return note;

}


int main(int argc, char **argv){

    ga_init();

    FILE* fh = fopen(argv[1], "r");
    yaml_parser_t parser;
    yaml_token_t token;
    if (!yaml_parser_initialize(&parser))
        fputs("Failed to initialize parser!\n", stderr);
    if (fh == NULL)
        fputs("Failed to open file!\n", stderr);
    yaml_parser_set_input_file(&parser, fh);

    unsigned int *sudoku = malloc(sizeof(unsigned int) * 81);

    int index = 0;

    do {

        int state = 0;
        char* tk;

        yaml_parser_scan(&parser, &token);
        switch(token.type)
        {
            case YAML_KEY_TOKEN:     state = 0; break;
            case YAML_VALUE_TOKEN:   state = 1; break;
            case YAML_SCALAR_TOKEN:
                tk = token.data.scalar.value;
                if (state == 0) {
                    if (strcmp(tk, "null") == 0){
                        tk = "0";
                    }
                    int value;
                    sscanf(tk, "%d", &value);
                    sudoku[index] = (int)value;
                    index++;
                }
                break;
            default: break;
        }
        if (token.type != YAML_STREAM_END_TOKEN)
            yaml_token_delete(&token);
    } while (token.type != YAML_STREAM_END_TOKEN);
    yaml_token_delete(&token);

    yaml_parser_delete(&parser);

    printf("Loaded Sudoku : \n");

    for(int i = 0; i < 9; i++){

        unsigned int *row = get_row(sudoku, i);

        for(int y = 0; y < 9; y++){

            printf("%d ", row[y]);

        }

        printf("\n");

    };

    GeneticGenerator* gen = genetic_generator_create(81);


    for(int i = 0; i < gen->size; i++){

        genetic_generator_set_cardinality(gen, i, 9);

    }

    float cross_over;
    float mutation;
    int individuals;
    int generations;

    sscanf(argv[2], "%f", &cross_over);
    sscanf(argv[3], "%f", &mutation);
    sscanf(argv[4], "%d", &individuals);
    sscanf(argv[5], "%d", &generations);

    printf("Generating a population of %d individuals\n", individuals);

    Population *population = ga_population_create(gen, individuals);

    printf("Evolving population with %f cross-over and %f mutation rates\n", cross_over, mutation);

    for(int i = 0; i < generations; i++){

        population = ga_population_next(population, cross_over, mutation, fitness, sudoku);

    }

    printf("Last best score : %d\n", get_best_score());
    Individual *individual = get_best_individual();

    for(int i = 0; i < 9; i++){

        unsigned int *row = get_row(individual->genome, i);

        printf("- [");

        for(int y = 0; y < 9; y++){

            printf("%d", row[y]);

            if (y != 8)
                printf(", ");

        }

        printf("]\n");

    };

    ga_population_destroy(population);
    genetic_generator_destroy(gen);

    return 0;

}