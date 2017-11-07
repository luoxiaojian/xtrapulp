#ifndef _PULP_DATA_H
#define _PULP_DATA_H

#include "xtrapulp.h"

struct pulp_data_t {
  double avg_size;                // n / num_parts
  double avg_edge_size;           // m * 2 / num_parts
  double avg_cut_size;
  double max_v;
  double max_e;
  double max_c;
  double running_max_v;
  double running_max_e;
  double running_max_c;
  double weight_exponent_e;
  double weight_exponent_c;

  int32_t num_parts;              // number of partitions
  int32_t* local_parts;           // [0:n_total-1]

  int64_t cut_size;
  int64_t cut_size_change;
  int64_t max_cut;
  int64_t* part_sizes;            // [0:num_parts-1]
  int64_t* part_edge_sizes;       // [0:num_parts-1]
  int64_t* part_cut_sizes;        // [0:num_parts-1]
  int64_t* part_size_changes;     // [0:num_parts-1]
  int64_t* part_edge_size_changes;// [0:num_parts-1]
  int64_t* part_cut_size_changes; // [0:num_parts-1]
};

struct thread_pulp_t {
  double* part_counts;
  double* part_weights;
  double* part_edge_weights;
  double* part_cut_weights;
};

void init_thread_pulp(thread_pulp_t* tp, pulp_data_t* pulp);

void clear_thread_pulp(thread_pulp_t* tp);

void init_pulp_data(dist_graph_t* g, pulp_data_t* pulp, int32_t num_parts);

void update_pulp_data(dist_graph_t* g, pulp_data_t* pulp);

void update_pulp_data_weighted(dist_graph_t* g, pulp_data_t* pulp);

void clear_pulp_data(pulp_data_t* pulp);

#endif
