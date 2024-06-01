#include <stdio.h>
#include <string.h>

#include "cache.h"
#include "cube.h"

// Maps a corner to the facelets that make it up. The first of the three
// facelets defines the orientation, and the other two proceed clockwise.
static facelet_t CORNER_TO_FACELETS[CORNER_COUNT][FACELETS_PER_CORNER] = {
  [URF] = { U9, R1, F3 },
  [UFL] = { U7, F1, L3 },
  [ULB] = { U1, L1, B3 },
  [UBR] = { U3, B1, R3 },
  [DFR] = { D3, F9, R7 },
  [DLF] = { D1, L9, F7 },
  [DBL] = { D7, B9, L7 },
  [DRB] = { D9, R9, B7 },
};

// Maps an edge to the facelets that make it up. The first of the two
// facelets defines the default orientation.
static facelet_t EDGE_TO_FACELETS[EDGE_COUNT][FACELETS_PER_EDGE] = {
  [UR] = { U6, R2 },
  [UF] = { U8, F2 },
  [UL] = { U4, L2 },
  [UB] = { U2, B2 },
  [DR] = { D6, R8 },
  [DF] = { D2, F8 },
  [DL] = { D4, L8 },
  [DB] = { D8, B8 },
  [FR] = { F6, R4 },
  [FL] = { F4, L6 },
  [BL] = { B6, L4 },
  [BR] = { B4, R6 },
};

// Maps a corner to the colors that make it up.
static color_t CORNER_TO_COLORS[CORNER_COUNT][FACELETS_PER_CORNER] = {
  [URF] = { U, R, F },
  [UFL] = { U, F, L },
  [ULB] = { U, L, B },
  [UBR] = { U, B, R },
  [DFR] = { D, F, R },
  [DLF] = { D, L, F },
  [DBL] = { D, B, L },
  [DRB] = { D, R, B },
};

// Maps an edge to the colors that make it up.
static color_t EDGE_TO_COLORS[EDGE_COUNT][FACELETS_PER_EDGE] = {
  [UR] = { U, R },
  [UF] = { U, F },
  [UL] = { U, L },
  [UB] = { U, B },
  [DR] = { D, R },
  [DF] = { D, F },
  [DL] = { D, L },
  [DB] = { D, B },
  [FR] = { F, R },
  [FL] = { F, L },
  [BL] = { B, L },
  [BR] = { B, R },
};

static int Cnk(int n, int k) {
  int i, j, s;
  if (n < k)
    return 0;
  if (k > n / 2)
    k = n - k;
  for (s = 1, i = n, j = 1; i != n - k; i--, j++) {
    s *= i;
    s /= j;
  }
  return s;
}

static void rotateLeft_corner(corner_t* arr, int l, int r) {
  int i;
  corner_t temp = arr[l];
  for (i = l; i < r; i++)
    arr[i] = arr[i + 1];
  arr[r] = temp;
}

static void rotateLeft_edge(edge_t* arr, int l, int r) {
  int i;
  edge_t temp = arr[l];
  for (i = l; i < r; i++)
    arr[i] = arr[i + 1];
  arr[r] = temp;
}

static short getTwist(cubiecube_t* cubiecube) {
  short ret = 0;
  int i;
  for (i = URF; i < DRB; i++)
    ret = (short) (3 * ret + cubiecube->co[i]);
  return ret;
}

static short getFlip(cubiecube_t* cubiecube) {
  int i;
  short ret = 0;
  for (i = UR; i < BR; i++)
    ret = (short) (2 * ret + cubiecube->eo[i]);
  return ret;
}

static short getFRtoBR(cubiecube_t* cubiecube) {
  int a = 0, x = 0, j;
  int b = 0;
  edge_t edge4[4] = {0};
  // compute the index a < (12 choose 4) and the permutation array perm.
  for (j = BR; j >= UR; j--)
    if (FR <= cubiecube->ep[j] && cubiecube->ep[j] <= BR) {
      a += Cnk(11 - j, x + 1);
      edge4[3 - x++] = cubiecube->ep[j];
    }

  for (j = 3; j > 0; j--)// compute the index b < 4! for the
  // permutation in perm
  {
    int k = 0;
    while (edge4[j] != j + 8) {
      rotateLeft_edge(edge4, 0, j);
      k++;
    }
    b = (j + 1) * b + k;
  }
  return (short) (24 * a + b);
}

static short getURFtoDLF(cubiecube_t* cubiecube) {
  int a = 0, x = 0, j, b = 0;
  corner_t corner6[6] = {0};
  // compute the index a < (8 choose 6) and the corner permutation.
  for (j = URF; j <= DRB; j++)
    if (cubiecube->cp[j] <= DLF) {
      a += Cnk(j, x + 1);
      corner6[x++] = cubiecube->cp[j];
    }

  for (j = 5; j > 0; j--)// compute the index b < 6! for the
  // permutation in corner6
  {
    int k = 0;
    while (corner6[j] != j) {
      rotateLeft_corner(corner6, 0, j);
      k++;
    }
    b = (j + 1) * b + k;
  }
  return (short) (720 * a + b);
}

static int getURtoDF(cubiecube_t* cubiecube) {
  int a = 0, x = 0;
  int b = 0, j;
  edge_t edge6[6] = {0};
  // compute the index a < (12 choose 6) and the edge permutation.
  for (j = UR; j <= BR; j++)
    if (cubiecube->ep[j] <= DF) {
      a += Cnk(j, x + 1);
      edge6[x++] = cubiecube->ep[j];
    }

  for (j = 5; j > 0; j--)// compute the index b < 6! for the
  // permutation in edge6
  {
    int k = 0;
    while (edge6[j] != j) {
      rotateLeft_edge(edge6, 0, j);
      k++;
    }
    b = (j + 1) * b + k;
  }
  return 720 * a + b;
}

static short getURtoUL(cubiecube_t* cubiecube) {
  int a = 0, b = 0, x = 0, j;
  edge_t edge3[3] = {0};
  // compute the index a < (12 choose 3) and the edge permutation.
  for (j = UR; j <= BR; j++)
    if (cubiecube->ep[j] <= UL) {
      a += Cnk(j, x + 1);
      edge3[x++] = cubiecube->ep[j];
    }

  for (j = 2; j > 0; j--)// compute the index b < 3! for the
  // permutation in edge3
  {
    int k = 0;
    while (edge3[j] != j) {
      rotateLeft_edge(edge3, 0, j);
      k++;
    }
    b = (j + 1) * b + k;
  }
  return (short) (6 * a + b);
}

static short getUBtoDF(cubiecube_t* cubiecube) {
  int a = 0, x = 0, b = 0, j;
  edge_t edge3[3] = {0};
  // compute the index a < (12 choose 3) and the edge permutation.
  for (j = UR; j <= BR; j++)
    if (UB <= cubiecube->ep[j] && cubiecube->ep[j] <= DF) {
      a += Cnk(j, x + 1);
      edge3[x++] = cubiecube->ep[j];
    }

  for (j = 2; j > 0; j--)// compute the index b < 3! for the
  // permutation in edge3
  {
    int k = 0;
    while (edge3[j] != UB + j) {
      rotateLeft_edge(edge3, 0, j);
      k++;
    }
    b = (j + 1) * b + k;
  }
  return (short) (6 * a + b);
}

static uint8_t corner_parity(cubiecube_t* cubiecube) {
  int i, j, sum = 0;

  for (i = CORNER_FIRST + 1; i <= CORNER_LAST; i++)
    for (j = CORNER_FIRST; j < i; j++)
      if (cubiecube->cp[j] > cubiecube->cp[i]) sum += 1;

  return (uint8_t) (sum & 1);
}

static uint8_t edge_parity(cubiecube_t* cubiecube) {
  int i, j, sum = 0;

  for (i = EDGE_FIRST + 1; i <= EDGE_LAST; i++)
    for (j = EDGE_FIRST; j < i; j++)
      if (cubiecube->ep[j] > cubiecube->ep[i]) sum += 1;

  return (uint8_t) (sum & 1);
}

void string_to_facecube(const char *cubestring, facecube_t *facecube) {
  int i;

  for (i = 0; i < FACELET_COUNT; i++) {
    switch (cubestring[i]) {

    case 'U': facecube->facelets[i] = U; break;
    case 'R': facecube->facelets[i] = R; break;
    case 'F': facecube->facelets[i] = F; break;
    case 'D': facecube->facelets[i] = D; break;
    case 'L': facecube->facelets[i] = L; break;
    case 'B': facecube->facelets[i] = B; break;

    }
  }
}

void facecube_to_cubiecube(facecube_t *facecube, cubiecube_t *cubiecube) {
  int i, j;
  int8_t ori;
  color_t col1, col2;

  for (i = 0; i < CORNER_COUNT; i++)
    cubiecube->cp[i] = CORNER_FIRST; // invalidate corners
  for (i = 0; i < EDGE_COUNT; i++)
    cubiecube->ep[i] = EDGE_FIRST;   // and edges

  for (i = 0; i < CORNER_COUNT; i++) {
    // get the colors of the cubie at corner i, starting with U/D
    for (ori = 0; ori < FACELETS_PER_CORNER; ori++)
      if (facecube->facelets[CORNER_TO_FACELETS[i][ori]] == U ||
          facecube->facelets[CORNER_TO_FACELETS[i][ori]] == D) break;

    col1 = facecube->facelets[CORNER_TO_FACELETS[i][(ori + 1) % 3]];
    col2 = facecube->facelets[CORNER_TO_FACELETS[i][(ori + 2) % 3]];

    for (j = 0; j < CORNER_COUNT; j++) {
      if (col1 == CORNER_TO_COLORS[j][1] &&
          col2 == CORNER_TO_COLORS[j][2]) {
        // in cornerposition i we have cornercubie j
        cubiecube->cp[i] = j;
        cubiecube->co[i] = ori % 3;
        break;
      }
    }
  }

  for (i = 0; i < EDGE_COUNT; i++) {
    for (j = 0; j < EDGE_COUNT; j++) {
      if (facecube->facelets[EDGE_TO_FACELETS[i][0]] == EDGE_TO_COLORS[j][0] &&
          facecube->facelets[EDGE_TO_FACELETS[i][1]] == EDGE_TO_COLORS[j][1]) {
        cubiecube->ep[i] = j;
        cubiecube->eo[i] = 0;
        break;
      }
      if (facecube->facelets[EDGE_TO_FACELETS[i][0]] == EDGE_TO_COLORS[j][1] &&
          facecube->facelets[EDGE_TO_FACELETS[i][1]] == EDGE_TO_COLORS[j][0]) {
        cubiecube->ep[i] = j;
        cubiecube->eo[i] = 1;
        break;
      }
    }
  }
}

void cubiecube_to_coordcube(cubiecube_t *cubiecube, coordcube_t *coordcube) {
  coordcube->twist    = getTwist(cubiecube);
  coordcube->flip     = getFlip(cubiecube);
  coordcube->parity   = corner_parity(cubiecube);
  coordcube->slice2   = getFRtoBR(cubiecube);
  coordcube->corner = getURFtoDLF(cubiecube);
  coordcube->edgeU   = getURtoUL(cubiecube);
  coordcube->edgeD   = getUBtoDF(cubiecube);
  coordcube->edgeUD   = getURtoDF(cubiecube); // only needed in phase2
}

/**
 * Checks the length of a cubestring, if it has the correct number of each
 * color, and if the center facelets are in the correct positions.
*/
bool validate_string(const char *cubestring) {
  int i;
  int color_count[COLOR_COUNT] = { 0 };

  if (strspn(cubestring, "URFDLB") != FACELET_COUNT) {
    printf("Cubestring length must be exactly %d.\n", FACELET_COUNT);
    return false;
  }

  for (i = 0; i < FACELET_COUNT; i++) {
    switch (cubestring[i]) {

    case 'U': color_count[U]++; break;
    case 'R': color_count[R]++; break;
    case 'F': color_count[F]++; break;
    case 'D': color_count[D]++; break;
    case 'L': color_count[L]++; break;
    case 'B': color_count[B]++; break;

    }
  }

  for (i = 0; i < COLOR_COUNT; i++) {
    if (color_count[i] != FACELET_COUNT / COLOR_COUNT) {
      printf(
        "Cubestring must contain exactly %d of each color.\n",
        FACELET_COUNT / COLOR_COUNT
      );
      return false;
    }
  }

  if (cubestring[ 4] != 'U' || cubestring[13] != 'R' || cubestring[22] != 'F' ||
      cubestring[31] != 'D' || cubestring[40] != 'L' || cubestring[49] != 'B') {
    puts("Center facelets are in the wrong order. See the help text.");
    return false;
  }

  return true;
}

bool validate_cubiecube(cubiecube_t *cubiecube) {
  int i, parity_sum;
  int edge_count[EDGE_COUNT]     = { 0 };
  int corner_count[CORNER_COUNT] = { 0 };

  for (i = 0, parity_sum = 0; i < EDGE_COUNT; i++) {
    edge_count[cubiecube->ep[i]]++;
    parity_sum += cubiecube->eo[i];
  }
  for (i = 0; i < EDGE_COUNT; i++)
    if (edge_count[i] != 1) return false;   // missing edges
  if (parity_sum % 2) return false;         // flipped edgea

  for (i = 0, parity_sum = 0; i < CORNER_COUNT; i++) {
    corner_count[cubiecube->cp[i]]++;
    parity_sum += cubiecube->co[i];
  }
  for (i = 0; i < CORNER_COUNT; i++)
    if (corner_count[i] != 1) return false; // missing corners
  if (parity_sum % 3) return false;         // twisted corner

  if (edge_parity(cubiecube) ^ corner_parity(cubiecube))
    return false;                           // parity error

  return true;                              // cube ok
}
