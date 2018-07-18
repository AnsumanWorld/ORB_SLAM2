project(utils)

add_subdirectory(geo_convertor)

set(src_list utils.h utils.cc extract_gps.cc gps_parser.cc)
add_library(${PROJECT_NAME} STATIC ${src_list})

target_link_libraries(${PROJECT_NAME}
    ${EIGEN3_LIBS}
    geo_convertor
    Boost::boost
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

install(TARGETS ${PROJECT_NAME} EXPORT orb-slam2
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY ${PROJECT_SOURCE_DIR} 
    DESTINATION include/orb-slam2
    FILES_MATCHING PATTERN *.h
)
