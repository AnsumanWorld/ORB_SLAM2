cmake_minimum_required(VERSION 3.0)
project(eval_odometry)

set(src_list
    evaluate_odometry.cpp
    matrix.cpp
    matrix.h
)

add_executable(${PROJECT_NAME} ${src_list})
