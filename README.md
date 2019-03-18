# README

## Distribution

The executable can be build from source.

A docker image can also be build from source.

```DockerFile``` contains the development version of the container.
All dependencies are build from source.


## Running the executable

#### Command line

Ex:

    ./main
     -v "/path/to/stitching_vector/img-global-positions-1.txt" 
     -i "/path/to/images/"
     -o "/path/to/outputDir"
     -t 256  -d 8U -b "overlay"
     -n "output"
    
#### Environment variables
Environment variables can be set to obtain logs.

GLOG_logtostderr=1; #print tostderr instead of log file
GLOG_v=2 #set log level.

#### Available Log Levels

1/ basic info and execution time.
2/ algorithmic steps.
3/ full log.