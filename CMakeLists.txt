cmake_minimum_required(VERSION 3.0)
project(ezclib)
include_directories(${CMAKE_SOURCE_DIR}/include)
aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")	
#MESSAGE(STATUS "current platform: Linux ")
set(OS_LIB )
aux_source_directory(${CMAKE_SOURCE_DIR}/src/linux SRC)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")	
#MESSAGE(STATUS "current platform: Windows")
set(OS_LIB ws2_32 winmm)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/win32 SRC)
elseif (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")	
#MESSAGE(STATUS "current platform: FreeBSD")
else ()	
#MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
endif (CMAKE_SYSTEM_NAME MATCHES "Linux")

set(CMAKE_C_STANDARD 11)

add_library(ezc STATIC  ${SRC})
target_link_libraries(ezc ${OS_LIB})

add_executable(test_vector ${CMAKE_SOURCE_DIR}/test/test_vector.c)
target_link_libraries(test_vector ezc )

add_executable(test_serialport ${CMAKE_SOURCE_DIR}/test/test_serialport.c)
target_link_libraries(test_serialport ezc)



add_executable(test_bstree ${CMAKE_SOURCE_DIR}/test/test_bstree.c)
target_link_libraries(test_bstree ezc)

add_executable(test_http_server ${CMAKE_SOURCE_DIR}/test/test_http_server.c)
target_link_libraries(test_http_server ezc)
add_executable(test_http ${CMAKE_SOURCE_DIR}/test/test_http.c)
target_link_libraries(test_http ezc )
add_executable(test_network ${CMAKE_SOURCE_DIR}/test/test_network.c)
target_link_libraries(test_network ezc )

add_executable(test_sort ${CMAKE_SOURCE_DIR}/test/test_sort.c ${CMAKE_SOURCE_DIR}/src/platform_sort.c)

