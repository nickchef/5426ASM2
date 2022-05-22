#include "utils.h"

/*
 * Read the command line arguments.
 */
void argParse(int argc, char *argv[], ARGS *args){
    const char *opts = "m:n:t:b:s::plkh",
               *usage = "Usage: pairwiseComp \n"
                        " -[n] Rows Number: > 0, default:1000\n"
                        " -[m] Columns Number: > 0, default:1000\n"
                        " -[t] Threads Number: > 0, default:4\n"
                        " -[b] Block Size: 2-4, default:2\n"
                        " -[s] Slient, no matrix printing\n"
                        " -[p] skip naive test\n"
                        " -[k] skip my algorithm test\n"
                        "Example: pairwiseComp -n5 -m4 -t3 -b2\n";

    int opt;

    while((opt = getopt(argc, argv, opts)) != -1){
        switch(opt){
            case 'm':
                args->m = strtol(optarg, NULL, 10);
                break;
            case 'n':
                args->n = strtol(optarg, NULL, 10);
                break;
            case 't':
                args->t = strtol(optarg, NULL, 10);
                break;
            case 'b':
                args->b = strtol(optarg, NULL, 10);
                break;
            case 's':
                args->s = true;
                break;
            case 'p':
                args->p = true;
                break;
            case 'k':
                args->k = true;
                break;
            case 'h':
            default:
                printf("%s\n",usage);
                exit(-1);
        }
    }

    if(args->m == 0 || args->n == 0 || args->t == 0 || args->b > 4 || args->b < 2){
        fprintf(stderr, "Invalid arguments given!\n");
        exit(-1);
    }
}

/*
 * Allocate memory for result vector and locating array.
 */
void resGen(RES *res, unsigned n){
    res->array = calloc(PAIR_NUM(n), sizeof(float));
    res->indexArray = calloc(n, sizeof(int));
    for(unsigned i = 1; i < n; ++i){
        res->indexArray[i] = res->indexArray[i-1] + n - i + 1;
    }
}

/*
 * Free the result and locating array space.
 */
void resFree(RES *res){
    free(res->indexArray);
    free(res->array);
}

/*
 * Allocate memory for matrix and randomly initialize them in [0-1] in the format of float32.
 */
void matrixGen(MATRIX *matrix, unsigned m, unsigned n){
    srand(time(NULL));
    matrix->rows = n;
    matrix->cols = m;
    matrix->matrix = malloc(sizeof(float*)*matrix->rows);

    for(unsigned row = 0; row < matrix->rows; ++row){
        matrix->matrix[row] = malloc(sizeof(float)*matrix->cols);
        for(unsigned col = 0; col < matrix->cols; ++col){
            matrix->matrix[row][col] = rand() / (RAND_MAX * 1.);
        }
    }
}

140 /*45+
 * Free the matrix.
 */
void matrixFree(MATRIX *matrix){
    for(unsigned row = 0; row < matrix->rows; ++row){
        free(matrix->matrix[row]);
    }
    free(matrix->matrix);
}

/*
 * Print the matrix row by row.
 */
void printMatrix(const MATRIX *matrix){
    printf("Matrix:\n");
    for(unsigned row = 0; row < matrix->rows; ++row){
        for(unsigned col = 0; col < matrix->cols; ++col){
            printf("%f ", matrix->matrix[row][col]);
        }
        printf("\n");
    }
    printf("==============================\n");
}

/*
 * Print time consumption. If the silent flag has not been presented, it will also print the result in triangle.
 */
void printResult(const unsigned n, const float *res, const unsigned long timeConsumption, const char *name, bool s){
    printf("Time usage of %s computation: %ldus\n",name ,timeConsumption);
    if(!s){
        printf("Result:\n");
        unsigned idx = 0;
        for(unsigned i = n; i > 0; --i){
            for(unsigned k = 0; k < n-i; ++k){
                printf("         ");
            }
            for(unsigned j = 0; j < i; ++j){
                printf("%f ", res[idx++]);
            }
            printf("\n");
        }
    }
}

/*
 * Compare 2 results.
 */
inline void resultCmp(const unsigned n, const float *res, const float *res2, const char* name){
    if(memcmp(res, res2, PAIR_NUM(n) * sizeof(float))!=0){
        printf("%s failed in comparison with naive result!\n", name);
    }
} 890-