set(ALL_SRCS ${ALL_SRCS} CACHE INTERNAL "sources")

add_executable(libtiff testLibtiff.cpp ${ALL_SRCS} ${SOURCES})
add_executable(testbasePerfMultiThreadHTGS testBasePerfMultiThreadHTGS.cpp ${ALL_SRCS} ${SOURCES} testBasePerf.h SimpleReadAndWriteGraph.h)