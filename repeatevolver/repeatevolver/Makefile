target=repeatevolver
CC=gcc
CFLAGS=-Wall -I/usr/local/include -L/usr/local/lib -lgsl -lgslcblas -pthread -lpthread -std=c99

$(target): queue.o evolver.o workflow.o main.o
	$(CC) $(CFLAGS) queue.o evolver.o main.o -o repeatevolver

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

queue.o: queue.c
	$(CC) $(CFLAGS) -c queue.c

evolver.o: evolver.c
	$(CC) $(CFLAGS) -c evolver.c

#workflow.o: workflow.c
#	$(CC) $(CFLAGS) -c workflow.c

clean:
	rm -rf *.o $(target)
