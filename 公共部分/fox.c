#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int rank, size;
int sqrtp;
MPI_Comm matcomm, rowcomm, colcomm;
int matrank;
int n, block;
int row, col;

void read_mat(FILE *fp, double *mat)
{
    int coords[2];
    int dst;
    if (rank == 0)
    {
        double *buf = malloc(sizeof(double) * block);
        for (int i = 0; i < n; i++)
        {
            coords[0] = i / block;
            for (int j = 0; j < sqrtp; j++)
            {
                coords[1] = j;
                MPI_Cart_rank(matcomm, coords, &dst);
                for (int k = 0; k < block; k++)
                {
                    fscanf(fp, "%lf", &buf[k]);
                    //printf("%d %d %d %f\n",i,j,k,buf[k]);
                }
                MPI_Send(buf, block, MPI_DOUBLE, dst, 0, matcomm);
            }
        }
        free(buf);
    }
    for (int i = 0; i < block; i++)
    {
        MPI_Recv(&mat[i * block], block, MPI_DOUBLE, 0, 0, matcomm, MPI_STATUS_IGNORE);
    }
}

void print_mat(double *mat)
{
    int coords[2];
    int src;
    for (int i = 0; i < block; i++)
    {
        MPI_Send(&mat[i * block], block, MPI_DOUBLE, 0, 0, matcomm);
    }
    if (rank == 0)
    {
        double *buf = malloc(sizeof(double) * block);
        for (int i = 0; i < n; i++)
        {
            coords[0] = i / block;
            for (int j = 0; j < sqrtp; j++)
            {
                coords[1] = j;
                MPI_Cart_rank(matcomm, coords, &src);
                MPI_Recv(buf, block, MPI_DOUBLE, src, 0, matcomm, MPI_STATUS_IGNORE);
                for (int k = 0; k < block; k++)
                {
                    printf("%8.2f ", buf[k]);
                }
            }
            printf("\n");
        }
        free(buf);
    }
}

void fox(double *A, double *B, double *C)
{
    memset(C, 0, sizeof(double) * block * block);
    double *buf = malloc(sizeof(double) * block * block);
    for (int i = 0; i < sqrtp; i++)
    {
        int root = (row + i) % sqrtp;
        if (root == col)
        { // I should bcast
            MPI_Bcast(A, block * block, MPI_DOUBLE, root, rowcomm);
            memcpy(buf, A, sizeof(double) * block * block);
        }
        else
        { // I should recv
            MPI_Bcast(buf, block * block, MPI_DOUBLE, root, rowcomm);
        }
        for (int a = 0; a < block; a++)
            for (int b = 0; b < block; b++)
                for (int c = 0; c < block; c++)
                    C[a * block + b] += buf[a * block + c] * B[c * block + b];
        MPI_Sendrecv_replace(B, block * block, MPI_DOUBLE, (row - 1 + sqrtp) % sqrtp, 0, (row + 1) % sqrtp, 0, colcomm, MPI_STATUS_IGNORE);
    }
}

int main(int argc, char **argv)
{
    double t1, t2;
    int dims[2];
    int periods[2];
    int coords[2];
    int remain_dims[2];
    FILE *fp;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    t1 = MPI_Wtime();

    sqrtp = (int)sqrt(size);
    if (rank == 0)
    {
        if (sqrtp * sqrtp != size)
        {
            printf("p is not square!\n");
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
    }

    dims[0] = dims[1] = sqrtp;
    periods[0] = periods[1] = 1;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &matcomm);
    MPI_Comm_rank(matcomm, &matrank);
    MPI_Cart_coords(matcomm, matrank, 2, coords);
    row = coords[0];
    col = coords[1];
    remain_dims[0] = 0;
    remain_dims[1] = 1;
    MPI_Cart_sub(matcomm, remain_dims, &rowcomm);
    remain_dims[0] = 1;
    remain_dims[1] = 0;
    MPI_Cart_sub(matcomm, remain_dims, &colcomm);

    // load matrix
    if (rank == 0)
    {
        fp = fopen(argv[1], "r");
        fscanf(fp, "%d", &n);
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    block = n / sqrtp;
    if (rank == 0)
    {
        if (sqrtp * block != n)
        {
            printf("n=%d cannot be divided by sqrt(p)=%d!\n", n, sqrtp);
            MPI_Abort(MPI_COMM_WORLD, -1);
        }
    }
    double *A = malloc(sizeof(double) * block * block);
    double *B = malloc(sizeof(double) * block * block);
    double *C = malloc(sizeof(double) * block * block);
    read_mat(fp, A);
    read_mat(fp, B);

    if (rank == 0)
        fclose(fp);

    // core algo
    fox(A, B, C);

    t2 = MPI_Wtime();

    print_mat(C);

    if (rank == 0)
        printf("Total time: %f\n", t2 - t1);

    free(A);
    free(B);
    free(C);
    MPI_Finalize();
    return 0;
}