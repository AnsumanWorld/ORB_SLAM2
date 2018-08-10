cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(orb-slam2-examples C CXX)

if (NOT COMMAND cotire)
    include(${VendorDir}/tools/cotire/CMake/cotire.cmake)
endif()

if(BUILD_ALL_EXAMPLES)
    add_executable(rgbd_tum RGB-D/rgbd_tum.cc) 
    target_link_libraries(rgbd_tum orb-slam2 orb-slam2-ext)

    add_executable(stereo_kitti Stereo/stereo_kitti.cc) 
    target_link_libraries(stereo_kitti orb-slam2 orb-slam2-ext)

    add_executable(stereo_euroc Stereo/stereo_euroc.cc) 
    target_link_libraries(stereo_euroc orb-slam2 orb-slam2-ext)

    add_executable(mono_tum Monocular/mono_tum.cc) 
    target_link_libraries(mono_tum orb-slam2 orb-slam2-ext)

    add_executable(mono_kitti Monocular/mono_kitti.cc) 
    target_link_libraries(mono_kitti orb-slam2 orb-slam2-ext)

    add_executable(mono_euroc Monocular/mono_euroc.cc) 
    target_link_libraries(mono_euroc orb-slam2 orb-slam2-ext)
endif()

if(BUILD_EXAMPLES)
    add_executable(mono_grabbing Monocular/mono_grabbing.cc)
    target_link_libraries(mono_grabbing 
        orb-slam2
        orb-slam2-ext
        Boost::filesystem
    )

    add_executable(mono_video Monocular/mono_video.cc)
    target_link_libraries(mono_video
        orb-slam2
        orb-slam2-ext
        Boost::filesystem
    )

    add_executable(run_ds_kitty Monocular/run_ds_kitty.cc)
    target_link_libraries(run_ds_kitty 
        orb-slam2
        orb-slam2-ext
        Boost::filesystem)

    add_executable(run_ds_tsr 
        # For cotire stdafx.cpp has to be the first file in the list!
        stdafx.cpp
        stdafx.h
        Monocular/run_ds_tsr.cc)
    target_include_directories(run_ds_tsr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(run_ds_tsr 
        orb-slam2
        orb-slam2-ext
        orb-slam2-ext_ds
        Boost::filesystem)
    set_target_properties(run_ds_tsr PROPERTIES 
        COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h"
        COTIRE_ADD_UNITY_BUILD FALSE)
    #cotire(run_ds_tsr)

    add_executable(run_ds_dashcam Monocular/run_ds_dashcam.cc)
    target_link_libraries(run_ds_dashcam
        orb-slam2
        orb-slam2-ext
        Boost::filesystem)

    if(CMAKE_SYSTEM_NAME MATCHES Windows) 
        add_custom_command(TARGET mono_video POST_BUILD
            COMMAND ${CMAKE_SOURCE_DIR}/products/copy_ffmpeg$<$<CONFIG:Debug>:d>.bat "$<TARGET_FILE_DIR:mono_video>")
        add_custom_command(TARGET run_ds_dashcam POST_BUILD
            COMMAND ${CMAKE_SOURCE_DIR}/products/copy_ffmpeg$<$<CONFIG:Debug>:d>.bat "$<TARGET_FILE_DIR:run_ds_dashcam>")
    endif()
endif()
