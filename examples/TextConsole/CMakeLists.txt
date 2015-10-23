cmake_minimum_required(VERSION 2.8)
INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR} )
ADD_EXECUTABLE( TextConsoleDemo TextConsoleDemo.cpp )
TARGET_LINK_LIBRARIES( TextConsoleDemo cvars )

