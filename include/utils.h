#ifndef COMP5426ASM1_UTILS_H
#define COMP5426ASM1_UTILS_H

#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <getopt.h>
#include "mpi.h"

#define PAIR_NUM(N) ((N)*((N)+1)/2) // Compute the work load
#define posInRes(IDXARR, y, x) ((IDXARR)[y] + (x) - (y)) // Compute the correspond index in the result vector by i, j
#define MICROSEC_PER_SEC (1000000)
#define timediff(before, after) (((after).tv_sec-(before).tv_sec)*MICROSEC_PER_SEC + ((after).tv_usec-(before).tv_usec))
#define NODEINFO_LEN (4)
#define LOG_PREFIX ("[PAIRWISE]")

typedef struct timeval TIME;

typedef struct{
    float *array;
    unsigned *indexArray;
}RES;

typedef struct{
    float *array;       // Result for the current processor.
    unsigned *index;    // Index in the final result of the result of current processor
    unsigned *indexDict;// Array helps to find the index in the final result.
    unsigned size;      // Result size on the current processor.
}distributed_res_t;

typedef struct{
    float **matrix;
    unsigned rows;
    unsigned cols;
}MATRIX;

// Command line arguments. See argParse() for detail.
typedef struct{
    unsigned nCols; // m
    unsigned nRows; // n
    unsigned nThreads; // t
    unsigned blockSize; // b
    bool silent; // s
    bool skipSequentialTest; // p
    bool skipAlgorithmTest; // k
}ARGS;

typedef struct{
    unsigned pad;            // How much has the matrix been padded.
    unsigned beginPositionX;
    unsigned beginPositionY;
    unsigned workLoad;       // Work load for the processor, in terms of block.
}nodeinfo_t;

typedef struct{
    int numProcs;
    int id;
}MPI_INFO;


void argParse(int argc, char *argv[], ARGS *args);
void resGen(RES *res, unsigned n);
void resFree(RES *res);
void matrixGen(MATRIX *matrix, unsigned m, unsigned n);
void matrixFree(MATRIX *matrix);
void printMatrix(const MATRIX *matrix);
void printResult(unsigned n, const float *res, unsigned long timeConsumption, const char *name, bool s);
void resultCmp(unsigned n, const float *res, const float *res2, const char* name);
void matrixAlloc(MATRIX *matrix, ARGS arg, nodeinfo_t task);
void distributedResGen(distributed_res_t *res, nodeinfo_t task, ARGS args);
void distributedResFree(distributed_res_t *res);
#endif //COMP5426ASM1_UTILS_H
