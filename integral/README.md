# Numerical Integration

## How to Run
1. First log into `openhpc`
2. Load the `gnu8` and `cmake` modules by running: `module load <module>`, where `<module>` is the name of the module to load.
3. Build the project by typing `make`. This should build an executable called `integrate`
4. Run the project from 1 to `N` threads by running `sbatch ./run.sh a b numSamples N`, where `a` and `b` are the lower and upper limits of integration, `numSamples` is the total number of samples to collect across all threads, and `N` is the maximum number of threads to execute.

## Files and Directories
* `res/` Project resources
* `prac/` Practice using pthreads and computing integrals sequentially
