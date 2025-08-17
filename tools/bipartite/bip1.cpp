/*
Graph structure:
    - Complete bipartite graph. Partition 1 contains vertices 0 to p1_size - 1,
      partition 2 contains vertices p1_size to n - 1. p1_size >= p2_size.

Partitions:
    - The only partition is the one that divides the graph horizontally into two
      parts. (We assume that in each partition, vertices lies vertically).
*/

#include <cmath>
#include <fstream>
#include <iostream>
#include <utility>

int main(int argc, char *argv[]) {
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0]
              << " <p1_size> <p2_size> <cluster1_size> <filename>\n";
    return 1;
  }

  int p1_size = std::stoi(argv[1]);
  int p2_size = std::stoi(argv[2]);
  if (p1_size < p2_size) {
    std::swap(p1_size, p2_size);
  }
  int n = p1_size + p2_size;
  int cluster1_size = std::stoi(argv[3]);
  if (cluster1_size < 1 || cluster1_size > n) {
    std::cerr << "Cluster size must be between 1 and " << n << ".\n";
    return 1;
  }

  std::string filename = argv[4];
  std::string filenameP = filename + "P.txt";
  filename += ".txt";

  std::ofstream outfile(filename);
  std::ofstream outfileP(filenameP);
  if (!outfile.is_open() || !outfileP.is_open()) {
    std::cerr << "Error opening output files.\n";
    return 1;
  }
  // number of vertices
  outfile << n << "\n";

  // edges beetwen partitions
  for (int i = 0; i != p1_size; ++i) {
    outfile << "m " << i << " " << p1_size << " " << n << "\n";
  }

  // cluster1 contains cluster1_size "highest"vertices
  int p1_counter = 0;
  int p2_counter = 0;
  outfileP << "n ";
  while (p1_counter + p2_counter < cluster1_size) {
    outfileP << p1_counter << " ";
    p1_counter++;

    if ((p2_counter < p2_size) && (p1_counter + p2_counter < cluster1_size)) {
      outfileP << p1_size + p2_counter << " ";
      p2_counter++;
    }
  }
  // cluster 2 contains remaining vertices
  outfileP << "\nn ";
  while (p1_counter + p2_counter < n) {
    if (p2_counter < p2_size) {
      outfileP << p1_counter << " " << p1_size + p2_counter << " ";
      p1_counter++;
      p2_counter++;
    } else if (p1_counter < p1_size) {
      outfileP << p1_counter << " ";
      p1_counter++;
    }
  }

  return 0;
}
