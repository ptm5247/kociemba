#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cache.h"

short MOVE_TWIST[N_TWIST][N_MOVE];
short MOVE_FLIP[N_FLIP][N_MOVE];
short MOVE_PARITY[N_PARITY][N_MOVE] = {
  { 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1 },
  { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 },
};
short MOVE_SLICE[N_FRtoBR][N_MOVE];
short MOVE_CORNER[N_URFtoDLF][N_MOVE] = {{0}};
short MOVE_EDGE_UD[N_URtoDF][N_MOVE] = {{0}};
short MOVE_EDGE_U[N_URtoUL][N_MOVE] = {{0}};
short MOVE_EDGE_D[N_UBtoDF][N_MOVE] = {{0}};
short MERGE_EDGE_UD[336][336] = {{0}};
signed char PRUNE_CORNER[N_SLICE2 * N_URFtoDLF * N_PARITY / 2] = {0};
signed char PRUNE_EDGE[N_SLICE2 * N_URtoDF * N_PARITY / 2] = {0};
signed char PRUNE_TWIST[N_SLICE1 * N_TWIST / 2 + 1] = {0};
signed char PRUNE_FLIP[N_SLICE1 * N_FLIP / 2] = {0};

bool CACHE_OK = false;

static void read_table(const char *cache_dir, const char *name, void *ptr, size_t len);

void init_cache(const char *cache_dir) {
  uint64_t cache_size = 0;

  read_table(cache_dir, "move/twist",   (void *)MOVE_TWIST,    sizeof(MOVE_TWIST));
  read_table(cache_dir, "move/flip",    (void *)MOVE_FLIP,     sizeof(MOVE_FLIP));
  read_table(cache_dir, "move/corner",  (void *)MOVE_CORNER,   sizeof(MOVE_CORNER));
  read_table(cache_dir, "move/slice",   (void *)MOVE_SLICE,    sizeof(MOVE_SLICE));
  read_table(cache_dir, "move/edgeUD",  (void *)MOVE_EDGE_UD,  sizeof(MOVE_EDGE_UD));
  read_table(cache_dir, "move/edgeU",   (void *)MOVE_EDGE_U,   sizeof(MOVE_EDGE_U));
  read_table(cache_dir, "move/edgeD",   (void *)MOVE_EDGE_D,   sizeof(MOVE_EDGE_D));
  read_table(cache_dir, "merge/edgeUD", (void *)MERGE_EDGE_UD, sizeof(MERGE_EDGE_UD));
  read_table(cache_dir, "prune/corner", (void *)PRUNE_CORNER,  sizeof(PRUNE_CORNER));
  read_table(cache_dir, "prune/edge",   (void *)PRUNE_EDGE,    sizeof(PRUNE_EDGE));
  read_table(cache_dir, "prune/twist",  (void *)PRUNE_TWIST,   sizeof(PRUNE_TWIST));
  read_table(cache_dir, "prune/flip",   (void *)PRUNE_FLIP,    sizeof(PRUNE_FLIP));

  cache_size =
    sizeof(MOVE_TWIST) + sizeof(MOVE_FLIP) + sizeof(MOVE_PARITY) +
    sizeof(MOVE_SLICE) + sizeof(MOVE_CORNER) + sizeof(MOVE_EDGE_UD) +
    sizeof(MOVE_EDGE_U) + sizeof(MOVE_EDGE_D) + sizeof(MERGE_EDGE_UD) +
    sizeof(PRUNE_CORNER) + sizeof(PRUNE_EDGE) +
    sizeof(PRUNE_TWIST) + sizeof(PRUNE_FLIP);
  printf("Cache Size: %ld\n", cache_size);

  CACHE_OK = true;
}

// Extract pruning value
signed char getPruning(signed char *table, int index) {
  signed char res;

  if ((index & 1) == 0)
    res = (table[index / 2] & 0x0f);
  else
    res = ((table[index / 2] >> 4) & 0x0f);

  return res;
}

#define MAX_PATH_LENGTH 512

static void read_table(const char *cache_dir, const char *name, void *ptr, size_t len) {
  char    path[MAX_PATH_LENGTH] = { '\0' };
  int     result;
  size_t  count;
  FILE   *file;

  result = snprintf(path, sizeof(path), "%s/%s", cache_dir, name);
  if (result >= sizeof(path)) {
    fprintf(stderr, "cache path (%s/%s) is too long\n", cache_dir, name);
    exit(EXIT_FAILURE);
  }

  result = access(path, F_OK | R_OK);
  if (result == -1) {
    fprintf(stderr, "failed to access %s: %s\n", path, strerror(errno));
    exit(EXIT_FAILURE);
  }

  file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "failed to open %s: %s\n", path, strerror(errno));
    exit(EXIT_FAILURE);
  }

  count = fread(ptr, len, 1, file);
  if (count != 1) {
    fprintf(stderr, "failed to read complete table %s\n", path);
    exit(EXIT_FAILURE);
  }

  result = fclose(file);
  if (result) {
    fprintf(stderr, "failed to close %s: %s\n", path, strerror(errno));
    exit(EXIT_FAILURE);
  }
}
