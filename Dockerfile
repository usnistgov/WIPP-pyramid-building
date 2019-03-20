FROM gcc

#!/usr/bin/env bash
#make sure libs are up -to-date
RUN apt-get update -y ;\
apt-get install -y cmake ;\
\
# Based on HTGS
cd /tmp ;\
git clone https://github.com/usnistgov/HTGS.git ;\
cd HTGS ;\
mkdir build && cd build ;\
cmake .. ;\
make ;\
make install ;\
\
# Fast Image use for big images
cd /tmp ;\
git clone https://github.com/usnistgov/FastImage.git ;\
cd FastImage ;\
mkdir build && cd build ;\
cmake .. ;\
make ;\
make install ;\
\
# opencv to write png images and calculate intersections of overlapping tiles.
## TODO CHANGE? - FOR NOW Pyramid-building depends on openCV3.
echo "Installing opencv3.4.5" > 2 ;\
cd /tmp ;\
apt-get install -y build-essential ;\
apt-get install -y cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev ;\
wget -O opencv3.zip https://github.com/opencv/opencv/archive/3.4.5.zip ;\
unzip -a opencv3.zip ;\
cd opencv-3.4.5 ;\
pwd;\
mkdir build && cd build ;\
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local .. ;\
#TODO try to reduce opencv size but fails so far
#cmake -D BUILD_LIST=core,imgproc,imgcodecs  -D CMAKE_BUILD_TYPE=Release \
#-D BUILD_EXAMPLES=OFF -D BUILD_opencv_apps=OFF -D BUILD_DOCS=OFF \
#-D BUILD_PERF_TESTS=OFF -D BUILD_TESTS=OFF  -D CMAKE_INSTALL_PREFIX=/usr/local .. ;\
make -j7 ;\
make install ;\
\
# From https://github.com/cggos/dip_cvqt/issues/1
# Runtime error when using opencv3 otherwise
echo /usr/lib/conf >> /etc/ld.so.conf.d/opencv.conf ;\
ldconfig -v ;\
\
# lib to parse command line arguments
cd /tmp ;\
wget -O tclap.tar.gz https://sourceforge.net/projects/tclap/files/latest/download ;\
tar -xzf tclap.tar.gz ;\
cd tclap-1.2.2 ;\
./configure ;\
make ;\
make install ;\
\
## TODO - no access to a public repo so we need to build the binary first and copy it.
mkdir -p /tmp/pyramid-building ;\
cd /tmp/pyramid-building/ ;

#ADD ./build/main /tmp/pyramid-building/build/main
ADD ./src /tmp/pyramid-building/src
ADD ./src /tmp/pyramid-building/test
ADD ./cmake-modules /tmp/pyramid-building/cmake-modules
ADD CMakeLists.txt /tmp/pyramid-building/CMakeLists.txt
ADD main.cpp /tmp/pyramid-building/main.cpp
ADD test/PyramidBuildingCLITest.cpp /tmp/pyramid-building/PyramidBuildingCLITest.cpp

RUN cd /tmp/pyramid-building/ ;\
mkdir build && cd build ;\
cmake .. ;\
make main ;\
\
## TODO - fall back to using github once the repo is made public.
#cd /tmp
#git clone https://gitlab.nist.gov/gitlab/gerardin/pyramid-building.git
#cd pyramid-building
#mkdir build && cd build
#cmake ..
#make main
\
# We will mount the host input dir in this directory.
INPUTS=/tmp/inputs ;\
OUTPUTS=/tmp/outputs ;\
#make sure the user running the algorithm can read
mkdir $INPUTS ;\
#make sure the user running the algorithm can read and write
mkdir $OUTPUTS ;
#=========
#Below are extra steps we will need to run container with an unprivileged user.

## TODO - maybe later we could prepare an unprivileged user.
#Create a user and a group wipp with UID/GID of 1000:1000
#MAIN_GROUP=wipp
#MAIN_GID=1000
#MAIN_USER=wipp
#MAIN_UID=1000
#
#groupadd ${MAIN_GROUP} -g ${MAIN_GID} ;\
#   && adduser -u ${MAIN_UID} --disabled-password --gecos "" --ingroup ${MAIN_GROUP} ${MAIN_USER} ;\
#   && adduser ${MAIN_USER} sudo ;\
#   && yes zaq123 | passwd ${MAIN_USER}
#
## TODO REMOVE probably unnecessary since the mount process overwrite these.
#chown MAIN_USER:MAIN_GROUP $INPUTS
#chown MAIN_USER:MAIN_GROUP $OUTPUTS
#
##All install command done with this user
#su wipp
#
#cd /tmp/pyramid-building/build

#============
CMD /tmp/pyramid-building/build/main



