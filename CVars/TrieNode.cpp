/*

    Cross platform "CVars" functionality.

    This Code is covered under the LGPL.  See COPYING file for the license.

    $Id: TrieNode.cpp 162 2010-02-15 19:11:05Z gsibley $
 */


#include "CVars/TrieNode.h" 
#include "CVars/CVar.h"

////////////////////////////////////////////////////////////////////////////////
TrieNode::TrieNode() { 
    m_children.clear();
    m_pNodeData = NULL;
}

////////////////////////////////////////////////////////////////////////////////
TrieNode::TrieNode( TrieNodeType t ) : m_nNodeType(t) 
{
    m_children.clear();
    m_pNodeData = NULL;
}

////////////////////////////////////////////////////////////////////////////////
TrieNode::TrieNode( std::string s ) :  m_nNodeType(TRIE_LEAF), m_sLeafText(s) 
{ 
    m_children.clear();
    m_pNodeData = NULL;
}

////////////////////////////////////////////////////////////////////////////////
TrieNode::TrieNode( char c ) : m_nNodeType(TRIE_NODE), m_cNodeChar(c) 
{ 
    m_children.clear();
    m_pNodeData = NULL;
}

////////////////////////////////////////////////////////////////////////////////
TrieNode::~TrieNode()
{
    std::list<TrieNode*>::iterator it;
    if( m_nNodeType == TRIE_LEAF &&
        m_pNodeData != NULL ) {
        //printf( "Destroying leaf node '%s'\n", m_sLeafText.c_str() );
        CVarUtils::CVar<int>* pCVar = (CVarUtils::CVar<int>*) m_pNodeData;
        delete pCVar;
    }
    else {
        for( it = m_children.begin() ; it != m_children.end() ; it++ ) {
            delete (*it); //destructor on TrieNodes will destroy all the children
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Go through this node and see if this char is a branch, if so, simply return
// the corresponding child otherwise, create a node and return its child
TrieNode* TrieNode::TraverseInsert( char addchar )
{
    std::list<TrieNode*>::iterator it;
    for( it = m_children.begin() ; it != m_children.end() ; it++ ) {
        //found child
        if( ((*it)->m_nNodeType == TRIE_NODE) && ((*it)->m_cNodeChar == addchar) ) {
            return (*it);
        }
    }

    TrieNode* newNode = new TrieNode( addchar );
    m_children.push_back( newNode );
    return newNode;
}

////////////////////////////////////////////////////////////////////////////////
// See if there is a child with this character, if so, return it otherwise,
// return NULL.
TrieNode* TrieNode::TraverseFind( char addchar )
{
    std::list<TrieNode*>::iterator it;
    for( it = m_children.begin() ; it != m_children.end() ; it++ ) {
        //found child
        if( ((*it)->m_nNodeType == TRIE_NODE) && 
            ((*it)->m_cNodeChar == addchar) ) {
            return (*it);
        }
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
// Recursively traverses
void TrieNode::PrintToVector( std::vector<std::string> &vec )
{
    if( m_nNodeType == TRIE_LEAF ) {
        vec.push_back( m_sLeafText );
    } else {
        std::list<TrieNode*>::iterator it;
        for( it = m_children.begin() ; it != m_children.end() ; it++ ) {
            (*it)->PrintToVector( vec );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Recursively traverses
void TrieNode::PrintNodeToVector( std::vector<TrieNode*> &vec )
{
    if( m_nNodeType == TRIE_LEAF ) {
        vec.push_back( this );
    } else {
        std::list<TrieNode*>::iterator it;
        for( it = m_children.begin() ; it != m_children.end() ; it++ ) {
            (*it)->PrintNodeToVector( vec );
        }
    }
}


