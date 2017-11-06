#ifndef _PULP_INIT_H_
#define _PULP_INIT_H_

#include "comms.h"
#include "pulp_data.h"
#include "xtrapulp.h"

void pulp_init_rand(dist_graph_t* g, mpi_data_t* comm, queue_data_t* q,
                    pulp_data_t* pulp);

void pulp_init_block(dist_graph_t* g, mpi_data_t* comm, queue_data_t* q,
                     pulp_data_t* pulp);

void pulp_init_bfs(dist_graph_t* g, mpi_data_t* comm, queue_data_t* q,
                   pulp_data_t* pulp);

void pulp_init_bfs_max(dist_graph_t* g, mpi_data_t* comm, queue_data_t* q,
                       pulp_data_t* pulp);

void pulp_init_label_prop_weighted(dist_graph_t* g, mpi_data_t* comm,
                                   queue_data_t* q, pulp_data_t* pulp,
                                   uint64_t lp_num_iter);

void pulp_init_label_prop(dist_graph_t* g, mpi_data_t* comm, queue_data_t* q,
                          pulp_data_t* pulp, uint64_t lp_num_iter);

#endif
