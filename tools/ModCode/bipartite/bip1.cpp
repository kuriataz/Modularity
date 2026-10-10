/*
Graph structure:
    - Complete bipartite graph. Partition 1 contains vertices 0 to p1_size - 1,
      partition 2 contains vertices p1_size to n - 1. p1_size >= p2_size.

Partitions:
    - The only partition is the one that divides the graph horizontally into two
      parts. (We assume that in each partition, vertices lies vertically).
*/

#include "../core/types.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <utility>

i32 main(i32 argc, char *argv[]) {
  if (argc != 5) {
    std::cerr << "Usage: " << argv[0]
              << " <p1_size> <p2_size> <cluster1_size> <filename>\n";
    return 1;
  }

  i32 p1_size = std::stoi(argv[1]);
  i32 p2_size = std::stoi(argv[2]);
  if (p1_size < p2_size) {
    std::swap(p1_size, p2_size);
  }
  i32 n = p1_size + p2_size;
  i32 cluster1_size = std::stoi(argv[3]);
  if (cluster1_size < 1 || cluster1_size > n) {
    std::cerr << "Cluster size must be between 1 and " << n << ".\n";
    return 1;
  }

  std::string filename = argv[4];
  std::string filenameP0 = filename + "P0.txt";
  std::string filenameP1 = filename + "P1.txt";
  filename += ".txt";

  std::ofstream outfile(filename);
  std::ofstream outfileP0(filenameP0);
  std::ofstream outfileP1(filenameP1);
  if (!outfile.is_open() || !outfileP0.is_open() || !outfileP1.is_open()) {
    std::cerr << "Error opening output files.\n";
    return 1;
  }
  // number of vertices
  outfile << n << "\n";

  // edges beetwen partitions
  for (i32 i = 0; i != p1_size; ++i) {
    outfile << "m " << i << " " << p1_size << " " << n << "\n";
  }

  // cluster1 contains cluster1_size "highest"vertices
  i32 p1_counter = 0;
  i32 p2_counter = 0;
  outfileP0 << "n ";
  while (p1_counter + p2_counter < cluster1_size) {
    outfileP0 << p1_counter << " ";
    p1_counter++;

    if ((p2_counter < p2_size) && (p1_counter + p2_counter < cluster1_size)) {
      outfileP0 << p1_size + p2_counter << " ";
      p2_counter++;
    }
  }
  // cluster 2 contains remaining vertices
  outfileP0 << "\nn ";
  while (p1_counter + p2_counter < n) {
    if (p2_counter < p2_size) {
      outfileP0 << p1_counter << " " << p1_size + p2_counter << " ";
      p1_counter++;
      p2_counter++;
    } else if (p1_counter < p1_size) {
      outfileP0 << p1_counter << " ";
      p1_counter++;
    }
  }

  // dumb clustring for P2: first p1_size vertices, then p2_size vertices
  outfileP1 << "k " << 0 << " " << p1_size << "\n";
  outfileP1 << "k " << p1_size << " " << n << "\n";

  return 0;
}
