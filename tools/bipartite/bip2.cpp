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
    - P0: cluster 1 contains min(cluster1_size/2, p2_size)(*) "highest"
      vertices from partition 2 and cluster1_size/2 + 1 or cluster1_size -
      p2_size (depending on (*)) "highest" vertices from partition 1. cluster 2
      contains the remaining vertices from both partitions.
    - P1: cluster 1 contains ceil(p1_size / 2) "highest" vertices from
      partition 1 and ceil(p2_size / 2) "highest" vertices from partition 2.
      cluster 2 contains the remaining vertices from both partitions.
*/

#include "../core/types.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <utility>

i32 main(i32 argc, char *argv[]) {
  if (argc != 6) {
    std::cerr << "Usage: " << argv[0]
              << " <p1_size> <p2_size> <cluster1_size> <edges_to_remove> "
                 "<filename>\n";
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
  i32 edges_to_remove = std::stoi(argv[4]);
  if (edges_to_remove < 0 || edges_to_remove > p1_size * p2_size) {
    std::cerr << "Edges to remove must be between 0 and " << p1_size * p2_size
              << ".\n";
    return 1;
  }

  std::string filename = argv[5];
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

  // Clustering 0
  // cluster1 contains cluster1_size "highest" vertices
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

  // remove edges between clusters if possible
  for (i32 i = 0; i < p1_counter; ++i) {
    if (edges_to_remove <= 0) {
      break;
    }
    for (i32 j = p1_size + p2_counter; j < n; ++j) {
      outfile << "x " << i << " " << j << "\n";
      edges_to_remove--;
      if (edges_to_remove <= 0) {
        break;
      }
    }
  }
  for (i32 i = p1_counter; i < p1_size; ++i) {
    if (edges_to_remove <= 0) {
      break;
    }
    for (i32 j = p1_size; j < p1_size + p2_counter; ++j) {
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

  // Clustering 1
  //  cluster1 contains ceil(p1_size / 2) "highest" vertices from partition 1
  //  and ceil(p2_size / 2) "highest" vertices from partition 2.
  i32 half_p1_size = std::ceil(p1_size / 2.0);
  i32 half_p2_size = std::ceil(p2_size / 2.0);
  outfileP1 << "n ";
  for (i32 i = 0; i < half_p1_size; ++i) {
    outfileP1 << i << " ";
  }
  for (i32 i = 0; i < half_p2_size; ++i) {
    outfileP1 << p1_size + i << " ";
  }
  outfileP1 << "\nn ";
  for (i32 i = half_p1_size; i < p1_size; ++i) {
    outfileP1 << i << " ";
  }
  for (i32 i = half_p2_size; i < p2_size; ++i) {
    outfileP1 << p1_size + i << " ";
  }
  outfileP1 << "\n";
  return 0;
}
