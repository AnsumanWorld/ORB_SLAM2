project(tests-orb-slam2)

find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME} 
    test_coord_conv.cpp)

target_link_libraries(${PROJECT_NAME} 
    GTest::GTest GTest::Main orb-slam2 orb-slam2-ext)

add_test(
    NAME ${PROJECT_NAME}
    COMMAND $<TARGET_FILE:${PROJECT_NAME}>
)
