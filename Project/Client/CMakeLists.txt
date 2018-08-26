cmake_minimum_required(VERSION 3.8)

project (Client)


file(GLOB SOURCES "Inc/*.h" "Inc/*.hpp" "Src/*.cpp")

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build )

set(Boost_INCLUDE_DIR "E:/Documents Work&Study/libraries/boost_1_67_0")
set(Boost_LIBRARY_DIR "E:/Documents Work&Study/libraries/boost_1_67_0/lib-msvc-14.1")

include_directories(${Boost_INCLUDE_DIR})
link_directories (${Boost_LIBRARY_DIR})

add_executable (Client ${SOURCES})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../Library ${CMAKE_CURRENT_BINARY_DIR}/Library)


target_link_libraries (Client LINK_PUBLIC Library)
target_include_directories (Client PRIVATE Inc)
target_include_directories (Client PRIVATE "../Library/Inc")

set_target_properties( Client PROPERTIES OUTPUT_NAME Client )
set_target_properties( Client PROPERTIES OUTPUT_NAME_DEBUG Client-debug )
set_target_properties( Client PROPERTIES OUTPUT_NAME_RELEASE Client )

