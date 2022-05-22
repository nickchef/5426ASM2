#include <stdio.h>
#include <string.h>
#include "mpi.h"

int main(int argc, char* argv[])
{
    int numprocs, myid, source;
    MPI_Status status;
    char message[100];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    if (myid != 0) {  //非0号进程发送消息
        strcpy(message, "Hello World!");
        MPI_Request r;
        MPI_Isend(message, (int)strlen(message) + 1, MPI_CHAR, 0, 0,
                  MPI_COMM_WORLD, &r);
        MPI_Request_free(&r);
    }
    else {   // myid == 0，即0号进程接收消息
        for (source = 1; source < numprocs; source++) {
            MPI_Recv(message, 100, MPI_CHAR, source, MPI_ANY_TAG,
                     MPI_COMM_WORLD, &status);
            printf("接收到第%d号进程发送的消息：%s\n", source, message);
        }
    }
    MPI_Finalize();
} /* end main */