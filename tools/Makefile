PROGRAMS = mod

CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3 -g

COMMON_SRC = core/graph.cpp core/options.cpp
COMMON_HEADERS = core/graph.hpp core/options.hpp core/types.hpp

MOD_SRC = modularity.cpp

all: $(PROGRAMS)

mod: $(COMMON_SRC) $(COMMON_HEADERS) $(MOD_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $(COMMON_SRC) $(MOD_SRC)
clean:
	rm -f $(PROGRAMS) *.o
