#ifndef _IO_PP_H_
#define _IO_PP_H_

#include "comms.h"
#include "dist_graph.h"
#include "xtrapulp.h"

int load_graph_edges_32(char *input_filename, graph_gen_data_t *ggi,
                        bool offset_vids);

int load_graph_edges_64(char *input_filename, graph_gen_data_t *ggi,
                        bool offset_vids);

int exchange_edges(graph_gen_data_t *ggi, mpi_data_t *comm);

#endif
