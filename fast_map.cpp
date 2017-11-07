#include <mpi.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "fast_map.h"
#include "util.h"

extern int procid, nprocs;
extern bool verbose, debug, verify;

void init_map(fast_map* map, uint64_t init_size) {
  // init_size = (m_local + n_local) * 2
  if (debug) {
    printf("Task %d init_map() start\n", procid);
  }

  map->arr = (uint64_t*)malloc(init_size * 2 * sizeof(uint64_t));
  map->unique_keys = (uint64_t*)malloc(init_size * sizeof(uint64_t));
  map->unique_indexes = (uint64_t*)malloc(init_size * sizeof(uint64_t));
  if (map->arr == NULL || map->unique_keys == NULL ||
      map->unique_indexes == NULL)
    throw_err("init_map(), unable to allocate resources\n", procid);

  map->capacity = init_size;
  map->num_unique = 0;
  map->hashing = true;

#pragma omp parallel for
  for (uint64_t i = 0; i < map->capacity; ++i) map->arr[i * 2] = NULL_KEY;

  if (debug) {
    printf("Task %d init_map() success\n", procid);
  }
}

void init_map_nohash(fast_map* map, uint64_t init_size) {
  if (debug) {
    printf("Task %d init_map_nohash() start\n", procid);
  }

  map->arr = (uint64_t*)malloc(init_size * 2 * sizeof(uint64_t));
  map->unique_keys = (uint64_t*)malloc(init_size * sizeof(uint64_t));
  map->unique_indexes = (uint64_t*)malloc(init_size * sizeof(uint64_t));
  if (map->arr == NULL || map->unique_keys == NULL ||
      map->unique_indexes == NULL)
    throw_err("init_map(), unable to allocate resources\n", procid);

  map->capacity = init_size;
  map->num_unique = 0;
  map->hashing = false;

#pragma omp parallel for
  for (uint64_t i = 0; i < map->capacity; ++i) map->arr[i * 2] = NULL_KEY;

  if (debug) {
    printf("Task %d init_map_nohash() success\n", procid);
  }
}

void clear_map(fast_map* map) {
  free(map->arr);
  free(map->unique_keys);
  free(map->unique_indexes);

  map->num_unique = 0;
  map->capacity = 0;
}

void empty_map(fast_map* map) {
  for (uint64_t i = 0; i < map->num_unique; ++i)
    map->arr[map->unique_indexes[i]] = NULL_KEY;

  map->num_unique = 0;
}
