#include <iostream>
#include <chrono>
#include <ctime>

#include <stdio.h>
#include <math.h>
#include <assert.h>

__global__ void helloFromGPU() {
    printf("Hello from GPU!\n");
}

int main() {
    
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now(); 
   
    // cuda function here
    helloFromGPU<<<1,10>>>();
 
    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_sec = end - start;
    printf("%.2f seconds\n", elapsed_sec);

    // cleans up device resources
    cudaDeviceReset(); 
}
