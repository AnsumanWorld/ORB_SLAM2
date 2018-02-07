project(geoconv)

set(src_list main.cpp)

add_executable(${PROJECT_NAME} ${src_list})
target_link_libraries(${PROJECT_NAME} orb-slam2-ext)
