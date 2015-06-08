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
include( SetPlatformVars )
include( CheckCXXSourceRuns )

# Properly source GL headers.
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
find_package(GLUT QUIET)

# Custom check for Modified OSX GLUT.
if(_OSX_)
  set(CMAKE_REQUIRED_LIBRARIES ${GLUT_LIBRARY})
  CHECK_CXX_SOURCE_RUNS("#include <GLUT/glut.h>\n int main () {return glutGetProcAddress(\"glutScrollFunc\")     ? 0 : -1;};" HAVE_MODIFIED_OSXGLUT)
  if(NOT HAVE_MODIFIED_OSXGLUT)
        message(STATUS "Install modified osxglut for smooth scroll support / pinch / zoom.")
        message(STATUS "(https://github.com/stevenlovegrove/osxglut)")
  endif()
endif(_OSX_)

# Prefer OSX_MODIFIED_GLUT > FREEGLUT > GLUT
if(FREEGLUT_FOUND AND NOT HAVE_MODIFIED_OSXGLUT)
  set(HAVE_FREEGLUT 1)
  list(APPEND USER_INC  ${FREEGLUT_INCLUDE_DIR} )
  list(APPEND LINK_LIBS ${FREEGLUT_LIBRARY} )
elseif(GLUT_FOUND)
  list(APPEND USER_INC  ${GLUT_INCLUDE_DIR} )
  list(APPEND LINK_LIBS ${GLUT_LIBRARY} )
  if(_OSX_)
    set(HAVE_APPLE_OPENGL_FRAMEWORK 1)
    list(APPEND LINK_LIBS "-framework GLUT" )
    if(NOT HAVE_MODIFIED_OSXGLUT)
        message(STATUS "Using Apple Framework GLUT.")
        message(STATUS "Install Freeglut or modified osxglut for scroll support.")
    endif()
  endif()
  if(FREEGLUT_FOUND OR GLUT_FOUND)
    set(HAVE_GLUT 1)
    message(STATUS "Glut Found and Enabled")
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
    include/CVars/glplatform.h
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
add_library( cvars SHARED ${CVAR_SRCS} ${CVAR_HDRS} ${GLCONSOLE_HEADERS}
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

