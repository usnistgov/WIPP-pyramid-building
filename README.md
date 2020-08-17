# README

## Installation Instructions

### Dependencies

Dependencies can be build from sources or obtained from package managers when available.

1. g++/gcc version  7.3.0+
2. [htgs](https://github.com/usnistgov/htgs)
3. [fast image](https://github.com/usnistgov/FastImage)
4. [LibTIFF](http://www.simplesystems.org/libtiff/)
5. [openCV](https://opencv.org/releases.html) version 3.4.2+
6. [tclap](http://tclap.sourceforge.net/)
7. [glog](https://github.com/google/glog)

NOTE : ```Dockerfile``` and ```docker-dev-init``` may provide some useful info about how to build those dependencies, 
 but we rather recommend to follow each dependency installation instructions.

### Building Pyramid Building

     mkdir build && cd build
     ccmake ../ (or cmake-gui)
     make

NOTE : Only a handful of OpenCV functions are used by the pyramid building algorithm.
Linking statically to OpenCV is recommended for container distribution in order to reduce the size of the container.


## Distribution with Docker Container

Docker distribution available on DockerHub at https://hub.docker.com/r/wipp/pyramid-building

A Docker image can also be build from source.

To build the Docker image for development/testing, run:
```
docker build -t pyramid-building:dev .
```
You can then run a simple test for building a pyramid using the Docker image:
```
sh run-simple-test.sh
```


## Running the executable

#### Command line

Example:

    ./main
     -v "/path/to/stitching_vector/img-global-positions-1.txt" 
     -i "/path/to/images/"
     -o "/path/to/outputDir"
     -t 256  -d 8U -b "overlay"
     -n "pyramidName"
    
    
#### Logging

Environment variables can be set to generate logs.

GLOG_logtostderr=1; #print tostderr instead of log file
GLOG_v=[0..4] #set log level between 0 and 4 for more detailed logs

Log Levels :

0/ no logs.
1/ basic info and execution time.
2/ algorithmic steps.
3/ detailed logs.
4/ very detailed logs.


## Viewing the pyramid

A basic integration of the deepzoom js library is provided to check the result of a pyramid building job.
A http server can quickly be deployed with npm.

    sudo apt-get install npm
    sudo npm install -g http-server

Edit ```test-webapp/index.html``` and update the tileSources map entry with the path to your pyramid dzi file.
(Make sure to provide a relative path to the ```test-webapp``` directory).

Start the http-server (the root must allow access the pyramid directory).

    http-server  ..
    
Visit ```http://127.0.0.1:8080```

    


    