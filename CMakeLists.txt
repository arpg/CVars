cmake_minimum_required( VERSION 2.8 )

# Be sure to change these with each update
SET( VERSION_MAJOR 2 )
SET( VERSION_MINOR 4 )
SET( VERSION_PATCH 0 )
SET( VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} )

project( CVars )

# Add to module path, so we can find our cmake modules
list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules )
include( install_package )

# Set version values in CVarVersion.h  (see CVarVersion.h.in)
configure_file( CVars/CVarVersion.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/CVars/CVarVersion.h IMMEDIATE )


###############################################################################
# Setup compiler flags
IF( NOT MSVC )
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

    # Setup strict debugging environment.
    IF( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g -fno-strict-aliasing ")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -fno-strict-aliasing ")
    ENDIF( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
ENDIF( NOT MSVC )

###############################################################################
# Build CVars library
IF(WIN32)
	SET(WIN32 1)
ENDIF(WIN32)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR} )
set( CVAR_SRCS
    CVars/cvars_tinyxml.cpp
    CVars/cvars_tinyxmlerror.cpp
    CVars/cvars_tinyxmlparser.cpp
    CVars/CVar.cpp
    CVars/CVarParse.cpp
    CVars/Timestamp.cpp
    CVars/Trie.cpp
    CVars/TrieNode.cpp
   )

set( CVAR_HDRS
    CVars/CVarVersion.h
    CVars/cvars_tinyxml.h
    CVars/CVar.h
    CVars/CVarVectorIO.h
    CVars/CVarMapIO.h
    CVars/Timestamp.h
    CVars/Trie.h
    CVars/TrieNode.h
  )

add_definitions( -DTIXML_USE_STL )
add_library( cvars ${CVAR_SRCS} ${CVAR_HDRS} )

###############################################################################
set( BUILD_GLCONSOLE_DEMO CACHE BOOL OFF )
if( BUILD_GLCONSOLE_DEMO )
    add_subdirectory( Example )
endif()

###############################################################################
# Headers for GLConsole
set( GLCONSOLE_HEADERS
        GLConsole/GLFont.h
        GLConsole/GLColor.h
        GLConsole/GLConsole.h
        GLConsole/GLConsoleFunction.h
   )
#install( FILES ${GLCONSOLE_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/GLConsole )

###############################################################################
# Headers for FLConsole
set( FLCONSOLE_HEADERS
        FLConsole/FLConsole.h
        FLConsole/FLConsoleFunction.h
   )
#install( FILES ${FLCONSOLE_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/FLConsole )


##############################################################################
install_package(
    PKG_NAME CVars
    LIB_NAME cvars
    VERSION ${VERSION}
    DESCRIPTION "Library for runtime variable editing for fast prototyping."
    INSTALL_HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
                        ${CMAKE_CURRENT_SOURCE_DIR}/CVars
                        ${CMAKE_CURRENT_SOURCE_DIR}/GLConsole
                        ${CMAKE_CURRENT_SOURCE_DIR}/FLConsole
    #INSTALL_HEADERS CVars/CVarVersion.h CVars/cvars_tinyxml.h CVars/CVar.h
    #                CVars/CVarVectorIO.h CVars/CVarMapIO.h CVars/Timestamp.h
    #                CVars/Trie.h CVars/TrieNode.h
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    #LINK_LIBS cvars
    )

# make an uninstall target
include(${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in)
add_custom_target(uninstall
    COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

