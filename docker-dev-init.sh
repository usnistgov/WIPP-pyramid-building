#!/usr/bin/env bash
#make sure libs are up -to-date
apt-get update
apt-get install cmake

# Based on HTGS
cd /tmp
git clone https://github.com/usnistgov/HTGS.git
cd HTGS
mkdir build && cd build
cmake .. 
make
make install

# Fast Image use for big images
cd /tmp
git clone https://github.com/usnistgov/FastImage.git
cd FastImage
make build && cd build
cmake ..
make
make install

# opencv to write png images and calculate intersections of overlapping tiles.
apt-get install build-essential
apt-get install cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev
wget -O opencv3.zip https://github.com/opencv/opencv/archive/3.4.5.zip
unzip -a opencv3.zip
make build && cd build
cmake -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local ..
make -j7
make install

# From https://github.com/cggos/dip_cvqt/issues/1
# Linking opencv requires this path
echo /usr/lib/conf >> /etc/ld.so.conf.d/opencv.conf
ldconfig -v

# lib to parse command line arguments
cd /tmp
wget -O tclap.tar.gz https://sourceforge.net/projects/tclap/files/latest/download
tar -xzf tclap.tar.gz
cd tclap-1.2.2
./configure
make
make install

#install glog
git clone https://github.com/google/glog.git
cd glog
./autogen.sh
./configure
make
make install

cd /tmp
git clone https://gitlab.nist.gov/gitlab/gerardin/pyramid-building.git
cd pyramid-building
mkdir build && cd build
cmake ..
make main

# We will mount the host input dir in this directory.
INPUTS=/tmp/inputs
OUTPUTS=/tmp/outputs
mkdir $INPUTS  #make sure the user running the algorithm can read
mkdir $OUTPUTS #make sure the user running the algorithm can read/write

#=========
#Below are extra steps we will need to run container with an unprivileged user.

#Create a user and a group wipp with UID/GID of 1000:1000
MAIN_GROUP=wipp
MAIN_GID=1000
MAIN_USER=wipp
MAIN_UID=1000

groupadd ${MAIN_GROUP} -g ${MAIN_GID} \
   && adduser -u ${MAIN_UID} --disabled-password --gecos "" --ingroup ${MAIN_GROUP} ${MAIN_USER} \
   && adduser ${MAIN_USER} sudo \
   && yes zaq123 | passwd ${MAIN_USER}

# TODO REMOVE probably unnecessary since the mount process overwrite these.
chown MAIN_USER:MAIN_GROUP $INPUTS
chown MAIN_USER:MAIN_GROUP $OUTPUTS

#All install command done with this user
su wipp

cd /tmp/pyramid-building/build





