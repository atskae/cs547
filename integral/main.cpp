#include <stdio.h>
#include <math.h>
#include <stdlib.h> // srand, rand

// from assignment description
double f1(double x) { return sin(x)/x; }
// double f2(double x) // how to handle infinity?
double f3(double x) { return sin(1/x)/x; }

double f4(double x) { return 4.0/(1+x*x); } // integral from 0 to 1 should approximately be pi
double f5(double x) { return sin(x); }

double integral(double (*f)(double), double a, double b) {
    double sum = 0.0;
    int numSamples = 10000;
    for(int i=0; i<numSamples; i++) {
        float x = (rand() / (float)RAND_MAX * b) + a;
        sum = sum + f(x);
    }
    return sum/numSamples;
}

int main() {
    printf("integral f1=%f\n", integral(f1, 0.0, 2.0));
    printf("integral f3=%f\n", integral(f3, 0.0, 2.0));
    printf("---\n");
    printf("integral f4=%f\n", integral(f4, 0.0, 1.0));
    printf("integral f5=%f\n", integral(f5, 0.0, 3.14));
    
    return 0;
}
