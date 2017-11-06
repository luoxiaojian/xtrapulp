#ifndef _PULP_UTIL_H_
#define _PULP_UTIL_H_

#include "dist_graph.h"
#include "pulp_data.h"

int part_eval(dist_graph_t* g, pulp_data_t* pulp);

int part_eval(dist_graph_t* g, int32_t* parts, int32_t num_parts);

int part_eval_weighted(dist_graph_t* g, pulp_data_t* pulp);

int output_parts(const char* filename, dist_graph_t* g, int32_t* parts);

int output_parts(const char* filename, dist_graph_t* g, int32_t* parts,
                 bool offset_vids);

int read_parts(const char* filename, dist_graph_t* g, pulp_data_t* pulp,
               bool offset_vids);

#endif
