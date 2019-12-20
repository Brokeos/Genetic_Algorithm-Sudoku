cmake_minimum_required(VERSION 3.0)

project("Genetic Algorithm" C)

set(CMAKE_INSTALL_RPATH_USE_LINK_PATH true)

add_library(ga SHARED ga.c ga.h ga.inc)

add_executable(sudoku sudoku.c)

target_link_libraries(sudoku ga)
target_link_libraries(sudoku /usr/local/lib/libyaml.a)

install(
	TARGETS ga
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
)

install(
	FILES ga.h
	DESTINATION include
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/GAConfig.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/GAConfig.cmake
	INSTALL_DESTINATION cmake
)
install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/GAConfig.cmake
	DESTINATION cmake
)

enable_testing()

find_program(VALGRIND "valgrind")

file(GLOB FILES "${CMAKE_CURRENT_SOURCE_DIR}/test-*.c")

foreach(FILENAME ${FILES})
	get_filename_component(SRC ${FILENAME} NAME)
	get_filename_component(TEST ${FILENAME} NAME_WE)
	add_executable(${TEST} ${SRC} ga.c ga.h ga.inc)
	add_dependencies(${TEST} ga)
	target_link_libraries(${TEST} ga)
	if(VALGRIND)
		add_test("${TEST}[valgrind]" ${VALGRIND} --leak-check=full --quiet --error-exitcode=1 ./${TEST})
    	add_test("${TEST}[normal]" ./${TEST})
    else()
    	add_test("${TEST}" ./${TEST})
	endif()
endforeach()

set(CPACK_SOURCE_GENERATOR "ZIP")
set(CPACK_SOURCE_IGNORE_FILES "~$;${CPACK_SOURCE_IGNORE_FILES}")
include(CPack)

