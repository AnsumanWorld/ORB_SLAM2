project(test-orb-slam2)

set(src_list
test_coord_conv.cpp
)

set(lib_list GTest::GTest GTest::Main orb-slam2)

add_executable(${PROJECT_NAME} ${src_list})
target_link_libraries(${PROJECT_NAME} ${lib_list})

add_test(
NAME ${PROJECT_NAME}
COMMAND $<TARGET_FILE:${PROJECT_NAME}>
)
