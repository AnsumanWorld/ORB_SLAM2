project(util)

find_package(Boost REQUIRED)

set(hpp_list
util.hpp
)

set(cpp_list
util.cpp
)

set(src_list ${hpp_list} ${cpp_list})
set(lib_list ${EIGEN3_LIBS})

add_library(${PROJECT_NAME} STATIC ${src_list})
target_link_libraries(${PROJECT_NAME} ${lib_list})

if(USE_COTIRE)
    cotire(${PROJECT_NAME})
endif()
