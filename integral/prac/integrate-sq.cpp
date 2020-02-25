#include <string> // stof, stoi
#include <random>
#include <algorithm>

#include <stdio.h>
#include <math.h>
#include <assert.h>

// function to integrate
float f1(float x) { return sin(x)/x; }

float integrate(float (*f)(float), float a, float b, int numSamples) {
    
    std::default_random_engine randEngine;
    std::uniform_real_distribution<float> randGen(a, b);
    float sum = 0.0;
    for(int i=0; i<numSamples; i++) {
        float x = randGen(randEngine);
        //printf("%.2f\n", x);
        assert(x >= a && x <= b);
        sum = sum + f(x);
    }
    return (b-a) * (sum/numSamples);
}

int main(int argc, char* argv[]) {
    
    if(argc < 5) {
        printf("./integrate lowerLimit, upperLimit, numSamples, numThreads\n");
        exit(1);
    }
   
    float a = std::stof(argv[1]); // lower limit
    float b = std::stof(argv[2]); // upper limit
    int numSamples = std::stoi(argv[3]); 
    int numThreads = std::stoi(argv[4]);
    
    printf("a=%.2f,b=%.2f,numSamples=%i,numThreads=%i\n", a, b, numSamples, numThreads);
    printf("integrate f1=%f\n", integrate(f1, a, b, numSamples/numThreads));
   
    return 0;
}
