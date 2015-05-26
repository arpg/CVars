cmake_minimum_required( VERSION 2.8 )

# Be sure to change these with each update
SET( VERSION_MAJOR 2 )
SET( VERSION_MINOR 5 )
SET( VERSION_PATCH 0 )
SET( VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} )

project( CVars )

# Add to module path, so we can find our cmake modules
list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules )
include( install_package )
include(SetPlatformVars)

# Properly source GL headers.
if(ANDROID OR IOS)
    set(HAVE_GLES 1)
    option(BUILD_FOR_GLES_2 "Build for OpenGL ES 2 instead of ES 1" ON )
    if(BUILD_FOR_GLES_2)
        set(HAVE_GLES_2 1)
    endif()
endif()

if( ANDROID  )
    # Android specific display code
    list(APPEND HEADERS ${INCDIR}/display_android.h )
    list(APPEND SOURCES ${SRCDIR}/display_android.cpp )

    if(HAVE_GLES_2)
        list(APPEND LINK_LIBS "-lEGL;-lGLESv2" )
    else()
        list(APPEND LINK_LIBS "-lEGL;-lGLESv1_CM" )
    endif()
elseif( IOS )
    list(APPEND LINK_LIBS "-framework OpenGLES" )
    list(APPEND IOS_HEADERS "${INCDIR}/ios/PangolinAppDelegate.h" "${INCDIR}/ios/PangolinViewController.h" )
    list(APPEND SOURCES ${SRCDIR}/ios/PangolinAppDelegate.mm
        ${SRCDIR}/ios/PangolinViewController.mm )
else()
    find_package(OpenGL REQUIRED QUIET)
    list(APPEND USER_INC  "${OPENGL_INCLUDE_DIR}" )
    list(APPEND LINK_LIBS "${OPENGL_LIBRARIES}" )

    find_package(GLEW REQUIRED QUIET)
    if(GLEW_FOUND)
        list(APPEND USER_INC  "${GLEW_INCLUDE_DIR}" )
        list(APPEND LINK_LIBS "${GLEW_LIBRARY}" )
        set(HAVE_GLEW 1)
    endif()

    find_package(FREEGLUT QUIET)
    if(FREEGLUT_FOUND)
        list(APPEND USER_INC "${FREEGLUT_INCLUDE_DIR}")
        list(APPEND LINK_LIBS "${FREEGLUT_LIBRARY}")
        set(HAVE_FREEGLUT 1)
    endif()
endif()

if( HAVE_GLES_2 )
    # Add Pangolins backwards compat layer.
    list(APPEND HEADERS ${INCDIR}/gl2engine.h )
    list(APPEND SOURCES ${SRCDIR}/gl2engine.cpp)
endif()


# Set version values in config.h  (see config.h.in)
configure_file( src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/CVars/config.h IMMEDIATE )
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
    include/CVars/config.h
    include/CVars/cvars_tinyxml.h
    include/CVars/CVar.h
    include/CVars/CVarVectorIO.h
    include/CVars/CVarMapIO.h
    include/CVars/Timestamp.h
    include/CVars/Trie.h
    include/CVars/TrieNode.h
  )

add_definitions( -DTIXML_USE_STL )

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

# include CVAR_HDRS in add_library so QtCreator will detect them.
add_library( cvars ${CVAR_SRCS} ${CVAR_HDRS} ${GLCONSOLE_HEADERS}
 ${FLCONSOLE_HEADERS} )

##############################################################################
install_package(
    PKG_NAME CVars
    LIB_NAME cvars
    VERSION ${VERSION}
    DESCRIPTION "Library for runtime variable editing for fast prototyping."
    INSTALL_INCLUDE_DIR true
    INSTALL_GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/include/CVars/config.h
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    )

# make an uninstall target
include(${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in)
add_custom_target(uninstall
    COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

