#!/bin/bash
gcc -o fik8 balloon_fik8.cpp -Wno-write-strings -L. -lpxcore -lminipix  -ldl -lm -lc -g
[ ! -d "./output-files" ] && mkdir ./output-files
# build the FIK8 balloon example (balloon_fik8.cpp) and create dirrectory for data saving
