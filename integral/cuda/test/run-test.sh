#!/bin/bash
#SBATCH --partition=gpu
#SBATCH --ntasks=1

# Build test program
make

# Run test program
srun ./test

# Clean executable
make clean
