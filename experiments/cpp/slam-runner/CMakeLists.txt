project(slam-runner)

find_package(Boost REQUIRED COMPONENTS filesystem program_options)

add_executable(${PROJECT_NAME}
    log.h
    config.h
    main.cpp)

target_link_libraries(${PROJECT_NAME}
    orb-slam2
    orb-slam2-ext
    Boost::filesystem
    Boost::program_options
)
