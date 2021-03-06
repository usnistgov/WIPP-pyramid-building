#!/usr/bin/env bash
#
#../scripts/run_benchmarks.sh dataset1_256 1 \
#-k exectime  \
#-v /home/gerardin/Documents/pyramidBuilding/resources/dataset1/stitching_vector/img-global-positions-1.txt \
#-i /home/gerardin/Documents/pyramidBuilding/resources/dataset1/tiled-images \
#-o /home/gerardin/Documents/pyramidBuilding/outputs \
#-t 256 -d 16U -n dataset1

#
#../scripts/run_benchmarks.sh dataset5_1024 1 --benchmark exectime  \
#-v /home/gerardin/Documents/images/dataset5/img-global-positions-1.txt \
#-i /home/gerardin/Documents/images/dataset5/images \
#-o /home/gerardin/Documents/pyramidBuilding/outputs \
#-t 1024 -d 8U -n dataset5



../scripts/run_benchmarks.sh dataset7_1024 1 --benchmark exectime  \
-v /home/gerardin/Documents/images/dataset7/manualStitching/img-global-positions-0.txt \
-i /home/gerardin/Documents/images/dataset7/tiled-images \
-o /home/gerardin/Documents/pyramidBuilding/outputs \
-t 1024 -d 8U -n dataset7 -e "reader=2;builder=1;writer=5;downsampler=4;tile=16;"



#../scripts/run_benchmarks.sh dataset7_1024 1 --benchmark exectime  \
#-v /home/gerardin/Documents/images/dataset7/img-global-positions-0.txt \
#-i /home/gerardin/Documents/images/dataset7/tiled-images \
#-o /home/gerardin/Documents/pyramidBuilding/outputs \
#-t 1024 -d 8U -n dataset7-hugeoverlap

