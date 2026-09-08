# MPI Image Negative Converter

## Project Overview
This project demonstrates distributed memory parallel computing using the Message Passing Interface (MPI) in C. It reads a grayscale `.pgm` image, computes its negative, and compares the performance of a baseline sequential implementation against an MPI-based parallel implementation.

## Features
- **Sequential Baseline:** Computes the image negative on a single process for performance comparison.
- **Distributed Processing (MPI):** 
  - Uses `MPI_Bcast` to share image dimensions.
  - Uses `MPI_Scatter` to distribute chunks of image rows across multiple processes.
  - Uses `MPI_Gather` to collect the processed pixels back to the root process.
- **Performance Profiling:** Calculates communication time (T_comm), computation time (T_comp), total parallel time (T_par), and overall speedup using `MPI_Wtime()` and `MPI_Reduce` (with `MPI_MAX` to account for the slowest process).

## Compilation and Execution
Ensure you have an MPI implementation (e.g., MPICH or OpenMPI) installed. 

To compile the program, run:
`mpicc main.c pgm.o -o image_negative`

To execute the program (e.g., using 4 processes), run:
`mpiexec -n 4 ./image_negative apple.pgm negative.pgm`

## Output
The program saves the processed image to the specified output file (`negative.pgm`) and prints the following performance metrics to the console:
- `Tseq`
- `Tcomm`
- `Tcomp`
- `Tpar`
- `Speedup`
