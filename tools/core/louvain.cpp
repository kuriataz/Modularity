#include "louvain.hpp"
#include "graph.hpp"
#include "types.hpp"
#include <algorithm>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

Partition move_nodes(Graph &graph, Partition &partition) {
  double m_double = (double)graph.m;
  bool improvement = true;

  // node -> cluster idx
  std::vector<int> node2cid(graph.n);
  for (size_t i = 0; i < partition.clusters.size(); ++i) {
    for (int node : partition.clusters[i].nodes) {
      node2cid[node] = i;
    }
  }

  while (improvement) {
    improvement = false;

    for (int v = 0; v < graph.n; ++v) {
      // Jeśli węzeł jest izolowany, pomiń
      if (graph.adjacency_list[v].empty()) {
        continue;
      }

      int current_cid = node2cid[v];
      Cluster &cluster_from = partition.clusters[current_cid];

      // cluster idx -> edges from v to that cluster
      std::map<int, double> neighbor_communities;
      std::vector<i32> &v_adjacency = graph.adjacency_list[v];

      // also for v's cluster
      for (int u : v_adjacency) {
        if (u != v) {
          int neighbor_cid = node2cid[u];
          neighbor_communities[neighbor_cid] += 1.0;
        }
      }

      double edges_to_current = neighbor_communities[current_cid];
      double v_deg = (double)v_adjacency.size();

      double delta_remove = -(edges_to_current / m_double) +
                            (v_deg * (2.0 * cluster_from.vol - v_deg)) /
                                (4.0 * m_double * m_double);

      double best_delta = 0;
      int best_cluster_idx = -1;

      for (const auto &entry : neighbor_communities) {
        int target_cid = entry.first;
        double edges_to_target = entry.second;

        if (target_cid == current_cid) {
          continue;
        }

        Cluster &cluster_to = partition.clusters[target_cid];

        double delta_insert = (edges_to_target / m_double) -
                              (v_deg * (2.0 * cluster_to.vol + v_deg)) /
                                  (4.0 * m_double * m_double);

        double total_delta = delta_remove + delta_insert;

        if (total_delta > best_delta) {
          best_delta = total_delta;
          best_cluster_idx = target_cid;
        }
      }

      if (best_cluster_idx != -1 && best_delta > 1e-10) {
        Cluster &best_cluster_to = partition.clusters[best_cluster_idx];

        cluster_from.nodes.erase(v);
        best_cluster_to.nodes.insert(v);

        cluster_from.vol -= (int)v_deg;
        best_cluster_to.vol += (int)v_deg;

        node2cid[v] = best_cluster_idx;
        improvement = true;
      }
    }
  }
  return partition;
}

Graph aggregate_graph(const Graph &graph, const Partition &partition,
                      const std::vector<int> &old_cid_to_new_nid,
                      int new_graph_size) {
  Graph new_graph(new_graph_size);

  std::vector<int> node_to_new_node_id(graph.n, -1);

  for (size_t i = 0; i < partition.clusters.size(); ++i) {
    if (partition.clusters[i].nodes.empty()) {
      continue;
    }

    int new_id = old_cid_to_new_nid[i];
    if (new_id == -1) {
      continue;
    }

    for (int node : partition.clusters[i].nodes) {
      node_to_new_node_id[node] = new_id;
    }
  }

  for (int u = 0; u < graph.n; ++u) {
    int new_u = node_to_new_node_id[u];

    for (int v : graph.adjacency_list[u]) {
      int new_v = node_to_new_node_id[v];
      if (new_v != -1) {
        new_graph.adjacency_list[new_u].push_back(new_v);
      }
    }
  }

  new_graph.compute_edges();
  return new_graph;
}

Partition get_singleton_partition(const Graph &graph) {
  Partition partition;
  partition.clusters.reserve(graph.n);
  for (int i = 0; i < graph.n; ++i) {
    Cluster cluster;
    cluster.nodes.insert(i);
    cluster.vol = graph.adjacency_list[i].size();
    partition.clusters.push_back(cluster);
  }
  return partition;
}

Partition louvain_method(Graph &graph) {
  Graph current_graph = graph;
  Partition current_partition = get_singleton_partition(current_graph);

  // original node -> community idx
  std::vector<int> original_node_to_community(graph.n);
  for (int i = 0; i < graph.n; ++i) {
    original_node_to_community[i] = i;
  }

  while (true) {
    current_partition = move_nodes(current_graph, current_partition);

    // in which cluster is node in current partition
    std::vector<int> curr_graph_node_to_cluster(current_graph.n);
    for (size_t cid = 0; cid < current_partition.clusters.size(); ++cid) {
      for (int node : current_partition.clusters[cid].nodes) {
        curr_graph_node_to_cluster[node] = cid;
      }
    }

    // get new cluster's idx
    for (int i = 0; i < graph.n; ++i) {
      int current_node_rep = original_node_to_community[i];
      int new_community = curr_graph_node_to_cluster[current_node_rep];
      original_node_to_community[i] = new_community;
    }

    int active_clusters = 0;
    std::vector<int> old_cid_to_new_nid(current_partition.clusters.size(), -1);

    for (size_t i = 0; i < current_partition.clusters.size(); ++i) {
      if (!current_partition.clusters[i].nodes.empty()) {
        old_cid_to_new_nid[i] = active_clusters++;
      }
    }

    // finish - aggregation would not change anything
    if (active_clusters == current_graph.n) {
      break;
    }

    for (int i = 0; i < graph.n; ++i) {
      int old_cluster_id = original_node_to_community[i];
      int new_node_id = old_cid_to_new_nid[old_cluster_id];

      original_node_to_community[i] = new_node_id;
    }

    current_graph = aggregate_graph(current_graph, current_partition,
                                    old_cid_to_new_nid, active_clusters);
    current_partition = get_singleton_partition(current_graph);
  }

  Partition final_partition;

  int max_community_id = 0;
  for (int cid : original_node_to_community) {
    if (cid > max_community_id)
      max_community_id = cid;
  }

  final_partition.clusters.resize(max_community_id + 1, Cluster());

  for (int i = 0; i < graph.n; ++i) {
    int cid = original_node_to_community[i];
    final_partition.clusters[cid].nodes.insert(i);
    final_partition.clusters[cid].vol += graph.adjacency_list[i].size();
  }

  return final_partition;
}