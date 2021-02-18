cmake_minimum_required(VERSION 3.17)
project(proga2021)

set(CMAKE_CXX_STANDARD 14)

#task 1
add_executable(Torus task1/tor.cpp)
add_executable(Cylinder cylinder.cpp)
add_executable(Obj task1/obj.cpp)
set(TARGETS Torus Cylinder Obj )
set(LIBS gmsh)
foreach(X IN LISTS TARGETS)
   target_link_libraries(${X} ${LIBS})
endforeach()

#add_executable(F task2/first.cpp)

#find_package(VTK REQUIRED)
#include(${VTK_USE_FILE})
##find_path(
##        GSLINCLUDE
##        NAMES "VTK_USE_FILE"
##)
##message(STATUS "GSLINCLUDE => ${GSLINCLUDE}")
#target_link_libraries(F gmsh)
