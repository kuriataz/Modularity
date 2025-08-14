#include "options.hpp"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
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

  while (std::getline(infile, line)) {
    if (line.empty())
      continue;

    std::istringstream linestream(line);
    std::string prefix;
    linestream >> prefix;

    if (prefix == "k") {
      i32 start, end;
      linestream >> start >> end;
      // std::cout << "Creating clique from " << start << " to " << end
      //           << std::endl;
      for (i32 i = start; i < end; ++i) {
        for (i32 j = i + 1; j < end; ++j) {
          graph.adjacency_list[i].push_back(j);
          graph.adjacency_list[j].push_back(i);
          graph.m++;
        }
      }
      continue;
    }
    if (prefix == "c") {
      i32 start, end;
      linestream >> start >> end;
      // std::cout << "Creating cycle from " << start << " to " << end
      //           << std::endl;
      for (i32 i = start; i < end - 1; ++i) {
        graph.adjacency_list[i].push_back(i + 1);
        graph.adjacency_list[i + 1].push_back(i);
        graph.m++;
      }
      graph.adjacency_list[end - 1].push_back(start);
      graph.adjacency_list[start].push_back(end - 1);
      graph.m++;
      continue;
    }

    if (prefix == "m") {
      i32 from, start, end;
      linestream >> from >> start >> end;
      // std::cout << "Connecting node " << from << " to range " << start << "
      // to "
      //           << end - 1 << std::endl;
      for (i32 to = start; to < end; ++to) {
        graph.adjacency_list[from].push_back(to);
        graph.adjacency_list[to].push_back(from);
        graph.m++;
      }
      continue;
    }

    i32 from, to;
    linestream >> from >> to;
    graph.adjacency_list[from].push_back(to);
    graph.adjacency_list[to].push_back(from);
    graph.m++;
  }

  // remove duplicates in adjacency list
  for (i32 i = 0; i < n; ++i) {
    std::sort(graph.adjacency_list[i].begin(), graph.adjacency_list[i].end());
    graph.adjacency_list[i].erase(std::unique(graph.adjacency_list[i].begin(),
                                              graph.adjacency_list[i].end()),
                                  graph.adjacency_list[i].end());
  }

  return graph;
}

std::vector<std::vector<i32>> parse_partition(const std::string &filename) {
  std::ifstream infile(filename);
  if (!infile.is_open()) {
    throw std::runtime_error("Cannot open file: " + filename);
  }

  std::string line;
  std::vector<std::vector<i32>> subsets;

  while (std::getline(infile, line)) {
    if (line.empty()) {
      continue;
    }

    std::istringstream iss(line);
    std::string type;
    iss >> type;

    if (type == "k" || type == "c") {
      i32 start, end;
      iss >> start >> end;

      std::vector<i32> subset;
      // Dodaj zakres [start, end)
      for (i32 i = start; i < end; ++i) {
        subset.push_back(i);
      }

      // Dodaj ewentualne dodatkowe wierzchołki
      i32 extra;
      while (iss >> extra) {
        subset.push_back(extra);
      }

      if (!subset.empty()) {
        subsets.push_back(subset);
      }
    } else {
      // Linia bez prefiksu: zakładamy że to lista wierzchołków
      std::vector<i32> subset;
      i32 v;
      while (iss >> v) {
        subset.push_back(v);
      }
      if (!subset.empty()) {
        subsets.push_back(subset);
      }
    }
  }

  return subsets;
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