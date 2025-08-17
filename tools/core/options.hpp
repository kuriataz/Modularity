#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include "graph.hpp"
#include <cstdio>
#include <string>
#include <unordered_set>
#include <vector>

struct Option_Definition {
  std::string short_name;
  i32 id = 0;
  bool mandatory_argument = false;
};

struct Option {
  i32 id = 0;
  std::string value;

  Option(i32 id) { this->id = id; }

  Option(i32 id, std::string value) {
    this->id = id;
    this->value = value;
  }
};

struct Parse_Result {
  std::vector<Option> options;
  std::vector<std::string> arguments;
};

Parse_Result parse_arguments(i32 argc, char **argv, Option_Definition defs[],
                             i32 size);

Graph parse_graph(const std::string &filename);
std::vector<std::unordered_set<i32>>
parse_partition(const std::string &filename);

void print_graph(const Graph &graph);

#endif
