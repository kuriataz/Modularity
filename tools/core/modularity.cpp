#include "graph.hpp"
#include "options.hpp"
#include "types.hpp"
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

i32 main(i32 argc, char **argv) {
  constexpr i32 O_INPUT_FILE = 1;
  constexpr i32 O_PARTITION_FILE = 2;
  constexpr i32 O_HELP = 3;

  Option_Definition option_defs[] = {
      Option_Definition{"-f", O_INPUT_FILE, true},
      Option_Definition{"-p", O_PARTITION_FILE, true},
      Option_Definition{"-h", O_HELP, false}};

  Parse_Result result = parse_arguments(argc, argv, option_defs, 3);

  std::string input_file;
  std::string partition_file;

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
      case O_HELP: {
        std::cout << "Usage: modularity -f <input_file> -p <partition_file>\n";
        return 0;
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
    std::cerr << "Input file is required\n";
    return 1;
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

  std::cout << "Modularity:      " << std::fixed << std::setprecision(20)
            << modularity << std::endl;

  std::cout << "Modularity Star: " << std::fixed << std::setprecision(20)
            << modularity_star << std::endl;

  // print_graph(graph);

  return 0;
}
