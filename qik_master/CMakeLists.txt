cmake_minimum_required(VERSION 2.8.9)
project(qik_master)
find_package (Threads)

file(GLOB SOURCES_LIBSERIAL "deps/libserial/*.cpp" "deps/libserial/*.hpp")
file(GLOB SOURCES "source/src/*.cpp" "source/include/*.hpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Werror -Wall")

include_directories(
  deps/libserial
)

add_executable(main ${SOURCES} ${SOURCES_LIBSERIAL})
