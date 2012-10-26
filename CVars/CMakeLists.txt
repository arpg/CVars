
IF(WIN32)
	SET(WIN32 1)
ENDIF(WIN32)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/.. )

SET( SRCS 
    cvars_tinyxml.cpp  
    cvars_tinyxmlerror.cpp  
    cvars_tinyxmlparser.cpp
    CVar.cpp
    CVarParse.cpp
    Timestamp.cpp
    Trie.cpp
    TrieNode.cpp
   )

SET( HDRS 
    CVarVersion.h
    cvars_tinyxml.h
    CVar.h
    CVarVectorIO.h
    CVarMapIO.h
    Timestamp.h
    Trie.h
    TrieNode.h
  )

ADD_DEFINITIONS( -DTIXML_USE_STL )

ADD_LIBRARY( cvars ${SRCS} ${HDRS} )

INSTALL( TARGETS cvars ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )
INSTALL( FILES ${HDRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/CVars )


##############################################################################
# These lines make CVARS_LIBRARY visable so we can add them to
# CVARSConfig.cmake in the top level CmakeLists.txt  
GET_TARGET_PROPERTY( CVARS_LIBRARY cvars LOCATION )
SET( CVARS_LIBRARY ${CVARS_LIBRARY} PARENT_SCOPE )
MARK_AS_ADVANCED( CVARS_LIBRARY )


