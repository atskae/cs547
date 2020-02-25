#include <iostream>
#include <string> // stof, stoi
#include <random>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <cmath>
#include <iomanip>

#include <stdio.h>
#include <math.h>
#include <assert.h>

// function to integrate: sin(x)/x

// shared across threads
struct param_t {
    const int numSamples;
    const double height; // height of each trapezoid sample
    double* sums; // use thread id to index into this array

    param_t(int n, double h): numSamples(n), height(h), sums(NULL) {}
};

// specific to a thread
struct args_t {
    int id;
    double lowerBound;
    param_t* param; // parameters used to compute the integral
};

// trapezoid
void* integrate(void* args) {

    int id = ((args_t*)args)->id;
    param_t* param = ((args_t*)args)->param;
    //printf("Thread %i in trapezoid ; numSamples=%i, range=%.2f, h=%.2f\n", id, param->numSamples, param->range, param->h);
    
    //param->sums[id] = 0.0;
    double sum = 0.0;
    double lowerBound = ((args_t*)args)->lowerBound;
    double h = param->height;
    for(int i=0; i<param->numSamples; i++) {
        double x1 = lowerBound + h*i;
        double x2 = lowerBound + h*(i+1);
        
        double b1 = sin(x1)/x1;
        double b2 = sin(x2)/x2;
        
        double area = h * ((b1+b2)/2);
        //printf("%i: x1=%.2f, x2=%.2f, area=%.5f\n", id, x1, x2, area);
        if(!isnan(area)) sum += area;
    }
    //printf("Thread %i: sum=%.2f\n", id, param->sums[id]); 
    param->sums[id] = sum;       

    delete (args_t*) args;
    pthread_exit(NULL);

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
    //printf("a=%.2f,b=%.2f,numSamples=%i,numThreads=%i\n", a, b, numSamples, numThreads);

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now(); 
    
    pthread_t* threads = new pthread_t[numThreads];
   
    // explicitly set thread attribute to joinable 
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    double integral = 0.0;
    double rangePerThread = abs(b-a)/numThreads;
    int samplesPerThread = numSamples/numThreads;
 
    param_t param(samplesPerThread, rangePerThread/samplesPerThread);
    param.sums = new double[numThreads];
    
    int err;
    for(int i=0; i<numThreads; i++) {
        args_t* args = new args_t;
        args->id = i;
        args->lowerBound = a + i*rangePerThread;
        args->param = &param;
        err = pthread_create(&threads[i], &attr, integrate, (void*) args);
        assert(!err);
    }
  
    // wait for all threads to complete
    void* status;
    for(int i=0; i<numThreads; i++) {
        err = pthread_join(threads[i], &status);
        assert(!err);
        integral += param.sums[i];
    }

    // free resources 
    delete[] threads; 
    delete[] param.sums;    
    pthread_attr_destroy(&attr);

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_sec = end - start; 
    //std::cout << numThreads << " threads: " << "Result: " << std::setprecision(10) << integral << "; Elapsed time: " << elapsed_sec.count() << "s" << std::endl;
    std::cout << std::setprecision(10) << integral << std::endl;
    
    //printf("At the end of execution, your code should print a single number as the answer. Do not print anything else. Comment this out when submitting assignment.\n");
    // remember to submit graphs and 1-page report!

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
            << "trapezoid" << ","
            << std::to_string(numThreads) << "\n";
    
    csvfile.close();

    pthread_exit(NULL);
}
