#include "partitions.hpp"
#include <fstream>
#include <iostream>

i32 main(i32 argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <n> <filename>\n";
    return 1;
  }

  i32 n = std::stoi(argv[1]);
  i32 N = n * n;

  std::string filename = argv[2];
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
  outfile << N << "\n";

  // grid
  // 0 1 2
  // 3 4 5
  // 6 7 8
  for (i32 i = 0; i < n; ++i) {
    outfile << "p " << i * n << " " << i * n + n << " " << 1 << "\n";
    outfile << "p " << i << " " << (n - 1) * n + i + 1 << " " << n << "\n";
  }
  // partition K_{1,4}
  pK1_4(outfileP0, n);

  // FSH partition
  p_FSH(outfileP1, n);
  return 0;
}
