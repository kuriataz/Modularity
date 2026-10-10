#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "types.hpp"
#include <unordered_set>
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

struct Cluster {

  int edges = 0;
  double vol = 0;
  std::unordered_set<i32> nodes;

  Cluster() = default;
};

struct Partition {
  double mod_score = 0;

  std::vector<Cluster> clusters;
};

struct Mods {
  double modularity = 0;
  double star1 = 0;
  double star2 = 0;
};

double subset_modularity(const Graph &graph, const std::unordered_set<i32> &a);
double
partition_modularity(const Graph &graph,
                     const std::vector<std::unordered_set<i32>> &partition);
double partition_modularity(const Graph &graph, const Partition &partition);
double partition_modularity_star(
    const Graph &graph, const std::vector<std::unordered_set<i32>> &partition);
double partition_modularity_star2(
    const Graph &graph, const std::vector<std::unordered_set<i32>> &partition);
Mods partition_mods(const Graph &graph,
                    const std::vector<std::unordered_set<i32>> &partition);

#endif
