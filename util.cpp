#include <mpi.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "util.h"

extern int procid, nprocs;
extern bool verbose, debug, verify;

double timer() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return ((double)(tp.tv_sec) + 1e-6 * tp.tv_usec);
}

void throw_err(char const* err_message) {
  fprintf(stderr, "Error: %s\n", err_message);
  MPI_Abort(MPI_COMM_WORLD, 1);
}

void throw_err(char const* err_message, int32_t task) {
  fprintf(stderr, "Task %d Error: %s\n", task, err_message);
  MPI_Abort(MPI_COMM_WORLD, 1);
}

void throw_err(char const* err_message, int32_t task, int32_t thread) {
  fprintf(stderr, "Task %d Thread %d Error: %s\n", task, thread, err_message);
  MPI_Abort(MPI_COMM_WORLD, 1);
}

void quicksort_dec(double* arr1, uint64_t* arr2, int64_t left, int64_t right) {
  int64_t i = left;
  int64_t j = right;
  double temp;
  uint64_t temp2;
  double pivot = arr1[(left + right) / 2];

  while (i <= j) {
    while (arr1[i] > pivot) {
      i++;
    }
    while (arr1[j] < pivot) {
      j--;
    }

    if (i <= j) {
      temp = arr1[i];
      arr1[i] = arr1[j];
      arr1[j] = temp;
      temp2 = arr2[i];
      arr2[i] = arr2[j];
      arr2[j] = temp2;
      ++i;
      --j;
    }
  }

  if (j > left) quicksort_dec(arr1, arr2, left, j);
  if (i < right) quicksort_dec(arr1, arr2, i, right);
}

void quicksort_inc(uint64_t* arr1, int64_t left, int64_t right) {
  int64_t i = left;
  int64_t j = right;
  int64_t temp;
  uint64_t pivot = arr1[(left + right) / 2];

  while (i <= j) {
    while (arr1[i] < pivot) {
      i++;
    }
    while (arr1[j] > pivot) {
      j--;
    }

    if (i <= j) {
      temp = arr1[i];
      arr1[i] = arr1[j];
      arr1[j] = temp;
      ++i;
      --j;
    }
  }

  if (j > left) quicksort_inc(arr1, left, j);
  if (i < right) quicksort_inc(arr1, i, right);
}

uint64_t xs1024star_next(xs1024star_t* xs) {
  const uint64_t s0 = xs->s[xs->p];
  uint64_t s1 = xs->s[xs->p = (xs->p + 1) & 15];
  s1 ^= s1 << 31;
  xs->s[xs->p] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30);
  return xs->s[xs->p] * uint64_t(1181783497276652981U);
}

double xs1024star_next_real(xs1024star_t* xs) {
  const uint64_t s0 = xs->s[xs->p];
  uint64_t s1 = xs->s[xs->p = (xs->p + 1) & 15];
  s1 ^= s1 << 31;
  xs->s[xs->p] = s1 ^ s0 ^ (s1 >> 11) ^ (s0 >> 30);
  double ret = (double)(xs->s[xs->p] * uint64_t(1181783497276652981U));
  return ret /= (double)uint64_t(18446744073709551615U);
}

void xs1024star_seed(uint64_t seed, xs1024star_t* xs) {
  for (uint64_t i = 0; i < 16; ++i) {
    uint64_t z = (seed += uint64_t(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * uint64_t(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * uint64_t(0x94D049BB133111EB);
    xs->s[i] = z ^ (z >> 31);
  }
  xs->p = 0;
}
