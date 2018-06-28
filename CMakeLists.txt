# CMakeList.txt: проект CMake для Server v0.03; включите исходный код и определения
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Добавьте источник для исполняемого файла этого проекта.

project (Server)

file (GLOB  SOURCES ./*.h ./*.cpp)
set(Boost_INCLUDE_DIR "E:/Documents Work&Study/libraries/boost_1_67_0")
set(Boost_LIBRARY_DIR "E:/Documents Work&Study/libraries/boost_1_67_0/lib64-msvc-14.1")
find_package (Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories (${Boost_LIBRARY_DIR})

#add_executable (Server main.cpp server_main.cpp server_main.h)
add_executable (Server ${SOURCES})

#target_link_libraries ( Server ${Boo})

#add_executable(main ${SOURCES})

#add_executable (Serverv003 "main.cpp" "server_main.cpp" "server_main.h")

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
