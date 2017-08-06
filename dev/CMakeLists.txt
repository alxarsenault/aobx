cmake_minimum_required(VERSION 3.8.0)

# Prevent from running cmake in root folder.
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "cmake needs to be called in a sub directory.")
endif ()

project(audio_box_dev)

set(ABOX_DEV_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/)
file(GLOB_RECURSE ABOX_DEV_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${ABOX_DEV_DIRECTORY}/*.cpp)

add_executable(${PROJECT_NAME} ${ABOX_DEV_SOURCE_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set(CMAKE_CXX_STANDARD 14)

target_include_directories(${PROJECT_NAME} PUBLIC ${ABOX_INCLUDE_DIRECTORY})
target_include_directories(${PROJECT_NAME} PUBLIC /usr/local/include/)
target_link_libraries(${PROJECT_NAME} audio_box)
target_link_libraries(${PROJECT_NAME} axlib)
# target_link_libraries(${PROJECT_NAME} ${LIBSNDFILE_LIBRARY})


