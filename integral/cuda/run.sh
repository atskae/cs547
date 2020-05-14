#!/bin/bash
#SBATCH --partition=gpu
#SBATCH --ntasks=1

if [ $# != 4 ] # if number of arguments != 4
then
    echo "./integrate lowerLimit upperLimit numSamples numNumThreads"
    exit 1
fi

# Build the program
make

A=$1
B=$2
NUM_SAMPLES=$3
NUM_THREADS=$4
echo "a=$A,b=$B,numSamples=$NUM_SAMPLES,numThreads=$NUM_THREADS"

## Run integrate
srun ./integrate $A $B $NUM_SAMPLES $NUM_THREADS

## Clean executable
make clean
