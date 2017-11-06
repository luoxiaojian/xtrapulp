#ifndef _GENERATE_H_
#define _GENERATE_H_

#include <stdint.h>

#include "dist_graph.h"

int generate_rand_out_edges(graph_gen_data_t* ggi, uint64_t num_verts,
                            uint64_t edges_per_vert, bool offset_vids);

int generate_rmat_out_edges(graph_gen_data_t* ggi, uint64_t num_verts,
                            uint64_t edges_per_vert, bool offset_vids);

int generate_hd_out_edges(graph_gen_data_t* ggi, uint64_t num_verts,
                          uint64_t edges_per_vert, bool offset_vids);

#endif
