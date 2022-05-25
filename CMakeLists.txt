cmake_minimum_required(VERSION 3.0)
project(COMP5426ASM1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-g -Wall")
find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})


set(EXE_NAME pairwiseComp)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

INCLUDE_DIRECTORIES(include)
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
#AUX_SOURCE_DIRECTORY(MPITest DIR_SRCS)

add_executable(${EXE_NAME} ${DIR_SRCS})
target_link_libraries(${EXE_NAME} pthread ${MPI_C_LIBRARIES})

