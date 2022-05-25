cmake_minimum_required(VERSION 3.0)
project(COMP5426ASM1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(EXE_NAME pairwiseComp)

find_package(MPI REQUIRED)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})
include_directories(include)

aux_source_directory(src DIR_SRCS)

add_executable(${EXE_NAME} ${DIR_SRCS})
target_link_libraries(${EXE_NAME} pthread ${MPI_C_LIBRARIES})

