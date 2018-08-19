cmake_minimum_required (VERSION 3.8)

project (Client)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build )

file (GLOB  SOURCES ./Sources/*.h ./Sources/*.cpp ./Sources/*.hpp)
set(Boost_INCLUDE_DIR "E:/Documents Work&Study/libraries/boost_1_67_0")
set(Boost_LIBRARY_DIR "E:/Documents Work&Study/libraries/boost_1_67_0/lib-msvc-14.1")
#find_package (Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories (${Boost_LIBRARY_DIR})

add_executable (Client ${SOURCES})
set_target_properties( Client PROPERTIES OUTPUT_NAME Client )
set_target_properties( Client PROPERTIES OUTPUT_NAME_DEBUG Client-debug )
set_target_properties( Client PROPERTIES OUTPUT_NAME_RELEASE Client )
