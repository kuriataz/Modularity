PROGRAMS = mod gen1 gen2 gen_ch gen_grid bip1 bip2 bip3

CXX = g++
CXXFLAGS = -std=c++17 -Wall -O3 -g

COMMON_SRC = core/graph.cpp core/options.cpp
COMMON_HEADERS = core/graph.hpp core/options.hpp core/types.hpp

MOD_SRC = core/modularity.cpp

LVN_SRC = core/test_louvain.cpp core/louvain.cpp
LVN_HEADERS = core/louvain.hpp

GEN_SRC = gens/partitions.cpp
GEN_HEADERS = gens/partitions.hpp core/types.hpp
GEN1_SRC = gens/gen1.cpp
GEN2_SRC = gens/gen2.cpp
GEN_CH_SRC = gens/gen_chandelier.cpp
GEN_GRID_SRC = gens/gen_grid.cpp

BIP_HEADERS = core/types.hpp
BIP1_SRC = bipartite/bip1.cpp
BIP2_SRC = bipartite/bip2.cpp
BIP3_SRC = bipartite/bip3.cpp

all: $(PROGRAMS)

lvn: $(COMMON_SRC) $(LVN_SRC) $(COMMON_HEADERS) $(LVN_HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(COMMON_SRC) $(LVN_SRC)

mod: $(COMMON_SRC) $(MOD_SRC) $(COMMON_HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(COMMON_SRC) $(MOD_SRC)

gen1: $(GEN_SRC) $(GEN1_SRC) $(GEN_HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(GEN_SRC) $(GEN1_SRC)

gen2: $(GEN_SRC) $(GEN2_SRC) $(GEN_HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(GEN_SRC) $(GEN2_SRC)

gen_ch: $(GEN_SRC) $(GEN_CH_SRC) $(GEN_HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(GEN_SRC) $(GEN_CH_SRC)

gen_grid: $(GEN_SRC) $(GEN_GRID_SRC) $(GEN_HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(GEN_SRC) $(GEN_GRID_SRC)

bip1: $(BIP1_SRC) $(BIP_HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(BIP1_SRC)

bip2: $(BIP2_SRC) $(BIP_HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(BIP2_SRC)

bip3: $(BIP3_SRC) $(BIP_HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(BIP3_SRC)

clean:
	rm -f $(PROGRAMS) *.o

# mod: $(COMMON_SRC) $(COMMON_HEADERS) $(MOD_SRC)
# 	$(CXX) $(CXXFLAGS) -o $@ $(COMMON_SRC) $(MOD_SRC)

# gen1: $(GEN_HEADERS) $(GEN_SRC) $(GEN1_SRC)
# 	$(CXX) $(CXXFLAGS) -o $@ $(GEN_SRC) $(GEN1_SRC)
# gen2: $(GEN_HEADERS) $(GEN_SRC) $(GEN2_SRC)
# 	$(CXX) $(CXXFLAGS) -o $@ $(GEN_SRC) $(GEN2_SRC)

# bip1: $(BIP_HEADERS) $(BIP1_SRC)
# 	$(CXX) $(CXXFLAGS) -o $@ $(BIP1_SRC)
# bip2: $(BIP_HEADERS) $(BIP2_SRC)
# 	$(CXX) $(CXXFLAGS) -o $@ $(BIP2_SRC)
# bip3: $(BIP_HEADERS) $(BIP3_SRC)
# 	$(CXX) $(CXXFLAGS) -o $@ $(BIP3_SRC)

# clean:
# 	rm -f $(PROGRAMS) *.o
