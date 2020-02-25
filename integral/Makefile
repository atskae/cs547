CC=g++
CFLAGS=-Wall -Wextra -pedantic -std=c++11 -lpthread -O
//CFLAGS=-Wall -Wextra -pedantic -std=c++11 -lpthread

all: integrate bothMethods

%.o: %.cpp 
	$(CC) -c -o $@ $< $(CFLAGS)

integrate: main.o
	$(CC) main.o $(CFLAGS) -o integrate

bothMethods: bothMethods.o
	$(CC) bothMethods.o $(CFLAGS) -o bothMethods

clean:
	rm *.o integrate bothMethods
