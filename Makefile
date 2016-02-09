#
CXXFLAGS=-O3 -std=gnu++11

all: AudioEngine.o
	$(CXX) -o AudioEngine AudioEngine.o -lopenal
