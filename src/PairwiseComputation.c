#include "Baseline.h"
#include "MyAlgorithm.h"
#include "utils.h"

int main(int argc, char *argv[]){
    // Set default command line arguments
    ARGS args = {1000, 1000, 4, 2, false, false, false};

    // MPI initialize
    MPI_INFO mpiInfo;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &(mpiInfo.id));

#define IS_ROOT (mpiInfo.id == 0)

    if(IS_ROOT){
        printf("%s Started.\n"
               "%s Parsing arguments.\n", LOG_PREFIX,LOG_PREFIX);
    }

    // Record time
    TIME before, after;
    gettimeofday(&before, NULL);

    // Read command line arguments
    argParse(argc, argv, &args);

    // Structure for workload distribution among processors
    nodeinfo_t *nodeInfo = NULL;
    nodeinfo_t task;

    if(IS_ROOT) printf("%s Registering MPI data type.\n", LOG_PREFIX);

    // Registering nodeinfo_t as a MPI type to optimize the task distribution process.
    // Declare the displacement of each member in the structure.
    MPI_Aint displacement[NODEINFO_LEN] = {0, 4, 8, 12};
    // Declare the type of each member in the structure.
    MPI_Datatype types[NODEINFO_LEN] = {MPI_UNSIGNED,MPI_UNSIGNED,MPI_UNSIGNED,MPI_UNSIGNED};
    // Declare the quantity of element of each member in the structure.
    int blockLength[NODEINFO_LEN] = {1, 1, 1, 1};
    // Declare the MPI_Datatype handle for the structure.
    MPI_Datatype MPINodeInfoDataType;
    // Register and commit the new data type.
    MPI_Type_create_struct(NODEINFO_LEN, blockLength, displacement, types, &MPINodeInfoDataType);
    MPI_Type_commit(&MPINodeInfoDataType);

    MATRIX matrix;

    if(IS_ROOT){
        MPI_Comm_size(MPI_COMM_WORLD, &(mpiInfo.numProcs));
        printf("%s Generating matrix.\n", LOG_PREFIX);
        nodeInfo = malloc(sizeof(nodeinfo_t) * mpiInfo.numProcs);

        matrixGen(&matrix, args.nCols, args.nRows);

        printf("%s Allocating tasks.\n", LOG_PREFIX);

        /*
         * Distribute the work load.
         * The work will be divided into blocks based on the block size arguments.
         * Blocks will be evenly distributed to the processors.
         * The remainder will be distributed to the processors who has the lower rank.
         * E.g., blocks = 30, processors = 4, dist = {8,8,7,7}
         */
        /*
         * Pad the matrix so that the matrix can be divided into blocks by the given block size.
         * The extra rows full of 0 will be added,
         * and extra 0s will be added to each row in order to process loop unrolling.
         * Since all extra members are 0, there will be no influence on the result.
        */
        unsigned pad = padding(&matrix, args.blockSize);
        unsigned blocks = PAIR_NUM( matrix.rows / args.blockSize);
        unsigned equalWorkload = blocks / mpiInfo.numProcs, remained = blocks % mpiInfo.numProcs;

        // If the blocks cannot be assigned to each processors for at least one,
        // calculate the number of nodes who can get task.
        unsigned workNode = equalWorkload == 0 ? remained : mpiInfo.numProcs;

        unsigned x = 0, y = 0;
        for(int nodeID = 0; nodeID < workNode; ++nodeID){
            nodeInfo[nodeID].pad = pad;
            nodeInfo[nodeID].beginPositionX = x;
            nodeInfo[nodeID].beginPositionY = y;
            nodeInfo[nodeID].workLoad = nodeID < remained ? equalWorkload + 1 : equalWorkload;
            if(nodeID < workNode - 1){ // calculate the begin position for next process
                x += nodeInfo[nodeID].workLoad * args.blockSize;
                while(x >= matrix.rows){
                    y += args.blockSize;
                    x -= matrix.rows - y;
                }
            }
        }
        // For processors who cannot get task, set the workload to zero.
        for(int nodeID = (int)workNode; nodeID < mpiInfo.numProcs; ++nodeID){
            nodeInfo[nodeID].workLoad = 0;
        }

        printf("%s Broadcasting tasks.\n", LOG_PREFIX);

    }
    // Scatter the task distribution information.
    MPI_Scatter(nodeInfo, 1, MPINodeInfoDataType, &task, 1, MPINodeInfoDataType, 0, MPI_COMM_WORLD);
    // After the information sent, the registered type is no longer needed.
    MPI_Type_free(&MPINodeInfoDataType);

    printf("%s [P%d] Task retrieved %d blocks.\n", LOG_PREFIX, mpiInfo.id, task.workLoad);
    // For the non-root processors, allocate an empty matrix.
    if(!IS_ROOT) matrixAlloc(&matrix, args, task);
    else printf("%s Broadcasting matrix.\n", LOG_PREFIX);
    // Broadcasting each row of the matrix one by one.
    for(int i = 0; i < matrix.rows; ++i)
        MPI_Bcast(matrix.matrix[i], (int)matrix.cols, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if(!args.silent && IS_ROOT){ // if silent flag is not present
        printMatrix(&matrix);
    }

    RES naiveResult;
    distributed_res_t myAlgorithmResult;
    float *finalResult = NULL;


    if(!args.skipAlgorithmTest){ // if the designed algorithm is not skipped
        if(IS_ROOT) printf("%s Task begin.\n", LOG_PREFIX);
        distributedResGen(&myAlgorithmResult, task, args); // Allocate memory for the result
        // do the algorithm computation and get time consumption
        unsigned long myAlgorithmTimeConsumption = myAlgorithm(&matrix, &myAlgorithmResult, args, task);

        // Before collecting the result from processors, collect their result size at first.
        unsigned *sizeBuffer = IS_ROOT ? malloc(sizeof(unsigned) * mpiInfo.numProcs) : NULL;

        if(IS_ROOT) printf("%s Gathering results.\n", LOG_PREFIX);
        printf("%s [P%d] Sending results. Size = %d\n", LOG_PREFIX ,mpiInfo.id, myAlgorithmResult.size);
        MPI_Gather(&(myAlgorithmResult.size), 1, MPI_UNSIGNED, sizeBuffer, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

        /*
         * Collecting results.
         * Result distributed on each process will be in two array:
         * One storing all of the results generated on the processor,
         * another one is the index in the final result array of the
         * corresponding element in the distributed result array.
         * e.g.:
         *  processor0: result array = [1,2,6,3], index array = [7,0,5,4]
         *  processor1: result array = [8,3,5,2], index array = [2,3,1,6]
         * The final result array will be [2,5,8,3,3,6,2,1]
         */
        float *resBuffer = NULL;
        unsigned *idxBuffer = NULL;

        int *displacementOffset = NULL;
        unsigned dataVolume;
        if(IS_ROOT){
            // Calculate the total volume of data need to be collected.
            dataVolume = PAIR_NUM(args.nRows);
            // Based on the data volume collected above, calculate the displacement of each array.
            displacementOffset = calloc(mpiInfo.numProcs, sizeof(int));
            for(int i = 1; i < mpiInfo.numProcs; ++i)
                displacementOffset[i] = displacementOffset[i - 1] + (int)sizeBuffer[i - 1];

            resBuffer = malloc(sizeof(float) * dataVolume);
            idxBuffer = malloc(sizeof(unsigned) * dataVolume);
            finalResult = calloc(dataVolume, sizeof(float));
        }
        // Gathering 2 array from each processor.
        MPI_Gatherv(myAlgorithmResult.array, (int)myAlgorithmResult.size, MPI_FLOAT, resBuffer, (int*)sizeBuffer, displacementOffset, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Gatherv(myAlgorithmResult.index, (int)myAlgorithmResult.size, MPI_UNSIGNED, idxBuffer, (int*)sizeBuffer, displacementOffset, MPI_UNSIGNED, 0, MPI_COMM_WORLD);
        // Reorder the array to get the final result.
        if(IS_ROOT){
            for(int i = 0; i < dataVolume; ++i){
                finalResult[idxBuffer[i]] = resBuffer[i];
            }
            gettimeofday(&after, NULL);
            printf("%s Total time consumption of the algorithm computation & communication: %ldus\n", LOG_PREFIX, timediff(before, after));
            free(resBuffer);
            free(idxBuffer);
            free(displacementOffset);
            free(sizeBuffer);
            printResult(args.nRows, finalResult, myAlgorithmTimeConsumption, "myAlgorithm", args.silent);
        }
    }

    if(!args.skipSequentialTest && IS_ROOT){ // if the sequential algorithm is not skipped
        printf("%s Sequential task begin.\n", LOG_PREFIX);
        resGen(&naiveResult, args.nRows); // allocate memory for result
        unsigned long naiveTimeConsumption = naiveSolve(&matrix, &naiveResult, args); // do the sequential algorithm and get time-consumption
        printResult(args.nRows, naiveResult.array, naiveTimeConsumption, "naive", args.silent); // print result and time
        if(!args.skipAlgorithmTest)
            resultCmp(args.nRows, naiveResult.array, finalResult, "myAlgorithm");
    }

    // Clean up
    if(IS_ROOT){
        printf("%s Cleaning up.\n", LOG_PREFIX);
        free(finalResult);
    }
    matrixFree(&matrix);
    if(!args.skipSequentialTest && IS_ROOT){
        resFree(&naiveResult);
    }
    if(!args.skipAlgorithmTest){
        distributedResFree(&myAlgorithmResult);
    }

    MPI_Finalize();

    return 0;
}