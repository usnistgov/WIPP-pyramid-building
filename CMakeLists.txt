cmake_minimum_required(VERSION 3.12)
project(pyramidBuilding)

set(CMAKE_CXX_STANDARD 14)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

find_package(FastImage REQUIRED)
include_directories(${HTGS_INCLUDE_DIR})
include_directories(${FastImage_INCLUDE_DIR})
link_libraries(${FastImage_LIBRARIES})

find_package(OpenCV REQUIRED)
message("OpenCV Version: ${OpenCV_VERSION}")

add_definitions(-DPROFILE)

set(SOURCES Helper.cpp Helper.h rules/WriteTileRule.h data/TileRequest.h tasks/WriteTileTask.h rules/PyramidRule.h tasks/CreateTileTask.h data/FakeTile.h utils/MatrixAllocator.h rules/MatrixMemoryRule.h utils/FakeTileAllocator.h data/Tile.h data/BlockType.cpp data/BlockType.h data/PartialFov.h utils/MistStitchedImageReader.h testLibtiff.cpp)

add_executable(pyramidBuilding main.cpp ${SOURCES})
add_executable(stitching stitchingVectorParsing.cpp ${SOURCES})
add_executable(libtiff testLibtiff.cpp utils/SingleTiledTiffWriter.h utils/BaseTileGenerator.h)