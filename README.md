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

## Version 0.5
### Changes
* complete rewrite from scratch
* add all goal distances to heuristic tables
### Goals
* reorganize and separate code
* replace IDA* with direct graph search
* trade a ton of memory for a ton of speed
### Output
```
Version 0.5
Allocating...                   Done in  0.000s
Reading move tables...          Done in  0.003s
Reading heuristic tables...     Done in  5.271s
Caching heuristic tables...     Done in 16.093s
Cache Sum:                      0x001BF2CBF0BD9
Cache Size:                     0x00002F7EC4FB4
Progress: 100%
                                            TOTAL | PER SOLVE
number of phase 1 nodes expanded:          864759 |        86
number of phase 1 solutions found:          10000 |         0
number of phase 2 trees explored:           10000 |         0
number of phase 2 nodes expanded:          778654 |        77
number of phase 2 solutions found:          10000 |      1.00
number of moves in solution (QTM):         345686 |     34.57
number of U moves in solution (HTM):        49290 |      4.93

2.03user 4.38system 0:21.66elapsed 29%CPU (0avgtext+0avgdata 12452336maxresident)k
26975952inputs+0outputs (117771major+1856458minor)pagefaults 0swaps
```
### Results:
|                       | Reference |  Previous |        Current | Difference | Cumulative |
|----------------------:|:---------:|:---------:|:--------------:|:----------:|:----------:|
|         Prep Time (s) |      0.00 |      0.00 |          21.37 |   +100.00% |   +100.00% |
|        Solve Time (s) |     92.65 |     37.27 |           0.29 |    -99.22% |    -99.69% |
|            Memory (B) | 4,368,407 | 7,119,184 | 12,749,393,844 |  +178,985% |  +291,755% |
| Solution Length (QTM) |     30.41 |     30.41 |          34.57 |    +13.68% |    +13.68% |
|         U Moves (HTM) |      4.06 |      4.06 |           4.93 |    +20.43% |    +20.43% |
