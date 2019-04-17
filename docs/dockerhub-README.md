We need to properly mount host directories and then we need to provide pyramid building arguments. 
Additional environment variables can be set to enable logging.

## Example usage

```
docker run -v $INPUT_DIR_HOST:$CONTAINER_DIR_INPUTS  -v $OUTPUT_DIR_HOST:$CONTAINER_DIR_OUTPUTS \
-e GLOG_v=3 -e GLOG_logtostderr=1 $CONTAINER_IMAGE \
 /tmp/main -i $IMAGES_PATH  -v $STITCHING_VECTOR_PATH -o $CONTAINER_DIR_OUTPUTS \
 -n pyramid -t 1024 -d 8U
```

### Mounting directories
Input and Output directories on the host should be mounted as volumes in the container, for example in /tmp/ :

```
INPUT_DIR_HOST=/path/on/host/to/inputDirectory
OUTPUT_DIR_HOST=path/on/host/to/outputDirectory
CONTAINER_DIR_INPUTS=/path/in/container/to/inputs
CONTAINER_DIR_OUTPUTS=/path/in/container/to/outputs
```
Now we provide the path to the images and stitching vector within the container
```
IMAGES_PATH=$CONTAINER_DIR_INPUTS/tiled-images
STITCHING_VECTOR_PATH=$CONTAINER_DIR_INPUTS/stitching_vector/img-global-positions-1.txt
```
```
CONTAINER_IMAGE=wipp/pyramid-building:1.0.0
```

### Container command arguments

* ```-i $IMAGES_PATH``` : path to the images in the container
* ```-v $STITCHING_VECTOR_PATH``` : path to the stitching vector in the container 
* ```-o $OUTPUT_DIR_HOST``` : path to the ouput directory in the container
* ```-n name```: name of the pyramid created
* ```-d depth``` : depth and encoding of the ouput images : 8U or 16U (8bits or 16bits unsigned intergers)
* ```-t size``` : size (in pixels) on the pyramid tiles

### Environment variables

Environment variables can be set to generate logs.

* ```GLOG_logtostderr=1```  : print tostderr instead of log file
* ```GLOG_v=[0..4]``` : set log level between 0 and 4 for more detailed logs

Log Levels :

0. no logs.
1. basic info and execution time.
2. algorithmic steps.
3. detailed logs.
4. very detailed logs.