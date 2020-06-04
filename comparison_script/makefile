CXX=g++
CXXFLAGS=-g -Wall -MMD -std=c++11
LDLIBS=-lstdc++ -lbrotlienc -lz

all: brotli-test

#brotli-test.o: brotli-test.cc
#    g++ -std=c++11 -c brotli-test.cc

brotli-test: brotli-test.o

clean:
	rm brotli-test.o brotli-test
