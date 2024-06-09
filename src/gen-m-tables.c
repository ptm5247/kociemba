#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>

#include "cube.h"
#include "tables.h"

#define write_move_table(CUBE_INITIALIZER, name, getter, setter, N) {          \
  cube_t    cube = CUBE_INITIALIZER;                                           \
  uint16_t *table = malloc(N * sizeof(uint16_t) * N_MOVE);                     \
  int       coordinate, axis, power;                                           \
  FILE     *file = fopen(TABLE_DIR"/"MOVE_DIR"/" name, "wb");                  \
                                                                               \
  assert(file != NULL);                                                        \
  assert(table != NULL);                                                       \
                                                                               \
  for (coordinate = 0; coordinate < N; coordinate++) {                         \
    setter(&cube, coordinate);                                                 \
    assert(getter(&cube) == coordinate);                                       \
    for (axis = 0; axis < MOVE_COUNT; axis++) {                                \
      for (power = 0; power < N_POWER; power++) {                              \
        apply_move(&cube, axis);                                               \
        table[N_MOVE * coordinate + N_POWER * axis + power] = getter(&cube);   \
      }                                                                        \
      apply_move(&cube, axis);                                                 \
    }                                                                          \
    assert(getter(&cube) == coordinate);                                       \
    if (coordinate & 0x3F == 0) {                                              \
      printf("\r" MOVE_DIR "/%s: %d/%d", name, coordinate + 1, N);             \
      fflush(stdout);                                                          \
    }                                                                          \
  }                                                                            \
  printf("\r" MOVE_DIR "/%s: %d/%d\n", name, N, N);                            \
                                                                               \
  fwrite(table, N * sizeof(uint16_t) * N_MOVE, 1, file);                       \
  fclose(file);                                                                \
  free(table);                                                                 \
}

static void write_merge_table() {
  cube_t    cube1 = EDGE_CUBE;
  cube_t    cube2 = EDGE_CUBE;
  uint16_t *table = malloc(N_MERGE_UD * N_MERGE_UD * sizeof(uint16_t));
  uint16_t  edgeU, edgeD;
  int       i;
  FILE     *file = fopen(TABLE_DIR"/"MOVE_DIR"/mergeUD", "wb");

  assert(file != NULL);
  assert(table != NULL);

  for (edgeU = 0; edgeU < N_MERGE_UD; edgeU++) {
    set_edgeU(&cube1, edgeU);
    for (edgeD = 0; edgeD < N_MERGE_UD; edgeD++) {
      set_edgeD(&cube2, edgeD);

      for (i = 0; i < EDGE_COUNT; i++) {
        if (cube1.permutation[i] != EDGE_COUNT - 1) {
          if (cube2.permutation[i] != EDGE_COUNT - 1) {
            break;
          } else {
            cube2.permutation[i] = cube1.permutation[i];
      } } }
      if (i == EDGE_COUNT) {
        assert(get_edgeU(&cube2) == edgeU);
        assert(get_edgeD(&cube2) == edgeD);
        table[N_MERGE_UD * edgeU + edgeD] = get_edgeUD(&cube2);
      } else {
        table[N_MERGE_UD * edgeU + edgeD] = 0xFFFF;
  } } }
  printf("\r"MOVE_DIR"/mergeUD: %d/%d\n",
    N_MERGE_UD * N_MERGE_UD, N_MERGE_UD * N_MERGE_UD);

  fwrite(table, N_MERGE_UD * N_MERGE_UD * sizeof(uint16_t), 1, file);
  fclose(file);
  free(table);
}

int main(int argc, char **argv) {
  if (mkdir(TABLE_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
    assert(errno == EEXIST);
  if (mkdir(TABLE_DIR "/" MOVE_DIR, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH))
    assert(errno == EEXIST);

  write_move_table(CORNER_CUBE, "twist",  get_twist,  set_twist,  N_TWIST );
  write_move_table(EDGE_CUBE,   "flip",   get_flip,   set_flip,   N_FLIP  );
  write_move_table(CORNER_CUBE, "corner", get_corner, set_corner, N_CORNER);
  write_move_table(EDGE_CUBE,   "slice",  get_slice,  set_slice,  N_SLICE );
  write_move_table(EDGE_CUBE,   "edgeU",  get_edgeU,  set_edgeU,  N_EDGE_U );
  write_move_table(EDGE_CUBE,   "edgeD",  get_edgeD,  set_edgeD,  N_EDGE_D );
  write_move_table(EDGE_CUBE,   "edgeUD", get_edgeUD, set_edgeUD, N_EDGE_UD);

  write_merge_table();

  return EXIT_SUCCESS;
}
