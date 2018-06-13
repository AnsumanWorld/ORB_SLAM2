project(util)

set(src_list util.h util.cc)
add_library(${PROJECT_NAME} STATIC ${src_list})

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
    DESTINATION include/orb-slam2/util 
    FILES_MATCHING PATTERN *.h
)
