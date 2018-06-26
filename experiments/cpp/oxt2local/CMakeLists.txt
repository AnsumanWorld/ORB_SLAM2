project(oxt2local)

add_executable(${PROJECT_NAME}
    main.cpp)

target_link_libraries(${PROJECT_NAME}
    orb-slam2-ext
    orb-slam2
    utils
    Boost::filesystem)
