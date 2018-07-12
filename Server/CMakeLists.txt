cmake_minimum_required (VERSION 3.8)

project (Server)

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build )

#set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/build-debug )
#set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/build-debug )
#set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/build-debug )
# Second, for multi-config builds (e.g. msvc)
#foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
    #string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    #set( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_SOURCE_DIR}/build )
    #set( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_SOURCE_DIR}/build )
    #set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PROJECT_SOURCE_DIR}/build )
#endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

file (GLOB  SOURCES ./Sources/*.h ./Sources/*.cpp)
set(Boost_INCLUDE_DIR "E:/Documents Work&Study/libraries/boost_1_67_0")
set(Boost_LIBRARY_DIR "E:/Documents Work&Study/libraries/boost_1_67_0/lib-msvc-14.1")
#find_package (Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories (${Boost_LIBRARY_DIR})

add_executable (Server ${SOURCES})
set_target_properties( Server PROPERTIES OUTPUT_NAME Server )
set_target_properties( Server PROPERTIES OUTPUT_NAME_DEBUG Server-debug )
set_target_properties( Server PROPERTIES OUTPUT_NAME_RELEASE Server )

#set_target_properties( Server PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR} )
#set_target_properties( Server PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/Debug )
#set_target_properties( Server PROPERTIES CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/Release )
#set_target_properties( Server PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR} )
#set_target_properties( Server PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/Debug )
#set_target_properties( Server PROPERTIES CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/Release )

