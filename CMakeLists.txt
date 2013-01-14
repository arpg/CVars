
PROJECT( CVars )

# Be sure to change these with each update
SET( MAJOR_VERSION 2 )
SET( MINOR_VERSION 3 )
SET( PATCH_VERSION 0 )

cmake_minimum_required( VERSION 2.8 )

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

###############################################################################
# Setup strict debugging environment.
IF( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g -fno-strict-aliasing ")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -fno-strict-aliasing ")
ENDIF( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )

# Set version values in CVarVersion.h  (see CVarVersion.h.in)
CONFIGURE_FILE( CVars/CVarVersion.h.in ${CMAKE_CURRENT_SOURCE_DIR}/CVars/CVarVersion.h IMMEDIATE )

###############################################################################
# Build CVars library
add_subdirectory( CVars )

###############################################################################
SET( BUILD_GLCONSOLE_DEMO CACHE BOOL OFF )
IF( BUILD_GLCONSOLE_DEMO )
    add_subdirectory( Example )
ENDIF( BUILD_GLCONSOLE_DEMO )

###############################################################################
# Headers for GLConsole
SET( GLCONSOLE_HEADERS
        GLConsole/GLFont.h
        GLConsole/GLColor.h
        GLConsole/GLConsole.h
        GLConsole/GLConsoleFunction.h
   )
INSTALL( FILES ${GLCONSOLE_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/GLConsole )

###############################################################################
# Headers for FLConsole
SET( FLCONSOLE_HEADERS
        FLConsole/FLConsole.h
        FLConsole/FLConsoleFunction.h
   )
INSTALL( FILES ${FLCONSOLE_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/FLConsole )


##############################################################################
# Create the CVarsConfig.cmake file for other cmake projects.
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/CVARSConfig.cmake.in
               ${CMAKE_CURRENT_BINARY_DIR}/CVARSConfig.cmake @ONLY IMMEDIATE )

# Add package to CMake package registery so external projects can Find CVARS automatically.
export( PACKAGE CVARS )

