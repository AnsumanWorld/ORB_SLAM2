project(orb-extract)

add_executable(${PROJECT_NAME}
    main.cpp)

target_link_libraries(${PROJECT_NAME}
    orb-slam2 
    Boost::filesystem)
