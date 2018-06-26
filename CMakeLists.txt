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

option(BUILD_EXAMPLES "Build Examples" ON )

if (CMAKE_SYSTEM_NAME MATCHES Windows AND CMAKE_VERSION LESS 3.11)
    # FindBoost for treatment of Boost imported targets
    LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules/FindBoost)
endif()

# FindG2O for treatment of G20 imported targets
LIST(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules/FindG2O)

add_definitions("-DCOMPILEDWITHC11")
if(WIN32)
    add_definitions("-DNOMINMAX")
    add_definitions("-DWINDOWS")
    add_definitions("-DORBSLAM_EXPORT")    
    add_compile_options("/MP")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -bigobj")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -bigobj")
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")
endif()

set(warning_options "")
if(NOT WARNINGS)
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(warning_options "-w")
    endif()
    if(MSVC)
        set(warning_options "/w")
    endif()
endif()
add_compile_options(${warning_options})

find_package(OpenCV REQUIRED)

if("${OpenCV_VERSION}" VERSION_LESS "2.4.3")
    message(FATAL_ERROR "OpenCV > 2.4.3 not found.")
endif()

find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem log)
# This is unfortunate but some Boost libraries (e.g. Boost::log) need this.
add_compile_options(-DBOOST_ALL_DYN_LINK)

if("${EIGEN3_VERSION_STRING}" VERSION_LESS "3.1.0")
    message(FATAL_ERROR "Eigen3 > 3.1.0 not found.")
endif()

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
add_library(orb-slam2-ext STATIC
    ext/app_monitor.h
    ext/app_monitor_api.h
    ext/app_monitor_api_impl.h
    ext/messages.h
    ext/statistics.h	
    ext/statistics.cc
    ext/GeodeticCoords.h
    ext/GeodeticCoords.cc
    ext/pos_adder.h
)

target_include_directories(orb-slam2-ext PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(orb-slam2-ext PUBLIC 
    Boost::boost
    Boost::log
    ${OpenCV_LIBS})

install(TARGETS orb-slam2-ext EXPORT orb-slam2
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/ext/ 
    DESTINATION include/orb-slam2/ext 
    FILES_MATCHING PATTERN *.h)

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
    utils)

target_link_libraries(orb-slam2 
    debug ${G2O_LIBS_DEBUG}
    optimized ${G2O_LIBS})


target_include_directories(orb-slam2 INTERFACE $<INSTALL_INTERFACE:include>)
install(TARGETS orb-slam2 EXPORT orb-slam2  
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include/orb-slam2)
install(EXPORT orb-slam2 FILE orb-slam2Config.cmake DESTINATION share/orb-slam2)

install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/ext/ 
    DESTINATION include/orb-slam2/ext 
    FILES_MATCHING PATTERN *.h)

# Build examples
if(BUILD_ALL_EXAMPLES)
    add_executable(rgbd_tum Examples/RGB-D/rgbd_tum.cc) 
    target_link_libraries(rgbd_tum orb-slam2)

    add_executable(stereo_kitti Examples/Stereo/stereo_kitti.cc) 
    target_link_libraries(stereo_kitti orb-slam2)

    add_executable(stereo_euroc Examples/Stereo/stereo_euroc.cc) 
    target_link_libraries(stereo_euroc orb-slam2)

    add_executable(mono_tum Examples/Monocular/mono_tum.cc) 
    target_link_libraries(mono_tum orb-slam2)

    add_executable(mono_kitti Examples/Monocular/mono_kitti.cc) 
    target_link_libraries(mono_kitti orb-slam2)

    add_executable(mono_euroc Examples/Monocular/mono_euroc.cc) 
    target_link_libraries(mono_euroc orb-slam2)

    add_executable(mono_video Examples/Monocular/mono_video.cc) 
    target_link_libraries(mono_video orb-slam2)
endif()

if(BUILD_EXAMPLES)
    add_executable(semantic_monocular 
        Examples/Monocular/semantic_monocular.cc 
        Examples/Monocular/semantic_monocular.hpp
        .vs/launch.vs.json
        CMakeSettings.json)
    target_link_libraries(semantic_monocular 
        orb-slam2
        orb-slam2-ext
        Boost::filesystem)

    # semantic_monocular_video is not using messages.h's types correctly and causing build issue, hence disabling
    # add_executable(semantic_monocular_video Examples/Monocular/semantic_monocular_video.cpp)
    # target_link_libraries(semantic_monocular_video orb-slam2 filesystem)

    add_executable(mono_grabbing Examples/Monocular/mono_grabbing.cc)
    target_link_libraries(mono_grabbing 
        orb-slam2
        orb-slam2-ext
        Boost::filesystem)

    add_executable(mono_video Examples/Monocular/mono_video.cc)
    target_link_libraries(mono_video
        orb-slam2
        orb-slam2-ext
        Boost::filesystem)

if(CMAKE_SYSTEM_NAME MATCHES Windows) 
    add_custom_command(TARGET mono_video POST_BUILD
                   COMMAND ${CMAKE_SOURCE_DIR}/products/copy_ffmpeg$<$<CONFIG:Debug>:d>.bat "$<TARGET_FILE_DIR:mono_video>")
endif()

endif()

# Build tools
if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()

# Experiments
if(BUILD_EXPERIMENTS)
    add_subdirectory(experiments)
endif()

# Tests
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
