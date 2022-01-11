CC = g++
CFLAGS = -g -Wall
OPTM = -O3
CPPS = seg.cpp util.cpp buildgraph.cpp buildsynteny.cpp main.cpp
OBJS = seg.o util.o buildgraph.o buildsynteny.o main.o
TARGET = sherlog

$(TARGET): $(OBJS)
	$(CC) $(OPTM) $(CFLAGS) -o $@ $(OBJS)	
	
clean:
	rm -f *.o $(TARGET)
