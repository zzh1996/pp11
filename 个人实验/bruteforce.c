// mpicc bruteforce.c -lcrypto -fopenmp
#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

#define chunk 1024 * 1024
#define MAX 1000000000

int main(int argc, char **argv)
{
    int rank, size, i;
    double t1, t2;
    char data[100];
    unsigned char result[16];
    unsigned char const target[] = {110, 190, 118, 201, 251, 65, 27, 233, 123, 59, 13, 72, 183, 145, 167, 201};

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("MPI %d/%d, %d openmp threads\n", rank, size, omp_get_max_threads());

    MD5_CTX c;

    int part = MAX / size;
    int begin = part * rank;
    int end = part * (rank + 1);

    t1 = MPI_Wtime();

    #pragma omp parallel for private(data, i, result, c) schedule(dynamic, chunk)
    for (i = begin; i < end; i++)
    {
        sprintf(data, "%d", i);
        MD5_Init(&c);
        MD5_Update(&c, data, strlen(data));
        MD5_Final(result, &c);
        if (!memcmp(result, target, 16))
        {
            t2 = MPI_Wtime();
            printf("result: %d\ntime: %f\n", i, t2 - t1);
            //MPI_Abort(MPI_COMM_WORLD, 0);
        }
        if (i % 10000000 == 0)
            printf("%d\n", i);
    }

    printf("MPI %d/%d finished\n", rank, size);
    MPI_Barrier(MPI_COMM_WORLD);
    t2 = MPI_Wtime();
    if (rank == 0)
        printf("Total time: %f\n", t2 - t1);

    MPI_Finalize();
    return 0;
}