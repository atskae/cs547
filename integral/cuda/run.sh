#!/bin/bash
#SBATCH --partition=gpu
#SBATCH --ntasks=1

if [ $# != 4 ] # if number of arguments != 4
then
    echo "./integrate lowerLimit upperLimit numSamples maxNumThreads"
    exit 1
fi

# Build the program
make

A=$1
B=$2
NUM_SAMPLES=$3
MAX_THREADS=$4
echo "a=$A,b=$B,numSamples=$NUM_SAMPLES,maxThreads=$MAX_THREADS"

## Run integrate
srun ./integrate $A $B $NUM_SAMPLES $NUM_THREADS

## Clean executable
#make clean
#
## Take absolute value for naming purposes
#ABS_1=$1
#ABS_2=$2
#
#DATA_DIR="$ABS_1-$ABS_2-$3-all"
#rm -rf $DATA_DIR
#mkdir $DATA_DIR
#mv *.csv $DATA_DIR
#tar -czvf "$DATA_DIR.tar.gz" $DATA_DIR
