#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "search.h"

extern uint64_t p1, p2, p3, p4, p5;
uint64_t p6 = 0, p7 = 0;

#define SOLUTIONS         10000

int main(int argc, char **argv) {
  int       solutions = 0;
  char      line_buf[SOLUTION_BUFSIZ];
  char      sol_buf[SOLUTION_BUFSIZ];
  size_t    sol_len;
  options_t options = DEFAULT_OPTIONS;
  int       argn, i;

  for (argn = 1; argn < argc; argn++) {
#define arg_match(lit) !strncmp(argv[argn], lit, sizeof(lit) - 1)
#define arg_get(lit)   (arg_match(lit) && argn++)
#define arg_geteq(lit) (arg_match(lit) && (argv[argn] += sizeof(lit) - 1))

    if (arg_get("-d") || arg_geteq("--depth=")) {
      options.max_depth = atoi(argv[argn]);
    }

    else if (arg_get("-t") || arg_geteq("--time=")) {
      options.timeout_sec = atoi(argv[argn]);
    }

    else if (arg_match("-s")) {
      options.use_separator = true;
    }

    else if (arg_get("-c") || arg_geteq("--cache=")) {
      options.cache_dir = argv[argn];
    }

    else {
      fprintf(stderr, "Unknown argument %s\n", argv[argn]);
      exit(EXIT_FAILURE);
    }
  }

  while (++solutions < SOLUTIONS) {
    if (!fgets(line_buf, sizeof(line_buf), stdin)) break;
    if (!solution(line_buf, &options, sol_buf)) {
      printf("%d: %s\n", solutions, line_buf);
      exit(EXIT_FAILURE);
    }
    sol_len = strlen(sol_buf);
    for (i = 0; i < sol_len; i++) {
      if (sol_buf[i] == ' ' || sol_buf[i] == '\'') continue;
      p6 += 1; // number of total moves (QTM)
      if (sol_buf[i] == 'U')
        p7 += 1; // number of U moves (HTM)
    }

    if (!fgets(line_buf, sizeof(line_buf), stdin)) break;
    if (solutions % (SOLUTIONS / 100) == 0) {
      printf("\rProgress: %3d%%", solutions / (SOLUTIONS / 100));
      fflush(stdout);
    }
  }
  printf("\rProgress: %3d%%\n", 100);

  printf(
    "                                            TOTAL | PER SOLVE\n"
    "number of phase 1 nodes expanded:    %12ld | %9ld\n"
    "number of phase 1 solutions found:   %12ld | %9ld\n"
    "number of phase 2 trees explored:    %12ld | %9ld\n"
    "number of phase 2 nodes expanded:    %12ld | %9ld\n"
    "number of phase 2 solutions found:   %12ld | %9.02f\n"
    "number of moves in solution (QTM):   %12ld | %9.02f\n"
    "number of U moves in solution (HTM): %12ld | %9.02f\n\n",
    p1, p1 / solutions,
    p2, p2 / solutions,
    p3, p3 / solutions,
    p4, p4 / solutions,
    p5, (double)p5 / (double)solutions,
    p6, (double)p6 / (double)solutions,
    p7, (double)p7 / (double)solutions
  );

  return EXIT_SUCCESS;
}
