# Declare Variables
CC = g++
OP = LoopSubdivision
CFLAGS = -c

all: main.o Cartesian3.o diredge.o io.o
	$(CC) main.o Cartesian3.o diredge.o io.o -o $(OP)

main.o: LoopSubdivision.cpp io.o diredge.o Cartesian3.o
	$(CC) $(CFLAGS) -std=c++11  LoopSubdivision.cpp

Cartesian3.o: Cartesian3.cpp
	$(CC) $(CFLAGS) -std=c++11 Cartesian3.cpp Cartesian3.h

diredge.o:
	$(CC) $(CFLAGS) -std=c++11 diredge.cpp diredge.h

io.o: utils.o
	$(CC) $(CFLAGS) -std=c++11 io.cpp io.h

# clean options
clean:
	rm -rf *o $(OP)