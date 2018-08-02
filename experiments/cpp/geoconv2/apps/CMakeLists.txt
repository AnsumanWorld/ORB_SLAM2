project(apps)

add_executable(app-gps2local app-gps2local.cpp)
target_link_libraries(app-gps2local slam-tools)

add_executable(app-oxts2local app-oxts2local.cpp)
target_link_libraries(app-oxts2local slam-tools ${Boost_LIBRARIES})

add_executable(app-local2gps app-local2gps.cpp)
target_link_libraries(app-local2gps slam-tools)