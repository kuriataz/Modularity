/*
Graph structure:
    - Graph consists of stars and cycles.
    - Every vertex in every cycle (H) is connected to one  chosen vertex in
      every star.
    - Every cycle has its own vertex in every star.

Partitions:
    - P: every star is a cluster, every cycle is a cluster.
    - P1: every star is a cluster and vertices from cycles are added to clusters
      of stars. Stars' clusters get P1SIZE additional vertices from cycles
      until all cycles are covered.
*/

#include <fstream>
#include <iostream>

#define P1SIZE 2

int main(int argc, char *argv[]) {
  if (argc < 6) {
    std::cerr << "Usage: " << argv[0]
              << " <number of stars> <star size> <number of cycles> "
                 "<cycle_size> <filename>\n";
    return 1;
  }

  int num_stars = std::stoi(argv[1]);
  int star_size = std::stoi(argv[2]);
  int num_cycles = std::stoi(argv[3]);
  int cycle_size = std::stoi(argv[4]);
  int cycle_start = star_size * num_stars;

  std::string filename = argv[5];
  std::string filenameP = filename + "P.txt";
  std::string filenameP1 = filename + "P1.txt";
  filename += ".txt";

  std::ofstream outfile(filename);
  std::ofstream outfileP(filenameP);
  std::ofstream outfileP1(filenameP1);
  if (!outfile.is_open() || !outfileP.is_open() || !outfileP1.is_open()) {
    std::cerr << "Error opening output files.\n";
    return 1;
  }
  // number of vertices
  outfile << cycle_start + cycle_size * num_cycles << "\n";

  // stars
  for (int i = 0; i < num_stars; ++i) {
    outfile << "m " << i * star_size << " " << (i * star_size) + 1 << " "
            << (i + 1) * star_size << "\n";
  }

  // cycles
  for (int i = 0; i < num_cycles; ++i) {
    outfile << "c " << cycle_start + i * cycle_size << " "
            << (cycle_start + (i + 1) * cycle_size) << "\n";
  }

  // edges between stars and cycles
  for (int i = 0; i < num_stars; ++i) {
    for (int j = 0; j < num_cycles; ++j) {
      outfile << "m " << (i * star_size) + j + 1 << " "
              << (cycle_start + j * cycle_size) << " "
              << (cycle_start + (j + 1) * cycle_size) << "\n";
    }
  }

  // clusters for stars
  for (int i = 0; i < num_stars; ++i) {
    outfileP << "k " << i * star_size << " " << (i + 1) * star_size << "\n";
  }
  // clusters for cycles
  for (int i = 0; i < num_cycles; ++i) {
    outfileP << "c " << cycle_start + i * cycle_size << " "
             << (cycle_start + (i + 1) * cycle_size) << "\n";
  }

  // clusters for stars with additional vertices
  for (int i = 0; i < num_stars; ++i) {
    outfileP1 << "k " << i * star_size << " " << (i + 1) * star_size;
    if (P1SIZE * i < cycle_size * num_cycles) {
      for (int j = 0; j < P1SIZE; ++j) {
        if (P1SIZE * i + j < cycle_size * num_cycles) {
          outfileP1 << " " << (cycle_start + P1SIZE * i + j);
        }
      }
    }
    outfileP1 << "\n";
  }

  return 0;
}
