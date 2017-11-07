#ifndef __XTRAPULP_H__
#define __XTRAPULP_H__

#include <stdint.h>

// typedef int64_t pulp_int;
// typedef double pulp_real;

struct mpi_data_t;
struct pulp_data_t;
struct queue_data_t;
struct fast_map;

typedef struct {
  double vert_balance;
  double edge_balance;

  bool do_lp_init;
  bool do_bfs_init;
  bool do_repart;
  bool do_edge_balance;
  bool do_maxcut_balance;

  bool verbose_output;

  int pulp_seed;
} pulp_part_control_t;

struct dist_graph_t {
  uint64_t n;           // global vertex number, max vertex id + 1
  uint64_t m;           // global edge number 
  uint64_t m_local;     // local edge number, after exchanged

  uint64_t n_local;     // n / nprocs + 1 of n - n_offset
  uint64_t n_offset;    // procid * (n / nprocs + 1)
  uint64_t n_ghost;     // my outer vertices number
  uint64_t n_total;     // my total vertices number

  uint64_t max_degree_vert;
  uint64_t max_degree;

  uint64_t* out_edges;          // same as mine
  uint64_t* out_degree_list;    // offsets of out edges of each vertex
  uint64_t* ghost_degrees;

  int32_t* vertex_weights;
  int32_t* edge_weights;
  uint64_t vertex_weights_sum;

  uint64_t* local_unmap;        // [0:n_local-1], local_unmap[i] = i + n_offset
                                // inner vertices: local id -> global id
  uint64_t* ghost_unmap;        // outer vertices: local id -> global id
  uint64_t* ghost_tasks;        // outer vertices: local id -> frag id
  fast_map* map;
};
#define out_degree(g, n) (g->out_degree_list[n + 1] - g->out_degree_list[n])
#define out_vertices(g, n) &g->out_edges[g->out_degree_list[n]]
#define out_weights(g, n) &g->edge_weights[g->out_degree_list[n]]

extern "C" int xtrapulp_run(dist_graph_t* g, pulp_part_control_t* ppc,
                            int* parts, int num_parts);

extern "C" int xtrapulp(dist_graph_t* g, pulp_part_control_t* ppc,
                        mpi_data_t* comm, pulp_data_t* pulp, queue_data_t* q);

extern "C" int create_xtrapulp_dist_graph(
    dist_graph_t* g, unsigned long n_global, unsigned long m_global,
    unsigned long n_local, unsigned long m_local, unsigned long* local_adjs,
    unsigned long* local_offsets, unsigned long* global_ids,
    unsigned long* vert_dist, int* vertex_weights, int* edge_weights);

double timer();

#endif
