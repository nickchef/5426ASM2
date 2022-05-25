#ifndef COMP5426ASM1_MYALGORITHM_H
#define COMP5426ASM1_MYALGORITHM_H

#include "utils.h"

// function to assign both index and result
#define record_result(res, index, currentSize, resArr, idxArr) { \
    (resArr)[(currentSize)] = (res);                             \
    (idxArr)[(currentSize)++] = (index);                         \
}

// Thread Arguments for designed algorithm
typedef struct{
    const MATRIX *matrix;   // Source matrix
    distributed_res_t *res; // Result vector and locating array
    float *tmpResArray;     // Temporary array to store the result of the current thread.
    unsigned *tmpIndexArray;// Temporary array to store the index in the final result of the result.
    unsigned tmpArraySize;  // Size of the temporary array.
    // Where the thread begin their work
    unsigned beginPositionX;
    unsigned beginPositionY;
    unsigned workLoad;
    unsigned padding; // How many rows has been added into the matrix
}myT_args;

unsigned long myAlgorithm(MATRIX *matrix, distributed_res_t *res, ARGS args, nodeinfo_t task);
unsigned padding(MATRIX *matrix, unsigned b);
#endif //COMP5426ASM1_MYALGORITHM_H
