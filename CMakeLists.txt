cmake_minimum_required (VERSION 2.6)
project(BiogamesTracking)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/externals/OpenCV/include")

link_directories("${CMAKE_CURRENT_SOURCE_DIR}/externals/OpenCV/lib")

add_executable(BiogamesTracking
  src/main.cpp
)

target_link_libraries(BiogamesTracking "${CMAKE_CURRENT_SOURCE_DIR}/externals/OpenCV/lib/*.lib")
