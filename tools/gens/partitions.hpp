#ifndef PARTITIONS_HPP
#define PARTITIONS_HPP

#include "../core/types.hpp"
#include <cmath>
#include <fstream>
#include <utility>
#include <vector>

/**
 * @brief p0 partitioning: clusters for swarms and one cluster for path and
 * single nodes
 * @param outfile output file stream
 * @param num_swarms number of swarms
 * @param swarm_size size of each swarm
 * @param path_size size of the path
 * @param num_single number of single nodes
 */
void p0(std::ofstream &outfile, i32 num_swarms, i32 swarm_size, i32 path_size,
        i32 num_single);

/**
 * @brief p1 partitioning: clusters for swarms with 'path_frac' path nodes and
 * single nodes, the rest of the swarms in their own clusters
 * @param outfile output file stream
 * @param num_swarms number of swarms
 * @param swarm_size size of each swarm
 * @param path_size size of the path
 * @param path_frac number of path nodes per cluster
 * @param num_single number of single nodes
 */
void p1(std::ofstream &outfile, i32 num_swarms, i32 swarm_size, i32 path_size,
        i32 path_frac, i32 num_single);

/**
 * @brief p2 partitioning: clusters for 'swarms_in_cluster' swarms with
 * 'path_frac' path nodes and single nodes, the rest of the swarms in their own
 * clusters
 * @param outfile output file stream
 * @param num_swarms number of swarms
 * @param swarm_size size of each swarm
 * @param path_size size of the path
 * @param path_frac number of path nodes per cluster
 * @param swarms_in_cluster number of swarms per cluster
 * @param num_single number of single nodes
 */
void p2(std::ofstream &outfile, i32 num_swarms, i32 swarm_size, i32 path_size,
        i32 path_frac, i32 swarms_in_cluster, i32 num_single);

/**
 * @brief p3 partitioning: clusters for 'num_connectors' connectors from swarms
 * with 'path_frac' path nodes and single nodes, the rest of the swarms (whole
 * and partial) in their own clusters
 * @param outfile output file stream
 * @param num_swarms number of swarms
 * @param swarm_size size of each swarm
 * @param path_size size of the path
 * @param path_frac number of path nodes per cluster
 * @param num_connectors number of connectors per cluster
 * @param num_single number of single nodes
 */
void p3(std::ofstream &outfile, i32 num_swarms, i32 swarm_size, i32 path_size,
        i32 path_frac, i32 num_connectors, i32 num_single = 0);
void p3(std::ofstream &outfile, i32 num_swarms, i32 swarm_size, i32 path_size,
        i32 path_frac, i32 swarms_in_cluster, i32 cluster_to_rob_size,
        i32 num_connectors, i32 num_single);
/**
 * @brief pK1_4 partitioning (only for grids): the aim is to create as many as
 * possible clusters that look like a bipartitr graph K_{1,4}. the rest of
 * vertices creates own clusters (trash)
 * @param outfile output file stream
 * @param n the size of the grid
 */
void pK1_4(std::ofstream &outfile, i32 n);

void p_FSH(std::ofstream &outfile, i32 n);

#endif // PARTITIONS_HPP
