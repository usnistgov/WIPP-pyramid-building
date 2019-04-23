#!/usr/bin/env bash
#
#
#../scripts/run_benchmarks.sh tracer_dataset3_1024 1 --benchmark memory  \
#-v /home/gerardin/Documents/pyramidBuilding/resources/dataset03/stitching_vector/img-global-positions-1.txt \
#-i /home/gerardin/Documents/pyramidBuilding/resources/dataset03/images \
#-o /home/gerardin/Documents/pyramidBuilding/outputs \
#-t 256 -d 8U -n tracer

#../scripts/run_benchmarks.sh dataset7_1024 1 --benchmark memory  \
#-v /home/gerardin/Documents/images/dataset7/img-global-positions-0.txt \
#-i /home/gerardin/Documents/images/dataset7/tiled-images \
#-o /home/gerardin/Documents/pyramidBuilding/outputs \
#-t 1024 -d 8U

#../scripts/run_benchmarks.sh dataset5_1024 1 --benchmark memory \
#-v /home/gerardin/Documents/images//dataset5/img-global-positions-1.txt \
#-i /home/gerardin/Documents/images/dataset5/images \
#-o /home/gerardin/Documents/pyramidBuilding/outputs \
#-t 1024 -d 8U



../scripts/run_benchmarks.sh dataset7_1024 1 --benchmark memory  \
-v /home/gerardin/Documents/images/dataset7/manualStitching/img-global-positions-0.txt \
-i /home/gerardin/Documents/images/dataset7/tiled-images \
-o /home/gerardin/Documents/pyramidBuilding/outputs \
-t 1024 -d 8U -n dataset7


