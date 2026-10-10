#ifndef LOUVAIN_HPP
#define LOUVAIN_HPP

#include "graph.hpp"
#include "types.hpp"
#include <vector>

double delta_mod(double m, const std::vector<i32> &v_adjacency, Cluster &from,
                 Cluster &to);

Partition move_nodes(Graph &graph, Partition &partition);
Graph aggregate_graph(const Graph &graph, const Partition &partition);
Partition get_singleton_partition(const Graph &graph);
Partition louvain_method(Graph &graph);

#endif