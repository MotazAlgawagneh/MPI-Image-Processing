#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include "pgm.h"

int main(int argc, char **argv) {

MPI_Init(&argc, &argv);

int rank, size;

MPI_Comm_rank(MPI_COMM_WORLD, &rank);
MPI_Comm_size(MPI_COMM_WORLD, &size);

if (argc != 3) {
        if (rank == 0) {
        printf("Usage: mpiexec -n <size> %s <input.pgm> <output.pgm>\n", argv[0]);
        }
        MPI_Finalize();
        return 0;
    }



int w, h, maxval;
    PGMImage *in_img = NULL, *out_seq = NULL, *out_par = NULL;
    double t0, t1;
    double Tseq = 0, Tbroadcast = 0, Tscatter = 0, Tgather = 0, Tcomp = 0;


if (rank == 0) {
        in_img = pgm_read(argv[1]);

        if (!in_img) {
            printf("Error reading input image.\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        w = in_img->width;
        h = in_img->height;
        maxval = in_img->maxval;
        
        out_seq = pgm_create(w, h, maxval);
        out_par = pgm_create(w, h, maxval);

        t0 = MPI_Wtime();
        int total_pixels = w * h;
        for (int i = 0; i < total_pixels; i++) {
            out_seq->data[i] = maxval - in_img->data[i];
        }
        t1 = MPI_Wtime();
        Tseq = t1 - t0;
    }

    MPI_Barrier(MPI_COMM_WORLD);


    int dims[3];
    if (rank == 0) {
        dims[0] = w;
        dims[1] = h;
        dims[2] = maxval;
    }

    t0 = MPI_Wtime();
    MPI_Bcast(dims, 3, MPI_INT, 0, MPI_COMM_WORLD);
    t1 = MPI_Wtime();
    Tbroadcast = t1 - t0;

    if (rank != 0) {
        w = dims[0];
        h = dims[1];
        maxval = dims[2];
    }


    int total_pixels = w * h;
    int chunk_size = total_pixels / size;
    int *local_buf = (int *)malloc(chunk_size * sizeof(int));

    int *send_buf = NULL;
    if (rank == 0) {
        send_buf = in_img->data;
    }

    t0 = MPI_Wtime();
    MPI_Scatter(send_buf, chunk_size, MPI_INT, local_buf, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    t1 = MPI_Wtime();
    Tscatter = t1 - t0;

    t0 = MPI_Wtime();
    for (int i = 0; i < chunk_size; i++) {
        local_buf[i] = maxval - local_buf[i];
    }
    t1 = MPI_Wtime();
    Tcomp = t1 - t0;

    MPI_Barrier(MPI_COMM_WORLD);


    int *recv_buf = NULL;
    if (rank == 0) {
        recv_buf = out_par->data;
    }

    t0 = MPI_Wtime();
    MPI_Gather(local_buf, chunk_size, MPI_INT, recv_buf, chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
    t1 = MPI_Wtime();
    Tgather = t1 - t0;



    double local_Tcomm = Tbroadcast + Tscatter + Tgather;
    double max_Tcomm = 0.0, max_Tcomp = 0.0;

    MPI_Reduce(&local_Tcomm, &max_Tcomm, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&Tcomp, &max_Tcomp, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);


    if (rank == 0) {
        double Tpar = max_Tcomm + max_Tcomp;
        double Speedup = Tseq / Tpar;

        printf("Tseq    = %f\n", Tseq);
        printf("Tcomm   = %f\n", max_Tcomm);
        printf("Tcomp   = %f\n", max_Tcomp);
        printf("Tpar    = %f\n", Tpar);
        printf("Speedup = %f\n", Speedup);

        pgm_write(argv[2], out_par);

        
        pgm_free(in_img);
        pgm_free(out_seq);
        pgm_free(out_par);
    }

   
    free(local_buf);
    MPI_Finalize();

    return 0;}
