#!/usr/bin/env bash

../tools/run_benchmarks.sh dataset1_256 5  \
-v /Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset1/stitching_vector/img-global-positions-1.txt \
-i /Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset1/tiled-images/ \
-o /Users/gerardin/Documents/projects/wipp++/pyramid-building/outputs \
-t 256 -d 8U

../tools/run_benchmarks.sh dataset1_512 5  \
-v /Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset01/stitching_vector/img-global-positions-1.txt \
-i /Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset01/images/ \
-o /Users/gerardin/Documents/projects/wipp++/pyramid-building/outputs \
-t 512 -d 8U

../tools/run_benchmarks.sh dataset1_1024 5  \
-v /Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset01/stitching_vector/img-global-positions-1.txt \
-i /Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset01/images/ \
-o /Users/gerardin/Documents/projects/wipp++/pyramid-building/outputs \
-t 1024 -d 8U

