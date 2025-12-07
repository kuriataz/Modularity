/**
 * Graph structure:
 *   - Graph consists of 'num_swarms' swarms ('swarm_size'-2 star + one vertex
 *     connected to two star's leaves) and a path (path_size)).
 *   - Every vertex from the path is connected to one vertex (the neighbour of
 *     non-star vertex) in each swarm.
 */

#include "partitions.hpp"
#include <fstream>
#include <iostream>

i32 main(i32 argc, char *argv[]) {
  if (argc != 9) {
    std::cerr << "Usage: " << argv[0]
              << " <num_swarms> <swarm_size> <swarms_in_cluster> "
                 "<path_size> <path_frac> "
                 "<num_connectors> <num_single> <filename>\n";
    return 1;
  }

  i32 num_swarms = std::stoi(argv[1]);
  i32 swarm_size = std::stoi(argv[2]);
  i32 path_start = num_swarms * swarm_size;
  i32 swarms_in_cluster = std::stoi(argv[3]);
  i32 path_size = std::stoi(argv[4]);
  i32 path_frac = std::stoi(argv[5]);
  i32 num_connectors = std::stoi(argv[6]);
  i32 num_single = std::stoi(argv[7]);
  if (path_frac > path_size || path_frac <= 0) {
    std::cerr << "Error: wrong path_frac: " << path_frac << "\n";
    return 1;
  }
  if (swarms_in_cluster > num_swarms || swarms_in_cluster <= 0) {
    std::cerr << "Error: wrong swarms_in_cluster: " << swarms_in_cluster
              << "\n";
    return 1;
  }

  i32 clusters_with_path = path_size / path_frac;

  if (clusters_with_path == 0) {
    std::cerr << "Error: path_frac is too large for the given path_size.\n";
    return 1;
  }

  std::string filename = argv[8];
  std::string filenameP0 = filename + "P0.txt";
  std::string filenameP1 = filename + "P1.txt";
  std::string filenameP2 = filename + "P2.txt";
  std::string filenameP3 = filename + "P3.txt";
  filename += ".txt";

  std::ofstream outfile(filename);
  std::ofstream outfileP0(filenameP0);
  std::ofstream outfileP1(filenameP1);
  std::ofstream outfileP2(filenameP2);
  std::ofstream outfileP3(filenameP3);
  if (!outfile.is_open() || !outfileP0.is_open() || !outfileP1.is_open() ||
      !outfileP2.is_open() || !outfileP3.is_open()) {
    std::cerr << "Error opening output files.\n";
    return 1;
  }

  // number of vertices
  outfile << path_start + path_size + num_single << "\n";

  // swarms
  for (i32 i = 0; i < num_swarms; ++i) {
    outfile << "m " << i * swarm_size << " " << i * swarm_size + 1 << " "
            << (i + 1) * swarm_size - 1 << "\n";
    outfile << "n " << i * swarm_size + 1 << " " << (i + 1) * swarm_size - 1
            << "\n";
    outfile << "n " << (i + 1) * swarm_size - 2 << " "
            << (i + 1) * swarm_size - 1 << "\n";
  }

  // path
  for (i32 j = 0; j < path_size - 1; ++j) {
    outfile << "n " << path_start + j << " " << (path_start + j + 1) << "\n";
  }
  // edges between swarms and path
  for (i32 i = 0; i < num_swarms; ++i) {
    outfile << "m " << (i * swarm_size) + 1 << " " << path_start << " "
            << (path_start + path_size) << "\n";
  }

  p0(outfileP0, num_swarms, swarm_size, path_size, num_single);
  p1(outfileP1, num_swarms, swarm_size, path_size, path_frac, num_single);
  p2(outfileP2, num_swarms, swarm_size, path_size, path_frac, swarms_in_cluster,
     num_single);
  p3(outfileP3, num_swarms, swarm_size, path_size, path_frac, num_connectors,
     num_single);

  return 0;
}
