cmake_minimum_required(VERSION 3.10)
project(pyramidBuilding)

set(CMAKE_CXX_STANDARD 14)

#TODO Set this flag only in debug
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic -Wno-reorder -Wno-unused-parameter -ansi" )


set(TIFF_INCLUDE_DIR /usr/lib)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
#TODO FIX opencv is not installed in a standard directory - THIS HAS BEEN FIXED
set(CMAKE_PREFIX_PATH /usr/local/opt/opencv@3/)

find_package(FastImage REQUIRED)
include_directories(${HTGS_INCLUDE_DIR})
include_directories(${FastImage_INCLUDE_DIR})
link_libraries(${FastImage_LIBRARIES})

find_package(OpenCV 3 REQUIRED)
link_libraries(${OpenCV_LIBRARIES})
message("OpenCV Version: ${OpenCV_VERSION}")

add_definitions(-DPROFILE)

add_subdirectory(src)
add_subdirectory(test)

set(ALL_SRCS CACHE INTERNAL "sources" FORCE)
set(SOURCES src/utils/Helper.cpp src/data/BlockType.cpp src/rules/PyramidRule.h src/tasks/WriteTileTask.h src/utils/SingleTiledTiffWriter.h src/tasks/Write32UTiffTileWriter.h)

#add_executable(old main.cpp ${ALL_SRCS} ${SOURCES})
add_executable(stitching testStitchingVectorParser.cpp ${ALL_SRCS} ${SOURCES})
add_executable(libtiff testLibtiff.cpp ${ALL_SRCS} ${SOURCES})
add_executable(pyramidBuilding testPyramidBuilding.cpp ${ALL_SRCS} ${SOURCES})
add_executable(pyramidBuildingUbuntu testPyramidBuildingUbuntu.cpp ${ALL_SRCS} ${SOURCES})
#add_executable(dowmsampling testDownSamplingOpenCV.cpp ${ALL_SRCS} ${SOURCES} )



