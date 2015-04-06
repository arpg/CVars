cmake_minimum_required( VERSION 2.8 )

# Be sure to change these with each update
SET( VERSION_MAJOR 2 )
SET( VERSION_MINOR 4 )
SET( VERSION_PATCH 0 )
SET( VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} )

project( CVARS )

# Add to module path, so we can find our cmake modules
list( APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules )
include( install_package )

# Set version values in CVarVersion.h  (see CVarVersion.h.in)
configure_file( CVars/CVarVersion.h.in ${CMAKE_CURRENT_SOURCE_DIR}/CVars/CVarVersion.h IMMEDIATE )


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
install( FILES ${GLCONSOLE_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/GLConsole )

###############################################################################
# Headers for FLConsole
set( FLCONSOLE_HEADERS
        FLConsole/FLConsole.h
        FLConsole/FLConsoleFunction.h
   )
install( FILES ${FLCONSOLE_HEADERS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/FLConsole )


##############################################################################
include( install_package )
install_package(
    PKG_NAME cvars
    LIB_NAME cvars
    DESCRIPTION "Library for runtime variable editing for fast prototyping."
    INSTALL_HEADERS CVars/CVarVersion.h CVars/cvars_tinyxml.h CVars/CVar.h
                    CVars/CVarVectorIO.h CVars/CVarMapIO.h CVars/Timestamp.h
                    CVars/Trie.h CVars/TrieNode.h
    DESTINATION  ${CMAKE_INSTALL_PREFIX}/include/CVars
    LINK_LIBS cvars
    )


## Figure out the relative path from the installed Config.cmake file to the
## install prefix. This relative path will be configured into CVARSConfig.cmake.
#SET( CMAKECONFIG_INSTALL_DIR lib/cmake/CVARS )
#FILE( RELATIVE_PATH REL_INCLUDE_DIR
#    "${CMAKE_INSTALL_PREFIX}/${CMAKECONFIG_INSTALL_DIR}"
#    "${CMAKE_INSTALL_PREFIX}/include" )

## Add package to CMake package registery for use from the build tree
#EXPORT( TARGETS cvars FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Targets.cmake" )

# Add package to CMake package registery for use from the build tree
#option( EXPORT_${PROJECT_NAME}
#  "Should the ${PROJECT_NAME} package be exported for use by other software" ON )

#if( EXPORT_${PROJECT_NAME} )
#  export( PACKAGE ${PROJECT_NAME} )
#endif()

## Install the export set for use with the install tree
#INSTALL( EXPORT ${PROJECT_NAME}Targets DESTINATION ${CMAKECONFIG_INSTALL_DIR})

# Create the CVARSConfig.cmake file for other cmake projects: FIND_PACKAGE( CVARS )
# ... for the build tree
#set( EXPORT_LIB_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}" )
#SET( CONF_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} )
#CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/CVARSConfig.cmake.in
#                ${CMAKE_CURRENT_BINARY_DIR}/CVARSConfig.cmake @ONLY )
#
# ... for the install tree
#SET( CONF_INCLUDE_DIR "\${CVARS_CMAKE_DIR}/${REL_INCLUDE_DIR}" )
#CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/CVARSConfig.cmake.in
#    ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CVARSConfig.cmake @ONLY )

# Version information
#configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/CVARSConfigVersion.cmake.in
#  "${CMAKE_CURRENT_BINARY_DIR}/CVARSConfigVersion.cmake" @ONLY)

#INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CVARSConfig.cmake"
#              "${CMAKE_CURRENT_BINARY_DIR}/CVARSConfigVersion.cmake"
#        DESTINATION ${CMAKECONFIG_INSTALL_DIR} )



