#pragma once

#include <stdbool.h>
#include <time.h>

#define SOLUTION_BUFSIZ         256
#define MAX_SOLUTION_LENGTH_HTM 31

typedef struct {

  int axis[MAX_SOLUTION_LENGTH_HTM];           // The axis of the move
  int power[MAX_SOLUTION_LENGTH_HTM];          // The power of the move
  int flip[MAX_SOLUTION_LENGTH_HTM];           // phase1 coordinates
  int twist[MAX_SOLUTION_LENGTH_HTM];
  int slice1[MAX_SOLUTION_LENGTH_HTM];
  int parity[MAX_SOLUTION_LENGTH_HTM];         // phase2 coordinates
  int corner[MAX_SOLUTION_LENGTH_HTM];
  int slice2[MAX_SOLUTION_LENGTH_HTM];
  int edgeU[MAX_SOLUTION_LENGTH_HTM];
  int edgeD[MAX_SOLUTION_LENGTH_HTM];
  int edgeUD[MAX_SOLUTION_LENGTH_HTM];
  int min_phase1_dist[MAX_SOLUTION_LENGTH_HTM];  // IDA* distance do goal estimations
  int min_phase2_dist[MAX_SOLUTION_LENGTH_HTM];

} search_t;

typedef struct {

  int         max_depth;
  time_t      timeout_sec;
  bool        use_separator;
  const char *cache_dir;

} options_t;

#define DEFAULT_OPTIONS { \
  .max_depth = 24,        \
  .timeout_sec = 60,      \
  .use_separator = false, \
  .cache_dir = "cache",   \
}

bool solution(const char *cubestring, options_t *options, char* solution);
