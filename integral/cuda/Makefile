CC=nvcc
CFLAGS=-std=c++11

all: integrate

integrate: main.cu
	$(CC) main.cu $(CFLAGS) -o integrate

clean:
	rm integrate
