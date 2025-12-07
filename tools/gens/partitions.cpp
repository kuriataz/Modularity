#include "partitions.hpp"
#include <iostream>

void p0(std::ofstream &outfile, i32 num_swarms, i32 swarm_size, i32 path_size,
        i32 num_single) {
  i32 path_start = num_swarms * swarm_size;
  // clusters for swarms
  for (i32 i = 0; i < num_swarms; ++i) {
    outfile << "k " << i * swarm_size << " " << (i + 1) * swarm_size << "\n";
  }
  // clusters for path and single nodes
  outfile << "k " << path_start << " " << (path_start + path_size + num_single)
          << "\n";
}

void p1(std::ofstream &outfile, i32 num_swarms, i32 swarm_size, i32 path_size,
        i32 path_frac, i32 num_single) {
  i32 path_start = num_swarms * swarm_size;
  i32 put_single = 0;
  i32 single_per_cluster = num_single / (path_size / path_frac);

  // cluster for swarms with additional vertices from path and single nodes
  for (i32 i = 0; i < num_swarms; ++i) {
    outfile << "k " << i * swarm_size << " " << (i + 1) * swarm_size;
    for (i32 j = 0; j < path_frac && path_frac * i + j < path_size; ++j) {
      outfile << " " << (path_start + path_frac * i + j);
    }
    for (; put_single < single_per_cluster * (i + 1) && put_single < num_single;
         ++put_single) {
      outfile << " " << (path_start + path_size + put_single);
    }
    outfile << "\n";
  }
}

void p2(std::ofstream &outfile, i32 num_swarms, i32 swarm_size, i32 path_size,
        i32 path_frac, i32 swarms_in_cluster, i32 num_single) {
  i32 path_start = num_swarms * swarm_size;
  i32 put_single = 0;
  i32 clusters_with_path = std::ceil(path_size / path_frac);
  i32 single_per_cluster = num_single / clusters_with_path;

  // clusters for few swarms and vertices from path
  for (i32 i = 0; i < clusters_with_path; ++i) {
    outfile << "k " << i * swarms_in_cluster * swarm_size << " "
            << (i + 1) * swarms_in_cluster * swarm_size;
    for (i32 j = 0; j < path_frac && path_frac * i + j < path_size; ++j) {
      outfile << " " << (path_start + path_frac * i + j);
    }
    for (; put_single < single_per_cluster * (i + 1) && put_single < num_single;
         ++put_single) {
      outfile << " " << (path_start + path_size + put_single);
    }
    outfile << "\n";
  }
  // clusters for swarms without vertices from path
  for (i32 i = clusters_with_path * swarms_in_cluster; i < num_swarms; ++i) {
    outfile << "k " << i * swarm_size << " " << (i + 1) * swarm_size << "\n";
  }
}

void p3(std::ofstream &outfile, i32 num_swarms, i32 swarm_size, i32 path_size,
        i32 path_frac, i32 num_connectors, i32 num_single) {
  i32 path_start = num_swarms * swarm_size;
  i32 put_single = 0;
  i32 clusters_with_path = std::ceil(path_size / path_frac);
  i32 single_per_cluster = num_single / clusters_with_path;

  // clusters for swarms without connectors
  for (i32 i = 0; i < num_connectors * clusters_with_path; ++i) {
    outfile << "k " << i * swarm_size + 2 << " " << (i + 1) * swarm_size << " "
            << i * swarm_size << "\n";
  }
  // clusters for whole swarms
  for (i32 i = num_connectors * clusters_with_path; i < num_swarms; ++i) {
    outfile << "k " << i * swarm_size << " " << (i + 1) * swarm_size << "\n";
  }
  // clusters for the path's fractions and connectors
  for (i32 i = 0; i < clusters_with_path; ++i) {
    outfile << "k " << path_start + path_frac * i << " "
            << std::min(path_start + path_frac * (i + 1),
                        path_start + path_size);
    for (i32 j = num_connectors * i; j < num_connectors * (i + 1); ++j) {
      outfile << " " << (j * swarm_size) + 1;
    }
    for (; put_single < single_per_cluster * (i + 1) && put_single < num_single;
         ++put_single) {
      outfile << " " << (path_start + path_size + put_single);
    }
    outfile << "\n";
  }
}
