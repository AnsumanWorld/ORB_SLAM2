project(tests-fuser)

set(src_list
test_config.cpp
test_g2o.cpp
test_gps_fuser.cpp
test_gps_fuser.hpp
test_types.cpp
test_utils.cpp
)

set(lib_list GTest::GTest GTest::Main fuser)

add_executable(${PROJECT_NAME} ${src_list})
target_link_libraries(${PROJECT_NAME} ${lib_list})
if(USE_COTIRE)
    cotire(${PROJECT_NAME})
endif()

add_test(
NAME ${PROJECT_NAME}
COMMAND $<TARGET_FILE:${PROJECT_NAME}>
)
