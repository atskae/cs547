CC=nvcc
CFLAGS=-std=c++11

all: integrate test

integrate: main.cu
	$(CC) main.cu $(CFLAGS) -o integrate

test: test.cu
	$(CC) test.cu $(CFLAGS) -o test

clean:
	rm integrate test
