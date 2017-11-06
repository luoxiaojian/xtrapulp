#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdint.h>

double timer();

void throw_err(char const* err_message);
void throw_err(char const* err_message, int32_t task);
void throw_err(char const* err_message, int32_t task, int32_t thread);

void quicksort_dec(double* arr1, uint64_t* arr2, int64_t left, int64_t right);
void quicksort_inc(uint64_t* arr1, int64_t left, int64_t right);

struct xs1024star_t {
  uint64_t s[16];
  int64_t p;
};

uint64_t xs1024star_next(xs1024star_t* xs);
double xs1024star_next_real(xs1024star_t* xs);
void xs1024star_seed(uint64_t seed, xs1024star_t* xs);

#endif
