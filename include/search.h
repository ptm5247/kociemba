#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#define SOLUTION_BUFSIZ         256
#define MAX_SOLUTION_LENGTH_HTM 31

typedef struct search_frame {

  uint8_t axis;         // The axis of the move
  uint8_t power;        // The power of the move

  int16_t flip;         // phase1 coordinates
  int16_t twist;
  int16_t slice1;

  int16_t edgeU;        // phase2 coordinates
  int16_t edgeD;
  int16_t edgeUD;
  int16_t corner;
  int16_t slice2;
  uint8_t parity;

  int     heuristic;

} search_frame_t;

bool solution(const char *cubestring, char solution[SOLUTION_BUFSIZ]);
