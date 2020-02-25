CC=g++
CFLAGS=-Wall -Wextra -pedantic -std=c++11

all: pthread integrate-sq

%.o: %.cpp 
	$(CC) -c -o $@ $< $(CFLAGS)

pthread: pthread.o
	$(CC) pthread.o $(CFLAGS) -lpthread -o pthread.exe

integrate-sq: integrate-sq.o
	$(CC) integrate-sq.o $(CFLAGS) -o integrate-sq.exe

clean:
	rm *.o *.exe
