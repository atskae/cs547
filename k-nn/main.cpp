#include <errno.h>
#include <sys/stat.h> // stat struct that holds metadata of binary file
#include <fcntl.h> // O_RDONLY
#include <unistd.h> // open(), close()

#include <cstdlib> // atoi()
#include <cstdio> // fprintf()
#include <cassert> // assert()
#include <cstring> // strerror()

int main(int argc, char* argv[]) {
    if(argc < 5) {
        std::fprintf(stderr, "Usage: k-nn n_cores training_file query_file result_file\n");
        exit(1);
    }

    // parse command-line arguments
    int numCores = atoi(argv[1]); // number of cores that have been allocated; can also use sched_getaffinity() to query this value
    char* trainingFile = argv[2]; // training points
    char* queryFile = argv[3]; // query points
    char* resultFile = argv[4]; 
    printf("numCores=%i, trainingFile=%s, queryFile=%s, resultFile=%s\n", numCores, trainingFile, queryFile, resultFile);

    // training file
    int fd = open(trainingFile, O_RDONLY);
    if(fd < 0) {
        int en = errno;
        std::fprintf(stderr, "Couldn't open %s: %s\n", trainingFile, strerror(en));
        exit(2);
    }
    struct stat sb;
    int rv = fstat(fd, &sb); assert(rv == 0);
    rv = close(fd); assert(rv == 0);

    return 0;
}
