project(g2o_slamext)

find_package(OpenGL REQUIRED)
add_definitions(
    -DG2O_HAVE_OPENGL
    -DG2O_SHARED_LIBS
    -Dtypes_slamext_EXPORTS
)

set(hpp_list
edge_se3_expmap_dist.h
edge_se3_expmap_prior_3d.h
g2o_types_slamext_api.h
types_slamext.h
)

set(cpp_list
edge_se3_expmap_dist.cc
edge_se3_expmap_prior_3d.cc
types_slamext.cc
)

set(src_list ${hpp_list} ${cpp_list})
set(lib_list ${G2O_LIBS} ${EIGEN3_LIBS} ${OPENGL_LIBRARY} utils)

add_library(${PROJECT_NAME} SHARED ${src_list})
target_link_libraries(${PROJECT_NAME} ${lib_list})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

install(TARGETS ${PROJECT_NAME} EXPORT orb-slam2
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

install(DIRECTORY ${PROJECT_SOURCE_DIR} 
    DESTINATION include/orb-slam2/ext_g2o 
    FILES_MATCHING PATTERN *.h)
