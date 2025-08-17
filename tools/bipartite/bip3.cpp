/*
Naming:
    - Partition - one part of the bipartite graph. There are no edges within
      partitions, only between them.
    - Cluster - a set of chosen vertices. Clusters are disjoint and cover all
      vertices of the graph. Clusters are used to compute modularity.

Graph structure:
    - Graph obtained from complete bipartite graph by removing some edges
      between clusters.
    - We remove edges between clusters as long as it is possible.
    - Partition 1 contains vertices 0 to p1_size - 1, partition 2 contains
      vertices p1_size to n - 1. p1_size >= p2_size.

Clusters:
    - P: cluster 1 contains ceil(p1_size / 2) "highest" vertices from
      partition 1 and ceil(p2_size / 2) "highest" vertices from partition 2.
      cluster 2 contains the remaining vertices from both partitions.- (same as
      P1 in bip2.cpp).
*/

#include <cmath>
#include <fstream>
#include <iostream>
#include <utility>

int main(int argc, char *argv[]) {
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0]
              << " <p1_size> <p2_size> <edges_to_remove> "
                 "<filename>\n";
    return 1;
  }

  int p1_size = std::stoi(argv[1]);
  int p2_size = std::stoi(argv[2]);
  if (p1_size < p2_size) {
    std::swap(p1_size, p2_size);
  }
  int n = p1_size + p2_size;
  int edges_to_remove = std::stoi(argv[3]);
  if (edges_to_remove < 0 || edges_to_remove > p1_size * p2_size) {
    std::cerr << "Edges to remove must be between 0 and " << p1_size * p2_size
              << ".\n";
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

  int half_p1_size = std::ceil(p1_size / 2.0);
  int half_p2_size = std::ceil(p2_size / 2.0);

  // remove edges between clusters if possible
  for (int i = 0; i < half_p1_size; ++i) {
    if (edges_to_remove <= 0) {
      break;
    }
    for (int j = p1_size + half_p2_size; j < n; ++j) {
      outfile << "x " << i << " " << j << "\n";
      edges_to_remove--;
      if (edges_to_remove <= 0) {
        break;
      }
    }
  }
  for (int i = half_p1_size; i < p1_size; ++i) {
    if (edges_to_remove <= 0) {
      break;
    }
    for (int j = p1_size; j < p1_size + half_p2_size; ++j) {
      outfile << "x " << i << " " << j << "\n";
      edges_to_remove--;
      if (edges_to_remove <= 0) {
        break;
      }
    }
  }
  if (edges_to_remove > 0) {
    std::cerr << "Warning: not enough edges between clusters to remove. "
              << "Left to remove: " << edges_to_remove << ".\n";
  }

  //  cluster1 contains ceil(p1_size / 2) "highest" vertices from partition 1
  //  and ceil(p2_size / 2) "highest" vertices from partition 2.
  outfileP << "n ";
  for (int i = 0; i < half_p1_size; ++i) {
    outfileP << i << " ";
  }
  for (int i = 0; i < half_p2_size; ++i) {
    outfileP << p1_size + i << " ";
  }
  outfileP << "\nn ";
  for (int i = half_p1_size; i < p1_size; ++i) {
    outfileP << i << " ";
  }
  for (int i = half_p2_size; i < p2_size; ++i) {
    outfileP << p1_size + i << " ";
  }
  outfileP << "\n";
  return 0;
}
