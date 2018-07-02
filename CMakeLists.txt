cmake_minimum_required(VERSION 3.0)
project(orb-slam2)

include(CTest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# I'd prefer to not set these but it seems to be much more convenient to have all the artifacts in out place 
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIGURATION>/bin)

if (CMAKE_SYSTEM_NAME MATCHES Windows AND CMAKE_VERSION LESS 3.11)
    # FindBoost for treatment of Boost imported targets
    LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules/FindBoost)
endif()

# FindG2O for treatment of G20 imported targets
LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules/FindG2O)

if(${CMAKE_CXX_COMPILER_ID} EQUAL MSVC)
    add_definitions(-DWINDOWS)
    add_compile_options(/MP /bigobj)
    if (DEFINED WARNINGS AND NOT WARNINGS)
        add_compile_options(/w)
    endif()
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    add_compile_options(-O3 -march=native)
    if (DEFINED WARNINGS AND NOT WARNINGS)
        add_compile_options(-w)
    endif()
endif()

find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem log)
# This is unfortunate but some Boost libraries (e.g. Boost::log) need this.
add_definitions(-DBOOST_ALL_DYN_LINK)

find_package(Pangolin REQUIRED)
find_package(G2O REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
    ${EIGEN3_INCLUDE_DIR}
    ${Pangolin_INCLUDE_DIRS}
    ${G2O_INCLUDE_DIR}
)

add_subdirectory(Thirdparty/DBoW2)
add_subdirectory(utils)
add_subdirectory(ext_g2o)

# INTERFACE targets seems to be broken for "Visual Studio" generators. 
# I want to see these files in the generated solutions, hence the hack with
# a static library.
add_subdirectory(ext)

add_library(orb-slam2 STATIC
    src/System.cc
    src/Tracking.cc
    src/LocalMapping.cc
    src/LoopClosing.cc
    src/ORBextractor.cc
    src/ORBmatcher.cc
    src/FrameDrawer.cc
    src/Converter.cc
    src/MapPoint.cc
    src/KeyFrame.cc
    src/Map.cc
    src/MapDrawer.cc
    src/Optimizer.cc
    src/PnPsolver.cc
    src/Frame.cc
    src/KeyFrameDatabase.cc
    src/Sim3Solver.cc
    src/Initializer.cc
    src/Viewer.cc
)

target_link_libraries(orb-slam2
    ${OpenCV_LIBS}
    ${EIGEN3_LIBS}
    ${Pangolin_LIBRARIES}
    DBoW2
    g2o_slamext
    utils
    ${G2O_LIBS}
)

target_include_directories(orb-slam2 INTERFACE $<INSTALL_INTERFACE:include>)
install(TARGETS orb-slam2 EXPORT orb-slam2  
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include/orb-slam2)
install(EXPORT orb-slam2 FILE orb-slam2Config.cmake DESTINATION share/orb-slam2)

add_subdirectory(Examples)

if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if(BUILD_EXPERIMENTS)
    add_subdirectory(experiments)
endif()

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# Scripts and utilities as target
add_custom_target(orb-slam2-scripts SOURCES
    appveyor.yml
    .travis.yml
    CMakeSettings.json
    CppProperties.json
    .vs/launch.vs.json
    .vs/tasks.vs.json
    scripts/windows/bootstrap.bat
    scripts/windows/configure.bat
    scripts/windows/build.bat
    scripts/windows/run_slam_on_garching_test_drive.bat
    scripts/windows/run_semantic_monocular.bat
    scripts/linux/bootstrap.sh
    scripts/linux/build.sh
    scripts/linux/run_slam_on_garching_test_drive.sh
    scripts/ros/build.sh)

if (PRINT_CMAKE_VARIABLES)
    message(STATUS "VARIABLES used in ${PROJECT_NAME}")
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
endif()
