/*
Graph structure:
    - Graph consists of stars and paths (Hs).
    - Every vertex in every path is connected to one chosen (not the central
      one) vertex (connector) in every star. Every path has its own vertex in
      every star.

Partitions:
    - P: every star is a cluster, every path is a cluster.
    - PP: every star is a cluster, all paths are in one cluster.
    - P1: every star is a cluster and vertices from the paths are added to the
      clusters of stars. Stars' clusters get P1SIZE additional vertices from
      the paths until all paths are covered.
*/

#include <fstream>
#include <iostream>

#define P1SIZE 2

int main(int argc, char *argv[]) {
  if (argc < 6) {
    std::cerr << "Usage: " << argv[0]
              << " <number of stars> <star size> <number of paths> "
                 "<path_size> <filename>\n";
    return 1;
  }

  int num_stars = std::stoi(argv[1]);
  int star_size = std::stoi(argv[2]);
  int num_paths = std::stoi(argv[3]);
  int path_size = std::stoi(argv[4]);
  int path_start = star_size * num_stars;

  std::string filename = argv[5];
  std::string filenameP = filename + "P.txt";
  std::string filenamePP = filename + "PP.txt";
  std::string filenameP1 = filename + "P1.txt";
  filename += ".txt";

  std::ofstream outfile(filename);
  std::ofstream outfileP(filenameP);
  std::ofstream outfilePP(filenamePP);
  std::ofstream outfileP1(filenameP1);
  if (!outfile.is_open() || !outfileP.is_open() || !outfileP1.is_open()) {
    std::cerr << "Error opening output files.\n";
    return 1;
  }
  // number of vertices
  outfile << path_start + path_size * num_paths << "\n";

  // stars
  for (int i = 0; i < num_stars; ++i) {
    outfile << "m " << i * star_size << " " << (i * star_size) + 1 << " "
            << (i + 1) * star_size << "\n";
    // Hamiltonian cycle is added to every star
    outfile << "c " << i * star_size << " " << (i + 1) * star_size << "\n";
  }
  // paths
  for (int i = 0; i < num_paths; ++i) {
    for (int j = 0; j < path_size - 1; ++j) {
      outfile << "n " << path_start + i * path_size + j << " "
              << (path_start + i * path_size + j + 1) << "\n";
    }
  }

  // edges between stars and paths
  for (int i = 0; i < num_stars; ++i) {
    for (int j = 0; j < num_paths; ++j) {
      outfile << "m " << (i * star_size) + j + 1 << " "
              << (path_start + j * path_size) << " "
              << (path_start + (j + 1) * path_size) << "\n";
    }
  }

  // clusters for stars
  for (int i = 0; i < num_stars; ++i) {
    outfileP << "k " << i * star_size << " " << (i + 1) * star_size << "\n";
  }
  // clusters for paths
  for (int i = 0; i < num_paths; ++i) {
    outfileP << "c " << path_start + i * path_size << " "
             << (path_start + (i + 1) * path_size) << "\n";
  }

  // clusters for stars
  for (int i = 0; i < num_stars; ++i) {
    outfilePP << "k " << i * star_size << " " << (i + 1) * star_size << "\n";
  }
  // cluster for all paths
  outfilePP << "c " << path_start << " " << (path_start + num_paths * path_size)
            << "\n";

  // cluster for stars with additional vertices
  for (int i = 0; i < num_stars; ++i) {
    outfileP1 << "k " << i * star_size << " " << (i + 1) * star_size;
    if (P1SIZE * i < path_size * num_paths) {
      for (int j = 0; j < P1SIZE; ++j) {
        if (P1SIZE * i + j < path_size * num_paths) {
          outfileP1 << " " << (path_start + P1SIZE * i + j);
        }
      }
    }
    outfileP1 << "\n";
  }

  return 0;
}
