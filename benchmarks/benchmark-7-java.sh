#!/usr/bin/env bash

ROOT_DIR=/home/gerardin/Documents/pyramidBuilding
EXEC_PATH=/home/gerardin/Documents/pyramidio-java/pyramidio/target
OUTPUT_DIR=$ROOT_DIR/benchmarks/results

#
#
#../tools/run_benchmarks_java.sh dataset5_java_1024 1 $EXEC_PATH $OUTPUT_DIR \
#-v /home/gerardin/Documents/images/dataset5/img-global-positions-1.txt \
#-i /home/gerardin/Documents/images/dataset5/images \
#-o /home/gerardin/Documents/pyramidio-java/outputs \
#-t 1024 -d 8U

../tools/run_benchmarks_java.sh java_dataset7_1024 1 $EXEC_PATH $OUTPUT_DIR \
-v /home/gerardin/Documents/images/dataset7/manualStitching/img-global-positions-0.txt \
-i /home/gerardin/Documents/images/dataset7/tiled-images \
-o /home/gerardin/Documents/pyramidio-java/outputs \
-t 1024 -d 8U --cache 0 --threads 5

../tools/run_benchmarks_java.sh java_dataset7_1024 1 $EXEC_PATH $OUTPUT_DIR \
-v /home/gerardin/Documents/images/dataset7/manualStitching/img-global-positions-0.txt \
-i /home/gerardin/Documents/images/dataset7/tiled-images \
-o /home/gerardin/Documents/pyramidio-java/outputs \
-t 1024 -d 8U --cache 0 --threads 10

../tools/run_benchmarks_java.sh java_dataset7_1024 1 $EXEC_PATH $OUTPUT_DIR \
-v /home/gerardin/Documents/images/dataset7/manualStitching/img-global-positions-0.txt \
-i /home/gerardin/Documents/images/dataset7/tiled-images \
-o /home/gerardin/Documents/pyramidio-java/outputs \
-t 1024 -d 8U --cache 0 --threads 20

../tools/run_benchmarks_java.sh java_dataset7_1024 1 $EXEC_PATH $OUTPUT_DIR \
-v /home/gerardin/Documents/images/dataset7/manualStitching/img-global-positions-0.txt \
-i /home/gerardin/Documents/images/dataset7/tiled-images \
-o /home/gerardin/Documents/pyramidio-java/outputs \
-t 1024 -d 8U --cache 0 --threads 30

../tools/run_benchmarks_java.sh java_dataset7_1024 1 $EXEC_PATH $OUTPUT_DIR \
-v /home/gerardin/Documents/images/dataset7/manualStitching/img-global-positions-0.txt \
-i /home/gerardin/Documents/images/dataset7/tiled-images \
-o /home/gerardin/Documents/pyramidio-java/outputs \
-t 1024 -d 8U --cache 0 --threads 50


#ROOT_DIR=/Users/gerardin/Documents/projects/wipp++/pyramid-building/
#EXEC_PATH=/Users/gerardin/Documents/projects/pyramidio/pyramidio/target/
#OUTPUT_DIR=$ROOT_DIR/benchmarks/results
#
#../tools/run_benchmarks_java.sh dataset7_java_1024 1 $EXEC_PATH $OUTPUT_DIR  \
#-v $ROOT_DIR/resources/dataset1/stitching_vector/img-global-positions-1.txt \
#-i $ROOT_DIR/resources/dataset1/images \
#-o /tmp/outputs \
#-t 1024 -d 8U