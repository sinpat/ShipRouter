cmake_minimum_required(VERSION 3.14)

#get options and compiling env
include(cmake/options.cmake)
include(cmake/linker.cmake)
include(cmake/ccache.cmake)

project(ShipRouter)
project(ShipRouter LANGUAGES CXX)

#set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

############################
###get external projects
############################
include(cmake/fmt.cmake)
include(cmake/tbb.cmake)
include(cmake/span.cmake)
include(cmake/CLI11.cmake)
include(cmake/gtest.cmake)
include(cmake/progress.cmake)

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

include_directories(/usr/local/include/)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# needed for multithreading
find_package(Threads REQUIRED)

include(cmake/flags.cmake)

#############################################
## library with all the logic which
## can later be easily linked against test
#############################################
add_library(ShipRouterSrc "")

target_sources(ShipRouterSrc
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/include/Graph.hpp
  ${CMAKE_CURRENT_LIST_DIR}/include/PBFExtractor.hpp

  PRIVATE
  src/Graph.cpp
  src/PBFExtractor.cpp
  )

# add the dependencies of the target to enforce
# the right order of compiling
add_dependencies(ShipRouterSrc fmt-project)
add_dependencies(ShipRouterSrc tbb-project)
add_dependencies(ShipRouterSrc span-lite-project)
add_dependencies(ShipRouterSrc CLI11-project)
add_dependencies(ShipRouterSrc progress-cpp-project)

# make headers available
target_include_directories(ShipRouterSrc PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/vendor/libosmpbfreader/include
  fmt
  tbb
  ${CLI11_INCLUDE_DIR}
  ${PROGRESS_CPP_INCLUDE_DIR}
  ${SPAN_LITE_INCLUDE_DIR}
  )

#link against libarys
target_link_libraries(ShipRouterSrc LINK_PUBLIC
  fmt
  tbb)

target_link_libraries(ShipRouterSrc PRIVATE
  ${CMAKE_THREAD_LIBS_INIT})


###############################
## THE ACTUAL BINARY
###############################
add_executable(ShipRouter src/main.cpp)

find_package(Protobuf REQUIRED)
find_package(ZLIB REQUIRED)

# make headers available
target_include_directories(ShipRouter PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/vendor/osmpbf/include
  ${CMAKE_CURRENT_SOURCE_DIR}/vendor/libosmpbfreader/include
  fmt
  tbb
  ZLIB::ZLIB
  ${CLI11_INCLUDE_DIR}
  ${PROGRESS_CPP_INCLUDE_DIR}
  ${SPAN_LITE_INCLUDE_DIR}
  )

#link against libarys
target_link_libraries(ShipRouter LINK_PUBLIC
  ShipRouterSrc
  fmt
  tbb
  ZLIB::ZLIB
  ${CMAKE_CURRENT_SOURCE_DIR}/vendor/osmpbf/lib/libosmpbf.so
  ${Protobuf_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT})

# add the dependencies of the target to enforce
# the right order of compiling
add_dependencies(ShipRouter ShipRouterSrc)

if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif (BUILD_TESTS)
