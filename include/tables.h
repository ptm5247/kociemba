#pragma once

#include <stdlib.h>

#define MAX_PATH_LENGTH 512

#define TABLE_DIR     "cache"
#define MOVE_DIR      "move"
#define HEURISTIC_DIR "heuristic"

#define PARITY_MOVE_TABLE { \
  { 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, /* 1, 0, 1, */ 0, 0, 0, 0, 0, 0, /* 0, 0, 0, */ }, \
  { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, /* 0, 1, 0, */ 1, 1, 1, 1, 1, 1, /* 1, 1, 1, */ }, \
}

void read_table(const char *dir, const char *name, void *ptr, size_t len);
