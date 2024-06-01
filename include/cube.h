#pragma once

#include <stdbool.h>
#include <stdint.h>

#define COLOR_COUNT 6
#define COLOR_FIRST U
#define COLOR_LAST  B
typedef enum color {

  // opposing faces must be 3 apart
  U, R, F, D, L, B,

} color_t;
#define FACE_COUNT 6
#define FACE_FIRST U
#define FACE_LAST  B
typedef color_t face_t;

#define FACELET_COUNT 54
#define FACELET_FIRST U1
#define FACELET_LAST  B9
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
#define CORNER_FIRST        URF
#define CORNER_LAST         DRB
typedef enum corner {

  URF, UFL, ULB, UBR,
  DFR, DLF, DBL, DRB,

} corner_t;

#define EDGE_COUNT        12
#define FACELETS_PER_EDGE 2
#define EDGE_FIRST        UR
#define EDGE_LAST         BR
typedef enum edge {

  UR, UF, UL, UB,
  DR, DF, DL, DB,
  FR, FL, BL, BR,

} edge_t;

typedef struct facecube {

  color_t facelets[FACELET_COUNT];

} facecube_t;

typedef struct cubiecube {

  corner_t cp[8]; // corner permutation
  uint8_t co[8];  // corner orientation
  edge_t ep[12];  // edge permutation
  uint8_t eo[12]; // edge orientation

} cubiecube_t;

typedef struct coordcube {

  // All coordinates are 0 for a solved cube except for edgeD, which is 114
  short twist;
  short flip;
  short parity;
  short slice2;
  short corner;
  short edgeU;
  short edgeD;
  int edgeUD;

} coordcube_t;

void string_to_facecube(const char *string, facecube_t *facecube);
void facecube_to_cubiecube(facecube_t *facecube, cubiecube_t *cubiecube);
void cubiecube_to_coordcube(cubiecube_t *cubiecube, coordcube_t *coordcube);

bool validate_string(const char *cubestring);
bool validate_cubiecube(cubiecube_t *cubiecube);
