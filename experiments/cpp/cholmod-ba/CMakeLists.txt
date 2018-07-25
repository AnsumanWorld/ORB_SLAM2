project(cholmod_ba)

find_package(Cholmod)
find_package(BLAS)
find_package(LAPACK)
find_package(SuiteSparse)

include_directories(
	${CHOLMOD_INCLUDE_DIR}
	${G2O_INCLUDE_DIR}
)

add_executable(
	${PROJECT_NAME}
	main.cpp
)

set(lib_list 
	${CHOLMOD_LIBRARY}
	${AMD_LIBRARY}
	${SUITESPARSECONFIG_LIBRARY}
	${COLAMD_LIBRARY}
	${BLAS_LIBRARIES}
	${LAPACK_LIBRARIES}
	${G2O_LIBS}
)

target_link_libraries(
	${PROJECT_NAME}
	${lib_list}
)
