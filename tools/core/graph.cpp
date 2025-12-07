#include "graph.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_set>

double subset_modularity(const Graph &graph, const std::unordered_set<i32> &a) {
  double degree_a = 0;
  for (const auto &v : a) {
    degree_a += graph.adjacency_list[v].size();
  }

  std::unordered_set<i32> a_set(a.begin(), a.end());

  double internal_edges = 0;
  for (const auto &v : a) {
    for (const auto &u : graph.adjacency_list[v]) {
      if (a_set.count(u)) {
        internal_edges++;
      }
    }
  }
  internal_edges /= 2;
  double modularity =
      (internal_edges / graph.m) - std::pow((degree_a / (2 * graph.m)), 2);

  return modularity;
}

double
partition_modularity(const Graph &graph,
                     const std::vector<std::unordered_set<i32>> &partition) {
  double total_modularity = 0;
  for (const auto &subset : partition) {
    total_modularity += subset_modularity(graph, subset);
  }

  return total_modularity;
}

double partition_modularity_star(
    const Graph &graph, const std::vector<std::unordered_set<i32>> &partition) {
  double total_modularity = 0;
  for (const auto &subset : partition) {
    std::unordered_set<i32> subset_set(subset.begin(), subset.end());
    i32 subset_size = subset.size();
    double max_degree = 0;

    for (const auto &v : subset) {
      double degree = 0;
      for (const auto &u : graph.adjacency_list[v]) {
        if (subset_set.count(u) && u != v) {
          degree++;
        }
      }
      max_degree = std::max(max_degree, degree);
    }

    double star_factor = (max_degree + 1.0) / subset_size;
    double subset_modularity_value = subset_modularity(graph, subset);
    total_modularity += (subset_modularity_value * star_factor);
  }

  return total_modularity;
}
