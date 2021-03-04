cmake_minimum_required(VERSION 3.17)
project(proga2021)

set(CMAKE_CXX_STANDARD 14)


#task 2
add_executable(F task2/first.cpp)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

target_link_libraries(F ${VTK_LIBRARIES})
target_link_libraries(F gmsh)

find_package(DOLFIN REQUIRED)
include(${DOLFIN_USE_FILE})
#
add_executable(hyper_elasticity task3/HyperElasticity.cpp)
target_link_libraries(hyper_elasticity dolfin)