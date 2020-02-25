CC=g++
CFLAGS=-Wall -Wextra -pedantic -std=c++11

all: k-nn

%.o: %.cpp 
	$(CC) -c -o $@ $< $(CFLAGS)

k-nn: main.o
	$(CC) main.o $(CFLAGS) -o k-nn

clean:
	rm *.o k-nn
