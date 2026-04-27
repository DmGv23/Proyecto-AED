cmake_minimum_required(VERSION 4.0)
project(PROYECTOAED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(PROYECTOAED
        src/main.cpp
        src/SparseMatrix.cpp
        include/SparseMatrix.h
        include/Node.h
)