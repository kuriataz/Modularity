#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "types.hpp"
#include <vector>

struct Graph {
  Graph() = default;
  Graph(i32 n) : n(n), adjacency_list(n, std::vector<i32>()) {}

  i32 n = 0; // number of vertices
  i32 m = 0; // number of edges

  std::vector<std::vector<i32>> adjacency_list;

  void compute_edges() {
    m = 0;
    for (const auto &neighbors : adjacency_list) {
      m += neighbors.size();
    }
    m /= 2;
  }
};

Graph create_clique(i32 n);

double subset_modularity(const Graph &graph, const std::vector<i32> &a);
double partition_modularity(const Graph &graph,
                            const std::vector<std::vector<i32>> &partition);
double
partition_modularity_star(const Graph &graph,
                          const std::vector<std::vector<i32>> &partition);

#endif
