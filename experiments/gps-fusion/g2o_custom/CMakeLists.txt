project(g2o_types_custom)

set(hpp_list
edge_point_xy_distance.hpp
edge_se3_expmap_distance.hpp
edge_se3_expmap_prior_2d.hpp
g2o_types_custom_api.hpp
precompile.hpp
types_custom.hpp
)

set(cpp_list
edge_point_xy_distance.cpp
edge_se3_expmap_distance.cpp
edge_se3_expmap_prior_2d.cpp
precompile.cpp
types_custom.cpp
)

set(src_list ${hpp_list} ${cpp_list})
set(lib_list ${G2O_LIBS} ${EIGEN3_LIBS} ${OPENGL_LIBRARY} utils)

add_library(${PROJECT_NAME} SHARED ${src_list})
target_link_libraries(${PROJECT_NAME} ${lib_list})
if(USE_COTIRE)
    cotire(${PROJECT_NAME})
endif()
