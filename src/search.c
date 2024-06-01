#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "cache.h"
#include "cube.h"
#include "search.h"

#define MIN(a, b) (((a)<(b))?(a):(b))
#define MAX(a, b) (((a)>(b))?(a):(b))

uint64_t p1 = 0, p2 = 0, p3 = 0, p4 = 0, p5 = 0;

static void solution_to_string(search_t* search, int length, int phase1_depth, char *solution);
static int totalDepth(search_t* search, int phase1_depth, int maxDepth);

bool solution(const char *cubestring, options_t *options, char* solution) {
  search_t    search;
  facecube_t  facecube;
  cubiecube_t cubiecube;
  coordcube_t coordcube;

  int     phase1_depth, total_depth;
  int     move, n;
  bool    busy;
  time_t  start_time;

  // ------------------------------- validation --------------------------------

  if (!validate_string(cubestring)) return false;
  string_to_facecube(cubestring, &facecube);
  facecube_to_cubiecube(&facecube, &cubiecube);
  if (!validate_cubiecube(&cubiecube)) return false;
  cubiecube_to_coordcube(&cubiecube, &coordcube);

  // ----------------------------- initialization ------------------------------

  if (!CACHE_OK) init_cache(options->cache_dir);

  search.power[0] = 0;
  search.axis[0] = 0;
  search.flip[0] = coordcube.flip;
  search.twist[0] = coordcube.twist;
  search.parity[0] = coordcube.parity;
  search.slice1[0] = coordcube.slice2 / 24;
  search.corner[0] = coordcube.corner;
  search.slice2[0] = coordcube.slice2;
  search.edgeU[0] = coordcube.edgeU;
  search.edgeD[0] = coordcube.edgeD;

  search.min_phase1_dist[1] = 1;// else failure for depth=1, n=0
  move = 0;
  n = 0;
  busy = false;
  phase1_depth = 1;

  start_time = time(NULL);

  // -------------------------------- main loop --------------------------------
  do {
    do {
      if ((phase1_depth - n > search.min_phase1_dist[n + 1]) && !busy) {
        if (search.axis[n] == 0 || search.axis[n] == 3)// Initialize next move
          search.axis[++n] = 1;
        else
          search.axis[++n] = 0;
        search.power[n] = 1;
      } else if (++search.power[n] > 3) {
        do {// increment axis
          if (++search.axis[n] > 5) {

            if (time(NULL) - start_time > options->timeout_sec)
              return false;

            if (n == 0) {
              if (phase1_depth >= options->max_depth)
                return false;
              else {
                phase1_depth++;
                search.axis[n] = 0;
                search.power[n] = 1;
                busy = false;
                break;
              }
            } else {
              n--;
              busy = true;
              break;
            }

          } else {
            search.power[n] = 1;
            busy = false;
          }
        } while (n != 0 && (search.axis[n - 1] == search.axis[n] || search.axis[n - 1] - 3 == search.axis[n]));
      } else
        busy = false;
    } while (busy);

    p1 += 1; // number of nodes expanded in phase 1

    // +++++++++++++ compute new coordinates and new min_phase1_dist ++++++++++
    // if min_phase1_dist =0, the H subgroup is reached
    move = 3 * search.axis[n] + search.power[n] - 1;
    search.flip[n + 1] = MOVE_FLIP[search.flip[n]][move];
    search.twist[n + 1] = MOVE_TWIST[search.twist[n]][move];
    search.slice1[n + 1] = MOVE_SLICE[search.slice1[n] * 24][move] / 24;
    search.min_phase1_dist[n + 1] = MAX(
      getPruning(PRUNE_FLIP, N_SLICE1 * search.flip[n + 1] + search.slice1[n + 1]),
      getPruning(PRUNE_TWIST, N_SLICE1 * search.twist[n + 1] + search.slice1[n + 1])
    );
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (search.min_phase1_dist[n + 1] == 0 && n >= phase1_depth - 5) {

      p2 += 1; // number of phase 1 solutions found

      search.min_phase1_dist[n + 1] = 10;// instead of 10 any value >5 is possible
      if (n == phase1_depth - 1 && (total_depth = totalDepth(&search, phase1_depth, options->max_depth)) >= 0) {
        if (total_depth == phase1_depth
            || (search.axis[phase1_depth - 1] != search.axis[phase1_depth] && search.axis[phase1_depth - 1] != search.axis[phase1_depth] + 3)) {
          if (options->use_separator) {
            solution_to_string(&search, total_depth, phase1_depth, solution);
          } else {
            solution_to_string(&search, total_depth, -1, solution);
          }
          return true;
        }
      }

    }
  } while (1);
}

static int totalDepth(search_t* search, int phase1_depth, int max_depth) {
  int move = 0, d1 = 0, d2 = 0, i;
  int max_phase2_depth = MIN(10, max_depth - phase1_depth);// Allow only max 10 moves in phase2
  int phase2_depth;
  int n;
  bool busy;

  for (i = 0; i < phase1_depth; i++) {
    move = 3 * search->axis[i] + search->power[i] - 1;
    search->corner[i + 1] = MOVE_CORNER[search->corner[i]][move];
    search->slice2[i + 1] = MOVE_SLICE[search->slice2[i]][move];
    search->parity[i + 1] = MOVE_PARITY[search->parity[i]][move];
  }

  if ((d1 = getPruning(PRUNE_CORNER,
      (N_SLICE2 * search->corner[phase1_depth] + search->slice2[phase1_depth]) * 2 + search->parity[phase1_depth])) > max_phase2_depth)
    return -1;

  for (i = 0; i < phase1_depth; i++) {
    move = 3 * search->axis[i] + search->power[i] - 1;
    search->edgeU[i + 1] = MOVE_EDGE_U[search->edgeU[i]][move];
    search->edgeD[i + 1] = MOVE_EDGE_D[search->edgeD[i]][move];
  }
  search->edgeUD[phase1_depth] = MERGE_EDGE_UD[search->edgeU[phase1_depth]][search->edgeD[phase1_depth]];

  if ((d2 = getPruning(PRUNE_EDGE,
      (N_SLICE2 * search->edgeUD[phase1_depth] + search->slice2[phase1_depth]) * 2 + search->parity[phase1_depth])) > max_phase2_depth)
    return -1;

  if ((search->min_phase2_dist[phase1_depth] = MAX(d1, d2)) == 0)// already solved
    return phase1_depth;

  p3 += 1; // number of phase 2 trees explored (prune value <= 10)

  // now set up search

  phase2_depth = 1;
  n = phase1_depth;
  busy = false;
  search->power[phase1_depth] = 0;
  search->axis[phase1_depth] = 0;
  search->min_phase2_dist[n + 1] = 1;// else failure for depthPhase2=1, n=0
  // +++++++++++++++++++ end initialization +++++++++++++++++++++++++++++++++
  do {
    do {
      if ((phase1_depth + phase2_depth - n > search->min_phase2_dist[n + 1]) && !busy) {

        if (search->axis[n] == 0 || search->axis[n] == 3)// Initialize next move
        {
          search->axis[++n] = 1;
          search->power[n] = 2;
        } else {
          search->axis[++n] = 0;
          search->power[n] = 1;
        }
      } else if ((search->axis[n] == 0 || search->axis[n] == 3) ? (++search->power[n] > 3) : ((search->power[n] = search->power[n] + 2) > 3)) {
        do {// increment axis
          if (++search->axis[n] > 5) {
            if (n == phase1_depth) {
              if (phase2_depth >= max_phase2_depth)
                return -1;
              else {
                phase2_depth++;
                search->axis[n] = 0;
                search->power[n] = 1;
                busy = false;
                break;
              }
            } else {
              n--;
              busy = true;
              break;
            }

          } else {
            if (search->axis[n] == 0 || search->axis[n] == 3)
              search->power[n] = 1;
            else
              search->power[n] = 2;
            busy = false;
          }
        } while (n != phase1_depth && (search->axis[n - 1] == search->axis[n] || search->axis[n - 1] - 3 == search->axis[n]));
      } else
        busy = false;
    } while (busy);

    p4 += 1; // number of nodes expanded phase 2

    // +++++++++++++ compute new coordinates and new minDist ++++++++++
    move = 3 * search->axis[n] + search->power[n] - 1;

    search->corner[n + 1] = MOVE_CORNER[search->corner[n]][move];
    search->slice2[n + 1] = MOVE_SLICE[search->slice2[n]][move];
    search->parity[n + 1] = MOVE_PARITY[search->parity[n]][move];
    search->edgeUD[n + 1] = MOVE_EDGE_UD[search->edgeUD[n]][move];

    search->min_phase2_dist[n + 1] = MAX(getPruning(PRUNE_EDGE, (N_SLICE2
        * search->edgeUD[n + 1] + search->slice2[n + 1])
        * 2 + search->parity[n + 1]), getPruning(PRUNE_CORNER, (N_SLICE2
        * search->corner[n + 1] + search->slice2[n + 1])
        * 2 + search->parity[n + 1]));
    // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  } while (search->min_phase2_dist[n + 1] != 0);

  p5 += 1; // number of phase 2 solutions found

  return phase1_depth + phase2_depth;
}

static void solution_to_string(search_t* search, int length, int phase1_depth, char *solution) {
  int i;

  for (i = 0; i < length; i++) {
    switch (search->axis[i]) {

    case 0: *solution++ = 'U'; break;
    case 1: *solution++ = 'R'; break;
    case 2: *solution++ = 'F'; break;
    case 3: *solution++ = 'D'; break;
    case 4: *solution++ = 'L'; break;
    case 5: *solution++ = 'B'; break;

    }

    switch (search->power[i]) {

    case 1:          break;
    case 2: *solution++ = '2';  break;
    case 3: *solution++ = '\''; break;

    }

    *solution++ = ' ';

    if (i == phase1_depth - 1) {
      *solution++ = '.';
      *solution++ = ' ';
    }
  }

  *solution++ = '\0';
}
