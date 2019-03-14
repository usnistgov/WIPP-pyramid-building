#!/usr/bin/env bash

INPUT_DIR_HOST=/Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset1

IMAGES_RELATIVE_PATH=tiled-images
VECTOR_RELATIVE_PATH=stitching_vector/img-global-positions-1.txt

CONTAINER_DIR=/tmp/inputs


docker run -v $INPUT_DIR_HOST:$CONTAINER_DIR pyramid-building:1.0.0-beta \
 /tmp/pyramid-building/build/main $CONTAINER_DIR/$IMAGES_RELATIVE_PATH \
 -v $CONTAINER_DIR/$VECTOR_RELATIVE_PATH -t 256

# For test
#docker run -it -v $INPUT_DIR_HOST:$CONTAINER_DIR pyramid-building:1.0.0-beta \
#bash