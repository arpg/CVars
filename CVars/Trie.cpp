/*

    Cross platform "CVars" functionality.

    This Code is covered under the LGPL.  See COPYING file for the license.

    $Id: Trie.cpp 162 2010-02-15 19:11:05Z gsibley $

 */

// Trie data structure implementation.

#include "CVars/CVar.h" 
#include "CVars/Trie.h" 
#include "CVars/TrieNode.h" 
#include "CVars/cvars_tinyxml.h"
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

////////////////////////////////////////////////////////////////////////////////
Trie::Trie()
{ 
    root = NULL;
    m_bVerbose = false;
}


////////////////////////////////////////////////////////////////////////////////
void Trie::Init()
{
    root = new TrieNode( TRIE_ROOT ); 

    std::string sVarName;

    //////
    sVarName = "console.VerbosePaddingWidth";
    CVarUtils::CVar<int> *pCVar1 = new CVarUtils::CVar<int>( sVarName, 30 );
    m_pVerboseCVarNamePaddingWidth = pCVar1->m_pVarData;
    Insert( sVarName, (void *) pCVar1 );
    //////
    sVarName = "console.CVarIndent";
    CVarUtils::CVar<int> *pCVar2 = new CVarUtils::CVar<int>( sVarName, 0 );
    m_pVerboseCVarNamePaddingWidth = pCVar2->m_pVarData;
    Insert( sVarName, (void *) pCVar2 );
    //////
    sVarName = "console.CVarIndentIncr";
    CVarUtils::CVar<int> *pCVar3 = new CVarUtils::CVar<int>( sVarName, 4 );
    m_pVerboseCVarNamePaddingWidth = pCVar3->m_pVarData;
    Insert( sVarName, (void *) pCVar3 );
    //////
}

////////////////////////////////////////////////////////////////////////////////
Trie::~Trie()
{
    if ( root != NULL ) {
        delete root; //destructor on TrieNodes will destroy all the children	
    }
}

////////////////////////////////////////////////////////////////////////////////
void Trie::Insert( std::string s, void *dataPtr )
{
    if( root == NULL ) {
        printf( "ERROR in Trie::Insert, root == NULL!!!!!\n" );
        return;
    }

    m_vCVarNames.push_back( s );

    TrieNode *traverseNode = root;
    for( unsigned int i = 0 ; i < s.length() ; i++ ) {
        traverseNode = traverseNode->TraverseInsert( s[i] );
    }

    //add leaf node
    TrieNode* newNode = new TrieNode( s );
    newNode->m_pNodeData = dataPtr;
    traverseNode->m_children.push_back(newNode); //create leaf node at end of chain
}

////////////////////////////////////////////////////////////////////////////////
TrieNode* Trie::Find( const std::string& s ) 
{ 
    TrieNode* node = FindSubStr( s );
    if(	node != NULL 
        && node->m_nNodeType == TRIE_LEAF ) {
        return node;
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////
void* Trie::FindData( const std::string& s ) 
{ 
    return Find( s )->m_pNodeData;
}

////////////////////////////////////////////////////////////////////////////////
bool Trie::Exists( const std::string& s )
{
    return Find( s ) != NULL;
}

////////////////////////////////////////////////////////////////////////////////
TrieNode* Trie::GetRoot() { 
    return root; 
}

////////////////////////////////////////////////////////////////////////////////
// Finds all the CVarNames that contain s as a substring.
std::vector<std::string> Trie::FindListSubStr( const std::string& s )
{
    std::vector<std::string> vCVars;
    for( size_t i=0; i<m_vCVarNames.size(); i++ ) {
        if( m_vCVarNames[i].find( s ) != string::npos ) {
            vCVars.push_back( m_vCVarNames[i] );
        }
    }
    return vCVars;
}

////////////////////////////////////////////////////////////////////////////////
// Finds s in the tree and returns the node (may not be a leaf) returns null
// otherwise.
TrieNode* Trie::FindSubStr( const std::string& s )
{
    if( root == NULL ) {
        printf( "ERROR in Trie::FindSubStr, root == NULL!!!!!\n" );
        return NULL;
    }

    if( s.length() == 0 )
        return root;

    TrieNode *traverseNode = root;

    for( unsigned int i = 0 ; i < s.length() ; i++ ){
        traverseNode = traverseNode->TraverseFind( s[i] );
        if( traverseNode ) {
            continue;
        } else {
            return NULL;
        }
    }

    // Look for a leaf node here and return it if no leaf node just return this
    // node.
    std::list<TrieNode*>::iterator it;
    for(it = traverseNode->m_children.begin() ; it != traverseNode->m_children.end() ; it++){
        //found child
        if((*it)->m_nNodeType == TRIE_LEAF) {
            return (*it);
        }
    }

    return traverseNode;
}

////////////////////////////////////////////////////////////////////////////////
// Chris, please comment this guy for me.  GTS
void Trie::SetAcceptedSubstrings( std::vector< std::string > vFilterSubstrings )
{
    m_vAcceptedSubstrings.clear();
    m_vNotAcceptedSubstrings.clear();

    // Check if verbose should be set
    if( vFilterSubstrings.size() > 0 ) {
        if( vFilterSubstrings[ vFilterSubstrings.size()-1 ] == "true" ) {
            m_bVerbose = true;
            vFilterSubstrings.pop_back();
        }
        else if( vFilterSubstrings[ vFilterSubstrings.size()-1 ] == "false" ) {
            m_bVerbose = false;
            vFilterSubstrings.pop_back();
        }
    }
    
    // Split the list between acceptable and not acceptable substrings.
    int nAccIndex = 0;
    for( nAccIndex=0; nAccIndex<int(vFilterSubstrings.size()); nAccIndex++ ) {
        if( vFilterSubstrings[nAccIndex].find( "not" ) != std::string::npos ) {
            nAccIndex++;
            break;
        }
        m_vAcceptedSubstrings.push_back( vFilterSubstrings[nAccIndex] );
    }
    for( int nNotAccIndex=nAccIndex; nNotAccIndex<int(vFilterSubstrings.size()); nNotAccIndex++ ) {
        m_vNotAcceptedSubstrings.push_back( vFilterSubstrings[nNotAccIndex] );
    }
}

////////////////////////////////////////////////////////////////////////////////
bool Trie::IsNameAcceptable( const std::string& sVarName )
{
    if( m_vAcceptedSubstrings.size() == 0 &&
        m_vNotAcceptedSubstrings.size() == 0 ) {
        return true;
    }
    bool bAcceptable;
    if( m_vAcceptedSubstrings.size() == 0 ) {
        bAcceptable = true;
    }
    else {
        bAcceptable = false;
    }

    for( size_t i=0; i<m_vAcceptedSubstrings.size(); i++ ) {
        if( sVarName.find( m_vAcceptedSubstrings[i] ) == 0 ) {
            bAcceptable = true;
            break;
        }
    }
    for( size_t i=0; i<m_vNotAcceptedSubstrings.size(); i++ ) {
        if( sVarName.find( m_vNotAcceptedSubstrings[i] ) == 0 ) {
            bAcceptable = false;
            break;
        }
    }
    return bAcceptable;
}
////////////////////////////////////////////////////////////////////////////////
bool Trie::IsVerbose()
{
    return m_bVerbose;
}

////////////////////////////////////////////////////////////////////////////////
void Trie::SetVerbose( bool bVerbose )
{
    m_bVerbose = bVerbose;
}

////////////////////////////////////////////////////////////////////////////////
// Does an in order traversal starting at node and printing all leaves to a list
std::vector<std::string> Trie::CollectAllNames( TrieNode* node )
{
    std::vector<std::string> res;
    node->PrintToVector( res );

    return res;	
}

////////////////////////////////////////////////////////////////////////////////
// Does an in order traversal starting at node and printing all leaves to a list
std::vector<TrieNode*> Trie::CollectAllNodes( TrieNode* node )
{
    std::vector<TrieNode*> res;
    node->PrintNodeToVector( res );
    return res;	
}

////////////////////////////////////////////////////////////////////////////////
std::ostream &operator<<( std::ostream &stream, Trie &rTrie )
{
	std::vector<TrieNode*> vNodes = rTrie.CollectAllNodes( rTrie.GetRoot() );
    stream << CVarUtils::CVarSpc() << "<cvars>" << std::endl;
    for( size_t ii = 0; ii < vNodes.size(); ii++ ){
        std::string sVal = ((CVarUtils::CVar<int>*)vNodes[ii]->m_pNodeData)->GetValueAsString();

        if( !sVal.empty() ) {       
            std::string sCVarName = ((CVarUtils::CVar<int>*)vNodes[ii]->m_pNodeData)->m_sVarName;
            if( !rTrie.IsNameAcceptable( sCVarName ) ) {
                if( rTrie.IsVerbose() ) {
                    printf( "NOT saving %s (not in acceptable name list).\n", sCVarName.c_str() );
                }
                continue;
            }   
            if( !((CVarUtils::CVar<int>*)vNodes[ii]->m_pNodeData)->m_bSerialise ) {
                if( rTrie.IsVerbose() ) {
                    printf( "NOT saving %s (set as not savable at construction time).\n", sCVarName.c_str() );
                }
                continue;
            }   
            if( rTrie.IsVerbose() ) {
                printf( "Saving \"%-*s\" with value \"%s\".\n", *rTrie.m_pVerboseCVarNamePaddingWidth, 
                        sCVarName.c_str(), sVal.c_str() );
            }
            CVarUtils::CVarIndent();
            stream << CVarUtils::CVarSpc() << "<" << sCVarName << ">  ";
            CVarUtils::CVarIndent();
            stream << sVal;
            CVarUtils::CVarUnIndent();
            stream << CVarUtils::CVarSpc() << "</" << sCVarName << ">" << std::endl;
            CVarUtils::CVarUnIndent();
        }
    }
    stream << CVarUtils::CVarSpc() << "</cvars>" << std::endl;

    return stream;
}

////////////////////////////////////////////////////////////////////////////////
std::istream &operator>>( std::istream &stream, Trie &rTrie )
{
    CVarUtils::TiXmlDocument doc;
    stream >> doc;

    CVarUtils::TiXmlNode* pCVarsNode = doc.FirstChild( "cvars" );

    if( pCVarsNode == NULL ) {
        cerr <<  "ERROR: Could not find <cvars> node." << endl;
        return stream;
    }

    for( CVarUtils::TiXmlNode* pNode = pCVarsNode->FirstChild();
         pNode != NULL;
         pNode = pNode->NextSibling() ) {
        std::string sCVarName( pNode->Value() );
        
        if( !rTrie.Exists( sCVarName ) ) {
            if( rTrie.IsVerbose() ) {
                printf( "NOT loading %s (not in Trie).\n", sCVarName.c_str() );
            }
            continue;
        }

        if( !rTrie.IsNameAcceptable( sCVarName ) ) {
            if( rTrie.IsVerbose() ) {
                printf( "NOT loading %s (not in acceptable name list).\n", sCVarName.c_str() );
            }
            continue;
        }

        CVarUtils::CVar<int>* pCVar = (CVarUtils::CVar<int>*)rTrie.Find( sCVarName )->m_pNodeData;
        CVarUtils::TiXmlNode* pChild = pNode->FirstChild();

        if( pCVar != NULL && pChild != NULL ) {
            std::string sCVarValue;
            sCVarValue << *pChild;
            pCVar->SetValueFromString( sCVarValue );

            if( rTrie.IsVerbose() ) {
                printf( "Loading \"%-*s\" with value \"%s... \".\n", *rTrie.m_pVerboseCVarNamePaddingWidth,
                        sCVarName.c_str(), sCVarValue.substr(0,40).c_str() );
            }
        }
        else {
            cerr << "WARNING: found a cvar in file with no value (name: " << sCVarName << ").\n" << endl;
        }
    }
    return stream;
}
