cmake_minimum_required(VERSION 3.17)
project(task1)

set(CMAKE_CXX_STANDARD 14)

add_executable(Torus tor.cpp)
add_executable(Torus2 tor2.cpp)
add_executable(Cylinder cylinder.cpp)
add_executable(Obj obj.cpp)

set(TARGETS Torus Cylinder Obj Torus2 )
set(LIBS gmsh)

foreach(X IN LISTS TARGETS)
    target_link_libraries(${X} ${LIBS})
endforeach()