project(g2o_slamext)

find_package(OpenGL REQUIRED)

add_definitions(
    -DG2O_HAVE_OPENGL
    -DG2O_SHARED_LIBS
    -Dtypes_slamext_EXPORTS
)

add_library(${PROJECT_NAME} SHARED
    edge_se3_expmap_dist.h
    edge_se3_expmap_prior_3d.h
    g2o_types_slamext_api.h
    types_slamext.h
    edge_se3_expmap_dist.cc
    edge_se3_expmap_prior_3d.cc
    types_slamext.cc)

target_link_libraries(${PROJECT_NAME}
    ${EIGEN3_LIBS}
    ${OPENGL_LIBRARY}
    utils
)

target_link_libraries(${PROJECT_NAME} ${G2O_LIBS})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>)

install(TARGETS ${PROJECT_NAME} EXPORT orb-slam2
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

install(DIRECTORY ${PROJECT_SOURCE_DIR} 
    DESTINATION include/orb-slam2
    FILES_MATCHING PATTERN *.h)
