cmake_minimum_required(VERSION 2.8)
project( BinaryzationVideo )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( BinaryzationVideo main.cpp )
target_link_libraries( BinaryzationVideo ${OpenCV_LIBS} )
