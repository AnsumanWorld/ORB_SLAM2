cmake_minimum_required(VERSION 3.0)
project(gps-fusion)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(USE_COTIRE)
    include(${PROJECT_SOURCE_DIR}/cmake/cotire.cmake)
endif()

if(WIN32)
    add_definitions(
        -DWINDOWS
        -DNOMINMAX
    )
    add_compile_options(/MP)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-Wall -Wextra -O3 -march=native -fPIC)
endif()

find_package(OpenGL REQUIRED)

enable_testing()
find_package(GTest REQUIRED)

find_package(Eigen3 REQUIRED)
find_package(G2O REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/core
    ${EIGEN3_INCLUDE_DIR}
    ${G2O_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
)

add_definitions(
    -DG2O_HAVE_OPENGL
    -DG2O_SHARED_LIBS
    -Dtypes_custom_EXPORTS
)

add_subdirectory(util)
add_subdirectory(g2o_custom)
add_subdirectory(core)
add_subdirectory(apps)
add_subdirectory(tests)
