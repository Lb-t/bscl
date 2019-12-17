cmake_minimum_required(VERSION 3.0)
project(bscl)

if(DEFINED $CMAKE_BUILD_TYPE)
message("-- Build Type: " ${CMAKE_BUILD_TYPE})
else()
set(CMAKE_BUILD_TYPE Debug)
message("-- Build Type Default: " ${CMAKE_BUILD_TYPE})
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
aux_source_directory(${CMAKE_SOURCE_DIR}/src SRC)

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
set(OS_LIB pthread ncurses)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/linux SRC)
elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
set(OS_LIB ws2_32 winmm setupapi)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/win32/ SRC)
if (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/win32/win32 SRC)
elseif(${CMAKE_C_COMPILER_ID} STREQUAL GNU)
add_definitions("-D__USE_MINGW_ANSI_STDIO")
endif()
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
elseif (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")	
else ()
endif (CMAKE_SYSTEM_NAME MATCHES "Linux")

if(${CMAKE_C_COMPILER_ID} STREQUAL GNU)
add_compile_options(-Wall)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL Clang)
add_compile_options(-Wall)
elseif (${CMAKE_C_COMPILER_ID} STREQUAL MSVC)
add_definitions("-D_CRT_SECURE_NO_WARNINGS") 
else()
message("unknown compiler:" ${CMAKE_C_COMPILER_ID})
endif()

set(CMAKE_C_STANDARD 11)

add_library(bscl STATIC  ${SRC})
target_link_libraries(bscl ${OS_LIB})

add_executable(test_vector ${CMAKE_SOURCE_DIR}/test/test_vector.c)
target_link_libraries(test_vector bscl )

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")	
add_executable(test_serialport ${CMAKE_SOURCE_DIR}/test/test_serialport.c)
target_link_libraries(test_serialport bscl)
endif (CMAKE_SYSTEM_NAME MATCHES "Windows")	


add_executable(test_bstree ${CMAKE_SOURCE_DIR}/test/test_bstree.c)
target_link_libraries(test_bstree bscl  ${OS_LIB})

add_executable(test_http_server ${CMAKE_SOURCE_DIR}/test/test_http_server.c)
target_link_libraries(test_http_server bscl  ${OS_LIB})
add_executable(test_http ${CMAKE_SOURCE_DIR}/test/test_http.c)
target_link_libraries(test_http bscl  ${OS_LIB})
add_executable(test_network ${CMAKE_SOURCE_DIR}/test/test_network.c)
target_link_libraries(test_network bscl  ${OS_LIB})

add_executable(test_sort ${CMAKE_SOURCE_DIR}/test/test_sort.c ${CMAKE_SOURCE_DIR}/src/bscl_sort.c)


add_executable(test_getopt ${CMAKE_SOURCE_DIR}/test/test_getopt.c)
target_link_libraries(test_getopt bscl ${OS_LIB})


add_executable(md5 ${CMAKE_SOURCE_DIR}/tools/md5/main.c)
target_link_libraries(md5 bscl ${OS_LIB})

add_executable(serial_term ${CMAKE_SOURCE_DIR}/tools/sterm/sterm.c)
target_link_libraries(serial_term bscl ${OS_LIB})

install(TARGETS bscl
  CONFIGURATIONS Debug
  ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/../libs/lib
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include
  CONFIGURATIONS Debug
  DESTINATION ${PROJECT_SOURCE_DIR}/../libs/
)

install(TARGETS bscl
  CONFIGURATIONS Release
  ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/../libs/lib
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include
  CONFIGURATIONS Release
  DESTINATION ${PROJECT_SOURCE_DIR}/../libs/
)
