#include <math.h>
#include <mpi.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <cstdlib>
#include <cstring>

#include "xtrapulp.h"

#include "comms.h"
#include "dist_graph.h"
#include "pulp_data.h"
#include "pulp_init.h"
#include "pulp_v.h"
#include "pulp_ve.h"
#include "pulp_vec.h"

int procid, nprocs;
int seed;
bool verbose, debug, verify;
float X, Y;

extern "C" int xtrapulp_run(dist_graph_t* g, pulp_part_control_t* ppc,
                            int* parts, int num_parts) {
  mpi_data_t comm;
  pulp_data_t pulp;
  queue_data_t q;
  init_comm_data(&comm);
  init_pulp_data(g, &pulp, num_parts);
  init_queue_data(g, &q);
  if (ppc->do_repart)
    memcpy(pulp.local_parts, parts, g->n_local * sizeof(int32_t));

  xtrapulp(g, ppc, &comm, &pulp, &q);

  memcpy(parts, pulp.local_parts, g->n_local * sizeof(int32_t));
  clear_comm_data(&comm);
  clear_pulp_data(&pulp);
  clear_queue_data(&q);

  return 0;
}

extern "C" int xtrapulp(dist_graph_t* g, pulp_part_control_t* ppc,
                        mpi_data_t* comm, pulp_data_t* pulp, queue_data_t* q) {
  double vert_balance = ppc->vert_balance;        // 1.1
  // double vert_balance_lower = 0.25;
  double edge_balance = ppc->edge_balance;        // 1.1
  double do_label_prop = ppc->do_lp_init;         // false
  double do_nonrandom_init = ppc->do_bfs_init;    // true
  verbose = ppc->verbose_output;                  // false
  debug = false;
  bool do_vert_balance = true;
  bool do_edge_balance = ppc->do_edge_balance;    // false
  bool do_maxcut_balance = ppc->do_maxcut_balance;// false
  bool do_repart = ppc->do_repart;                // false
  bool has_vert_weights = (g->vertex_weights != NULL);  // false
  bool has_edge_weights = (g->edge_weights != NULL);    // false
  int balance_outer_iter = 1;
  int label_prop_iter = 3;
  int vert_outer_iter = 3;
  int vert_balance_iter = 5;
  int vert_refine_iter = 10;
  // int edge_outer_iter = 3;
  // int edge_balance_iter = 5;
  // int edge_refine_iter = 10;
  int num_parts = (int)pulp->num_parts;
  seed = ppc->pulp_seed;

  X = 1.0;
  Y = 0.25;
  // Tighten up allowable exchange for small graphs,
  //  and just use block initialization
  if (g->n / (long unsigned)nprocs < 100) {
    Y = 1.0;
    X = 2.0;
    do_label_prop = false;
    do_nonrandom_init = false;
  }

  double elt, elt2, elt3;
  elt = timer();
  if (do_label_prop && (has_vert_weights || has_edge_weights)) {
    // no
    if (procid == 0 && verbose)
      printf("\tDoing (weighted) lp init stage with %d parts\n", num_parts);
    elt2 = timer();
    pulp_init_label_prop_weighted(g, comm, q, pulp, label_prop_iter);
    elt2 = timer() - elt2;
    if (procid == 0 && verbose) printf("done: %9.6lf(s)\n", elt2);
  } else if (do_label_prop) {
    // no
    if (procid == 0 && verbose)
      printf("\tDoing lp init stage with %d parts\n", num_parts);
    elt2 = timer();
    pulp_init_label_prop(g, comm, q, pulp, label_prop_iter);
    elt2 = timer() - elt2;
    if (procid == 0 && verbose) printf("done: %9.6lf(s)\n", elt2);
  } else if (do_nonrandom_init) {
    // yes
    if (procid == 0 && verbose)
      printf("\tDoing bfs init stage with %d parts\n", num_parts);
    elt2 = timer();
    pulp_init_bfs_max(g, comm, q, pulp);
    elt2 = timer() - elt2;
    if (procid == 0 && verbose) printf("done: %9.6lf(s)\n", elt2);
  } else if (!do_repart) {
    if (procid == 0 && verbose)
      printf("\tDoing block init stage with %d parts\n", num_parts);
    elt2 = timer();
    pulp_init_block(g, comm, q, pulp);
    elt2 = timer() - elt2;
    if (procid == 0 && verbose) printf("done: %9.6lf(s)\n", elt2);
  }

  if (procid == 0 && verbose)
    printf("\tBeginning vertex (and edge) refinement\n");
  for (int boi = 0; boi < balance_outer_iter; ++boi) {
    // balance_outer_iter = 1
    elt2 = timer();
    if (do_vert_balance && (has_vert_weights || has_edge_weights)) {
      if (procid == 0 && verbose)
        printf("\t\tDoing (weighted) vert balance and refinement stage\n");
      elt3 = timer();
      pulp_v_weighted(g, comm, q, pulp, vert_outer_iter, vert_balance_iter,
                      vert_refine_iter, vert_balance, edge_balance);
      elt3 = timer() - elt3;
      if (procid == 0 && verbose) printf("done: %9.6lf(s)\n", elt3);
    } else if (do_vert_balance) {
      if (procid == 0 && verbose)
        printf("\t\tDoing vert balance and refinement stage\n");
      elt3 = timer();
      pulp_v(g, comm, q, pulp, vert_outer_iter, vert_balance_iter,
             vert_refine_iter, vert_balance, edge_balance);
      elt3 = timer() - elt3;
      if (procid == 0 && verbose) printf("done: %9.6lf(s)\n", elt3);
    }

    if (do_edge_balance && !do_maxcut_balance &&
        (has_vert_weights || has_edge_weights)) {
      if (procid == 0 && verbose)
        printf("\t\tDoing (weighted) edge balance and refinement stage\n");
      elt3 = timer();
      pulp_ve_weighted(g, comm, q, pulp, vert_outer_iter, vert_balance_iter,
                       vert_refine_iter, vert_balance, edge_balance);
      elt3 = timer() - elt3;
      if (procid == 0 && verbose) printf("done: %9.6lf(s)\n", elt3);
    } else if (do_edge_balance && !do_maxcut_balance) {
      if (procid == 0 && verbose)
        printf("\t\tDoing edge balance and refinement stage\n");
      elt3 = timer();
      pulp_ve(g, comm, q, pulp, vert_outer_iter, vert_balance_iter,
              vert_refine_iter, vert_balance, edge_balance);
      elt3 = timer() - elt3;
      if (procid == 0 && verbose) printf("done: %9.6lf(s)\n", elt3);
    } else if (do_edge_balance && do_maxcut_balance &&
               (has_vert_weights || has_edge_weights)) {
      if (procid == 0 && verbose)
        printf("\t\tDoing (weighted) maxcut balance and refinement stage\n");
      elt3 = timer();
      pulp_vec_weighted(g, comm, q, pulp, vert_outer_iter, vert_balance_iter,
                        vert_refine_iter, vert_balance, edge_balance);
      elt3 = timer() - elt3;
      if (procid == 0 && verbose) printf("done: %9.6lfs\n", elt3);
    } else if (do_edge_balance && do_maxcut_balance) {
      if (procid == 0 && verbose)
        printf("\t\tDoing maxcut balance and refinement stage\n");
      elt3 = timer();
      pulp_vec(g, comm, q, pulp, vert_outer_iter, vert_balance_iter,
               vert_refine_iter, vert_balance, edge_balance);
      elt3 = timer() - elt3;
      if (procid == 0 && verbose) printf("done: %9.6lfs\n", elt3);
    }

    elt2 = timer() - elt2;
    if (procid == 0 && verbose)
      printf("\tFinished outer loop iter %d: %9.6lf(s)\n", (boi + 1), elt2);
  }
  elt = timer() - elt;
  if (procid == 0 && verbose) printf("Partitioning finished: %9.6lf(s)\n", elt);

  return 0;
}
