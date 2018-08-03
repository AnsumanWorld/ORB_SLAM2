cmake_minimum_required(VERSION 3.0)
project(orb-slam2-ext_ds C CXX)

add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/ds_tsr_args.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ds_tsr.h)
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME} INTERFACE
    orb-slam2-ext
    Boost::filesystem
    Boost::program_options)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME} 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
install(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DESTINATION include/${PROJECT_NAME}
    FILES_MATCHING PATTERN *.h)
