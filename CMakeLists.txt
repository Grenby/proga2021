cmake_minimum_required(VERSION 3.17)
project(proga2021)

set(CMAKE_CXX_STANDARD 14)

add_executable(Torus task1/tor.cpp)
add_executable(Cylinder task1/cylinder.cpp)


set(TARGETS Torus Cylinder)
set(LIBS gmsh)


foreach(X IN LISTS TARGETS)
    target_link_libraries(${X} ${LIBS})
endforeach()
