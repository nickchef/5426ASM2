#include "Baseline.h"
#include "MyAlgorithm.h"
#include "utils.h"


int main(int argc, char *argv[]){
    // set default command line arguments
    ARGS args = {1000, 1000, 4, 2, false, false, false};

    // read command line arguments
    argParse(argc, argv, &args);

    MPI_Status status;
    MPI_Request r;
    MPI_INFO mpiInfo;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &(mpiInfo.id));
    MPI_Comm_size(MPI_COMM_WORLD, &(mpiInfo.numprocs));

    MATRIX matrix;

    if(mpiInfo.id == 0){
        matrixGen(&matrix, args.m, args.n);
        for(unsigned sendTo = 1; sendTo < mpiInfo.numprocs; sendTo++){
            for(unsigned i = 0; i < matrix.rows; ++i)
                MPI_Isend(matrix.matrix[i], matrix.cols, MPI_FLOAT, sendTo, i, MPI_COMM_WORLD, &r);
        }
        
    }
    // Generate matrix
    

    if(!args.s){ // if silent flag is not present
        printMatrix(&matrix);
    }

    RES naiveResult;
    RES myAlgorithmResult;

    if(!args.p){ // if the sequential algorithm is not skipped
        resGen(&naiveResult, args.n); // allocate memory for result
        unsigned long naiveTimeConsumption = naiveSolve(&matrix, &naiveResult); // do the sequential algorithm and get time-consumption
        printResult(args.n, naiveResult.array, naiveTimeConsumption, "naive", args.s); // print result and time
    }

    if(!args.k){ // if the designed algorithm is not skipped
        resGen(&myAlgorithmResult, args.n); // allocate memory for result
        unsigned long myAlgorithmTimeConsumption = myAlgorithm(&matrix, &myAlgorithmResult, args.t, args.b); // do the computation and get time consumption
        printResult(args.n, myAlgorithmResult.array, myAlgorithmTimeConsumption, "myAlgorithm", args.s);// print result and time
        if(!args.p){ // if the sequential algorithm is not skipped, compare the result with the sequential result
            resultCmp(args.n, naiveResult.array, myAlgorithmResult.array, "myAlgorithm");
        }
    }

    // Clean up
    matrixFree(&matrix);
    if(!args.p){
        resFree(&naiveResult);
    }
    if(!args.k){
        resFree(&myAlgorithmResult);
    }
    return 0;
}