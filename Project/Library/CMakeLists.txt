cmake_minimum_required(VERSION 3.8)

file(GLOB SOURCES "Inc/*.h" "Inc/*.hpp" "Src/*.cpp")

set(Boost_INCLUDE_DIR "E:/Documents Work&Study/libraries/boost_1_67_0")
set(Boost_LIBRARY_DIR "E:/Documents Work&Study/libraries/boost_1_67_0/lib-msvc-14.1")

include_directories(${Boost_INCLUDE_DIR})
link_directories (${Boost_LIBRARY_DIR})

add_library (Library STATIC ${SOURCES})

target_include_directories (Library PRIVATE Inc)