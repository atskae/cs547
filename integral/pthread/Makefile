CC=g++
CFLAGS=-Wall -Wextra -pedantic -std=c++11
LIBS=-lpthread

all: pthread

%.o: %.cpp 
	$(CC) -c -o $@ $< $(CFLAGS)

pthread: main.o
	$(CC) main.o $(CFLAGS) $(LIBS) -o pthread.exe

clean:
	rm *.o *.exe
