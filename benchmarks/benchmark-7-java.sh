#!/usr/bin/env bash
#../tools/run_benchmarks_java.sh dataset7_java_1024 1  \
#-v /home/gerardin/Documents/images/dataset7/img-global-positions-0.txt \
#-i /home/gerardin/Documents/images/dataset7/tiled-images \
#-o /home/gerardin/Documents/pyramidio-java/outputs \
#-t 1024 -d 8U

ROOT_DIR=/Users/gerardin/Documents/projects/wipp++/pyramid-building/
EXEC_PATH=/Users/gerardin/Documents/projects/pyramidio/pyramidio/target/
OUTPUT_DIR=$ROOT_DIR/benchmarks/results

../tools/run_benchmarks_java.sh dataset7_java_1024 1 $EXEC_PATH $OUTPUT_DIR  \
-v $ROOT_DIR/resources/dataset1/stitching_vector/img-global-positions-1.txt \
-i $ROOT_DIR/resources/dataset1/images \
-o /tmp/outputs \
-t 1024 -d 8U