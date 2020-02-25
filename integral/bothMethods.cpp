#include <iostream>
#include <string> // stof, stoi
#include <random>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <fstream>
#include <cmath>

#include <stdio.h>
#include <math.h>
#include <assert.h>

// function to integrate: sin(x)/x

// integration method
#define MC // monte carlo
//#define TZ // trapezoid

#ifdef MC
    struct param_t {
        const double a; // lower limit
        const double b; // upper limit
        const int numSamples;
        double* sums; // use thread id to index into this array
    
        param_t(double a, double b, int n): a(a), b(b), numSamples(n), sums(NULL) {}
    };
#endif

#ifdef TZ
    struct param_t {
        double a; // lower limit
        double range;
        double h; // height of each trapezoid sample
        int numSamples;
        double* sums; // use thread id to index into this array
    
        param_t(): a(0.0), range(0.0), h(0.0), numSamples(0), sums(NULL) {}
    };
#endif

struct args_t {
    int id;
    param_t* param; // parameters used to compute the integral
};

// monte carlo
#ifdef MC
    void* integrate(void* args) {
       
        int id = ((args_t*)args)->id;
        param_t* param = ((args_t*)args)->param;
        //printf("Thread %i in monte carlo; numSamples=%i\n", id, param->numSamples);
        
        std::default_random_engine randEngine;
        randEngine.seed(id);
        std::uniform_real_distribution<double> randGen(param->a, param->b);
    
        //param->sums[id] = 0.0;
        double s = 0.0;
        for(int i=0; i<param->numSamples; i++) {
            double x = randGen(randEngine);
    		double result = sin(x)/x;
            //if(!isnan(result)) param->sums[id] += result;
            if(!isnan(result)) s += result;
        }
        param->sums[id] = s;
        //printf("Thread %i: sum=%.2f\n", id, param->sums[id]); 
        
        delete (args_t*) args;
        pthread_exit(NULL);
    }
#endif

// trapezoid
#ifdef TZ
    void* integrate(void* args) {
    
        int id = ((args_t*)args)->id;
        param_t* param = ((args_t*)args)->param;
        //printf("Thread %i in trapezoid ; numSamples=%i, range=%.2f, h=%.2f\n", id, param->numSamples, param->range, param->h);
        
        //param->sums[id] = 0.0;
        double s = 0.0;
        double h = param->h;
        double lowerBound = param->a + id*param->range;
        for(int i=0; i<param->numSamples; i++) {
            double x1 = lowerBound + h*i;
            double x2 = lowerBound + h*(i+1);
 
            double b1 = sin(x1)/x1;
            double b2 = sin(x2)/x2;
            double area = h * ((b1+b2)/2);
            //printf("%i: x1=%.2f, x2=%.2f, area=%.5f\n", id, x1, x2, area);
            if(!isnan(area)) s += area;
        }
        //printf("Thread %i: sum=%.2f\n", id, param->sums[id]); 
        param->sums[id] = s;       
 
        delete (args_t*) args;
        pthread_exit(NULL);
    
    }
#endif

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

    #ifdef MC    
        param_t param(a, b, numSamples/numThreads);
        //param.a = a;
        //param.b = b;
        //param.numSamples = numSamples/numThreads; // number of samples PER thread
        param.sums = new double[numThreads];
    #endif

    #ifdef TZ
        param_t param;
        param.a = a; // start
        param.range = abs(b-a)/numThreads; // start bound = a + id*range
        param.numSamples = numSamples/numThreads; // number of samples PER thread
        param.h = param.range/param.numSamples; // height; also step
        param.sums = new double[numThreads];
    #endif
 
    int err;
    for(int i=0; i<numThreads; i++) {
        args_t* args = new args_t;
        args->id = i;
        args->param = &param;
        err = pthread_create(&threads[i], &attr, integrate, (void*) args);
        assert(!err);
    }
  
    // wait for all threads to complete
    void* status;
    double sum = 0.0;
    for(int i=0; i<numThreads; i++) {
        err = pthread_join(threads[i], &status);
        assert(!err);
        sum += param.sums[i];
    }

    // compute integral
    #ifdef MC
        double integral = abs(b-a) * (sum/numSamples);
    #endif

    #ifdef TZ
        double integral = sum;
    #endif

    // free resources 
    delete[] threads; 
    delete[] param.sums;    
    pthread_attr_destroy(&attr);

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_sec = end - start; 
    std::cout << numThreads << " threads: " << "Result: " << integral << "; Elapsed time: " << elapsed_sec.count() << "s" << std::endl;
    //std::cout << integral << std::endl;
    
    //printf("At the end of execution, your code should print a single number as the answer. Do not print anything else. Comment this out when submitting assignment.\n");
    // remember to submit graphs and 1-page report!

    // write to csv file
    std::string csvfile_name = std::to_string((int)abs(a)) + "-" + std::to_string((int)abs(b)) + "-" + std::to_string(numSamples) + "-" + std::to_string(numThreads) + ".csv";
    std::ofstream csvfile;
    csvfile.open(csvfile_name);
    // write csv header
    csvfile << "a,b,numSamples,numThreads,integral,elapsed(sec),method\n";
    
    csvfile << std::to_string((int)a) << "," << std::to_string((int)b) << "," << std::to_string(numSamples) << "," << std::to_string(numThreads)
        << "," << std::to_string(integral) << "," << elapsed_sec.count() << ",";
    #ifdef MC
        csvfile << "monte-carlo";
    #endif
    
    #ifdef TZ
        csvfile << "trapezoid";
    #endif
    csvfile << "\n";
    csvfile.close();

    pthread_exit(NULL);
}
