#!/bin/bash
#SBATCH --cpus-per-task=24
#SBATCH -N 1
#SBATCH --mem=8G

if [ $# != 4 ] # if number of arguments != 4
then
    echo "./integrate lowerLimit upperLimit numSamples maxNumThreads"
    exit 1
fi

# Load modules
module load gnu8
module load cmake

A=$1
B=$2
NUM_SAMPLES=$3
MAX_THREADS=$4
echo "a=$A,b=$B,numSamples=$NUM_SAMPLES,maxThreads=$MAX_THREADS"

# Build the executable
#srun make

# run the first 12 without multithreading
#for NUM_THREADS in $(seq 1 12)
#do
#    srun --hint=nomultithread /home/kchiu/time -p ./integrate $A $B $NUM_SAMPLES $NUM_THREADS
#done
#
#for NUM_THREADS in $(seq 12 $MAX_THREADS)
#do
#    srun /home/kchiu/time -p ./integrate $A $B $NUM_SAMPLES $NUM_THREADS
#done

## Run integrate for 1 to MAX_THREADS
for NUM_THREADS in $(seq 1 $MAX_THREADS)
do
    srun /home/kchiu/time -p ./integrate $A $B $NUM_SAMPLES $NUM_THREADS
    #srun /home/kchiu/time -p ./bothMethods $A $B $NUM_SAMPLES $NUM_THREADS
done

# Clean executable
#srun make clean

# Combine all data into a .csv file
#srun python make-csv.py $A $B $NUM_SAMPLES

# Take absolute value for naming purposes

ABS_1=$1
ABS_2=$2

#if [ $1 -lt 0 ]
#then
#    ABS_1= [ $1*(-1) ]
#fi
#
#if [ $2 -lt 0 ]
#then
#    ABS_2= [ $1*(-1) ] 
#fi

DATA_DIR="$ABS_1-$ABS_2-$3-all"
rm -rf $DATA_DIR
mkdir $DATA_DIR
mv *.csv $DATA_DIR
tar -czvf "$DATA_DIR.tar.gz" $DATA_DIR
