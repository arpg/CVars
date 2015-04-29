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
configure_file( src/CVarVersion.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/CVars/CVarVersion.h IMMEDIATE )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/include )


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
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )
set( CVAR_SRCS
    src/cvars_tinyxml.cpp
    src/cvars_tinyxmlerror.cpp
    src/cvars_tinyxmlparser.cpp
    src/CVar.cpp
    src/CVarParse.cpp
    src/Timestamp.cpp
    src/Trie.cpp
    src/TrieNode.cpp
   )

set( CVAR_HDRS
    include/CVars/CVarVersion.h
    include/CVars/cvars_tinyxml.h
    include/CVars/CVar.h
    include/CVars/CVarVectorIO.h
    include/CVars/CVarMapIO.h
    include/CVars/Timestamp.h
    include/CVars/Trie.h
    include/CVars/TrieNode.h
  )

add_definitions( -DTIXML_USE_STL )
add_library( cvars ${CVAR_SRCS}) #${CVAR_HDRS} )

###############################################################################
set( BUILD_GLCONSOLE_DEMO CACHE BOOL OFF )
if( BUILD_GLCONSOLE_DEMO )
    add_subdirectory( Example )
endif()

###############################################################################
# Headers for GLConsole
set( GLCONSOLE_HEADERS
        include/GLConsole/GLFont.h
        include/GLConsole/GLColor.h
        include/GLConsole/GLConsole.h
        include/GLConsole/GLConsoleFunction.h
   )

###############################################################################
# Headers for FLConsole
set( FLCONSOLE_HEADERS
        include/FLConsole/FLConsole.h
        include/FLConsole/FLConsoleFunction.h
   )

##############################################################################
install_package(
    PKG_NAME CVars
    LIB_NAME cvars
    VERSION ${VERSION}
    DESCRIPTION "Library for runtime variable editing for fast prototyping."
    INSTALL_HEADERS ${FLCONSOLE_HEADERS} ${GLCONSOLE_HEADERS} ${CVAR_HDRS}
    INSTALL_GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/include/CVars/CVarVersion.h
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    )

# make an uninstall target
include(${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in)
add_custom_target(uninstall
    COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

