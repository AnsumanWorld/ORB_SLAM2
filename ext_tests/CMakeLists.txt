cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(orb-slam2-ext_tests.boost C CXX)

if (NOT COMMAND cotire)
    include(${VendorDir}/tools/cotire/CMake/cotire.cmake)
endif()

add_executable(${PROJECT_NAME} 
    # For cotire stdafx.cpp has to be the first file in the list!
    stdafx.cpp
    stdafx.h
    ${PROJECT_NAME}.h
    ${PROJECT_NAME}.cpp
    tests.boost.ds_tsr.cpp)
 
# cotire support for precompiled headers
set_target_properties(${PROJECT_NAME} PROPERTIES 
    COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h"
    COTIRE_ADD_UNITY_BUILD FALSE)
cotire(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    orb-slam2-ext_ds
    orb-slam2-ext
    orb-slam2
    Boost::unit_test_framework)

# support for ctest
add_test(
    NAME ${PROJECT_NAME}
    COMMAND $<TARGET_FILE:${PROJECT_NAME}> 
    --build_info=yes 
    --detect_memory_leaks=1 
    --log_format=HRF
    --log_level=test_suite 
    --log_sink=stdout 
    --report_level=short) 

# add runsettings file if we are targeting Visual Studio
if(${CMAKE_GENERATOR} MATCHES "^Visual.*")
    target_sources(${PROJECT_NAME} 
        PRIVATE 
        tests.runsettings)
endif()