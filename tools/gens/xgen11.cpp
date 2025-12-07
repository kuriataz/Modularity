/*
Graph structure:
    - Graph consists of stars with additional edges that create Hamiltonian
      cycles and paths (Hs).
    - Every vertex in every path is connected to one chosen (the same for all
      and not the central one) vertex (connector) in every star. Every path has
      its own vertex in every star.

Partitions:
    - P: every star is a cluster, every path is a cluster.
    - PP: every star is a cluster, all paths are in one cluster.
    - P1: every star is a cluster and vertices from the paths are added to the
      clusters of stars. Stars' clusters get PATH_FRAC additional vertices from
      the paths until all paths are covered.
*/

#include <fstream>
#include <iostream>

#define PATH_FRAC 2

int main(int argc, char *argv[]) {
  if (argc < 6) {
    std::cerr << "Usage: " << argv[0]
              << " <number of stars> <star size> <stars_in_smaller_cluster> "
                 "<stars_with_special> <filename>\n";
    return 1;
  }

  int num_stars = std::stoi(argv[1]);
  int star_size = std::stoi(argv[2]);
  int stars_in_smaller_cluster = std::stoi(argv[3]);
  int stars_with_special = std::stoi(argv[4]);

  std::string filename = argv[5];
  std::string filenameP1 = filename + "P1.txt";
  std::string filenameP2 = filename + "P2.txt";
  std::string filenameP3 = filename + "P3.txt";
  std::string filenameP4 = filename + "P4.txt";
  std::string filenameP5 = filename + "P5.txt";
  filename += ".txt";

  std::ofstream outfile(filename);
  std::ofstream outfileP1(filenameP1);
  std::ofstream outfileP2(filenameP2);
  std::ofstream outfileP3(filenameP3);
  std::ofstream outfileP4(filenameP4);
  std::ofstream outfileP5(filenameP5);
  if (!outfile.is_open() || !outfileP1.is_open() || !outfileP2.is_open() ||
      !outfileP3.is_open() || !outfileP4.is_open()) {
    std::cerr << "Error opening output files.\n";
    return 1;
  }
  // number of vertices
  outfile << star_size * num_stars << "\n";

  int num_special = 2;
  // stars
  for (int i = 0; i < num_stars; ++i) {
    outfile << "m " << i * star_size << " " << (i * star_size) + 1 << " "
            << (i + 1) * star_size << "\n";
    // Hamiltonian cycle is added to every star
    outfile << "c " << i * star_size << " " << (i + 1) * star_size << "\n";
  }

  // special edges for the first num_special stars
  for (int k = 0; k < num_special; ++k) {
    for (int i = num_special; i < num_stars; ++i) {
      for (int j = 0; j < star_size; ++j) {
        outfile << "n " << (i * star_size) + j << " " << (star_size * k) - j - 1
                << "\n";
      }
    }
  }

  // own cluster for every star
  for (int i = 0; i < num_stars; ++i) {
    outfileP1 << "k " << i * star_size << " " << (i + 1) * star_size << "\n";
  }

  // smaller cluster
  outfileP2 << "k " << 0 << " " << num_special * star_size << "\n";

  // clusters for the rest of stars
  outfileP2 << "k " << stars_in_smaller_cluster * star_size << " "
            << num_stars * star_size << "\n";

  // smaller cluster with special star
  outfileP3 << "k " << 0 << " " << stars_in_smaller_cluster * star_size;
  for (int i = star_size * (num_stars - 1); i < star_size * num_stars; ++i) {
    outfileP3 << " " << i;
  }
  outfileP3 << "\n";
  // bigger cluster
  outfileP3 << "k " << stars_in_smaller_cluster * star_size << " "
            << (num_stars - 1) * star_size << "\n";

  // cluster for special plus some stars
  outfileP4 << "k " << stars_in_smaller_cluster * star_size << " "
            << (stars_in_smaller_cluster + stars_with_special) * star_size;
  for (int i = star_size * (num_stars - 1); i < star_size * num_stars; ++i) {
    outfileP4 << " " << i;
  }
  outfileP4 << "\n";
  // own clusters for the rest of stars
  for (int i = stars_in_smaller_cluster + stars_with_special; i < num_stars - 1;
       ++i) {
    outfileP4 << "k " << i * star_size << " " << (i + 1) * star_size << "\n";
  }

  // cluster for all non-special stars
  outfileP5 << "k " << 0 << " " << (num_stars - num_special) * star_size
            << "\n";

  // cluster for special stars
  outfileP5 << "k " << (num_stars - num_special) * star_size << " "
            << num_stars * star_size << "\n";

  return 0;
}
