#!/usr/bin/env bash

#INPUT_DIR_HOST=/Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset1
#OUTPUT_DIR_HOST=/Users/gerardin/Documents/projects/wipp++/pyramid-building/outputs

#IMAGES_RELATIVE_PATH=tiled-images
#VECTOR_RELATIVE_PATH=stitching_vector/img-global-positions-1.txt


# Docker requires absolute paths
# INPUT_DIR_HOST = base dir for inputs
# OUTPUT_DIR_HOST = base dir for outputs
INPUT_DIR_HOST=/home/gerardin/Documents/pyramidBuilding/resources/dataset1
OUTPUT_DIR_HOST=/home/gerardin/Documents/pyramidBuilding/outputs

## Where to find images and stitching vector
IMAGES_RELATIVE_PATH=tiled-images
VECTOR_RELATIVE_PATH=stitching_vector/img-global-positions-1.txt

# Mount locations inside the container. DO NOT EDIT.
CONTAINER_DIR_INPUTS=/tmp/inputs
CONTAINER_DIR_OUTPUTS=/tmp/outputs

# image name
CONTAINER_IMAGE=pyramid-building:1.0.0

# Full command
docker run -v $INPUT_DIR_HOST:$CONTAINER_DIR_INPUTS  -v $OUTPUT_DIR_HOST:$CONTAINER_DIR_OUTPUTS \
-e GLOG_v=3 -e GLOG_logtostderr=1 $CONTAINER_IMAGE \
 /tmp/main -i $CONTAINER_DIR_INPUTS/$IMAGES_RELATIVE_PATH -o $CONTAINER_DIR_OUTPUTS -n containerTest \
 -v $CONTAINER_DIR_INPUTS/$VECTOR_RELATIVE_PATH -t 256 -d 8U

# FOR DEBUGGING ONLY. Pop up a bash shell in the container.
# docker run -it -v $INPUT_DIR_HOST:$CONTAINER_DIR_INPUTS  -v $OUTPUT_DIR_HOST:$CONTAINER_DIR_OUTPUTS pb:0.2 \
# bash