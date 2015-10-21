cmake_minimum_required( VERSION 2.8 )

# Be sure to change these with each update
SET( VERSION_MAJOR 2 )
SET( VERSION_MINOR 6 )
SET( VERSION_PATCH 0 )
SET( VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} )

project( cvars )

# Add to module path, so we can find our cmake modules
list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules )
include( install_package )
include( SetPlatformVars )
include( CheckCXXSourceRuns )

# Properly source GL headers.
find_package(OpenGL QUIET)
list(APPEND USER_INC  "${OPENGL_INCLUDE_DIR}" )
list(APPEND LINK_LIBS "${OPENGL_LIBRARIES}" )

find_package(GLEW QUIET)
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
option(FORCE_FREEGLUT "Force use of freeglut if found." OFF)
if(FREEGLUT_FOUND AND (NOT HAVE_MODIFIED_OSXGLUT OR FORCE_FREEGLUT))
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

###############################################################################
if (COMMAND catkin_package)
  find_package( catkin QUIET )
   catkin_package(
			 INCLUDE_DIRS include
			 LIBRARIES cvars
			 CATKIN_DEPENDS
			 DEPENDS glut
  )
  # Install catkin package.xml
  install(FILES package.xml DESTINATION share/cvars)
endif()



# Set version values in config.h  (see config.h.in)
configure_file( src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/cvars/config.h IMMEDIATE )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/include )

###############################################################################
# Setup compiler flags
IF( NOT MSVC )
    # Setup strict debugging environment.
    IF( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g -fno-strict-aliasing ")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -fno-strict-aliasing ")
    ENDIF( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
ENDIF( NOT MSVC )

###############################################################################
# Build cvars library

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
    include/cvars/config.h
    include/cvars/cvars_tinyxml.h
    include/cvars/CVar.h
    include/cvars/CVarVectorIO.h
    include/cvars/CVarMapIO.h
    include/cvars/Timestamp.h
    include/cvars/Trie.h
    include/cvars/TrieNode.h
    include/cvars/glplatform.h
  )

add_definitions( -DTIXML_USE_STL )

###############################################################################
set( BUILD_GLCONSOLE_DEMO CACHE BOOL OFF )
if( BUILD_GLCONSOLE_DEMO )
    add_subdirectory( examples )
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
if(NOT COMMAND catkin_package)
install_package(
    PKG_NAME cvars
    LIB_NAME cvars
    VERSION ${VERSION}
    DESCRIPTION "Library for runtime variable editing for fast prototyping."
    INSTALL_INCLUDE_DIR true
    INSTALL_GENERATED_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/include/cvars/config.h
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    )
endif()

# make an uninstall target
include(${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in)
add_custom_target(uninstall
    COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

