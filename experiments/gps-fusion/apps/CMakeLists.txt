project(apps-gps-fusion)

set(lib_list fuser)

add_executable(app-gps-fusion app-gps-fusion.cpp)
target_link_libraries(app-gps-fusion ${lib_list})

add_executable(app-circle-gen app-circle-gen.cpp)
target_link_libraries(app-circle-gen ${lib_list})

add_executable(app-extract-vertex app-extract-vertex.cpp)
target_link_libraries(app-extract-vertex ${lib_list})

add_executable(app-gps-ba app-gps-ba.cpp)
target_link_libraries(app-gps-ba ${lib_list})

add_executable(app-add-cam-param app-add-cam-param.cpp)
target_link_libraries(app-add-cam-param ${lib_list})

set(cotire_targets
app-add-cam-param
app-gps-fusion
app-circle-gen
app-extract-vertex
app-gps-ba)

if(USE_COTIRE)
    cotire(${cotire_targets})
endif()