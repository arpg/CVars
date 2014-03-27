#include <CVars/CVar.h>
#include <memory>

namespace CVarUtils
{

struct TrieContext
{
    TrieContext() {
        trie.Init();
    }

    Trie trie;
};

Trie& TrieInstance()
{
    static TrieContext context;
    return context.trie;
}

}
