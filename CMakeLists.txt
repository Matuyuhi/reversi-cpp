cmake_minimum_required(VERSION 3.22.1)
project(work)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES "src/riversi/private/*.cpp" "src/winsoc/private/*.cpp")
file(GLOB HEADER "src/riversi/public/*.h" "src/winsoc/public/*.h")
add_executable(work
        src/main.cpp
        ${SOURCES}
        ${HEADER}
)