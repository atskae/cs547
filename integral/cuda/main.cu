#include <iostream>
#include <string> // stof, stoi
#include <chrono>
#include <ctime>
#include <fstream>
#include <cmath>
#include <iomanip>

#include <stdio.h>
#include <math.h>
#include <assert.h>

// CUDA libraries
#include <curand.h>
#include <curand_kernel.h>

// function to integrate: sin(x)/x

struct mem_t {
    double* ptr;
    size_t numItems;
    size_t numBytes;

    mem_t(): ptr(NULL), numItems(0), numBytes(0) {};
};

// monte carlo ; each CUDA thread
__global__ void integrate(double a, double b, int numSamples, double* work, double* results, int bx, int by) {
    int ix = threadIdx.x + blockIdx.x * blockDim.x;
    int iy = threadIdx.y + blockIdx.y * blockDim.y;
    int id = iy*bx + ix;
    
    //std::default_random_engine randEngine;
    //randEngine.seed(id);
    //std::uniform_real_distribution<double> randGen(a, b);
    curandState_t state;
    curand_init(id, 0, 0, &state); // curand_init(seed, sequence number, offset, &state)

    for(int i=0; i<numSamples; i++) {
        //double x = randGen(randEngine);
		// (rand() % (upper - lower + 1)) + lower; 
        double x = curand_uniform_double(&state);
        x *= (b - a + 0.999999);
        x += a;
        double result = sin(x)/x;
        work[id] += (result * !isnan(result)); // if nan, it will simply add zero ; reduces branches
    }
    
    //synchronize threads within block
    __syncthreads();

    // sum up the values in each row
    if(threadIdx.x == 0) { // if thread column is left-most
        for(int x=1; x<bx; x++) { // sum up all the partial sums in this row
            int index = iy*bx + (x + blockIdx.x * blockDim.x);
            work[id] += work[index];
        }
    }   
 
    //synchronize threads within block
    __syncthreads();

    // sum up each row's sum into the results
    if(threadIdx.x == 0 && threadIdx.y == 0) {
        for(int y=0; y<by; y++) {
            int index = (y + blockIdx.y * blockDim.y)*bx + ix;
            results[id % (bx*by)] += work[index];
        }
    }

}

int main(int argc, char* argv[]) {
    
    if(argc < 5) {
        printf("./integrate lowerLimit, upperLimit, numSamples, numThreads\n");
        exit(1);
    }
   
    double a = std::stof(argv[1]); // lower limit
    double b = std::stof(argv[2]); // upper limit
    int numSamples = std::stoi(argv[3]); 
    int numThreads = std::stoi(argv[4]);

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now(); 
   
    // define grid and block structure
    const int bx = 32;
    const int by = 32;
    const int numBlocks = ceil(numThreads / (bx*by));
    dim3 block(bx, by); // 2D block of 32x32 = 1,024 threads per block
    dim3 grid(numBlocks); // 1D grid
    printf("a=%.2f,b=%.2f,numSamples=%i,numThreads=%i,numBlocks=%i\n", a, b, numSamples, numThreads, numBlocks);
    printf("block.x=%u, block.y=%u\n", block.x, block.y);
 
    // set up memory for results
    //double* d_results;
    //size_t numBytes = numBlocks * sizeof(double);
    mem_t d_results;
    d_results.numItems = numBlocks;
    d_results.numBytes = d_results.numItems * sizeof(double);
    cudaMalloc((void**)&d_results.ptr, d_results.numBytes); // each block will compute sum ; host will add up the block sums
    //cudaMalloc((void**)&d_results, numBytes); // each block will compute sum ; host will add up the block sums

    // set up memory for work buffer ; this buffer does not have to be sent back to the host
    mem_t d_work;
    d_work.numItems = (bx*by) * numBlocks;
    d_work.numBytes = d_work.numItems * sizeof(double);
    cudaMalloc((void**)&d_work.ptr, d_work.numBytes); // each block will compute sum ; host will add up the block sums

    // launch CUDA kernel 
    integrate<<<grid, block>>>(a, b, numSamples/numThreads, d_results.ptr, d_work.ptr, bx, by);

    // copy results from device to host
    double* h_results = (double*) malloc(d_results.numBytes);
    cudaMemcpy(h_results, d_results.ptr, d_results.numBytes, cudaMemcpyDeviceToHost); 
   
    // compute the sum of each block
    double integral = 0.0;
    for(int i=0; i<numBlocks; i++) {
        integral += h_results[i];
    }

    // free device memory
    cudaFree(d_results.ptr);
    cudaFree(d_work.ptr);
    
    // free host memory
    free(h_results);

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_sec = end - start; 
    std::cout << numThreads << " threads: " << " numBlocks " << numBlocks << " Result: " << std::setprecision(10) << integral << "; Elapsed time: " << elapsed_sec.count() << "s" << std::endl;
    //std::cout << std::setprecision(10) << integral << std::endl;

    // write to csv file
    std::string csvfile_name = std::to_string((int)abs(a)) + "-" + std::to_string((int)abs(b)) + "-" + std::to_string(numSamples) + "-" + std::to_string(numThreads) + ".csv";
    std::ofstream csvfile;
    csvfile.open(csvfile_name);
    // write csv header
    csvfile << "a,b,numSamples,integral,elapsed(sec),method,numThreads\n";
    
    csvfile << std::to_string((int)a) << ","
            << std::to_string((int)b) << ","
            << std::to_string(numSamples) << ","
            << std::to_string(integral) << ","
            << elapsed_sec.count() << ","
            << "monte-carlo" << ','
            << std::to_string(numThreads) << "\n";
    
    csvfile.close();
}
