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