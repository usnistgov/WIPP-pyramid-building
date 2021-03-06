cmake_minimum_required(VERSION 3.7)

project(pyramidBuilding CXX)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(LOG ON)

#TODO Set this flag only in debug
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic -Wno-reorder -Wno-unused-parameter" )

#TODO CHECK release flags
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -funwind-tables -g")


# Detect compiler
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # using regular Clang or AppleClang
    set(CMAKE_COMPILER_IS_CLANGCXX 1)
    message(Clang Detected!)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_COMPILER_IS_GNUCXX 1)
    # using GCC
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
endif()

# IMPORTANT to allow CMAKE to find HTGS and FAST IMAGE
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

# if installed through package manager a FindTIFF.cmake is available on the path
# (ex : /usr/share/cmake-3.10/Modules/FindTIFF.cmake)
# and provide default variables such as ${TIFF_LIBRARIES}
# if installed from sources, libtiff needs to be link manually.
message(libTiff path :  ${TIFF_LIBRARIES} )

#TODO Set only in debug mode
find_package(NVTX QUIET)
if(NVTX_FOUND)
    message(STATUS "FOUND NVTX!")
    add_definitions(-DUSE_NVTX)
    include_directories(${NVTX_INCLUDE_DIR})
    link_libraries(${NVTX_LIBRARIES})
endif()

# FAST IMAGE pull HTGS transitively
find_package(FastImage REQUIRED)
include_directories(${HTGS_INCLUDE_DIR})
include_directories(${FastImage_INCLUDE_DIR})
link_libraries(${FastImage_LIBRARIES})

find_package(OpenCV REQUIRED)
link_libraries(${OpenCV_LIBRARIES} )
message("OpenCV Version: ${OpenCV_VERSION}")

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} ${FastLoader_CXX_FLAGS})

if (NOT CMAKE_COMPILER_IS_CLANGCXX)
    list(APPEND PYRAMID_LIBS "stdc++fs")
    message("compiler will link to the filesystem library -lstdc++fs (experimental)")
endif(NOT CMAKE_COMPILER_IS_CLANGCXX)

# Enable logs
if (LOG)
    find_package(GLOG REQUIRED)
    message(STATUS "GLOG found")
    include_directories(${GLOG_INCLUDE_DIRS})
    link_libraries(${GLOG_LIBRARIES})
endif(LOG)

add_definitions(-DPROFILE)
#add_definitions(-DUSE_PRIORITY_QUEUE)

include_directories(src)

#TODO add only for tests
add_subdirectory(test)

set(SRC_FILES
        src/mem/Mem.cpp
        src/mem/Mem.h
        src/pyramidBuilding/api/CommandLineCli.h
        src/pyramidBuilding/api/OptionsType.h
        src/pyramidBuilding/api/PyramidBuilding.h
        src/pyramidBuilding/data/FOVMetadata.h
        src/pyramidBuilding/data/Tile.h
        src/pyramidBuilding/data/TileBlock.h
        src/pyramidBuilding/pyramid/data/TileRequest.h
        src/pyramidBuilding/pyramid/data/PartialFOV.h
        src/pyramidBuilding/pyramid/PyramidBuilder.h
        src/pyramidBuilding/pyramid/fastImage/PyramidTileLoader.h
        src/pyramidBuilding/pyramid/RecursiveBlockTraversal.h
        src/pyramidBuilding/memory/TileAllocator.h
        src/pyramidBuilding/rules/DeepZoomDownsampleTileRule.h
        src/pyramidBuilding/rules/PyramidCacheRule.h
        src/pyramidBuilding/memory/ReleaseMemoryRule.h
        src/pyramidBuilding/rules/WriteTileRule.h
        src/pyramidBuilding/tasks/DeepZoomTileWriter.h
        src/pyramidBuilding/tasks/PyramidalTiffWriter.h
        src/pyramidBuilding/tasks/TileDownsampler.h
        src/pyramidBuilding/tasks/TileResizer.h
        src/pyramidBuilding/utils/deprecated/Blender.h
        src/pyramidBuilding/utils/AverageDownsampler.h
        src/pyramidBuilding/utils/Downsampler.h
        src/pyramidBuilding/pyramid/TiffImageLoader.h
        src/pyramidBuilding/utils/Utils.h src/pyramidBuilding/data/Pyramid.h
        src/pyramidBuilding/data/BlockType.h
        src/pyramidBuilding/pyramid/data/PartialFOV.h
        src/pyramidBuilding/pyramid/data/TileRequest.h
        src/pyramidBuilding/tasks/TileResizer.h
        src/mem/Mem.cpp
        src/mem/Mem.h
        src/pyramidBuilding/memory/TileAllocator.h
        src/pyramidBuilding/pyramid/RecursiveBlockTraversal.h
        )

add_executable(commandLineCli commandLineCli.cpp
        ${SRC_FILES}
        )

message("extra libs to link to :  ${PYRAMID_LIBS}")
target_link_libraries(commandLineCli ${PYRAMID_LIBS})