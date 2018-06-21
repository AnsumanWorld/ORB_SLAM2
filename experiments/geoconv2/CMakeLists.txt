cmake_minimum_required(VERSION 3.0)
project(geoconv2)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
find_package(Boost REQUIRED COMPONENTS filesystem)

include_directories(
    ${CMAKE_SOURCE_DIR}
)

add_subdirectory(apps)
add_subdirectory(utils)
