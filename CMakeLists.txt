cmake_minimum_required(VERSION 3.8.0)

# Prevent from running cmake in root folder.
if (CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "cmake needs to be called in a sub directory.")
endif ()

project(audio_box)

find_package(fst REQUIRED)
find_package(libsndfile REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(PortAudio REQUIRED)

link_directories(${LIBSNDFILE_LIBRARY_DIRS})

# Use folder.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Debug output path.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}/Debug)

# Release output path.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/${CMAKE_SYSTEM_PROCESSOR}/Release)

set(ABOX_INCLUDE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ABOX_SOURCE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source)

add_definitions("-DRESOURCES_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/resources\"")

file(GLOB_RECURSE ABOX_INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${ABOX_INCLUDE_DIRECTORY}/*.hpp)
file(GLOB_RECURSE ABOX_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${ABOX_SOURCE_DIRECTORY}/*.cpp)

# File groups.
source_group(source FILES ${ABOX_SOURCE_FILES})
source_group(include FILES ${ABOX_INCLUDE_FILES})

add_library(${PROJECT_NAME} SHARED ${ABOX_SOURCE_FILES} ${ABOX_INCLUDE_FILES})

# Target properties.
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O3 ${CMAKE_CXX_FLAGS}")

link_directories(/usr/local/lib/)
target_include_directories(${PROJECT_NAME} PUBLIC ${ABOX_INCLUDE_DIRECTORY})
target_include_directories(${PROJECT_NAME} PUBLIC ${PORTAUDIO_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${FST_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${LIBSNDFILE_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})

target_include_directories(${PROJECT_NAME} PUBLIC /usr/local/include/)
target_include_directories(${PROJECT_NAME} PUBLIC /usr/local/include/freetype2)


# target_link_libraries(${PROJECT_NAME} /usr/local/lib/libportaudio.a)
target_link_libraries(${PROJECT_NAME} ${PORTAUDIO_LIBRARY})

# TODO : Change this !!! Why is LIBSNDFILE_LIBRARY not working ??
target_link_libraries(${PROJECT_NAME} /usr/local/lib/libsndfile.dylib)
target_link_libraries(${PROJECT_NAME} ${LIBSNDFILE_LIBRARY})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreAudio")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AudioUnit")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreAudioKit")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework CoreFoundation")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework AudioToolbox")

add_subdirectory(dev)
