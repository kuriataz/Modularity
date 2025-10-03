/*
Graph structure:
    - Graph consists of cliques and cycles.
    - Every vertex in every cycle (H) is connected to one chosen vertex
      (connector) in every clique.

Partitions:
    - P0: every clique is a cluster, the cycles are clusters.
    - P1: every clique is a cluster and vertices from the cycles are added to
      clusters of cliques. Cliques' clusters get PATH_FRAC additional vertices
from the cycles until all cycles are covered.
*/

#include <fstream>
#include <iostream>

#define PATH_FRAC 4

int main(int argc, char *argv[]) {
  if (argc < 6) {
    std::cerr << "Usage: " << argv[0]
              << " <number of cliques> <clique size> <number of cycles> "
                 "<cycle_size> <filename>\n";
    return 1;
  }

  int num_cliques = std::stoi(argv[1]);
  int clique_size = std::stoi(argv[2]);
  int num_cycles = std::stoi(argv[3]);
  int cycle_size = std::stoi(argv[4]);
  int cycle_start = clique_size * num_cliques;

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
  outfile << cycle_start + cycle_size << "\n";

  // cliques
  for (int i = 0; i < num_cliques; ++i) {
    outfile << "k " << i * clique_size << " " << (i + 1) * clique_size << "\n";
  }
  // cycles
  for (int i = 0; i < num_cycles; ++i) {
    outfile << "c " << cycle_start + i * cycle_size << " "
            << (cycle_start + (i + 1) * cycle_size) << "\n";
  }

  // edges between cliques and cycles
  for (int i = 0; i < num_cliques; ++i) {
    for (int j = 0; j < num_cycles; ++j) {
      outfile << "m " << (i * clique_size) + j + 1 << " "
              << (cycle_start + j * cycle_size) << " "
              << (cycle_start + (j + 1) * cycle_size) << "\n";
    }
  }

  // clusters for cliques
  for (int i = 0; i < num_cliques; ++i) {
    outfileP0 << "k " << i * clique_size << " " << (i + 1) * clique_size
              << "\n";
  }
  // clusters for cycles
  for (int i = 0; i < num_cycles; ++i) {
    outfileP0 << "c " << cycle_start + i * cycle_size << " "
              << (cycle_start + (i + 1) * cycle_size) << "\n";
  }

  // clusters for cliques with additional vertices
  for (int i = 0; i < num_cliques; ++i) {
    outfileP1 << "k " << i * clique_size << " " << (i + 1) * clique_size;
    if (PATH_FRAC * i < cycle_size * num_cycles) {
      for (int j = 0; j < PATH_FRAC; ++j) {
        if (PATH_FRAC * i + j < cycle_size * num_cycles) {
          outfileP1 << " " << (cycle_start + PATH_FRAC * i + j);
        }
      }
    }
    outfileP1 << "\n";
  }

  return 0;
}
