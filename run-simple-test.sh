#!/usr/bin/env bash

# Docker requires absolute paths
# INPUT_DIR_HOST = base dir for inputs
# OUTPUT_DIR_HOST = base dir for outputs
INPUT_DIR_HOST=$PWD/resources/dataset1
OUTPUT_DIR_HOST=$PWD/outputs

## Where to find images and stitching vector
IMAGES_RELATIVE_PATH=tiled-images
VECTOR_RELATIVE_PATH=stitching_vector/img-global-positions-1.txt

# Mount locations inside the container. DO NOT EDIT.
CONTAINER_DIR_INPUTS=/tmp/inputs
CONTAINER_DIR_OUTPUTS=/tmp/outputs

# image name
CONTAINER_IMAGE=pyramid-building:dev

# Full command
docker run -v $INPUT_DIR_HOST:$CONTAINER_DIR_INPUTS  -v $OUTPUT_DIR_HOST:$CONTAINER_DIR_OUTPUTS \
-e GLOG_v=3 -e GLOG_logtostderr=1 $CONTAINER_IMAGE \
 -i $CONTAINER_DIR_INPUTS/$IMAGES_RELATIVE_PATH -o $CONTAINER_DIR_OUTPUTS -n containerTest -f "all"\
 -v $CONTAINER_DIR_INPUTS/$VECTOR_RELATIVE_PATH -t 256 -d 8U
