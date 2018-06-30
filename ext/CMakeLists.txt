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
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/orb-slam2
    FILES_MATCHING PATTERN *.h)
