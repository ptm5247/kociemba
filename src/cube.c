#include <string.h>

#include "cube.h"

uint8_t CORNER_PERMUTATION_TABLE[][MAX_PIECES] = {
  { UBR, URF, UFL, ULB, DFR, DLF, DBL, DRB, }, // U
  { DFR, UFL, ULB, URF, DRB, DLF, DBL, UBR, }, // R
  { UFL, DLF, ULB, UBR, URF, DFR, DBL, DRB, }, // F
  { URF, UFL, ULB, UBR, DLF, DBL, DRB, DFR, }, // D
  { URF, ULB, DBL, UBR, DFR, UFL, DLF, DRB, }, // L
  // { URF, UFL, UBR, DRB, DFR, DLF, ULB, DBL, }, // B
  { UBR, URF, UFL, ULB, DLF, DBL, DRB, DFR, }, // E
  { DFR, ULB, DBL, URF, DRB, UFL, DLF, UBR, }, // M
  // { UFL, DLF, UBR, DRB, URF, DFR, ULB, DBL, }, // S
}, CORNER_ORIENTATION_TABLE[][MAX_PIECES] = {
  {   0,   0,   0,   0,   0,   0,   0,   0, }, // U
  {   2,   0,   0,   1,   1,   0,   0,   2, }, // R
  {   1,   2,   0,   0,   2,   1,   0,   0, }, // F
  {   0,   0,   0,   0,   0,   0,   0,   0, }, // D
  {   0,   1,   2,   0,   0,   2,   1,   0, }, // L
  // {   0,   0,   1,   2,   0,   0,   2,   1, }, // B
  {   0,   0,   0,   0,   0,   0,   0,   0, }, // E
  {   2,   1,   2,   1,   1,   2,   1,   2, }, // M
  // {   1,   2,   1,   2,   2,   1,   2,   1, }, // S
}, EDGE_PERMUTATION_TABLE[][MAX_PIECES] = {
  { UB, UR, UF, UL, DR, DF, DL, DB, FR, FL, BL, BR, }, // U
  { FR, UF, UL, UB, BR, DF, DL, DB, DR, FL, BL, UR, }, // R
  { UR, FL, UL, UB, DR, FR, DL, DB, UF, DF, BL, BR, }, // F
  { UR, UF, UL, UB, DF, DL, DB, DR, FR, FL, BL, BR, }, // D
  { UR, UF, BL, UB, DR, DF, FL, DB, FR, UL, DL, BR, }, // L
  // { UR, UF, UL, BR, DR, DF, DL, BL, FR, FL, UB, DB, }, // B
  { UB, UR, UF, UL, DF, DL, DB, DR, FR, FL, BL, BR, }, // E
  { FR, UF, BL, UB, BR, DF, FL, DB, DR, UL, DL, UR, }, // M
  // { UR, FL, UL, BR, DR, FR, DL, BL, UF, DF, UB, DB, }, // S
}, EDGE_ORIENTATION_TABLE[][MAX_PIECES] = {
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, }, // U
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, }, // R
  {  0,  1,  0,  0,  0,  1,  0,  0,  1,  1,  0,  0, }, // F
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, }, // D
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, }, // L
  // {  0,  0,  0,  1,  0,  0,  0,  1,  0,  0,  1,  1, }, // B
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, }, // E
  {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, }, // M
  // {  0,  1,  0,  1,  0,  1,  0,  1,  1,  1,  1,  1, }, // S
};

static int choose(int n, int k) {
  int i = n, j = 1, s = 1;

  if (n < k)         return 0;
  if (k > n / 2)     k = n - k;
  while (i != n - k) s = (s * i--) / j++;

  return s;
}

uint8_t get_parity(cube_t *cube) {
  int i, j, sum = 0;

  for (i = 1; i < cube->piece_count; i++)
    for (j = 0; j < i; j++)
      if (cube->permutation[j] > cube->permutation[i]) sum += 1;

  return (uint8_t) (sum & 1);
}

// orientation coordinate
//
// The orientation of a piece is the number of CCW turns (or flips) required to
// get a piece's primary color onto the piece's primary face. The primary color
// is the U or D color (or the F or B color for edges that contain nether the U
// color nor the D color), and the primary face is the U or D face (or the F or
// B face for edges which touch neither the U face nor the D face).
//
// The orientation coordinate is an X-digit, base-Y number (with Y^X values)
// where X=N_PIECES-1 and Y=FACELETS_PER_PIECE. The Nth digit (from the left)
// is the orientation of the piece in position N (i.e. for n=0 on a CornerCube,
// the corner in the URF position, not the URF corner itself). The sum of all
// orientations must be divisible by FACELETS_PER_PIECE, so the orientation of
// the final piece can be determined by parity.

uint16_t get_orientation(cube_t *cube) {
  uint16_t orientation = 0;
  int      i;

  for (i = 0; i < cube->piece_count - 1; i++) {
    orientation *= cube->facelets_per_piece;
    orientation += cube->orientation[i];
  }

  return orientation;
}

void set_orientation(cube_t *cube, uint16_t orientation) {
  int parity = 0;
  int i;

  for (i = cube->piece_count - 2; i >= 0; i--) {
    cube->orientation[i] = orientation % cube->facelets_per_piece;
    orientation /= cube->facelets_per_piece;
    parity += cube->orientation[i];
  }

  cube->orientation[cube->piece_count - 1] =
    (cube->piece_count * cube->facelets_per_piece - parity) % cube->facelets_per_piece;
}

// permutation coordinate
//
// The permutation has two components: combination and order.
// The value is determined by: perm = (combination * MAX_ORDER) + order.
// The permutation is defined for a subset S of this cube's piece list. For
// example, slice coordinate is the permutation of the E-slice edges. So, for
// the slice coordinate, S = { FR, FL, BL, BR }.
//
// For the slice coordinate example, consider the following table:
//   Postion: | UR | UF | UL | UB | DR | DF | DL | DB | FR | FL | BL | BR |
//         n: |  0 |  1 |  2 |  3 |  4 |  5 |  6 |  7 |  8 |  9 | 10 | 11 |
// Occupied?: |    | FR |    |    | BL |    |    |    | FL | BR |    |    |
// where the Occupied? row indicates the positions of each member of S.
//
// The combination component has (N_PIECES choose len(S)) possible values.
// Consider a bitmask constructed from the Occupied? row of the above table.
// The combination component = the sum of nCk for all set bits, where n = the
// bit position (from the left) and k-1 = the number of set bits to the left of
// bit n. For the above table (bitmask = 010010001100):
// combination = 1C1 + 4C2 + 8C3 + 9C4 = 189
//
// The order component has len(S)! possible values.
// Consider the ordering of S defined by reading the Occupied? row of the table
// from left to right. To calculate the order component:
// For I = len(S) downto 2:
//   Rotate the first I elements of S to the left K times such that the largest
//   of the first I elements of S ends up in the rightmost position.
//   Increment the order component by K * (I-1)!
// For the above table (S_ord = [ FR, BL, FL, BR ]):
// order = 0*3! + 2*2! + 1*1! = 5

uint16_t get_permutation(cube_t *cube, int min, int max, bool right) {
  int      max_order = 1; // len(S)!
  uint8_t  ordered[MAX_PIECES], temp;
  uint16_t combination, order, partial;
  int      n, k, i;

  combination = 0;
  order       = 0;

  if (right) for (n = cube->piece_count - 1, k = max - min; n >= 0; n--) {
    if (min <= cube->permutation[n] && cube->permutation[n] <= max) {
      ordered[k--] = cube->permutation[n];
      combination += choose(cube->piece_count - 1 - n, max - min - k);
      max_order *= k + 2;
    }
  } else for (n = 0, k = 0; n < cube->piece_count; n++) {
    if (min <= cube->permutation[n] && cube->permutation[n] <= max) {
      ordered[k++] = cube->permutation[n];
      combination += choose(n, k);
      max_order *= k;
    }
  }

  for (n = max - min; n >= 1; n--) {
    for (partial = 0; ordered[n] != min + n; partial++) {
      temp = ordered[0];
      for (i = 0; i < n; i++) ordered[i] = ordered[i + 1];
      ordered[n] = temp;
    }
    order = ((n + 1) * order) + partial;
  }

  return (combination * max_order) + order;
}

void set_permutation(
  cube_t *cube, uint16_t permutation, int min, int max, bool right) {
  int      max_order = 1; // len(S)!
  uint8_t  ordered[MAX_PIECES], temp;
  uint16_t combination, order, partial;
  int      n, k, i;

  partial = right ? 0 : (cube->piece_count - 1);
  for (i = 0; i < cube->piece_count; i++)
    cube->permutation[i] = partial;

  for (n = min, k = 0; n <= max; n++) {
    ordered[k++] = n;
    max_order *= k;
  }
  combination = permutation / max_order;
  order       = permutation % max_order;

  for (n = 1; n <= max - min; n++) {
    k = order % (n + 1);
    order /= (n + 1);
    while (k--) {
      temp = ordered[n];
      for (i = n; i >= 1; i--) ordered[i] = ordered[i - 1];
      ordered[0] = temp;
    }
  }

  if (right) for (n = 0, k = max - min; k >= 0; n++) {
    if (combination >= (partial = choose(cube->piece_count - 1 - n, k + 1))) {
      cube->permutation[n] = ordered[max - min - k--];
      combination -= partial;
    }
  } else for (n = cube->piece_count - 1, k = max - min; k >= 0; n--) {
    if (combination >= (partial = choose(n, k + 1))) {
      cube->permutation[n] = ordered[k--];
      combination -= partial;
    }
  }
}

void apply_move(cube_t *cube, int move) {
  uint8_t permutation[MAX_PIECES];
  uint8_t orientation[MAX_PIECES];
  uint8_t i, p, o;

  for (i = 0; i < cube->piece_count; i++) {
    p = cube->permutation_table[move][i];
    o = cube->orientation_table[move][i];
    permutation[i] = cube->permutation[p];
    orientation[i] = (cube->orientation[p] + o) % cube->facelets_per_piece;
  }

  memcpy(cube->permutation, permutation, sizeof(permutation));
  memcpy(cube->orientation, orientation, sizeof(orientation));
}

// twist: corner orientation coordinate [0, 3^7) (2187)

void set_twist(cube_t *cube, uint16_t twist) {
  return set_orientation(cube, twist);
}

uint16_t get_twist(cube_t *cube) {
  return get_orientation(cube);
}

// corner: corner permutation coordinate [0, 8C6 * 6!) (20160)

void set_corner(cube_t *cube, uint16_t corner) {
  return set_permutation(cube, corner, URF, DLF, false);
}

uint16_t get_corner(cube_t *cube) {
  return get_permutation(cube, URF, DLF, false);
}

// flip: edge orientation coordinate [0, 2^11) (2048)

void set_flip(cube_t *cube, uint16_t flip) {
  return set_orientation(cube, flip);
}

uint16_t get_flip(cube_t *cube) {
  return get_orientation(cube);
}

// slice: E edge permutation coordinate [0, 12C4 * 4!) (11880)

void set_slice(cube_t *cube, uint16_t slice) {
  return set_permutation(cube, slice, FR, BR, true);
}

uint16_t get_slice(cube_t *cube) {
  return get_permutation(cube, FR, BR, true);
}

// edgeU: Phase 1 U edge permutation coordinate [0, 12C3 * 3!) (1320)

void set_edgeU(cube_t *cube, uint16_t edgeU) {
  return set_permutation(cube, edgeU, UR, UL, false);
}

uint16_t get_edgeU(cube_t *cube) {
  return get_permutation(cube, UR, UL, false);
}

// edgeD: Phase 1 D edge permutation coordinate [0, 12C3 * 3!) (1320)

void set_edgeD(cube_t *cube, uint16_t edgeD) {
  return set_permutation(cube, edgeD, UB, DF, false);
}

uint16_t get_edgeD(cube_t *cube) {
  return get_permutation(cube, UB, DF, false);
}

// edgeUD: Phase 2 non-E edge permutation coordinate [0, 8C6 * 6!) (20160)

void set_edgeUD(cube_t *cube, uint16_t edgeUD) {
  return set_permutation(cube, edgeUD, UR, DF, false);
}

uint16_t get_edgeUD(cube_t *cube) {
  return get_permutation(cube, UR, DF, false);
}
