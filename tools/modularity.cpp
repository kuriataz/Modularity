#include "core/graph.hpp"
#include "core/options.hpp"
#include "core/types.hpp"
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv) {
  constexpr i32 O_INPUT_FILE = 1;
  constexpr i32 O_OUTPUT_FILE = 2;
  constexpr i32 O_PARTITION_FILE = 3;
  constexpr i32 O_CLIQUE = 4;

  Option_Definition option_defs[] = {
      Option_Definition{"-f", O_INPUT_FILE, true},
      Option_Definition{"-p", O_PARTITION_FILE, true},
      Option_Definition{"-c", O_CLIQUE, true},
      Option_Definition{"-o", O_OUTPUT_FILE, true}};

  Parse_Result result = parse_arguments(argc, argv, option_defs, 4);

  std::string input_file;
  std::string partition_file;
  std::string output_file;
  i32 clique_size = 0;

  if (!(result.options.empty())) {
    for (Option const &option : result.options) {
      switch (option.id) {
      case O_INPUT_FILE: {
        input_file = option.value;
        break;
      }
      case O_PARTITION_FILE: {
        partition_file = option.value;
        break;
      }
      case O_CLIQUE: {
        clique_size = std::stoi(option.value);
        break;
      }
      case O_OUTPUT_FILE: {
        output_file = option.value;
        break;
      }
      default: {
        std::cerr << "Unknown option\n";
        return 1;
      }
      }
    }
  }
  Graph graph;
  if (!input_file.empty()) {
    try {
      graph = parse_graph(input_file);
    } catch (std::exception &e) {
      std::cerr << "parse_graph: " << e.what() << std::endl;
      return 1;
    }
  } else {
    graph = create_clique(clique_size);
  }
  graph.compute_edges();

  if (partition_file.empty()) {
    std::cerr << "Partition file is required\n";
    return 1;
  }
  std::vector<std::unordered_set<i32>> partition;
  try {
    partition = parse_partition(partition_file);
  } catch (std::exception &e) {
    std::cerr << "parse_partition: " << e.what() << std::endl;
    return 1;
  }
  if (partition.empty()) {
    std::cerr << "Partition is empty\n";
    return 1;
  }

  double modularity = partition_modularity(graph, partition);
  double modularity_star = partition_modularity_star(graph, partition);

  // std::cout << "N = " << graph.n << std::endl;
  // std::cout << "M = " << graph.m << std::endl;
  // std::cout << "Number of subsets: " << partition.size() << std::endl;
  // i32 p_size = partition.size();
  // for (i32 i = 0; i < p_size; ++i) {
  //   std::cout << "Subset " << i + 1 << ": ";
  //   for (i32 v : partition[i]) {
  //     std::cout << v << " ";
  //   }
  //   std::cout << std::endl;
  // }

  std::cout << "Modularity:      " << std::fixed << std::setprecision(20)
            << modularity << std::endl;
  std::cout << "Modularity Star: " << std::fixed << std::setprecision(20)
            << modularity_star << std::endl;

  // print_graph(graph);

  return 0;
}
