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
  i32 clusters_with_path =
      std::ceil(static_cast<double>(path_size) / path_frac);
  i32 single_per_cluster = num_single / clusters_with_path;
  i32 total_swarms_in_cluster = clusters_with_path * swarms_in_cluster;

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
    if (path_frac == 3 && i == 0) {
      total_swarms_in_cluster = swarms_in_cluster;
      swarms_in_cluster = swarms_in_cluster / 3 + 1;
      total_swarms_in_cluster += swarms_in_cluster;
    }
  }
  // clusters for swarms without vertices from path
  for (i32 i = total_swarms_in_cluster; i < num_swarms; ++i) {
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

void p3(std::ofstream &outfile, i32 num_swarms, i32 swarm_size, i32 path_size,
        i32 path_frac, i32 swarms_in_cluster, i32 cluster_to_rob_size,
        i32 num_connectors, i32 num_single) {
  i32 path_start = num_swarms * swarm_size;
  i32 clusters_with_path = std::ceil(path_size / path_frac);

  // cluster for some free diamonds and num_connectors*clusters_with_path
  // triangles
  outfile << "k " << 0 << " " << swarm_size * cluster_to_rob_size;
  for (int i = 0; i < num_connectors * clusters_with_path; ++i) {
    outfile << " " << swarm_size * cluster_to_rob_size + i * swarm_size;
    outfile << " " << swarm_size * cluster_to_rob_size + 2 + i * swarm_size;
    outfile << " " << swarm_size * cluster_to_rob_size + 3 + i * swarm_size;
  }
  outfile << "\n";
  i32 after_rob = swarm_size * cluster_to_rob_size +
                  num_connectors * clusters_with_path * swarm_size;

  // clusters for few swarms and vertices from path and connectors
  for (i32 i = 0; i < clusters_with_path; ++i) {
    outfile << "k " << after_rob + i * swarms_in_cluster * swarm_size << " "
            << after_rob + (i + 1) * swarms_in_cluster * swarm_size;

    // path vertices
    for (i32 j = 0; j < path_frac && path_frac * i + j < path_size; ++j) {
      outfile << " " << (path_start + path_frac * i + j);
    }

    // connectors assigned to this path cluster
    for (i32 j = 0; j < num_connectors; ++j) {
      i32 connector_id = i * num_connectors + j;

      outfile << " "
              << swarm_size * cluster_to_rob_size + 1 +
                     connector_id * swarm_size;
    }

    outfile << "\n";
  }
  // clusters for whole swarms
  for (i32 i = cluster_to_rob_size + num_connectors * clusters_with_path +
               clusters_with_path * swarms_in_cluster;
       i < num_swarms; ++i) {
    outfile << "k " << i * swarm_size << " " << (i + 1) * swarm_size << "\n";
  }
}

void pK1_4(std::ofstream &outfile, i32 n) {
  std::vector<char> assigned(n * n, 0);

  auto try_add = [&](i32 r, i32 c) {
    if (r >= 0 && r < n && c >= 0 && c < n) {
      i32 v = r * n + c;
      if (!assigned[v]) {
        outfile << " " << v;
        assigned[v] = 1;
      }
    }
  };

  auto write_cluster = [&](i32 r, i32 c) {
    outfile << "n";
    try_add(r, c);     // center
    try_add(r - 1, c); // top
    try_add(r + 1, c); // bottom
    try_add(r, c - 1); // left
    try_add(r, c + 1); // right
    outfile << "\n";
  };

  for (i32 r = 0; r < n; ++r) {
    for (i32 c = 0; c < n; ++c) {
      if ((2 * r + c) % 5 == 4) {
        if (!assigned[r * n + c]) {
          write_cluster(r, c);
        }
      }
    }
  }

  for (i32 c = 0; c < n; ++c) {
    if (!assigned[c]) {
      write_cluster(0, c);
    }
    if (!assigned[(n - 1) * n + c]) {
      write_cluster(n - 1, c);
    }
  }
  for (i32 r = 0; r < n; ++r) {
    if (!assigned[r]) {
      write_cluster(r, 0);
    }
    if (!assigned[r * n + (n - 1)]) {
      write_cluster(r, n - 1);
    }
  }
}

void p_FSH(std::ofstream &outfile, i32 n) {
  std::vector<char> assigned(n * n, 0);

  auto write_cluster = [&](i32 r, i32 c) {
    std::vector<i32> valid_nodes;

    auto try_add = [&](i32 nr, i32 nc) {
      if (nr >= 0 && nr < n && nc >= 0 && nc < n) {
        i32 v = nr * n + nc;
        if (!assigned[v]) {
          valid_nodes.push_back(v);
          assigned[v] = 1;
        }
      }
    };

    try_add(r, c);
    try_add(r - 1, c);     // top
    try_add(r + 1, c);     // bottom
    try_add(r, c - 1);     // left
    try_add(r, c + 1);     // right
    try_add(r - 1, c - 1); // top-left

    // Zapisz klaster tylko, jeśli przynajmniej jeden jego węzeł leży na planszy
    if (!valid_nodes.empty()) {
      outfile << "n";
      for (i32 v : valid_nodes) {
        outfile << " " << v;
      }
      outfile << "\n";
    }
  };

  // Iterujemy z marginesem, aby uwzględnić środki leżące tuż za planszą
  for (i32 r = -1; r <= n; ++r) {
    for (i32 c = -1; c <= n; ++c) {
      // W C++ operator % dla liczb ujemnych może zwrócić wartość ujemną.
      // Dodajemy 6, aby upewnić się, że wynik jest zawsze dodatni.
      i32 mod_val = ((2 * r + c) % 6 + 6) % 6;

      if (mod_val == 2) {
        write_cluster(r, c);
      }
    }
  }
}