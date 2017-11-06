#ifndef _PULP_VE_H_
#define _PULP_VE_H_

#include "comms.h"
#include "dist_graph.h"
#include "pulp_data.h"

int pulp_ve(dist_graph_t* g, mpi_data_t* comm, queue_data_t* q,
            pulp_data_t* pulp, uint64_t edge_outer_iter,
            uint64_t edge_balance_iter, uint64_t edge_refine_iter,
            double vert_balance, double edge_balance);

int pulp_ve_weighted(dist_graph_t* g, mpi_data_t* comm, queue_data_t* q,
                     pulp_data_t* pulp, uint64_t edge_outer_iter,
                     uint64_t edge_balance_iter, uint64_t edge_refine_iter,
                     double vert_balance, double edge_balance);
#endif
