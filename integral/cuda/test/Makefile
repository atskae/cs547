CC=nvcc
CFLAGS=-std=c++11

all: test

test: test.cu
	$(CC) test.cu $(CFLAGS) -o test

clean:
	rm test
