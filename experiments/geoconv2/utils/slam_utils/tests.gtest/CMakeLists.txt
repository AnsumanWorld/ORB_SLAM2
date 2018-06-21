project(slam-tools.tests.gtest)

find_package(GTest REQUIRED)
include_directories(
	${OpenCV_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}
	main.cpp
	gtest_run_coordinates_basic_functions.cpp
	gtest_run_pose_basic_functions.cpp
)

target_link_libraries(${PROJECT_NAME}
	${OpenCV_LIBS}
    GTest::GTest
    slam-tools)

# support for ctest
add_test(
    NAME ${PROJECT_NAME}
    COMMAND $<TARGET_FILE:${PROJECT_NAME}>) 
