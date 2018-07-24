cmake_minimum_required(VERSION 3.0)
project(orb-slam2-ext C CXX)

add_library(orb-slam2-ext STATIC
    app_monitor.h
    app_monitor_api.h
    app_monitor_api_impl.h
    messages.h
    statistics.h	
    statistics.cc
    GeodeticCoords.h
    GeodeticCoords.cc
    pos_adder.h
    ds_kitty.h
    ds_dashcam.h
    ds_traficsign.h
    slam_object.h
    run_tracking.h
)

target_include_directories(orb-slam2-ext PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(orb-slam2-ext PUBLIC 
    Boost::boost
    Boost::log
    ${OpenCV_LIBS}
    geo_convertor)

install(TARGETS orb-slam2-ext EXPORT orb-slam2
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/orb-slam2
    FILES_MATCHING PATTERN *.h)
