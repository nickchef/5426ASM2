//
//

#include "../include/Baseline.h"

/*
 * Sequential algorithm.
 */
unsigned long naiveSolve(const MATRIX *matrix, RES *res, ARGS args){
    unsigned pos = 0;
    TIME before, after;
    gettimeofday(&before, NULL);

    float *resArray = res->array;

    for(unsigned i = 0; i < args.nRows; ++i){
        for(unsigned j = i; j < args.nRows; ++j){
            for(unsigned k = 0; k < args.nCols; ++k){
                resArray[pos] += matrix->matrix[i][k] * matrix->matrix[j][k];
            }
            ++pos;
        }
    }

    gettimeofday(&after, NULL);
    return timediff(before, after);
}