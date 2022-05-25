#include "Baseline.h"
#include "MyAlgorithm.h"
#include "utils.h"

int main(int argc, char *argv[]){
    // set default command line arguments
    ARGS args = {1000, 1000, 4, 2, false, false, false};


    MPI_INFO mpiInfo;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &(mpiInfo.id));


#define IS_ROOT (mpiInfo.id == 0)

//    if(IS_ROOT){
//        int stop = 1;
//        while(stop)
//            sleep(5);
//    }

    if(IS_ROOT) printf("%s Started.\n", LOG_PREFIX);
    if(IS_ROOT) printf("%s Parsing arguments.\n", LOG_PREFIX);

    TIME before, after;
    gettimeofday(&before, NULL);
    // read command line arguments
    argParse(argc, argv, &args);

    nodeinfo_t *nodeInfo = NULL;
    nodeinfo_t task;

    if(IS_ROOT) printf("%s Registering MPI data type.\n", LOG_PREFIX);

    MPI_Aint address[NODEINFO_LEN];

    MPI_Get_address(&(task.pad), &(address[0]));
    MPI_Get_address(&(task.beginPositionX), &(address[1]));
    MPI_Get_address(&(task.beginPositionY), &(address[2]));
    MPI_Get_address(&(task.workLoad), &(address[3]));

    MPI_Aint displacement[NODEINFO_LEN] = {
            0,
            address[1] - address[0],
            address[2] - address[0],
            address[3] - address[0]
    };

    MPI_Datatype types[NODEINFO_LEN] = {MPI_UNSIGNED,MPI_UNSIGNED,MPI_UNSIGNED,MPI_UNSIGNED};
    int blockLength[NODEINFO_LEN] = {1, 1, 1, 1};
    MPI_Datatype MPINodeInfoDataType;

    MPI_Type_create_struct(NODEINFO_LEN, blockLength, displacement, types, &MPINodeInfoDataType);
    MPI_Type_commit(&MPINodeInfoDataType);

    MATRIX matrix;

    if(IS_ROOT){
        MPI_Comm_size(MPI_COMM_WORLD, &(mpiInfo.numProcs));
        printf("%s Generating matrix.\n", LOG_PREFIX);
        nodeInfo = malloc(sizeof(nodeinfo_t) * mpiInfo.numProcs);

        matrixGen(&matrix, args.nCols, args.nRows);

        printf("%s Allocating tasks.\n", LOG_PREFIX);

        unsigned pad = padding(&matrix, args.blockSize);
        unsigned blocks = PAIR_NUM( matrix.rows / args.blockSize);
        unsigned equalWorkload = blocks / mpiInfo.numProcs, remained = blocks % mpiInfo.numProcs;
        unsigned workNode = equalWorkload == 0 ? remained : mpiInfo.numProcs;

        unsigned x = 0, y = 0;
        for(int nodeID = 0; nodeID < workNode; ++nodeID){
            nodeInfo[nodeID].pad = pad;
            nodeInfo[nodeID].beginPositionX = x;
            nodeInfo[nodeID].beginPositionY = y;
            nodeInfo[nodeID].workLoad = nodeID < remained ? equalWorkload + 1 : equalWorkload;
            if(nodeID < workNode - 1){ // calculate the begin position for next thread
                x += nodeInfo[nodeID].workLoad * args.blockSize;
                while(x >= matrix.rows){
                    y += args.blockSize;
                    x -= matrix.rows - y;
                }
            }
        }

        for(int nodeID = (int)workNode; nodeID < mpiInfo.numProcs; ++nodeID){
            nodeInfo[nodeID].workLoad = 0;
        }

        printf("%s Broadcasting tasks.\n", LOG_PREFIX);

    }
    // Generate matrix
    MPI_Scatter(nodeInfo, 1, MPINodeInfoDataType, &task, 1, MPINodeInfoDataType, 0, MPI_COMM_WORLD);

    printf("%s [P%d] Task retrieved %d blocks.\n", LOG_PREFIX, mpiInfo.id, task.workLoad);

    if(!IS_ROOT) matrixAlloc(&matrix, args, task);
    else printf("%s Broadcasting matrix.\n", LOG_PREFIX);

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
        distributedResGen(&myAlgorithmResult, task, args);
        unsigned long myAlgorithmTimeConsumption = myAlgorithm(&matrix, &myAlgorithmResult, args, task); // do the sequential algorithm andcomputation and get time consumption

        // gathering the results
        unsigned *sizeBuffer = IS_ROOT ? malloc(sizeof(unsigned) * mpiInfo.numProcs) : NULL;

        if(IS_ROOT) printf("%s Gathering results.\n", LOG_PREFIX);
        // gathering size at first;
        printf("%s [P%d] Sending results. Size = %d\n", LOG_PREFIX ,mpiInfo.id, myAlgorithmResult.size);
        MPI_Gather(&(myAlgorithmResult.size), 1, MPI_UNSIGNED, sizeBuffer, 1, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

        float *resBuffer = NULL;
        unsigned *idxBuffer = NULL;

        int *displacementOffset = NULL;
        unsigned dataVolume;
        if(IS_ROOT){
            dataVolume = PAIR_NUM(args.nRows);
            displacementOffset = calloc(mpiInfo.numProcs, sizeof(int));

            for(int i = 1; i < mpiInfo.numProcs; ++i)
                displacementOffset[i] = displacementOffset[i - 1] + (int)sizeBuffer[i - 1];

            resBuffer = malloc(sizeof(float) * dataVolume);
            idxBuffer = malloc(sizeof(unsigned) * dataVolume);
            finalResult = calloc(dataVolume, sizeof(float));
        }

        MPI_Gatherv(myAlgorithmResult.array, (int)myAlgorithmResult.size, MPI_FLOAT, resBuffer, (int*)sizeBuffer, displacementOffset, MPI_FLOAT, 0, MPI_COMM_WORLD);
        MPI_Gatherv(myAlgorithmResult.index, (int)myAlgorithmResult.size, MPI_UNSIGNED, idxBuffer, (int*)sizeBuffer, displacementOffset, MPI_UNSIGNED, 0, MPI_COMM_WORLD);

        if(IS_ROOT){
            for(int i = 0; i < dataVolume; ++i){
                finalResult[idxBuffer[i]] = resBuffer[i];
            }
            gettimeofday(&after, NULL);
            printf("Total time consumption of the algorithm computation & communication: %ldus\n", timediff(before, after));
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
    if(IS_ROOT) printf("%s Cleaning up.\n", LOG_PREFIX);
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