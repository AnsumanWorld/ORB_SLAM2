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
    add_executable(semantic_monocular 
        Monocular/semantic_monocular.cc 
        Monocular/semantic_monocular.hpp
    )
    target_link_libraries(semantic_monocular 
        orb-slam2
        orb-slam2-ext
        Boost::filesystem
    )

    # semantic_monocular_video is not using messages.h's types correctly and causing build issue, hence disabling
    # add_executable(semantic_monocular_video Monocular/semantic_monocular_video.cpp)
    # target_link_libraries(semantic_monocular_video orb-slam2 filesystem)

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

    add_executable(run_ds_semantic Monocular/run_ds_semantic.cc)
    target_link_libraries(run_ds_semantic 
        orb-slam2
        orb-slam2-ext
        Boost::filesystem)

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
