cmake_minimum_required(VERSION 3.12)
project(pyramidBuilding)

set(CMAKE_CXX_STANDARD 14)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")

find_package(FastImage REQUIRED)
include_directories(${HTGS_INCLUDE_DIR})
include_directories(${FastImage_INCLUDE_DIR})
link_libraries(${FastImage_LIBRARIES})

add_executable(pyramidBuilding main.cpp Helper.cpp Helper.h rules/WriteTileRule.h data/TileRequest.h tasks/WriteTileTask.h rules/PyramidRule.h tasks/CreateTileTask.h data/FakeTile.h utils/MatrixAllocator.h rules/MatrixMemoryRule.h utils/FakeTileAllocator.h data/Tile.h)