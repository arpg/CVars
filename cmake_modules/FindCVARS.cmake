# Try to find CVars library
#
# This module defines
#
#  CVARS_FOUND       - system has CVars
#  CVARS_INCLUDE_DIR - the CVars include directories
#  CVARS_LIBRARIES   - link these to use CVars


# Be quite if already in cache.
IF( CVARS_FOUND )
    SET( CVARS_FIND_QUIETLY TRUE )
ENDIF( CVARS_FOUND )

# Try to find headers.
FIND_PATH( _header_dir CVars/CVar.h
	    PATHS
        $ENV{HOME}/include
        $ENV{HOME}/local/include
        /usr/local/include
        /usr/include
        /opt/local/include
	    ENV CPATH
        )
SET( _header_dir ${_header_dir} CACHE INTERNAL "" FORCE )


IF( NOT "${_header_dir}" STREQUAL "_header_dir-NOTFOUND" )
    FIND_LIBRARY( CVARS_LIBRARIES
         NAMES libcvars cvars
         PATHS 
         $ENV{HOME}/local/lib 
         $ENV{HOME}/lib 
         /usr/lib
         /usr/local/lib
         )


    SET( CVARS_INCLUDE_DIR ${_header_dir} CACHE PATH "" FORCE )
ENDIF()

#Did we find it?
IF( CVARS_LIBRARIES AND CVARS_INCLUDE_DIR )
    SET( CVARS_FOUND TRUE CACHE INTERNAL "" )
ELSE()
    SET( CVARS_FOUND FALSE CACHE INTERNAL "" )
ENDIF()

#print out status
IF( CVARS_FOUND )
    IF( NOT CVARS_FIND_QUIETLY )
        MESSAGE(STATUS "Looking for CVars headers -- found")
        MESSAGE(STATUS "Looking for CVars libraries -- found")
    ENDIF()
ELSE()
    IF( CVARS_FIND_REQUIRED )
        MESSAGE(FATAL_ERROR "Could not find CVars library")
    ENDIF()
ENDIF()

#MARK_AS_ADVANCED( CVARS_INCLUDE_DIR )
#MARK_AS_ADVANCED( CVARS_LIBRARIES )

