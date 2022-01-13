CC = g++
CFLAGS = -g -Wall -std=c++11
OPTM = -O3
CPPS = seg.cpp util.cpp buildgraph.cpp buildsynteny.cpp main.cpp
OBJS = seg.o util.o buildgraph.o buildsynteny.o main.o
TARGET = sherlog

$(TARGET): $(OBJS)
	$(CC) $(OPTM) $(CFLAGS) -o $@ $(OBJS)	

util.o : util.cpp
	$(CC) $(OPTM) $(CFLAGS) -c util.cpp
	
seg.o : seg.cpp
	$(CC) $(OPTM) $(CFLAGS) -c seg.cpp

buildgraph.o : buildgraph.cpp
	$(CC) $(OPTM) $(CFLAGS) -c buildgraph.cpp

buildsynteny.o : buildsynteny.cpp
	$(CC) $(OPTM) $(CFLAGS) -c buildsynteny.cpp

main.o : main.cpp
	$(CC) $(OPTM) $(CFLAGS) -c main.cpp

clean:
	rm -f *.o $(TARGET)
