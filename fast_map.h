#ifndef _FAST_MAP_H_
#define _FAST_MAP_H_

#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern int procid, nprocs;
extern bool verbose, debug, verify;

#define NULL_KEY 18446744073709551615U

struct fast_map {
  uint64_t* arr;
  uint64_t* unique_keys;
  uint64_t* unique_indexes;
  uint64_t capacity;
  uint64_t num_unique;
  bool hashing;
};

void init_map(fast_map* map, uint64_t init_size);
void init_map_nohash(fast_map* map, uint64_t init_size);
void clear_map(fast_map* map);
void empty_map(fast_map* map);

inline uint64_t mult_hash(fast_map* map, uint64_t key);
inline void set_value(fast_map* map, uint64_t key, uint64_t value);
inline void set_value_uq(fast_map* map, uint64_t key, uint64_t value);
inline uint64_t get_value(fast_map* map, uint64_t key);
inline uint64_t get_max_key(fast_map* map);

inline uint64_t mult_hash(fast_map* map, uint64_t key) {
  if (map->hashing)
    return (key * (uint64_t)2654435761 % map->capacity);
  else
    return key;
}

inline void set_value(fast_map* map, uint64_t key, uint64_t value) {
  uint64_t cur_index = mult_hash(map, key) * 2;
  uint64_t count = 0;
  while (map->arr[cur_index] != key && map->arr[cur_index] != NULL_KEY) {
    cur_index = (cur_index + 2) % (map->capacity * 2);
    ++count;
    if (debug && count % 100 == 0)
      fprintf(stderr,
              "Warning: fast_map set_value(): Big Count %d -- %lu - %lu, %lu, "
              "%lu\n",
              procid, count, cur_index, key, value);
  }
  if (map->arr[cur_index] == NULL_KEY) {
    map->arr[cur_index] = key;
  }
  map->arr[cur_index + 1] = value;
}

inline void set_value_uq(fast_map* map, uint64_t key, uint64_t value) {
  uint64_t cur_index = mult_hash(map, key) * 2;
  uint64_t count = 0;
  while (map->arr[cur_index] != key && map->arr[cur_index] != NULL_KEY) {
    cur_index = (cur_index + 2) % (map->capacity * 2);
    ++count;
    if (debug && count % 100 == 0)
      fprintf(stderr,
              "Warning: fast_map set_value_uq(): Big Count %d -- %lu - %lu, "
              "%lu, %lu\n",
              procid, count, cur_index, key, value);
  }
  if (map->arr[cur_index] == NULL_KEY) {
    map->arr[cur_index] = key;
    map->unique_keys[map->num_unique] = key;
    map->unique_indexes[map->num_unique] = cur_index;
    ++map->num_unique;
  }
  map->arr[cur_index + 1] = value;
}

inline uint64_t get_value(fast_map* map, uint64_t key) {
  uint64_t cur_index = mult_hash(map, key) * 2;
  while (map->arr[cur_index] != key && map->arr[cur_index] != NULL_KEY)
    cur_index = (cur_index + 2) % (map->capacity * 2);
  if (map->arr[cur_index] == NULL_KEY)
    return NULL_KEY;
  else
    return map->arr[cur_index + 1];
}

inline uint64_t get_max_key(fast_map* map) {
  uint64_t max_val = 0;
  uint64_t max_key = NULL_KEY;
  for (uint64_t i = 0; i < map->num_unique; ++i)
    if (map->arr[map->unique_indexes[i] + 1] > max_val) {
      max_val = map->arr[map->unique_indexes[i] + 1];
      max_key = map->arr[map->unique_indexes[i]];
    }

  return max_key;
}

#endif
