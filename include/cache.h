#pragma once

#include <stdbool.h>
#include <stdint.h>

#define N_TWIST     2187
#define N_FLIP      2048
#define N_SLICE1    495
#define N_SLICE2    24
#define N_PARITY    2
#define N_URFtoDLF  20160
#define N_FRtoBR    11880
#define N_URtoUL    1320
#define N_UBtoDF    1320
#define N_URtoDF    20160
#define N_URFtoDLB  40320
#define N_URtoBR    479001600
#define N_MOVE      18

// ******************************************Phase 1 move tables*****************************************************

// Move table for the twists of the corners
// twist < 2187 in phase 2.
// twist = 0 in phase 2.
extern short MOVE_TWIST[N_TWIST][N_MOVE];

// Move table for the flips of the edges
// flip < 2048 in phase 1
// flip = 0 in phase 2.
extern short MOVE_FLIP[N_FLIP][N_MOVE];

// Parity of the corner permutation. This is the same as the parity for the edge permutation of a valid cube.
// parity has values 0 and 1
extern short MOVE_PARITY[N_PARITY][N_MOVE];

// ***********************************Phase 1 and 2 movetable********************************************************

// Move table for the four UD-slice1 edges FR, FL, Bl and BR
// FRtoBRMove < 11880 in phase 1
// FRtoBRMove < 24 in phase 2
// FRtoBRMove = 0 for solved cube
extern short MOVE_SLICE[N_FRtoBR][N_MOVE];

// Move table for permutation of six corners. The positions of the DBL and DRB corners are determined by the parity.
// corner < 20160 in phase 1
// corner < 20160 in phase 2
// corner = 0 for solved cube.
extern short MOVE_CORNER[N_URFtoDLF][N_MOVE];

// Move table for the permutation of six U-face and D-face edges in phase2. The positions of the DL and DB edges are
// determined by the parity.
// edgeUD < 665280 in phase 1
// edgeUD < 20160 in phase 2
// edgeUD = 0 for solved cube.
extern short MOVE_EDGE_UD[N_URtoDF][N_MOVE];

// **************************helper move tables to compute edgeUD for the beginning of phase2************************

// Move table for the three edges UR,UF and UL in phase1.
extern short MOVE_EDGE_U[N_URtoUL][N_MOVE];

// Move table for the three edges UB,DR and DF in phase1.
extern short MOVE_EDGE_D[N_UBtoDF][N_MOVE];

// Table to merge the coordinates of the UR,UF,UL and UB,DR,DF edges at the beginning of phase2
extern short MERGE_EDGE_UD[336][336];

// ****************************************Pruning tables for the search*********************************************

// Pruning table for the permutation of the corners and the UD-slice1 edges in phase2.
// The pruning table entries give a lower estimation for the number of moves to reach the solved cube.
#define SLICE_URF_DLF_PARITY_PRUN_CAPACITY (N_SLICE2 * N_URFtoDLF * N_PARITY / 2)
extern int8_t PRUNE_CORNER[SLICE_URF_DLF_PARITY_PRUN_CAPACITY];

// Pruning table for the permutation of the edges in phase2.
// The pruning table entries give a lower estimation for the number of moves to reach the solved cube.
#define SLICE_UR_DF_PARITY_PRUN_CAPACITY (N_SLICE2 * N_URtoDF * N_PARITY / 2)
extern int8_t PRUNE_EDGE[SLICE_UR_DF_PARITY_PRUN_CAPACITY];

// Pruning table for the twist of the corners and the position (not permutation) of the UD-slice1 edges in phase1
// The pruning table entries give a lower estimation for the number of moves to reach the H-subgroup.
#define SLICE_TWIST_PRUN_CAPACITY (N_SLICE1 * N_TWIST / 2 + 1)
extern int8_t PRUNE_TWIST[SLICE_TWIST_PRUN_CAPACITY];

// Pruning table for the flip of the edges and the position (not permutation) of the UD-slice1 edges in phase1
// The pruning table entries give a lower estimation for the number of moves to reach the H-subgroup.
#define SLICE_FLIP_PRUN_CAPACITY (N_SLICE1 * N_FLIP / 2)
extern int8_t PRUNE_FLIP[SLICE_FLIP_PRUN_CAPACITY];

extern bool CACHE_OK;
void init_cache(const char *cache_dir);
signed char getPruning(signed char *table, int index);
