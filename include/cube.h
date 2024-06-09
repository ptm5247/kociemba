#pragma once

#include <stdbool.h>
#include <stdint.h>

#define FACE_COUNT    6
typedef enum face {

  U, R, F, D, L, B, E, M, S,

} face_t;

#define FACELET_COUNT 54
typedef enum facelet {

  U1, U2, U3, U4, U5, U6, U7, U8, U9,
  R1, R2, R3, R4, R5, R6, R7, R8, R9,
  F1, F2, F3, F4, F5, F6, F7, F8, F9,
  D1, D2, D3, D4, D5, D6, D7, D8, D9,
  L1, L2, L3, L4, L5, L6, L7, L8, L9,
  B1, B2, B3, B4, B5, B6, B7, B8, B9,

} facelet_t;

#define CORNER_COUNT        8
#define FACELETS_PER_CORNER 3
typedef enum corner {

  URF, UFL, ULB, UBR,
  DFR, DLF, DBL, DRB,

} corner_t;

#define EDGE_COUNT        12
#define FACELETS_PER_EDGE 2
typedef enum edge {

  UR, UF, UL, UB,
  DR, DF, DL, DB,
  FR, FL, BL, BR,

} edge_t;

#define MAX_PIECES    EDGE_COUNT
#define MOVE_COUNT    7
typedef struct cube {

  uint8_t   piece_count;
  uint8_t   facelets_per_piece;
  uint8_t   permutation[MAX_PIECES];
  uint8_t   orientation[MAX_PIECES];
  uint8_t (*permutation_table)[MAX_PIECES];
  uint8_t (*orientation_table)[MAX_PIECES];

} cube_t;

extern uint8_t CORNER_PERMUTATION_TABLE[][MAX_PIECES];
extern uint8_t CORNER_ORIENTATION_TABLE[][MAX_PIECES];
extern uint8_t EDGE_PERMUTATION_TABLE[][MAX_PIECES];
extern uint8_t EDGE_ORIENTATION_TABLE[][MAX_PIECES];

#define CORNER_CUBE { \
  .piece_count = CORNER_COUNT, \
  .facelets_per_piece = FACELETS_PER_CORNER, \
  .permutation = { URF, UFL, ULB, UBR, DFR, DLF, DBL, DRB, }, \
  .orientation = {   0,   0,   0,   0,   0,   0,   0,   0, }, \
  .permutation_table = CORNER_PERMUTATION_TABLE, \
  .orientation_table = CORNER_ORIENTATION_TABLE, \
}

#define EDGE_CUBE { \
  .piece_count = EDGE_COUNT, \
  .facelets_per_piece = FACELETS_PER_EDGE, \
  .permutation = { UR, UF, UL, UB, DR, DF, DL, DB, FR, FL, BL, BR, }, \
  .orientation = {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, }, \
  .permutation_table = EDGE_PERMUTATION_TABLE, \
  .orientation_table = EDGE_ORIENTATION_TABLE, \
}

uint8_t  get_parity(cube_t *cube);
uint16_t get_orientation(cube_t *cube);
void     set_orientation(cube_t *cube, uint16_t orientation);
uint16_t get_permutation(cube_t *cube, int min, int max, bool right);
void     set_permutation(
            cube_t *cube, uint16_t permutation, int min, int max, bool right);

void     apply_move(cube_t *cube, int move);

#define N_TWIST     2187
#define N_FLIP      2048
#define N_SLICE     11880
#define N_CORNER    20160
#define N_EDGE_UD   20160
#define N_EDGE_U    1320
#define N_EDGE_D    1320

#define N_PARITY    2
#define N_SLICE1    495
#define N_SLICE2    24
#define N_MERGE_UD  336

#define N_PHASE1 ((uint32_t)N_TWIST  * (uint32_t)N_FLIP    * (uint32_t)N_SLICE1)
#define N_PHASE2 ((uint64_t)N_CORNER * (uint64_t)N_EDGE_UD * (uint64_t)N_SLICE2 * (uint64_t)N_PARITY)

#define N_POWER 3
#define N_MOVE  (MOVE_COUNT * N_POWER)

void     set_twist(cube_t *cube, uint16_t twist);
uint16_t get_twist(cube_t *cube);
void     set_corner(cube_t *cube, uint16_t corner);
uint16_t get_corner(cube_t *cube);
void     set_flip(cube_t *cube, uint16_t flip);
uint16_t get_flip(cube_t *cube);
void     set_slice(cube_t *cube, uint16_t slice);
uint16_t get_slice(cube_t *cube);
void     set_edgeU(cube_t *cube, uint16_t edgeU);
uint16_t get_edgeU(cube_t *cube);
void     set_edgeD(cube_t *cube, uint16_t edgeD);
uint16_t get_edgeD(cube_t *cube);
void     set_edgeUD(cube_t *cube, uint16_t edgeUD);
uint16_t get_edgeUD(cube_t *cube);
