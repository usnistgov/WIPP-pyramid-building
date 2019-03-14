#!/usr/bin/env bash

INPUT_DIR_HOST=/Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset1
OUTPUT_DIR_HOST=.

IMAGES_RELATIVE_PATH=tiled-images
VECTOR_RELATIVE_PATH=stitching_vector/img-global-positions-1.txt

CONTAINER_DIR_INPUTS=/tmp/inputs
CONTAINER_DIR_OUTPUTS=/tmp/outputs


docker run -v $INPUT_DIR_HOST:CONTAINER_DIR_INPUTS  -v OUTPUT_DIR_HOST:CONTAINER_DIR_OUTPUTS pyramid-building:1.0.0-beta \
 /tmp/pyramid-building/build/main CONTAINER_DIR_INPUTS/$IMAGES_RELATIVE_PATH \
 -v CONTAINER_DIR_INPUTS/$VECTOR_RELATIVE_PATH -t 256

# For test
#docker run -it -v $INPUT_DIR_HOST:$CONTAINER_DIR pyramid-building:1.0.0-beta \
#bash