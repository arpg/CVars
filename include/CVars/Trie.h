/*

    Cross platform "CVars" functionality.

    This Code is covered under the LGPL.  See COPYING file for the license.

    $Id: Trie.h 201 2012-10-17 18:43:27Z effer $

 */


#ifndef _TRIE_H__
#define _TRIE_H__

#include <string>
#include <vector>

#include "CVar.h"

class TrieNode;


namespace CVarUtils {
    class MemoryHolder {
    private:
        struct holder {
            virtual ~holder() {}
        };

        template <class T>
            struct obj_holder : public holder {
            T obj;
            
        obj_holder()
            : obj() {}
            
            template <class Arg1>
                obj_holder( Arg1 arg1 )
                : obj(arg1) {}
        };

        std::vector<holder*> owned;

    private:
        MemoryHolder(const MemoryHolder&);
        void operator=(const MemoryHolder&);

    public:
        MemoryHolder() {}
        ~MemoryHolder() {
            destroy_all();
        }
  
        template<class T>
            T* create() {
            obj_holder<T>* p = new obj_holder<T>();
            owned.push_back(p);
            return &p->obj;
        }
 
        template<class T, class Arg1>
            T* create(Arg1 arg1) {
            obj_holder<T>* p = new obj_holder<T>(arg1);
            owned.push_back(p);
            return &p->obj;
        }

        void destroy_all() {
            for( std::vector<holder*>::size_type ii = 0; ii < owned.size(); ii++ ) {
                delete owned[ ii ];
            }
            owned.clear();
        }
    };
}

enum CVARS_STREAM_TYPE
  {
    CVARS_XML_STREAM,
    CVARS_TXT_STREAM
  };

class Trie
{
 public:
    Trie();
    ~Trie();
    void Init();
    // add string to tree and store data at leaf
    void         Insert( std::string s, void *data );
    // finds s in the tree and returns the node (may not be a leaf)
    // returns null otherwise
    TrieNode*    FindSubStr( const std::string& s );
    std::vector<std::string> FindListSubStr( const std::string& s );
    TrieNode*    Find( const std::string& s );
    void*        FindData( const std::string& s );

    bool         Exists( const std::string& s );

    TrieNode*    GetRoot();
    void         SetAcceptedSubstrings( std::vector< std::string > vAcceptedSubstrings );
    bool         IsNameAcceptable( const std::string& sVarName );
    bool         IsVerbose();
    void         SetVerbose( bool bVerbose );

    // does an in order traversal starting at node and printing all leaves
    // to a list
    std::vector<std::string> CollectAllNames( TrieNode* node );
    // traverse from the supplied node and return a list of all nodes
    std::vector<TrieNode*>   CollectAllNodes( TrieNode* node );

    CVARS_STREAM_TYPE GetStreamType() { return m_StreamType; }
    void SetStreamType( const CVARS_STREAM_TYPE& streamType ) { m_StreamType = streamType; }

    // CVar
    int*   m_pVerboseCVarNamePaddingWidth;

    // To avoid memory leaks, CVars should be created using the memory holder
    CVarUtils::MemoryHolder mem;

 private:
    TrieNode* root;
    std::vector< std::string > m_vAcceptedSubstrings;
    std::vector< std::string > m_vNotAcceptedSubstrings;
    std::vector< std::string > m_vCVarNames; // Keep a list of CVar names
    bool m_bVerbose;
    CVARS_STREAM_TYPE m_StreamType;
};

std::ostream &operator<<(std::ostream &stream, Trie &rTrie );
std::istream &operator>>(std::istream &stream, Trie &rTrie );


#endif
