#include "options.hpp"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string.h>
#include <string>
#include <vector>

Parse_Result parse_arguments(i32 argc, char **argv, Option_Definition defs[],
                             i32 size) {
  Parse_Result result;
  if (argc < 2) {
    return result;
  }
  for (i32 i = 1; i < argc; i++) {
    i32 options_added = 0;
    for (i32 j = 0; j < size; j++) {
      if ((strcmp(argv[i], defs[j].short_name.c_str()) == 0)) {
        if (defs[j].mandatory_argument == true && argc > i + 1) {
          Option new_option = Option(defs[j].id, argv[i + 1]);
          result.options.push_back(new_option);
          i++;
          options_added++;
          break;
        } else {
          Option new_option = Option(defs[j].id);
          result.options.push_back(new_option);
          options_added++;
          break;
        }
      }
    }
    if (options_added == 0) {
      result.arguments.push_back(argv[i]);
    }
  }
  return result;
}

Graph parse_graph(const std::string &filename) {
  std::ifstream infile(filename);
  if (!infile.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  std::string line;
  i32 n = 0;

  std::getline(infile, line);
  std::istringstream iss(line);
  iss >> n;
  Graph graph(n);
  std::map<i32, std::vector<i32>> edges_to_remove;

  while (std::getline(infile, line)) {
    if (line.empty()) {
      continue;
    }

    std::istringstream linestream(line);
    std::string prefix;
    linestream >> prefix;

    // clique
    if (prefix == "k") {
      i32 start, end;
      linestream >> start >> end;
      for (i32 i = start; i < end; ++i) {
        for (i32 j = i + 1; j < end; ++j) {
          graph.adjacency_list[i].push_back(j);
          graph.adjacency_list[j].push_back(i);
          graph.m++;
        }
      }
    } else if (prefix == "c") { // cycle
      i32 start, end;
      linestream >> start >> end;
      for (i32 i = start; i < end - 1; ++i) {
        graph.adjacency_list[i].push_back(i + 1);
        graph.adjacency_list[i + 1].push_back(i);
        graph.m++;
      }
      graph.adjacency_list[end - 1].push_back(start);
      graph.adjacency_list[start].push_back(end - 1);
      graph.m++;
    } else if (prefix == "p") { // path
      i32 start, end, step;
      linestream >> start >> end >> step;
      for (i32 i = start; i < end - 1; i += step) {
        graph.adjacency_list[i].push_back(i + step);
        graph.adjacency_list[i + step].push_back(i);
        graph.m++;
      }
    } else if (prefix == "m") { // multiple edges from one vertex
      i32 from, start, end;
      linestream >> from >> start >> end;
      for (i32 to = start; to < end; ++to) {
        graph.adjacency_list[from].push_back(to);
        graph.adjacency_list[to].push_back(from);
        graph.m++;
      }
    } else if (prefix == "x") { // edge removal
      i32 from, to;
      linestream >> from >> to;
      if (from < 0 || from >= n || to < 0 || to >= n) {
        throw std::runtime_error("Invalid edge to remove: " + line);
      }
      edges_to_remove[from].push_back(to);
    } else {
      i32 from, to;
      linestream >> from >> to;
      graph.adjacency_list[from].push_back(to);
      graph.adjacency_list[to].push_back(from);
      graph.m++;
    }
  }

  // remove duplicates in adjacency list
  for (i32 i = 0; i < n; ++i) {
    std::sort(graph.adjacency_list[i].begin(), graph.adjacency_list[i].end());
    graph.adjacency_list[i].erase(std::unique(graph.adjacency_list[i].begin(),
                                              graph.adjacency_list[i].end()),
                                  graph.adjacency_list[i].end());
  }

  // remove edges that were marked for removal
  for (const auto &pair : edges_to_remove) {
    i32 from = pair.first;
    for (const auto &to : pair.second) {
      auto &neighbors = graph.adjacency_list[from];
      auto it = std::remove(neighbors.begin(), neighbors.end(), to);
      if (it != neighbors.end()) {
        neighbors.erase(it, neighbors.end());
        graph.m--;
      } else {
        std::cerr << "Warning: edge from " << from << " to " << to
                  << " not found in the graph.\n";
      }
      // also remove the reverse edge
      auto &to_neighbors = graph.adjacency_list[to];
      it = std::remove(to_neighbors.begin(), to_neighbors.end(), from);
      if (it != to_neighbors.end()) {
        to_neighbors.erase(it, to_neighbors.end());
        graph.m--;
      } else {
        std::cerr << "Warning: edge from " << to << " to " << from
                  << " not found in the graph.\n";
      }
    }
  }

  return graph;
}

std::vector<std::unordered_set<i32>>
parse_partition(const std::string &filename) {
  std::ifstream infile(filename);
  if (!infile.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  std::string line;
  std::vector<std::unordered_set<i32>> subsets;

  while (std::getline(infile, line)) {
    if (line.empty()) {
      continue;
    }

    std::istringstream iss(line);
    std::string type;
    iss >> type;
    std::unordered_set<i32> subset;

    if (type == "k") {
      i32 start, end;
      iss >> start >> end;

      // subset from range [start, end)
      for (i32 i = start; i < end; ++i) {
        subset.insert(i);
      }

      // extra vertices beyond the range
      i32 extra;
      while (iss >> extra) {
        subset.insert(extra);
      }

      if (!subset.empty()) {
        subsets.push_back(subset);
      }
    } else if (type == "n") {
      // list of vertices for one subset
      i32 v;
      while (iss >> v) {
        subset.insert(v);
      }
      if (!subset.empty()) {
        subsets.push_back(subset);
      }
    } else {
      std::cerr << "Warning: unknown subset type: " << type << "\n";
    }
  }

  return subsets;
}

std::string reparse_partition(const Partition &partition) {
  std::ostringstream oss;
  for (const auto &cluster : partition.clusters) {
    if (cluster.nodes.empty()) {
      continue; // Skip empty clusters
    }
    oss << "n";
    int count = 0;
    for (const auto &node : cluster.nodes) {
      oss << " " << node;
      count++;
    }
    oss << " " << count << " \n";
  }
  return oss.str();
}

void print_graph(const Graph &graph) {
  std::cout << "Graph with " << graph.n << " vertices and " << graph.m
            << " edges:\n";
  for (i32 i = 0; i < graph.n; ++i) {
    std::cout << i << ": ";
    for (const auto &neighbor : graph.adjacency_list[i]) {
      std::cout << neighbor << " ";
    }
    std::cout << "\n";
  }
}
