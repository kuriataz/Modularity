#include "graph.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
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

double partition_modularity(const Graph &graph, const Partition &partition) {
  double total_modularity = 0;
  for (const auto &cluster : partition.clusters) {
    total_modularity += subset_modularity(graph, cluster.nodes);
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

double partition_modularity_star2(
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

    double star_factor =
        subset_size != 1 ? (max_degree) / (subset_size - 1) : 1;
    double subset_modularity_value = subset_modularity(graph, subset);
    total_modularity += (subset_modularity_value * star_factor);
  }

  return total_modularity;
}

Mods partition_mods(const Graph &graph,
                    const std::vector<std::unordered_set<i32>> &partition) {
  Mods mods;
  int i = 0;
  double sum = 0;
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
      // if (i == 0 || i == partition.size() - 1) {
      //   std::cout << v << " ";
      // }
    }

    double star_factor1 = (max_degree + 1.0) / subset_size;
    double star_factor2 =
        subset_size != 1 ? (max_degree) / (subset_size - 1) : 1;
    double subset_mod = subset_modularity(graph, subset);
    if (subset_mod < 0) {
      std::cout << "subset mod < 0:";
      for (const auto &v : subset) {
        std::cout << " " << v;
      }
      std::cout << "\n";
    }
    mods.modularity += subset_mod;
    // if (i == 0 || i == partition.size() - 1) {
    //   sum += subset_mod;
    //   std::cout << " -> " << std::fixed << std::setprecision(20) <<
    //   subset_mod
    //             << "\n";
    // }
    mods.star1 += (subset_mod * star_factor1);
    mods.star2 += (subset_mod * star_factor2);
    ++i;
  }
  // std::cout << "SUM: " << sum << "\n";

  return mods;
}
