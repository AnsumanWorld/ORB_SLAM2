cmake_minimum_required(VERSION 3.0)
project(slam-tools VERSION 0.1)

find_package(Eigen3 REQUIRED)

include_directories(
    ${EIGEN3_INCLUDE_DIR}
)

set(SOURCES
    src/CoordEcef.cpp
    src/CoordEnu.cpp
    src/CoordLocal.cpp
    src/CoordWgs84.cpp
    src/helper.cpp
    src/Point3.cpp
    src/PoseEnu.cpp
    src/PoseLocal.cpp
    src/PoseWgs84.cpp
    src/SE3.cpp
    src/SphericalCoordinates.cpp
)

set(HEADERS
    include/constants.h
    include/coord_constants.h
    include/CoordEcef.h
    include/CoordEnu.h
    include/CoordLocal.h
    include/CoordWgs84.h
    include/helper.h
    include/Point3.h
    include/PoseEnu.h
    include/PoseLocal.h
    include/PoseWgs84.h
    include/SE3.h
    include/SphericalCoordinates.h
    include/YawPitchRoll.h
)

add_library(slam-tools STATIC ${SOURCES} ${HEADERS})

target_include_directories(slam-tools PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)

if(BUILD_TESTING)
    add_subdirectory(tests.gtest)
endif()
