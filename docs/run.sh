#!/usr/bin/env bash

INPUT_DIR_HOST=/Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset1
OUTPUT_DIR_HOST=/Users/gerardin/Documents/projects/wipp++/pyramid-building/outputs

IMAGES_RELATIVE_PATH=tiled-images
VECTOR_RELATIVE_PATH=stitching_vector/img-global-positions-1.txt

CONTAINER_DIR_INPUTS=/tmp/inputs
CONTAINER_DIR_OUTPUTS=/tmp/outputs





docker run -v $INPUT_DIR_HOST:$CONTAINER_DIR_INPUTS  -v $OUTPUT_DIR_HOST:$CONTAINER_DIR_OUTPUTS pyramid-building:1.0.0 \
 /tmp/pyramid-building/build/main -i $CONTAINER_DIR_INPUTS/$IMAGES_RELATIVE_PATH -o $CONTAINER_DIR_OUTPUTS \
 -v $CONTAINER_DIR_INPUTS/$VECTOR_RELATIVE_PATH -t 256

#docker run -it -v $INPUT_DIR_HOST:$CONTAINER_DIR_INPUTS  -v $OUTPUT_DIR_HOST:$CONTAINER_DIR_OUTPUTS pyramid-building:1.0.0 \
# bash

## For test

#docker run -it -v $INPUT_DIR_HOST:$CONTAINER_DIR_INPUTS  -v $OUTPUT_DIR_HOST:$CONTAINER_DIR_OUTPUTS pyramid-building:1.0.0 \
#bash

#docker run -it -v $INPUT_DIR_HOST:$CONTAINER_DIR_INPUTS  -v $OUTPUT_DIR_HOST:$CONTAINER_DIR_OUTPUTS pyramid-building:1.0.0-beta \
#bash

#docker exec nostalgic_fermat \
# /tmp/pyramid-building/build/main $CONTAINER_DIR_INPUTS/$IMAGES_RELATIVE_PATH $CONTAINER_DIR_OUTPUTS \
# -v $CONTAINER_DIR_INPUTS/$VECTOR_RELATIVE_PATH -t 256

