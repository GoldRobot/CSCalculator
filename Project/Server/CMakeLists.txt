cmake_minimum_required(VERSION 3.8)

project (Server)


file(GLOB SOURCES "Inc/*.h" "Inc/*.hpp" "Src/*.cpp")

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build )

set(Boost_INCLUDE_DIR "E:/Documents Work&Study/libraries/boost_1_67_0")
set(Boost_LIBRARY_DIR "E:/Documents Work&Study/libraries/boost_1_67_0/lib-msvc-14.1")

include_directories(${Boost_INCLUDE_DIR})
link_directories (${Boost_LIBRARY_DIR})

add_executable (Server ${SOURCES})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../Library ${CMAKE_CURRENT_BINARY_DIR}/Library)


target_link_libraries (Server LINK_PUBLIC Library)
target_include_directories (Server PRIVATE Inc)
target_include_directories (Server PRIVATE "../Library/Inc")

set_target_properties( Server PROPERTIES OUTPUT_NAME Server )
set_target_properties( Server PROPERTIES OUTPUT_NAME_DEBUG Server-debug )
set_target_properties( Server PROPERTIES OUTPUT_NAME_RELEASE Server )

