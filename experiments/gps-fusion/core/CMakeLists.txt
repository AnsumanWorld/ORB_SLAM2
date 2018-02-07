project(fuser)

set(hpp_list
    calci.hpp
    config.hpp
    gps_ba.hpp
    gps_fuser.hpp
    log.hpp
    macros.hpp
    precompile.hpp
    types.hpp
)

set(cpp_list
    calci.cpp
    config.cpp
    gps_ba.cpp
    gps_fuser.cpp
    log.cpp
    precompile.cpp
    types.cpp
)

set(src_list ${hpp_list} ${cpp_list})
set(lib_list ${G2O_LIBS} ${EIGEN3_LIBS} g2o_types_custom utils)

add_library(${PROJECT_NAME} STATIC ${src_list})
target_link_libraries(${PROJECT_NAME} ${lib_list})
if(USE_COTIRE)
    cotire(${PROJECT_NAME})
endif()
