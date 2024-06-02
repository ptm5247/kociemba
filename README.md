## Version 0.0 (reference)
### Changes
* removed python implementation and python wrapper
* reorganized files
* added performance counters
* replaced `solve.c` with a full test program `test.c`
### Goals
* set up project
### Output
```
Version 0.0
Cache Size: 4368407
Progress: 100%
                                            TOTAL | PER SOLVE
number of phase 1 nodes expanded:      7545950972 |    754595
number of phase 1 solutions found:        2132356 |       213
number of phase 2 trees explored:         1228850 |       122
number of phase 2 nodes expanded:      1025050731 |    102505
number of phase 2 solutions found:          11685 |      1.17
number of moves in solution (QTM):         304123 |     30.41
number of U moves in solution (HTM):        40619 |      4.06

92.65user 0.00system 1:32.65elapsed 100%CPU (0avgtext+0avgdata 6072maxresident)k
0inputs+0outputs (0major+631minor)pagefaults 0swaps
```
### Results:
|                       | Reference |  Previous |   Current | Difference | Cumulative |
|----------------------:|:---------:|:---------:|:---------:|:----------:|:----------:|
|              Time (s) |     92.65 |     92.65 |     92.65 |        +0% |        +0% |
|            Memory (B) | 4,368,407 | 4,368,407 | 4,368,407 |        +0% |        +0% |
| Solution Length (QTM) |     30.41 |     30.41 |     30.41 |        +0% |        +0% |
|         U Moves (HTM) |      4.06 |      4.06 |      4.06 |        +0% |        +0% |

## Version 0.1
### Changes
* moved cache creation into python
* reorganized cache
* reorganized code
* removed patternizing
* redid CLI
* removed explicit heap usage
### Goals
* organize code
* strip unneeded code
### Output
```
Version 0.1
Cache Size: 4368407
Progress: 100%
                                            TOTAL | PER SOLVE
number of phase 1 nodes expanded:      7545950972 |    754595
number of phase 1 solutions found:        2132356 |       213
number of phase 2 trees explored:         1228850 |       122
number of phase 2 nodes expanded:      1025050731 |    102505
number of phase 2 solutions found:          11685 |      1.17
number of moves in solution (QTM):         304123 |     30.41
number of U moves in solution (HTM):        40619 |      4.06

87.32user 0.00system 1:27.33elapsed 99%CPU (0avgtext+0avgdata 5988maxresident)k
0inputs+0outputs (0major+630minor)pagefaults 0swaps
```
### Results:
|                       | Reference |  Previous |   Current | Difference | Cumulative |
|----------------------:|:---------:|:---------:|:---------:|:----------:|:----------:|
|              Time (s) |     92.65 |     92.65 |     87.32 |     -5.75% |     -5.75% |
|            Memory (B) | 4,368,407 | 4,368,407 | 4,368,407 |        +0% |        +0% |
| Solution Length (QTM) |     30.41 |     30.41 |     30.41 |        +0% |        +0% |
|         U Moves (HTM) |      4.06 |      4.06 |      4.06 |        +0% |        +0% |

## Version 0.2
### Changes
* fixed inefficient macro usage
### Goals
* save 1 prune table lookup per node expanded (~850,000 per solve, ~33% of all lookups)
### Output
```
Version 0.2
Cache Size: 4368407
Progress: 100%
                                            TOTAL | PER SOLVE
number of phase 1 nodes expanded:      7545950972 |    754595
number of phase 1 solutions found:        2132356 |       213
number of phase 2 trees explored:         1228850 |       122
number of phase 2 nodes expanded:      1025050731 |    102505
number of phase 2 solutions found:          11685 |      1.17
number of moves in solution (QTM):         304123 |     30.41
number of U moves in solution (HTM):        40619 |      4.06

52.34user 0.00system 0:52.35elapsed 99%CPU (0avgtext+0avgdata 6080maxresident)k
0inputs+0outputs (0major+631minor)pagefaults 0swaps
```
### Results:
|                       | Reference |  Previous |   Current | Difference | Cumulative |
|----------------------:|:---------:|:---------:|:---------:|:----------:|:----------:|
|              Time (s) |     92.65 |     87.32 |     52.34 |    -40.06% |    -43.51% |
|            Memory (B) | 4,368,407 | 4,368,407 | 4,368,407 |        +0% |        +0% |
| Solution Length (QTM) |     30.41 |     30.41 |     30.41 |        +0% |        +0% |
|         U Moves (HTM) |      4.06 |      4.06 |      4.06 |        +0% |        +0% |

## Version 0.3
### Changes
* decreased prune table density to 1 entry per byte
* extract phase 1 subset of MOVE_SLICE into MOVE_SLICE1
* expand cache layout to use factors of 2 instead of N_SLICE1 and N_SLICE2
### Goals
* save 2 function calls & some math per node expanded (~1,700,000 per solve)
* save 1 multiply and 1 divide per phase 1 node (~750,000 per solve)
* exchange 2 multiplies for shifts per node expanded (~1,700,000 per solve)
### Output
```
Version 0.3
Cache Size: 7119184
Progress: 100%
                                            TOTAL | PER SOLVE
number of phase 1 nodes expanded:      7545950972 |    754595
number of phase 1 solutions found:        2132356 |       213
number of phase 2 trees explored:         1228850 |       122
number of phase 2 nodes expanded:      1025050731 |    102505
number of phase 2 solutions found:          11685 |      1.17
number of moves in solution (QTM):         304123 |     30.41
number of U moves in solution (HTM):        40619 |      4.06

40.55user 0.00system 0:40.56elapsed 99%CPU (0avgtext+0avgdata 8688maxresident)k
0inputs+0outputs (0major+792minor)pagefaults 0swaps
```
### Results:
|                       | Reference |  Previous |   Current | Difference | Cumulative |
|----------------------:|:---------:|:---------:|:---------:|:----------:|:----------:|
|              Time (s) |     92.65 |     52.34 |     40.55 |    -22.53% |    -56.23% |
|            Memory (B) | 4,368,407 | 4,368,407 | 7,119,184 |    +62.97% |    +62.97% |
| Solution Length (QTM) |     30.41 |     30.41 |     30.41 |        +0% |        +0% |
|         U Moves (HTM) |      4.06 |      4.06 |      4.06 |        +0% |        +0% |

## Version 0.4
### Changes
* removed timeouts
* flatted `search.c` into a single function
### Goals
* avoid calling `time` in the middle of hot code
* more precise jumps
* more efficient variable usage
### Output
```
Version 0.4
Cache Size: 7119184
Progress: 100%
                                            TOTAL | PER SOLVE
number of phase 1 nodes expanded:      7545950972 |    754595
number of phase 1 solutions found:        2007604 |       200
number of phase 2 trees explored:         1228850 |       122
number of phase 2 nodes expanded:      1025050731 |    102505
number of phase 2 solutions found:          11685 |      1.17
number of moves in solution (QTM):         304123 |     30.41
number of U moves in solution (HTM):        40619 |      4.06

37.27user 0.00system 0:37.27elapsed 99%CPU (0avgtext+0avgdata 8692maxresident)k
0inputs+0outputs (0major+790minor)pagefaults 0swaps
```
### Results:
|                       | Reference |  Previous |   Current | Difference | Cumulative |
|----------------------:|:---------:|:---------:|:---------:|:----------:|:----------:|
|              Time (s) |     92.65 |     40.55 |     37.27 |     -8.09% |    -59.77% |
|            Memory (B) | 4,368,407 | 7,119,184 | 7,119,184 |    +62.97% |    +62.97% |
| Solution Length (QTM) |     30.41 |     30.41 |     30.41 |        +0% |        +0% |
|         U Moves (HTM) |      4.06 |      4.06 |      4.06 |        +0% |        +0% |
