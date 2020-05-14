# Numerical Integration in CUDA

## How to Run
1. First log into `openhpc`
2. Build the project by typing `make`. This should build an executable called `integrate`
3. Run the project from 1 to `N` threads by running `sbatch ./run.sh a b numSamples numThreads`, where `a` and `b` are the lower and upper limits of integration, `numSamples` is the total number of samples to collect across all threads, and `numThreads` is the number of threads to execute.
