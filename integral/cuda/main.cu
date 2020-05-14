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

#define DEBUG 0

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
    
    curandState_t state;
    curand_init(id, 0, 0, &state); // curand_init(seed, sequence number, offset, &state)

    for(int i=0; i<numSamples; i++) {
		// (rand() % (upper - lower + 1)) + lower; 
        double x = curand_uniform_double(&state);
        x *= (b - a + 0.999999);
        x += a;
        double result = sin(x)/x;
        result = result * (isnan(result) == 0);
        work[id] += result; // if nan, it will simply add zero ; reduces branches
    }
    
    //synchronize threads within block
    __syncthreads();

    #if DEBUG
    if(id==0) {
        for(int i=0; i<bx*by; i++) {
            if(i%bx==0 && i!=0) printf("\n");
            printf("%.2f ", work[i]);
        }
        printf("\n");
    }

    __syncthreads();
    #endif
    
    // sum up the values in each row
    if(threadIdx.x == 0) { // if thread column is left-most
        for(int x=1; x<bx; x++) { // sum up all the partial sums in this row
            int index = iy*bx + (x + blockIdx.x * blockDim.x);
            work[id] += work[index];
        }
    }   
 
    //synchronize threads within block
    __syncthreads();
   
    #if DEBUG 
    if(id==0) {
        printf("After row sums computed\n");
        for(int i=0; i<bx*by; i++) {
            if(i%bx==0 && i!=0) printf("\n");
            printf("%.2f ", work[i]);
        }
        printf("\n");
    }

    __syncthreads();
    #endif

    // sum up each row's sum into the results
    if(threadIdx.x == 0 && threadIdx.y == 0) {
        for(int y=0; y<by; y++) {
            int index = (y + blockIdx.y * blockDim.y)*bx + ix;
            results[blockIdx.x] += work[index];
            #if DEBUG
            printf("id=%i, writing to results[%u]=%.2f, added=%.2f\n", id, blockIdx.x, results[blockIdx.x], work[index]);
            #endif
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
   
    // define grid and block structure ; largest possible block can hold 1,024 threads (32x32)
    int bx = 32;
    int by = 32;
    {
        int minLength = sqrt(numThreads);
        if(minLength <= 32) {
            bx = minLength;
            by = minLength;
        }
    }
    const int numBlocks = ceil(numThreads / (bx*by));
    dim3 block(bx, by);
    dim3 grid(numBlocks); // 1D grid
 
    // set up memory for results
    mem_t d_results;
    d_results.numItems = numBlocks;
    d_results.numBytes = d_results.numItems * sizeof(double);
    cudaMalloc((void**)&d_results.ptr, d_results.numBytes); // each block will compute sum ; host will add up the block sums

    // set up memory for work buffer ; this buffer does not have to be sent back to the host
    mem_t d_work;
    d_work.numItems = (bx*by) * numBlocks;
    d_work.numBytes = d_work.numItems * sizeof(double);
    cudaMalloc((void**)&d_work.ptr, d_work.numBytes); // each block will compute sum ; host will add up the block sums

    // launch CUDA kernel 
    int samplesPerThread = numSamples / ((bx*by) * numBlocks);
    integrate<<<grid, block>>>(a, b, samplesPerThread, d_work.ptr, d_results.ptr, bx, by);

    #if DEBUG
    printf("a=%.2f,b=%.2f,numSamples=%i,numThreads=%i,samplesPerThread=%i,numBlocks=%i\n", a, b, numSamples, numThreads, samplesPerThread, numBlocks);
    printf("block.x=%u, block.y=%u\n", block.x, block.y);
    #endif
    
    // copy results from device to host
    double* h_results = (double*) malloc(d_results.numBytes);
    cudaMemcpy(h_results, d_results.ptr, d_results.numBytes, cudaMemcpyDeviceToHost); 
   
    // compute the sum of each block
    double integral = 0.0;
    for(int i=0; i<numBlocks; i++) {
        #if DEBUG
        printf("Block %i: sum=%.2f\n", i, h_results[i]);
        #endif
        integral += h_results[i];
    }
    integral = abs(b-a) * (integral/(samplesPerThread * (bx*by) * numBlocks)); // sum/numSamples
    
    // free device memory
    cudaFree(d_results.ptr);
    cudaFree(d_work.ptr);
    
    // free host memory
    free(h_results);

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_sec = end - start; 
    std::cout << std::setprecision(10) << integral << std::endl;

    #if DEBUG
    std::cout << numThreads << " threads: " << " numBlocks " << numBlocks << ", Result: " << std::setprecision(10) << integral << "; Elapsed time: " << elapsed_sec.count() << "s" << std::endl;
    #endif    

    // write to csv file
    std::string csvfile_name = std::to_string((int)abs(a)) + "-" + std::to_string((int)abs(b)) + "-" + std::to_string(numSamples) + "-" + std::to_string(numThreads) + ".csv";
    std::ofstream csvfile;
    csvfile.open(csvfile_name);
    // write csv header
    csvfile << "a,b,numSamples,integral,elapsed(sec),method,block,numBlocks,numThreads\n";
    
    csvfile << std::to_string((int)a) << ","
            << std::to_string((int)b) << ","
            << std::to_string(numSamples) << ","
            << std::to_string(integral) << ","
            << elapsed_sec.count() << ","
            << "monte-carlo" << ','
            << "(" << bx << ";" << by << ")" << ","
            << numBlocks << ","
            << std::to_string(numThreads) << "\n";
    
    csvfile.close();
}
