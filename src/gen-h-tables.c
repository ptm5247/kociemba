#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>

#include "cube.h"
#include "tables.h"

static int16_t MOVE_CORNER[N_CORNER][N_MOVE];
static int16_t MOVE_EDGE_UD[N_EDGE_UD][N_MOVE];
static int16_t MOVE_FLIP[N_FLIP][N_MOVE];
static int16_t MOVE_SLICE[N_SLICE][N_MOVE];
static int16_t MOVE_TWIST[N_TWIST][N_MOVE];
static int16_t MOVE_PARITY[N_PARITY][N_MOVE] = PARITY_MOVE_TABLE;

#define table_setup {                                                          \
  result = snprintf(path, sizeof(path), TABLE_DIR"/"HEURISTIC_DIR"/%s", name); \
  if (result >= sizeof(path)) {                                                \
    fprintf(stderr,                                                            \
      "table path ("TABLE_DIR"/"HEURISTIC_DIR"/%s) is too long\n", name);      \
    exit(EXIT_FAILURE);                                                        \
  }                                                                            \
                                                                               \
  if (!table) {                                                                \
    perror("failed to allocate");                                              \
    exit(EXIT_FAILURE);                                                        \
  }                                                                            \
                                                                               \
  file = fopen(path, "wb");                                                    \
  if (!file) {                                                                 \
    perror("failed to open");                                                  \
    exit(EXIT_FAILURE);                                                        \
  }                                                                            \
}

#define print_summary(depth, prog, N, end) {                                   \
  printf("\r%2d %3.0f%% | %6.02f%% (%10ld) | %6.02f%% (%11ld)" end,            \
    depth, 100. * (long)prog / N, 100. * ((long)done - (long)last_done) / N,   \
    (long)done - (long)last_done, 100. * (long)done / N, (long)done);          \
}

static void write_h_table_phase1(const char *name, int min, int max) {
  int8_t   *table = malloc(N_PHASE1);
  uint32_t  i, j, k;
  int       depth;
  uint32_t  done, last_done;
  uint32_t  twist1, twist2;
  uint32_t  flip1, flip2;
  uint32_t  slice1, slice2;
  int       result;
  char      path[MAX_PATH_LENGTH] = { '\0' };
  FILE     *file;

  table_setup

  depth = 0;
  memset(table, -1, N_PHASE1);
  table[0] = 0;
  last_done = 0;
  done = 1;

  printf("%s:\n", path);
  printf("  DEPTH |   DONE (THIS PASS)   |     DONE (TOTAL)\n");

  while (done < N_PHASE1) {
    print_summary(depth, N_PHASE1, N_PHASE1, "\n");
    last_done = done;
    for (i = 0; i < N_PHASE1; i++) {
      if (table[i] == depth) {
        slice1 = i % N_SLICE1;
        k = i / N_SLICE1;
        flip1 = k % N_FLIP;
        twist1 = k / N_FLIP;
        for (j = min; j < max; j++) {
          twist2 = MOVE_TWIST[twist1     ][j];
          flip2  =  MOVE_FLIP[flip1      ][j];
          slice2 = MOVE_SLICE[slice1 * 24][j] / 24;
          k = N_SLICE1 * (N_FLIP * twist2 + flip2) + slice2;
          if (table[k] == -1) {
            table[k] = depth + 1;
            done += 1;
            if (!(done & 0xFFFF)) {
              print_summary(depth + 1, i, N_PHASE1, "");
              fflush(stdout);
      } } } }
      if (i % (N_PHASE1 / 100) == 0) {
        print_summary(depth + 1, i, N_PHASE1, "");
        fflush(stdout);
      }
    }
    depth += 1;
  }
  print_summary(depth, N_PHASE1, N_PHASE1, "\n");

  for (i = 0; i < N_PHASE1 / 4; i++) {
    ((uint8_t *)table)[i] =
      ((table[4 * i + 0] % 3) << 6) |
      ((table[4 * i + 1] % 3) << 4) |
      ((table[4 * i + 2] % 3) << 2) |
      ((table[4 * i + 3] % 3) << 0);
  }

  fwrite(table, N_PHASE1 / 4, 1, file);
  fclose(file);
  free(table);
}

static void write_h_table_phase2(const char *name, int min, int max) {
  uint8_t  *table = malloc(N_PHASE2 * 5 / 8);
  uint64_t  i, j, k;
  int       depth;
  uint64_t  done, last_done;
  uint64_t  corner1, corner2;
  uint64_t  edge1, edge2;
  uint64_t  slice1, slice2;
  uint64_t  parity1, parity2;
  int       result;
  char      path[MAX_PATH_LENGTH] = { '\0' };
  FILE     *file;

  table_setup

  depth = 0;
  memset(table, 0xFF, N_PHASE2 * 5 / 8);
  table[0] = 0b00000111;
  last_done = 0;
  done = 1;

  printf("%s:\n", path);
  printf("  DEPTH |   DONE (THIS PASS)   |      DONE (TOTAL)\n");

#define get_table(table, i) \
  ((i * 5 % 8) < 4 ? ( \
    (table[i * 5 / 8] >> (3 - i * 5 % 8)) & 0b11111 \
  ) : ( \
    ((table[i * 5 / 8] & (1 << 8 - i * 5 % 8) - 1) << i * 5 % 8 - 3) | \
    (table[i * 5 / 8 + 1] >> 11 - i * 5 % 8) \
  ))

#define set_table(table, i, value) \
  if (i * 5 % 8 < 4) { \
    table[i * 5 / 8] &= ~(0b11111 << 3 - i * 5 % 8); \
    table[i * 5 / 8] |=  ((value) << 3 - i * 5 % 8); \
  } else { \
    table[i * 5 / 8]     &= ~((1 << 8 - i * 5 % 8) - 1); \
    table[i * 5 / 8]     |=  ((value) >> i * 5 % 8 - 3); \
    table[i * 5 / 8 + 1] &= ~(((1 << i * 5 % 8 - 3) - 1) << 11 - i * 5 % 8); \
    table[i * 5 / 8 + 1] |=  (((value) & (1 << i * 5 % 8 - 3) - 1) << 11 - i * 5 % 8); \
  }

  while (done < N_PHASE2) {
    print_summary(depth, N_PHASE2, N_PHASE2, "\n");
    last_done = done;
    for (i = 0; i < N_PHASE2; i++) {
      if (get_table(table, i) == depth) {
        parity1 = i % N_PARITY;
        k = i / N_PARITY;
        slice1 = k % N_SLICE2;
        k = k / N_SLICE2;
        edge1 = k % N_EDGE_UD;
        corner1 = k / N_EDGE_UD;
        for (j = min; j < max; j++) {
          switch (j) {
          case  3: case  5: case  6: case  8:
          case 12: case 14: case 15: case 17:
            continue;
          default:
            corner2 =  MOVE_CORNER[corner1][j];
            edge2   = MOVE_EDGE_UD[edge1  ][j];
            slice2  =   MOVE_SLICE[slice1 ][j];
            parity2 =  MOVE_PARITY[parity1][j];
            k = N_PARITY * (N_SLICE2 * (N_EDGE_UD * corner2 + edge2) + slice2) + parity2;
            if (get_table(table, k) == 0b11111) {
              set_table(table, k, depth + 1);
              done += 1;
              if (!(done & 0x7FFFF)) {
                print_summary(depth + 1, i, N_PHASE2, "");
                fflush(stdout);
      } } } } }
      if (i % (N_PHASE2 / 100) == 0) {
        print_summary(depth + 1, i, N_PHASE2, "");
        fflush(stdout);
      }
    }
    depth += 1;
  }
  print_summary(depth, N_PHASE2, N_PHASE2, "\n");

  fwrite(table, N_PHASE2 * 5 / 8, 1, file);
  fclose(file);
  free(table);
}

int main() {
  if (mkdir(TABLE_DIR "/" HEURISTIC_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
    assert(errno == EEXIST);

  read_table(TABLE_DIR, MOVE_DIR "/corner", MOVE_CORNER,  sizeof(MOVE_CORNER) );
  read_table(TABLE_DIR, MOVE_DIR "/edgeUD", MOVE_EDGE_UD, sizeof(MOVE_EDGE_UD));
  read_table(TABLE_DIR, MOVE_DIR "/flip",   MOVE_FLIP,    sizeof(MOVE_FLIP)   );
  read_table(TABLE_DIR, MOVE_DIR "/slice",  MOVE_SLICE,   sizeof(MOVE_SLICE)  );
  read_table(TABLE_DIR, MOVE_DIR "/twist",  MOVE_TWIST,   sizeof(MOVE_TWIST)  );

  write_h_table_phase1("phase1",     0, 18);
  write_h_table_phase1("phase1-noU", 3, 18);
  write_h_table_phase1("phase1-noB", 0, 15);

  write_h_table_phase2("phase2",     0, 18);
  write_h_table_phase2("phase2-noU", 3, 18);
  write_h_table_phase2("phase2-noB", 0, 15);

  exit(EXIT_SUCCESS);
}
