#!/usr/bin/env bash

#INPUT_DIR_HOST=/Users/gerardin/Documents/projects/wipp++/pyramid-building/resources/dataset1
#OUTPUT_DIR_HOST=/Users/gerardin/Documents/projects/wipp++/pyramid-building/outputs

#IMAGES_RELATIVE_PATH=tiled-images
#VECTOR_RELATIVE_PATH=stitching_vector/img-global-positions-1.txt


# Docker requires absolute paths
# INPUT_DIR_HOST = base dir for inputs
# OUTPUT_DIR_HOST = base dir for outputs
INPUT_DIR_HOST=/home/gerardin/Documents/projects/pyramid-building/resources/dataset1
OUTPUT_DIR_HOST=/home/gerardin/Documents/projects/pyramid-building/outputs

## Where to find images and stitching vector
IMAGES_RELATIVE_PATH=tiled-images
VECTOR_RELATIVE_PATH=stitching_vector/img-global-positions-1.txt

# Mount locations inside the container. DO NOT EDIT.
CONTAINER_DIR_INPUTS=/tmp/inputs
CONTAINER_DIR_OUTPUTS=/tmp/outputs

# image name
CONTAINER_IMAGE=wipp/pyramid-building:1.0.9

# Full command
docker run -v $INPUT_DIR_HOST:$CONTAINER_DIR_INPUTS  -v $OUTPUT_DIR_HOST:$CONTAINER_DIR_OUTPUTS \
-e GLOG_v=3 -e GLOG_logtostderr=1 $CONTAINER_IMAGE \
 -i $CONTAINER_DIR_INPUTS/$IMAGES_RELATIVE_PATH -o $CONTAINER_DIR_OUTPUTS -n containerTest \
 -v $CONTAINER_DIR_INPUTS/$VECTOR_RELATIVE_PATH -t 256 -d 8U

### ========================
### BIG DATASET TEST (20GB)
### ========================

INPUT_DIR_HOST=/home/gerardin/Documents/images/dataset7-NanoFibers/
OUTPUT_DIR_HOST=/home/gerardin/Documents/projects/pyramid-building/outputs

IMAGES_RELATIVE_PATH=tiled_images
VECTOR_RELATIVE_PATH=manualStitching/img-global-positions-0.txt

# Full command
docker run -v $INPUT_DIR_HOST:$CONTAINER_DIR_INPUTS  -v $OUTPUT_DIR_HOST:$CONTAINER_DIR_OUTPUTS \
-e GLOG_v=3 -e GLOG_logtostderr=1 $CONTAINER_IMAGE \
 -i $CONTAINER_DIR_INPUTS/$IMAGES_RELATIVE_PATH -o $CONTAINER_DIR_OUTPUTS -n posterTest \
 -v $CONTAINER_DIR_INPUTS/$VECTOR_RELATIVE_PATH -t 1024 -d 8U
