/*
Graph structure:
    - Graph consists of stars (S) with additional edges that create Hamiltonian
      cycles and a path (H).
    - Every vertex in the path is connected to one chosen (the same for all
      path's vertices and not the central one) vertex (connector) in every star.

Partitions:
    - P0: every star is a cluster, the path is a cluster.
    - P1: every star is a cluster and vertices from the path are added to the
      clusters of stars. Stars' clusters get PATH_FRAC additional vertices from
      the path until all path vertices are covered.
    - P2: stars are grouped in clusters of STARS_IN_CLUSTER and vertices
      from the path are added to these clusters. Clusters get PATH_FRAC
      additional vertices from the path until all path vertices are covered.
    - P3: every star (without the connector) is a cluster, the path and all
      connectors are in one cluster.
*/

#include <fstream>
#include <iostream>

#define PATH_FRAC 1
#define STARS_IN_CLUSTER 1000

int main(int argc, char *argv[]) {
  if (argc < 5) {
    std::cerr << "Usage: " << argv[0]
              << " <number of stars> <star size> <path_size> <filename>\n";
    return 1;
  }

  int num_stars = std::stoi(argv[1]);
  int star_size = std::stoi(argv[2]);
  int path_size = std::stoi(argv[3]);
  int path_start = star_size * num_stars;

  std::string filename = argv[4];
  std::string filenameP0 = filename + "P0.txt";
  std::string filenameP1 = filename + "P1.txt";
  std::string filenameP2 = filename + "P2.txt";
  std::string filenameP3 = filename + "P3.txt";
  std::string filenameP4 = filename + "P4.txt";
  std::string filenameP5 = filename + "P5.txt";
  filename += ".txt";

  std::ofstream outfile(filename);
  std::ofstream outfileP0(filenameP0);
  std::ofstream outfileP1(filenameP1);
  std::ofstream outfileP2(filenameP2);
  std::ofstream outfileP3(filenameP3);
  std::ofstream outfileP4(filenameP4);
  std::ofstream outfileP5(filenameP5);
  if (!outfile.is_open() || !outfileP0.is_open() || !outfileP1.is_open() ||
      !outfileP2.is_open() || !outfileP3.is_open() || !outfileP4.is_open()) {
    std::cerr << "Error opening output files.\n";
    return 1;
  }
  // number of vertices
  outfile << path_start + path_size << "\n";

  // stars
  for (int i = 0; i < num_stars; ++i) {
    outfile << "m " << i * star_size << " " << (i * star_size) + 1 << " "
            << (i + 1) * star_size << "\n";
    // Hamiltonian cycle is added to every star
    outfile << "c " << i * star_size << " " << (i + 1) * star_size << "\n";
  }
  // paths
  for (int j = 0; j < path_size - 1; ++j) {
    outfile << "n " << path_start + j << " " << (path_start + j + 1) << "\n";
  }
  // edges between stars and paths
  for (int i = 0; i < num_stars; ++i) {
    outfile << "m " << (i * star_size) + 1 << " " << path_start << " "
            << (path_start + path_size) << "\n";
  }

  // clusters for stars
  for (int i = 0; i < num_stars; ++i) {
    outfileP0 << "k " << i * star_size << " " << (i + 1) * star_size << "\n";
  }
  // clusters for paths
  outfileP0 << "c " << path_start << " " << (path_start + path_size) << "\n";

  // cluster for stars with additional vertices from path
  for (int i = 0; i < num_stars; ++i) {
    outfileP1 << "k " << i * star_size << " " << (i + 1) * star_size;
    if (PATH_FRAC * i < path_size) {
      for (int j = 0; j < PATH_FRAC; ++j) {
        if (PATH_FRAC * i + j < path_size) {
          outfileP1 << " " << (path_start + PATH_FRAC * i + j);
        }
      }
    }
    outfileP1 << "\n";
  }

  // clusters for few stars and vertices from path
  for (int i = 0; i < num_stars - 1; i = i + STARS_IN_CLUSTER) {
    outfileP2 << "k " << i * star_size << " "
              << (i + STARS_IN_CLUSTER) * star_size;
    if (PATH_FRAC * (i / STARS_IN_CLUSTER) < path_size) {
      for (int j = 0; j < PATH_FRAC; ++j) {
        if (PATH_FRAC * (i / STARS_IN_CLUSTER) + j < path_size) {
          outfileP2 << " "
                    << (path_start + (PATH_FRAC * (i / STARS_IN_CLUSTER)) + j);
        }
      }
    }
    outfileP2 << "\n";
  }

  int fraction = num_stars / 900; // fraction of stars without connectors
  // clusters for stars without connectors
  for (int i = 0; i < fraction; ++i) {
    outfileP3 << "k " << i * star_size + 2 << " " << (i + 1) * star_size << " "
              << i * star_size << "\n";
  }
  for (int i = fraction; i < num_stars; ++i) {
    outfileP3 << "k " << i * star_size << " " << (i + 1) * star_size << "\n";
  }
  // cluster for the paths and connectors
  outfileP3 << "c " << path_start << " " << (path_start + path_size);
  for (int i = 0; i < fraction; ++i) {
    outfileP3 << " " << (i * star_size) + 1;
  }
  outfileP3 << "\n";

  // clusters for few stars and vertices from path
  outfileP4 << "k " << 0 << " " << STARS_IN_CLUSTER * star_size;
  for (int j = 0; j < path_size; ++j) {
    outfileP4 << " " << (path_start + j);
  }
  outfileP4 << "\n";
  for (int i = STARS_IN_CLUSTER; i < num_stars; ++i) {
    outfileP4 << "k " << i * star_size << " " << (i + 1) * star_size << "\n";
  }

  // clusters for few stars and vertices from path
  int big_clusters = 2;
  for (int i = 0; i < big_clusters; ++i) {
    outfileP5 << "k " << i * STARS_IN_CLUSTER * star_size << " "
              << (i + 1) * STARS_IN_CLUSTER * star_size;
    for (int j = i * (path_size / big_clusters);
         j < (i + 1) * (path_size / big_clusters); ++j) {
      outfileP5 << " " << (path_start + j);
    }
    outfileP5 << "\n";
  }
  // outfileP5 << "\n";
  for (int i = STARS_IN_CLUSTER * big_clusters; i < num_stars; ++i) {
    outfileP5 << "k " << i * star_size << " " << (i + 1) * star_size << "\n";
  }

  return 0;
}
