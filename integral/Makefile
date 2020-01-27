CC=g++
CFLAGS=-Wall -Wextra -pedantic -std=c++11

all: integral

%.o: %.cpp 
	$(CC) -c -o $@ $< $(CFLAGS)

integral: main.o
	$(CC) main.o $(CFLAGS) -o integral.exe

clean:
	rm *.o *.exe
