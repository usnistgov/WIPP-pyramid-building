FROM wipp/wipp-fi-base-image:1.1.0 as build
LABEL maintainer="National Institute of Standards and Technology"
ENV DEBIAN_FRONTEND noninteractive

# Install required packages
RUN apt-get update -y \
    && apt-get install -y \
    build-essential \
    cmake \
    libtiff5-dev \
    unzip \
    wget
    
# Install HTGS
RUN cd /tmp \
    && wget -O HTGS.zip https://github.com/usnistgov/HTGS/archive/2.0.0.zip \
    && unzip -a HTGS.zip \
    && cd HTGS-2.0.0 \
    && mkdir build && cd build \
    && cmake .. \
    && make \
    && make install

# Install FastImage
RUN cd /tmp \
    && wget -O FastImage.zip https://github.com/usnistgov/FastImage/archive/FI_1.0.15.zip \
    && unzip -a FastImage.zip \
    && cd FastImage-FI_1.0.15 \
    && mkdir build && cd build \
    && cmake .. \
    && make \
    && make install
    
# Install OpenCV 4.1.1
RUN cd /tmp \
    && wget -O opencv4.zip https://github.com/opencv/opencv/archive/4.1.1.zip \
    && unzip -a opencv4.zip \
    && cd opencv-4.1.1 \
    && mkdir build && cd build \
    && cmake -DBUILD_SHARED_LIBS=OFF -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr/local .. \
    && make -j7 \
    && make install

# Install lib to parse command line arguments
RUN cd /tmp \
     && wget -O tclap.tar.gz https://sourceforge.net/projects/tclap/files/tclap-1.2.2.tar.gz/download \
     && tar -xzf tclap.tar.gz \
     && cd tclap-1.2.2 \
     && ./configure \
     && make \
     && make install

# Build PyramidBuilding executable
RUN mkdir /pyramid-building
COPY . /pyramid-building
RUN cd /pyramid-building && mkdir cmake-build-release && cd cmake-build-release \
    && cmake .. \
    && make

FROM wipp/wipp-fi-base-image:1.1.0 as runtime
LABEL maintainer="National Institute of Standards and Technology"
ENV DEBIAN_FRONTEND noninteractive

ARG DATA_DIR="/data"

# Create folders
RUN mkdir -p /opt/executables \
    && mkdir -p ${DATA_DIR}/outputs \
    && mkdir -p ${DATA_DIR}/inputs

# Copy executable from build image
COPY --from=build /pyramid-building/cmake-build-release/commandLineCli /opt/executables/commandLineCli

ENTRYPOINT ["/opt/executables/commandLineCli"]
