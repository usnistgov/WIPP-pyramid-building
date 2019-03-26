cmake_minimum_required(VERSION 3.7)
project(pyramidBuilding CXX)

set(CMAKE_CXX_STANDARD 17)

#TODO Set this flag only in debug
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic -Wno-reorder -Wno-unused-parameter" )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -funwind-tables -g")

set(CMAKE_VERBOSE_MAKEFILE ON)

set(TIFF_INCLUDE_DIR /usr/lib)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
#TODO FIX opencv is not installed in a standard directory - THIS HAS BEEN FIXED
set(CMAKE_PREFIX_PATH /usr/local/opt/opencv@3/)

find_package(FastImage REQUIRED)
include_directories(${HTGS_INCLUDE_DIR})
include_directories(${FastImage_INCLUDE_DIR})
link_libraries(${FastImage_LIBRARIES})

find_package(NVTX QUIET)

if(NVTX_FOUND)
    message(STATUS "FOUND NVTX!")
    add_definitions(-DUSE_NVTX)
    include_directories(${NVTX_INCLUDE_DIR})
    link_libraries(${NVTX_LIBRARIES})
endif()

# TODO CHANGE? SHOULD KEEP THIS DEPENDENCY? REMOVE IT? SWITCH TO OPENCV2?
find_package(OpenCV 3 REQUIRED)
link_libraries(${OpenCV_LIBRARIES})
message("OpenCV Version: ${OpenCV_VERSION}")

#TODO FIX tclap is also a dependency (NOT DECLARED YET)

if(APPLE)
    link_directories(/usr/local/Cellar/llvm/7.0.1/lib/)
    link_libraries(c++fs)
else(APPLE)
    link_libraries(stdc++fs)
endif(APPLE)

link_libraries(glog)

add_definitions(-DPROFILE)

#add_subdirectory(src)
#add_subdirectory(test)

add_subdirectory(test)

#set(ALL_SRCS CACHE INTERNAL "sources" FORCE)
set(SOURCES src/pyramidBuilding/api/PyramidBuilding.h src/pyramidBuilding/api/CommandLineCli.h src/pyramidBuilding/rules/PyramidRule.h src/pyramidBuilding/tasks/WriteDeepZoomTileTask.h src/pyramidBuilding/rules/DeepZoomDownsamplingRule.h src/pyramidBuilding/utils/BaseTileGenerator.h)

#add_executable(testbasePerfMultiThreadHTGS testBasePerfMultiThreadHTGS.cpp ${ALL_SRCS} ${SOURCES} testBasePerf.h SimpleReadAndWriteGraph.h)
#add_executable(testbasePerfMultiThread testBasePerfMultiThread.cpp ${ALL_SRCS} ${SOURCES} testBasePerf.h)
#add_executable(testbasePerf testBasePerf.cpp ${ALL_SRCS} ${SOURCES} testBasePerf.h)
#add_executable(stitching testStitchingVectorParser.cpp ${ALL_SRCS} ${SOURCES})
#add_executable(libtiff testLibtiff.cpp ${ALL_SRCS} ${SOURCES} src/utils/StitchedFOVTileLoader.h src/tasks/WriteTiffTileTask.h)
add_executable(main main.cpp ${ALL_SRCS} ${SOURCES} src/pyramidBuilding/utils/TileLoader.h src/pyramidBuilding/utils/BaseTileGeneratorLibTiff.h)
add_executable(pyramidBuildingCLITest test/PyramidBuildingCLITest.cpp ${ALL_SRCS} ${SOURCES})

target_include_directories(main PUBLIC src)
target_include_directories(pyramidBuildingCLITest PUBLIC src)