#include <stdlib.h>
#include <stdio.h>

#include "cache.h"
#include "cube.h"
#include "search.h"

#define MIN(a, b) (((a)<(b))?(a):(b))
#define MAX(a, b) (((a)>(b))?(a):(b))

uint64_t p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5 = 0;

bool solution(const char *cubestring, options_t *options, char* solution) {
  search_t    search;
  facecube_t  facecube;
  cubiecube_t cubiecube;
  coordcube_t coordcube;

  int     phase1_depth, phase2_depth, max_phase2_depth, total_depth;
  int     move, n1, n2, next, i;
  bool    busy;
  int8_t  pruneA, pruneB;

  // ------------------------------- validation --------------------------------

  if (!validate_string(cubestring)) return false;
  string_to_facecube(cubestring, &facecube);
  facecube_to_cubiecube(&facecube, &cubiecube);
  if (!validate_cubiecube(&cubiecube)) return false;
  cubiecube_to_coordcube(&cubiecube, &coordcube);

  // ----------------------------- initialization ------------------------------

  if (!CACHE_OK) init_cache(options->cache_dir);

  search.axis[0] = 0;
  search.power[0] = 0;

  search.flip[0] = coordcube.flip;
  search.twist[0] = coordcube.twist;
  search.slice1[0] = coordcube.slice2 / 24;

  search.parity[0] = coordcube.parity;
  search.corner[0] = coordcube.corner;
  search.slice2[0] = coordcube.slice2;
  search.edgeU[0] = coordcube.edgeU;
  search.edgeD[0] = coordcube.edgeD;

  search.min_phase1_dist[1] = 1; // else failure for depth=1, n=0
  n1 = 0;
  busy = false;
  phase1_depth = 1;
  phase2_depth = 0;

MAIN_LOOP: // --------------------- main loop ----------------------------------
  do {
    if ((phase1_depth - n1 > search.min_phase1_dist[n1 + 1]) && !busy) {
      if (search.axis[n1] == 0 || search.axis[n1] == 3)// Initialize next move
        search.axis[++n1] = 1;
      else
        search.axis[++n1] = 0;
      search.power[n1] = 1;
    } else if (++search.power[n1] > 3) {
      do {// increment axis
        if (++search.axis[n1] > 5) {

          if (n1 == 0) {
            if (phase1_depth >= options->max_depth)
              return false;
            else {
              phase1_depth++;
              search.axis[n1] = 0;
              search.power[n1] = 1;
              busy = false;
              break;
            }
          } else {
            n1--;
            busy = true;
            break;
          }

        } else {
          search.power[n1] = 1;
          busy = false;
        }
      } while (n1 != 0 && (search.axis[n1 - 1] == search.axis[n1] || search.axis[n1 - 1] - 3 == search.axis[n1]));
    } else
      busy = false;
  } while (busy);

  // ------------ compute new coordinates and new min_phase1_dist ------------
  // if min_phase1_dist=0, the H subgroup is reached

  p1 += 1; // number of nodes expanded in phase 1
  next = n1 + 1;
  move = 3 * search.axis[n1] + search.power[n1] - 1;

  search.flip[next] = MOVE_FLIP[search.flip[n1]][move];
  search.twist[next] = MOVE_TWIST[search.twist[n1]][move];
  search.slice1[next] = MOVE_SLICE1[search.slice1[n1]][move];

  pruneA = PRUNE_FLIP[512 * search.flip[next] + search.slice1[next]];
  pruneB = PRUNE_TWIST[512 * search.twist[next] + search.slice1[next]];
  search.min_phase1_dist[next] = MAX(pruneA, pruneB);

  if (search.min_phase1_dist[next] != 0)  goto MAIN_LOOP; // not in H subgroup
  if (n1 < phase1_depth - 5)              goto MAIN_LOOP;
  search.min_phase1_dist[next] = 10; // instead of 10 any value >5 is possible
  if (next != phase1_depth)               goto MAIN_LOOP;
  p2 += 1;                           // number of phase 1 solutions found

  // -------------------------------------------------------------------------

  max_phase2_depth = MIN(10, options->max_depth - phase1_depth); // Allow only max 10 moves in phase2

  for (n2 = 0; n2 < phase1_depth; n2++) {
    move = 3 * search.axis[n2] + search.power[n2] - 1;

    search.corner[n2 + 1] = MOVE_CORNER[search.corner[n2]][move];
    search.slice2[n2 + 1] =  MOVE_SLICE[search.slice2[n2]][move];
    search.parity[n2 + 1] = MOVE_PARITY[search.parity[n2]][move];
  }

  pruneA = PRUNE_CORNER[(((
    search.corner[phase1_depth]) * 32 +
    search.slice2[phase1_depth]) *  2 +
    search.parity[phase1_depth])];
  if (pruneA > max_phase2_depth) goto MAIN_LOOP;

  for (n2 = 0; n2 < phase1_depth; n2++) {
    move = 3 * search.axis[n2] + search.power[n2] - 1;

    search.edgeU[n2 + 1] = MOVE_EDGE_U[search.edgeU[n2]][move];
    search.edgeD[n2 + 1] = MOVE_EDGE_D[search.edgeD[n2]][move];
  }
  search.edgeUD[phase1_depth] = MERGE_EDGE_UD
    [search.edgeU[phase1_depth]] [search.edgeD[phase1_depth]];

  pruneB = PRUNE_EDGE[(((
    search.edgeUD[phase1_depth]) * 32 +
    search.slice2[phase1_depth]) *  2 +
    search.parity[phase1_depth])];
  if (pruneB > max_phase2_depth) goto MAIN_LOOP;

  search.min_phase2_dist[phase1_depth] = MAX(pruneA, pruneB);
  if (search.min_phase2_dist[phase1_depth] == 0) // already solved
    goto SOLUTION;

  p3 += 1; // number of phase 2 trees explored (prune value <= 10)

  // ----------------------------- initialization ------------------------------

  search.axis[phase1_depth] = 0;
  search.power[phase1_depth] = 0;

  search.min_phase2_dist[phase1_depth + 1] = 1;// else failure for depth=1, n=0
  n2 = phase1_depth;
  busy = false;
  phase2_depth = 1;

  // -------------------------------- main loop --------------------------------
  do {
    do {
      if ((phase1_depth + phase2_depth - n2 > search.min_phase2_dist[n2 + 1]) && !busy) {

        if (search.axis[n2] == 0 || search.axis[n2] == 3)// Initialize next move
        {
          search.axis[++n2] = 1;
          search.power[n2] = 2;
        } else {
          search.axis[++n2] = 0;
          search.power[n2] = 1;
        }
      } else if ((search.axis[n2] == 0 || search.axis[n2] == 3) ? (++search.power[n2] > 3) : ((search.power[n2] = search.power[n2] + 2) > 3)) {
        do {// increment axis
          if (++search.axis[n2] > 5) {
            if (n2 == phase1_depth) {
              if (phase2_depth >= max_phase2_depth)
                goto MAIN_LOOP;
              else {
                phase2_depth++;
                search.axis[n2] = 0;
                search.power[n2] = 1;
                busy = false;
                break;
              }
            } else {
              n2--;
              busy = true;
              break;
            }

          } else {
            if (search.axis[n2] == 0 || search.axis[n2] == 3)
              search.power[n2] = 1;
            else
              search.power[n2] = 2;
            busy = false;
          }
        } while (n2 != phase1_depth && (search.axis[n2 - 1] == search.axis[n2] || search.axis[n2 - 1] - 3 == search.axis[n2]));
      } else
        busy = false;
    } while (busy);

    // ------------ compute new coordinates and new min_phase2_dist ------------
    // if min_phase2_dist=0, the solution is reached

    p4 += 1; // number of nodes expanded phase 2
    next = n2 + 1;
    move = 3 * search.axis[n2] + search.power[n2] - 1;

    search.corner[next] =  MOVE_CORNER[search.corner[n2]][move];
    search.slice2[next] =   MOVE_SLICE[search.slice2[n2]][move];
    search.parity[next] =  MOVE_PARITY[search.parity[n2]][move];
    search.edgeUD[next] = MOVE_EDGE_UD[search.edgeUD[n2]][move];

    pruneA = PRUNE_EDGE[(((
      search.edgeUD[next]) * 32 +
      search.slice2[next]) *  2 +
      search.parity[next])];
    pruneB = PRUNE_CORNER[(((
      search.corner[next]) * 32 +
      search.slice2[next]) *  2 +
      search.parity[next])];
    search.min_phase2_dist[next] = MAX(pruneA, pruneB);

    // -------------------------------------------------------------------------

  } while (search.min_phase2_dist[next] != 0);

  p5 += 1; // number of phase 2 solutions found

  if (search.axis[phase1_depth - 1] != search.axis[phase1_depth] &&
      search.axis[phase1_depth - 1] != search.axis[phase1_depth] + 3)
    goto SOLUTION;

  // ---------------------------------------------------------------------------

  goto MAIN_LOOP;

SOLUTION:

  for (i = 0; i < phase1_depth + phase2_depth; i++) {
    switch (search.axis[i]) {
    case 0: *solution++ = 'U'; break;
    case 1: *solution++ = 'R'; break;
    case 2: *solution++ = 'F'; break;
    case 3: *solution++ = 'D'; break;
    case 4: *solution++ = 'L'; break;
    case 5: *solution++ = 'B'; break;
    }
    switch (search.power[i]) {
    case 1:          break;
    case 2: *solution++ = '2';  break;
    case 3: *solution++ = '\''; break;
    }
    *solution++ = ' ';
    if (options->use_separator && i == phase1_depth - 1) {
      *solution++ = '.';
      *solution++ = ' ';
    }
  }
  *solution++ = '\0';
  return true;
}
