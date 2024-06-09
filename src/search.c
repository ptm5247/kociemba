#include <stdio.h>
#include <string.h>
#include <time.h>

#include "cube.h"
#include "search.h"
#include "tables.h"

static facelet_t CORNER_TO_FACELETS[CORNER_COUNT][FACELETS_PER_CORNER] = {
  [URF] = { U9, R1, F3 }, [UFL] = { U7, F1, L3 },
  [ULB] = { U1, L1, B3 }, [UBR] = { U3, B1, R3 },
  [DFR] = { D3, F9, R7 }, [DLF] = { D1, L9, F7 },
  [DBL] = { D7, B9, L7 }, [DRB] = { D9, R9, B7 },
};
static facelet_t EDGE_TO_FACELETS[EDGE_COUNT][FACELETS_PER_EDGE] = {
  [UR] = { U6, R2 }, [UF] = { U8, F2 }, [UL] = { U4, L2 }, [UB] = { U2, B2 },
  [DR] = { D6, R8 }, [DF] = { D2, F8 }, [DL] = { D4, L8 }, [DB] = { D8, B8 },
  [FR] = { F6, R4 }, [FL] = { F4, L6 }, [BL] = { B6, L4 }, [BR] = { B4, R6 },
};
static face_t CORNER_TO_COLORS[CORNER_COUNT][FACELETS_PER_CORNER] = {
  [URF] = { U, R, F }, [UFL] = { U, F, L },
  [ULB] = { U, L, B }, [UBR] = { U, B, R },
  [DFR] = { D, F, R }, [DLF] = { D, L, F },
  [DBL] = { D, B, L }, [DRB] = { D, R, B },
};
static face_t EDGE_TO_COLORS[EDGE_COUNT][FACELETS_PER_EDGE] = {
  [UR] = { U, R }, [UF] = { U, F }, [UL] = { U, L }, [UB] = { U, B },
  [DR] = { D, R }, [DF] = { D, F }, [DL] = { D, L }, [DB] = { D, B },
  [FR] = { F, R }, [FL] = { F, L }, [BL] = { B, L }, [BR] = { B, R },
};

uint64_t p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5 = 0;

static bool     TABLES_READY = false;

static int16_t  MOVE_FLIP[N_FLIP][N_MOVE];
static int16_t  MOVE_TWIST[N_TWIST][N_MOVE];
static int16_t  MOVE_SLICE[N_SLICE][N_MOVE];

static int16_t  MOVE_EDGE_U[N_EDGE_U][N_MOVE];
static int16_t  MOVE_EDGE_D[N_EDGE_D][N_MOVE];
static int16_t  MOVE_EDGE_UD[N_EDGE_UD][N_MOVE];
static int16_t  MOVE_CORNER[N_CORNER][N_MOVE];
static int16_t  MOVE_PARITY[N_PARITY][N_MOVE] = PARITY_MOVE_TABLE;

static int16_t  MERGE_EDGE_UD[N_MERGE_UD][N_MERGE_UD];

static uint8_t *HEURISTIC_PHASE1;
static uint8_t *HEURISTIC_PHASE2;

static bool initialize(const char *cubestring, cube_t *corners, cube_t *edges);
static void cache_tables();
static bool to_string(search_frame_t *search, int depth, char *solution);

bool solution(const char *cubestring, char solution[SOLUTION_BUFSIZ]) {
  cube_t          corners = CORNER_CUBE;
  cube_t          edges = EDGE_CUBE;
  search_frame_t  search[MAX_SOLUTION_LENGTH_HTM];
  int             depth, i;
  uint32_t        phase1, flip, twist, slice1;
  uint64_t        phase2, edgeUD, corner, slice2, parity;
  int             move, heuristic;

#define phase1_coordinate(flip, twist, slice1) \
  (N_SLICE1 * (N_FLIP * (twist) + flip) + slice1)

#define phase1_heuristic(i) \
  ((HEURISTIC_PHASE1[i / 4] >> 6 - 2 * (i % 4)) & 0b11)

#define phase2_coordinate(edgeUD, corner, slice2, parity) \
  (N_PARITY * (N_SLICE2 * (N_EDGE_UD * (                  \
  (long)corner) + (long)edgeUD) + (long)slice2) + (long)parity)

#define phase2_heuristic(i) \
  ((i * 5 % 8) < 4 ? ( \
    (HEURISTIC_PHASE2[i * 5 / 8] >> (3 - i * 5 % 8)) & 0b11111 \
  ) : ( \
    ((HEURISTIC_PHASE2[i * 5 / 8] & (1 << 8 - i * 5 % 8) - 1) << i * 5 % 8 - 3) | \
    (HEURISTIC_PHASE2[i * 5 / 8 + 1] >> 11 - i * 5 % 8) \
  ))

  // ------------------------------ preparation -------------------------------

  if (!TABLES_READY) {
    cache_tables();
    TABLES_READY = true;
  }

  // --------------------------- convert & validate ---------------------------

  if (!initialize(cubestring, &corners, &edges)) return false;

  // ------------------------- phase 1 initialization -------------------------

  depth = 0;
  search[0].flip = get_flip(&edges);
  search[0].twist = get_twist(&corners);
  search[0].slice1 = get_slice(&edges) / 24;

  phase1 = phase1_coordinate(search[0].flip, search[0].twist, search[0].slice1);
  search[0].heuristic = phase1_heuristic(phase1);

  // ----------------------------- phase 1 search -----------------------------

  while (search[depth].flip || search[depth].twist || search[depth].slice1) {
    for (move = 0; move < N_MOVE; move++) {
      p1 += 1;
      flip   =  MOVE_FLIP[search[depth].flip       ][move];
      twist  = MOVE_TWIST[search[depth].twist      ][move];
      slice1  = MOVE_SLICE[search[depth].slice1 * 24][move] / 24;
      phase1 = phase1_coordinate(flip, twist, slice1);
      heuristic = phase1_heuristic(phase1);
      if (search[depth].heuristic ?
          (heuristic == search[depth].heuristic - 1) : (heuristic == 2)) {
        search[depth].axis = move / 3;
        search[depth].power = move % 3 + 1;
        depth += 1;
        search[depth].flip = flip;
        search[depth].twist = twist;
        search[depth].slice1 = slice1;
        search[depth].heuristic = heuristic;
        break;
      }
    }
    if (move == N_MOVE) {
      printf("no way down\n");
      return false;
    }
  }
  p2 += 1;
  p3 += 1;

  // ------------------------- phase 2 initialization -------------------------

  search[0].edgeU = get_edgeU(&edges);
  search[0].edgeD = get_edgeD(&edges);
  search[0].corner = get_corner(&corners);
  search[0].slice2 = get_slice(&edges);
  search[0].parity = 1 - get_parity(&corners);

  for (i = 0; i < depth; i++) {
    move = N_POWER * search[i].axis + search[i].power - 1;

    search[i + 1].edgeU  = MOVE_EDGE_U[search[i].edgeU][move];
    search[i + 1].edgeD  = MOVE_EDGE_D[search[i].edgeD][move];
    search[i + 1].corner = MOVE_CORNER[search[i].corner][move];
    search[i + 1].slice2 =  MOVE_SLICE[search[i].slice2][move];
    search[i + 1].parity = MOVE_PARITY[search[i].parity][move];
  }
  search[depth].edgeUD = MERGE_EDGE_UD[search[depth].edgeU][search[depth].edgeD];

  phase2 = phase2_coordinate(search[depth].edgeUD, search[depth].corner,
    search[depth].slice2, search[depth].parity);
  search[depth].heuristic = phase2_heuristic(phase2);

  // ----------------------------- phase 2 search -----------------------------

  while (search[depth].edgeUD || search[depth].corner || search[depth].slice2 || search[depth].parity) {
    for (move = 0; move < N_MOVE; move++) {
      if (move ==  3 || move ==  5 || move ==  6 || move ==  8 ||
          move == 12 || move == 14 || move == 15 || move == 17) continue;
      p4 += 1;
      edgeUD = MOVE_EDGE_UD[search[depth].edgeUD][move];
      corner =  MOVE_CORNER[search[depth].corner][move];
      slice2 =   MOVE_SLICE[search[depth].slice2][move];
      parity =  MOVE_PARITY[search[depth].parity][move];
      phase2 = phase2_coordinate(edgeUD, corner, slice2, parity);
      heuristic = phase2_heuristic(phase2);
      if (heuristic < search[depth].heuristic) {
        search[depth].axis = move / 3;
        search[depth].power = move % 3 + 1;
        depth += 1;
        search[depth].edgeUD = edgeUD;
        search[depth].corner = corner;
        search[depth].slice2 = slice2;
        search[depth].parity = parity;
        search[depth].heuristic = heuristic;
        break;
      }
    }
    if (move == N_MOVE) {
      printf("no way down\n");
      return false;
    }
  }
  p5 += 1;

  return to_string(search, depth, solution);
}

static bool initialize(const char *cubestring, cube_t *corners, cube_t *edges) {
  int     color_count[FACE_COUNT] = { 0 };
  int     edge_count[EDGE_COUNT] = { 0 };
  int     corner_count[CORNER_COUNT] = { 0 };
  face_t  facelets[FACELET_COUNT];
  uint8_t orientation;
  face_t  color1, color2;
  int     i, j, sum;

  if (strspn(cubestring, "UFRDLB") != FACELET_COUNT) {
    printf("Cubestring length must be exactly %d.\n", FACELET_COUNT);
    return false;
  }

  for (i = 0; i < FACELET_COUNT; i++) {
    switch (cubestring[i]) {
    case 'U': color_count[U]++; facelets[i] = U; break;
    case 'R': color_count[R]++; facelets[i] = R; break;
    case 'F': color_count[F]++; facelets[i] = F; break;
    case 'D': color_count[D]++; facelets[i] = D; break;
    case 'L': color_count[L]++; facelets[i] = L; break;
    case 'B': color_count[B]++; facelets[i] = B; break;
    }
  }

  for (i = 0; i < FACE_COUNT; i++) {
    if (color_count[i] != FACELET_COUNT / FACE_COUNT) {
      printf("Cubestring must contain exactly %d of each color.\n",
        FACELET_COUNT / FACE_COUNT);
      return false;
    }
  }

  if (cubestring[ 4] != 'U' || cubestring[13] != 'R' || cubestring[22] != 'F' ||
      cubestring[31] != 'D' || cubestring[40] != 'L' || cubestring[49] != 'B') {
    printf("Center facelets are in the wrong order.\n");
    return false;
  }

  memset(corners->permutation, 0x00, MAX_PIECES);
  memset(edges->permutation,   0x00, MAX_PIECES);

  for (i = 0; i < CORNER_COUNT; i++) {
    for (orientation = 0; orientation < FACELETS_PER_CORNER; orientation++)
      if (facelets[CORNER_TO_FACELETS[i][orientation]] == U ||
          facelets[CORNER_TO_FACELETS[i][orientation]] == D) break;

    color1 = facelets[CORNER_TO_FACELETS[i][(orientation + 1) % FACELETS_PER_CORNER]];
    color2 = facelets[CORNER_TO_FACELETS[i][(orientation + 2) % FACELETS_PER_CORNER]];

    for (j = 0; j < CORNER_COUNT; j++) {
      if (color1 == CORNER_TO_COLORS[j][1] && color2 == CORNER_TO_COLORS[j][2]) {
        corners->permutation[i] = j;
        corners->orientation[i] = orientation % FACELETS_PER_CORNER;
        break;
      }
    }
  }

  for (i = 0; i < EDGE_COUNT; i++) {
    for (j = 0; j < EDGE_COUNT; j++) {
      if (facelets[EDGE_TO_FACELETS[i][0]] == EDGE_TO_COLORS[j][0] &&
          facelets[EDGE_TO_FACELETS[i][1]] == EDGE_TO_COLORS[j][1]) {
        edges->permutation[i] = j;
        edges->orientation[i] = 0;
        break;
      }
      if (facelets[EDGE_TO_FACELETS[i][0]] == EDGE_TO_COLORS[j][1] &&
          facelets[EDGE_TO_FACELETS[i][1]] == EDGE_TO_COLORS[j][0]) {
        edges->permutation[i] = j;
        edges->orientation[i] = 1;
        break;
      }
    }
  }

  for (i = 0, sum = 0; i < EDGE_COUNT; i++) {
    edge_count[edges->permutation[i]]++;
    sum += edges->orientation[i];
  }
  for (i = 0; i < EDGE_COUNT; i++) {
    if (edge_count[i] != 1) {
      printf("missing edge\n");
      return false;
    }
  }
  if (sum % FACELETS_PER_EDGE) {
    printf("flipped edge\n");
    return false;
  }

  for (i = 0, sum = 0; i < CORNER_COUNT; i++) {
    corner_count[corners->permutation[i]]++;
    sum += corners->orientation[i];
  }
  for (i = 0; i < CORNER_COUNT; i++) {
    if (corner_count[i] != 1) {
      printf("missing corner\n");
      return false;
    }
  }
  if (sum % FACELETS_PER_CORNER) {
    printf("twisted corner\n");
    return false;
  }

  // if (get_parity(edges) ^ get_parity(corners)) {
  //   printf("parity error\n");
  //   return false;
  // }

  return true;
}

static void cache_tables() {
  uint64_t        index, sum = 0;
  struct timespec start, now;

#define delta_t(now, start) \
  (now.tv_sec - start.tv_sec + (now.tv_nsec - start.tv_nsec) / 1000000000.)

#define start(description)           \
  printf("%-32s", description"..."); \
  fflush(stdout);                    \
  start = now;

#define finish()                       \
  clock_gettime(CLOCK_REALTIME, &now); \
  printf("Done in %6.3fs\n", delta_t(now, start));

  clock_gettime(CLOCK_REALTIME, &now);
  start("Allocating");
  HEURISTIC_PHASE1 = malloc(N_PHASE1 / 4);
  HEURISTIC_PHASE2 = malloc(N_PHASE2 * 5 / 8);
  finish();
  start("Reading move tables");
  read_table(TABLE_DIR, MOVE_DIR"/flip",   MOVE_FLIP,    sizeof(MOVE_FLIP) );
  read_table(TABLE_DIR, MOVE_DIR"/twist",  MOVE_TWIST,   sizeof(MOVE_TWIST));
  read_table(TABLE_DIR, MOVE_DIR"/slice",  MOVE_SLICE,   sizeof(MOVE_SLICE));
  read_table(TABLE_DIR, MOVE_DIR"/edgeU",  MOVE_EDGE_U,  sizeof(MOVE_EDGE_U));
  read_table(TABLE_DIR, MOVE_DIR"/edgeD",  MOVE_EDGE_D,  sizeof(MOVE_EDGE_D));
  read_table(TABLE_DIR, MOVE_DIR"/edgeUD", MOVE_EDGE_UD, sizeof(MOVE_EDGE_UD));
  read_table(TABLE_DIR, MOVE_DIR"/corner", MOVE_CORNER,  sizeof(MOVE_CORNER));
  read_table(TABLE_DIR, MOVE_DIR"/mergeUD", MERGE_EDGE_UD, sizeof(MERGE_EDGE_UD));
  finish();
  start("Reading heuristic tables");
  read_table(TABLE_DIR, HEURISTIC_DIR"/phase1", HEURISTIC_PHASE1, N_PHASE1 / 4);
  read_table(TABLE_DIR, HEURISTIC_DIR"/phase2", HEURISTIC_PHASE2, N_PHASE2 * 5 / 8);
  finish();
  start("Caching heuristic tables");
  for (index = 0; index < N_PHASE1 / 4; index++)
    sum += HEURISTIC_PHASE1[index];
  for (index = 0; index < N_PHASE2 * 5 / 8; index++)
    sum += HEURISTIC_PHASE2[index];
  finish();
  printf("%-32s0x%013lX\n", "Cache Sum:", sum);
  printf("%-32s0x%013lX\n", "Cache Size:",
    sizeof(MOVE_FLIP) + sizeof(MOVE_TWIST) + sizeof(MOVE_SLICE) +
    sizeof(MOVE_EDGE_U) + sizeof(MOVE_EDGE_D) + sizeof(MOVE_EDGE_UD) +
    sizeof(MOVE_CORNER) + sizeof(MERGE_EDGE_UD) + sizeof(MOVE_PARITY) +
    N_PHASE1 / 4 + N_PHASE2 * 5 / 8);
}

static bool to_string(search_frame_t *search, int depth, char *solution) {
  int i;

  for (i = 0; i < depth; i++) {
    switch (search[i].axis) {
    case U: *solution++ = 'U'; break;
    case R: *solution++ = 'R'; break;
    case F: *solution++ = 'F'; break;
    case D: *solution++ = 'D'; break;
    case L: *solution++ = 'L'; break;
    case B: *solution++ = 'B'; break;
    }
    switch (search[i].power) {
    case 1:                     break;
    case 2: *solution++ = '2';  break;
    case 3: *solution++ = '\''; break;
    }
    *solution++ = ' ';
  }
  *solution++ = '\0';

  return true;
}
