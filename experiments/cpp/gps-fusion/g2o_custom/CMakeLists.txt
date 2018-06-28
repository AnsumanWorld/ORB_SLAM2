project(g2o_types_custom)

add_library(${PROJECT_NAME} SHARED 
    edge_point_xy_distance.hpp
    edge_se3_expmap_distance.hpp
    edge_se3_expmap_prior_2d.hpp
    g2o_types_custom_api.hpp
    precompile.hpp
    types_custom.hpp
    edge_point_xy_distance.cpp
    edge_se3_expmap_distance.cpp
    edge_se3_expmap_prior_2d.cpp
    precompile.cpp
    types_custom.cpp)

target_link_libraries(${PROJECT_NAME} 
    ${EIGEN3_LIBS}
    ${OPENGL_LIBRARY}
    util
    ${G2O_LIBS}
)

if(USE_COTIRE)
    cotire(${PROJECT_NAME})
endif()
