cmake_minimum_required(VERSION 3.5)

project (cv_test)

set(EXECUTABLE_OUTPUT_PATH ../bin)  

set(CMAKE_BUILD_TYPE Debug)

find_package(OpenCV REQUIRED)

aux_source_directory(src DIR_SRC)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(main ${DIR_SRC})

link_directories(${OpenCV_LIBRARY_DIRS})

target_link_libraries (main ${OpenCV_LIBS})