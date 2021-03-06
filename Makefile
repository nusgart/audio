#
CXXFLAGS=-O3 -std=gnu++11
CFLAGS=-O3 -std=gnu11

all: AudioEngine.o test
	$(CXX) -o AudioEngine AudioEngine.o -lopenal

test: AudioInterface.o test.o
	cc -o test AudioInterface.o test.o

clean:
	rm *.o AudioEngine test
