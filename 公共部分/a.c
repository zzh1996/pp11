#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void my_MPI_Allgather(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf,
                        int recvcount, MPI_Datatype recvtype, MPI_Comm comm)
{
    int rank, size, i, recvtypesize;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Type_size(recvtype, &recvtypesize);
    for (i = 0; i < size; i++)
    {
        if (i != rank)
        {
            MPI_Send(sendbuf, sendcount, sendtype, i, 0, comm);
        }
    }
    for (i = 0; i < size; i++)
    {
        if (i != rank)
        {
            MPI_Recv(recvbuf + recvtypesize * i, recvcount, recvtype, i, 0, comm, MPI_STATUS_IGNORE);
        }
    }
    memcpy(recvbuf + recvtypesize * rank, sendbuf, recvcount * recvtypesize);
}

int main(int argc, char **argv)
{
    int rank, size, i, cnt;
    int *data, *data2;
    double t1, t2;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    data = malloc(size * sizeof(int));

    data[rank] = rank;

    data2 = malloc(size * sizeof(int));

    t1 = MPI_Wtime();

    for (cnt = 0; cnt < 1000000; cnt++)
        my_MPI_Allgather(&data[rank], 1, MPI_INT, data2, 1, MPI_INT, MPI_COMM_WORLD);

    t2 = MPI_Wtime();

    if (rank == 0)
        printf("my_MPI_Allgather time = %f\n", t2 - t1);

    for (i = 0; i < size; i++)
    {
        if (data2[i] != i)
        {
            printf("Process %d data wrong!\n", rank);
        }
    }

    data2 = malloc(size * sizeof(int));

    t1 = MPI_Wtime();

    for (cnt = 0; cnt < 1000000; cnt++)
        MPI_Allgather(&data[rank], 1, MPI_INT, data2, 1, MPI_INT, MPI_COMM_WORLD);

    t2 = MPI_Wtime();

    if (rank == 0)
        printf("MPI_Allgather time = %f\n", t2 - t1);

    for (i = 0; i < size; i++)
    {
        if (data2[i] != i)
        {
            printf("Process %d data wrong!\n", rank);
        }
    }

    MPI_Finalize();
    return 0;
}