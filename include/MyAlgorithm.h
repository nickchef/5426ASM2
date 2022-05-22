#ifndef COMP5426ASM1_MYALGORITHM_H
#define COMP5426ASM1_MYALGORITHM_H

#include "utils.h"

// Thread Arguments for designed algorithm
typedef struct{
    const MATRIX *matrix; // Source matrix
    RES *res; // Result vector and locating array

    // Where the thread begin their work
    unsigned beginPositionX;
    unsigned beginPositionY;
    unsigned workLoad;
    unsigned padding; // How many rows has been added into the matrix
}myT_args;

unsigned long myAlgorithm(MATRIX *matrix, RES *res, unsigned t, unsigned b);

#endif //COMP5426ASM1_MYALGORITHM_H
