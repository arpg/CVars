#include <CVars/CVar.h>
#include <memory>

// Single global instance
std::auto_ptr<Trie> g_pCVarTrie;

////////////////////////////////////////////////////////////////////////////////
// This can get called many times -- esp when global constructors with defalut
// parameters are use, because they can be called by the compiler *before* main 
// runs -- basically, the CVarUtils functions that depend on g_pCVarTrie can be
// called before main runs, so they must all call this function to be sure
// g_pCVarTrie is allocated.
void InitCVars()
{
    if( g_pCVarTrie.get() == NULL ){
        g_pCVarTrie = std::auto_ptr<Trie>( new Trie() );
        g_pCVarTrie.get()->Init();
    }
}

