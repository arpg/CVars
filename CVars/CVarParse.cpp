/*

   Cross platform "CVars" functionality.

   This Code is covered under the LGPL.  See COPYING file for the license.

   $Id: CVarParse.cpp 189 2011-07-26 14:29:48Z effer $

 */

#include "CVar.h"

using namespace CVarUtils;

#include <set>

////////////////////////////////////////////////////////////////////////////////
// remove all spaces from the front and back...
std::string& _RemoveSpaces( std::string &str )
{
    str.erase( str.find_last_not_of( ' ' ) + 1 );
    str.erase( 0, str.find_first_not_of( ' ' ) );
    return str;
}

////////////////////////////////////////////////////////////////////////////////
namespace CVarUtils 
{
    ////////////////////////////////////////////////////////////////////////////////
    bool ProcessCommand( 
            const std::string& sCommand, 
            std::string& sResult,
            bool bExecute                       //< Input:
            )
    {
        Trie& trie = TrieInstance();

        TrieNode*node;
        bool bSuccess = true;
        std::string sCmd = sCommand;

        // remove leading and trailing spaces
        int pos = sCmd.find_first_not_of( " ", 0 );
        if( pos >= 0 ) {
            sCmd = sCmd.substr( pos );
        }
        pos = sCmd.find_last_not_of( " " );
        if( pos >= 0 ) {
            sCmd= sCmd.substr( 0, pos+1 );
        }

        // Simply print value if the command is just a variable
        if( ( node = trie.Find( sCmd ) ) ) {
            //execute function if this is a function cvar
            if( IsConsoleFunc( node ) ) {
                bSuccess &= ExecuteFunction( 
                        sCmd,
                        (CVarUtils::CVar<ConsoleFunc>*)node->m_pNodeData,
                        sResult,
                        bExecute
                        );
            }
            else { //print value associated with this cvar
                sResult = GetValueAsString(node->m_pNodeData).c_str();
            }
        }
        //see if it is an assignment or a function execution (with arguments)
        else{
            int eq_pos; //get the position of the equal sign
            //see if this an assignment
            if( ( eq_pos = sCmd.find( "=" ) ) != -1 ) {
                std::string command, value;
                std::string tmp = sCmd.substr(0, eq_pos ) ;
                command = _RemoveSpaces( tmp );
                value = sCmd.substr( eq_pos+1, sCmd.length() );
                if( !value.empty() ) {
                    value = _RemoveSpaces( value );
                    if( ( node = trie.Find(command) ) ) {
                        if( bExecute ) {
                            SetValueFromString( node->m_pNodeData, value );
                        }
                        sResult = GetValueAsString(node->m_pNodeData).c_str();
                    }
                    else {
                        sResult = command + ": variable not found";
                        bSuccess = false;
                    }
                }
                else {
                    if( bExecute ) {
                        sResult = command + ": command not found";
                    }
                    bSuccess = false;
                }
            }
            //check if this is a function
            else if( ( eq_pos = sCmd.find(" ") ) != -1 ) {
                std::string function;
                std::string args;
                function = sCmd.substr( 0, eq_pos );
                //check if this is a valid function name
                if( ( node = trie.Find( function ) ) && IsConsoleFunc( node ) ) {
                    bSuccess &= ExecuteFunction( 
                            sCmd,
                            ( CVar<ConsoleFunc>*)node->m_pNodeData,
                            sResult,
                            bExecute
                            );
                }
                else {
                    if( bExecute ) {
                        sResult = function + ": function not found";
                    }
                    bSuccess = false;
                }
            }
            else if( !sCmd.empty() ) {
                if( bExecute ) {
                    sResult = sCmd + ": command not found";
                }
                bSuccess = false;
            }
        }
        if( sResult == "" && bSuccess == false ){
            sResult = sCmd + ": command not found";
        }
        return bSuccess;
    }


    ////////////////////////////////////////////////////////////////////////////////
    /// Parses the argument list and calls the function object associated with the
    //  provided variable.
    bool ExecuteFunction( 
            const std::string& sCommand,         //< Input:
            CVarUtils::CVar<ConsoleFunc> *cvar,  //< Input:
            std::string& ,                //< Output:
            bool bExecute                        //< Input:
            )
    {
        std::vector<std::string> argslist;
        std::string args;
        bool bSuccess = true;
        ConsoleFunc func = *(cvar->m_pVarData);
        std::string sCmd = sCommand;

        //extract arguments string
        int pos = sCmd.find( " " );
        if( pos != -1 ) {
            args = sCmd.substr(pos+1, sCmd.length() - pos);
        }
        //parse arguments into a list of strings
        if( args.length() > 0 ) {
            while( ( pos = args.find(" ") ) != -1 ) {
                argslist.push_back(args.substr(0, pos));
                args = args.substr(pos+1, args.length() - pos);
            }
            if( args.length() > 0 ) {
                argslist.push_back(args);
            }
        }

        if( bExecute ) {
            const bool res = (*func)( &argslist );
            if( !res ) {
                bSuccess = false;
            }
        }

        return bSuccess;
    }


    ////////////////////////////////////////////////////////////////////////////////
    bool IsConsoleFunc( TrieNode *node )
    {
        if( typeid( ConsoleFunc ).name() == ((CVar<int>*)node->m_pNodeData)->type() ) {
            return true;
        }
        return false; 
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool IsConsoleFunc( const std::string sCmd ) {
        TrieNode* pNode = TrieInstance().Find( sCmd );
        if( pNode == NULL ) { return false; }
        if( typeid( ConsoleFunc ).name() == ((CVar<int>*)pNode->m_pNodeData)->type() ) {
            return true;
        }
        return false; 
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Utility function.
    inline std::string FindLevel( std::string sString, int iMinRecurLevel ) {   
        int level = 0;
        int index = sString.length();
        for( unsigned int ii = 0; ii < sString.length(); ii++ ) {
            if( sString.c_str()[ii]=='.' ) {
                level ++;
            }
            if( level == iMinRecurLevel ) {
                index = ii+1;
            }
        }
        return sString.substr( 0, index );
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline int _FindRecursionLevel( const std::string& sCommand ) {
        int nLevel = 0;
        for( unsigned int ii = 0; ii < sCommand.length(); ii++ ) {
            if( sCommand.c_str()[ii] == '.' ) {
                nLevel++;
            }
        }
        return nLevel;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Return whether first element is greater than the second.
    inline bool _StringIndexPairGreater
        ( const std::pair<std::string,int>& e1, const std::pair<std::string,int>& e2 ) {
        return e1.first < e2.first;
    }

    ////////////////////////////////////////////////////////////////////////////////
    bool TabComplete( const unsigned int nMaxNumCharactersPerLine,
                      std::string& sCommand,
                      std::vector<std::string>& vResult
                      ) {
        Trie& trie = TrieInstance();

        sCommand = _RemoveSpaces( sCommand );
        TrieNode* node = trie.FindSubStr( sCommand );
        if( node == NULL ) {
            std::string sCommandStripEq = sCommand.substr( 0, sCommand.rfind( "=" ) );
            sCommandStripEq = _RemoveSpaces( sCommandStripEq );
            node = trie.FindSubStr( sCommandStripEq );
            if( node != NULL ) { sCommand = sCommandStripEq; }
        }
        if( node == NULL ) {
            return false;
        }
        else if( node->m_nNodeType == TRIE_LEAF || 
                 ( node->m_children.size() == 0 ) ) {
            node = trie.Find( sCommand );
            if( !IsConsoleFunc( node ) ) {
                sCommand += " = " + CVarUtils::GetValueAsString( node->m_pNodeData );
                vResult.push_back( sCommand );
            }
        } 
        else {
            // Retrieve suggestions (retrieve all leaves by traversing from current node)
            std::vector<TrieNode*> suggest = trie.CollectAllNodes( node );
            //output suggestions
            if( suggest.size() == 1 ) {
                // Is this what the use wants? Clear the left bit...
                sCommand = 
                    ((CVarUtils::CVar<int>*) suggest[0]->m_pNodeData)->m_sVarName;
            } 
            else if( suggest.size() > 1) {
                std::vector<std::pair<std::string,int> > suggest_name_index_full;            
                std::vector<std::pair<std::string,int> > suggest_name_index_set;
                // Build list of names with index from suggest
                // Find lowest recursion level
                int nMinRecurLevel = 100000;
                for( unsigned int ii = 0; ii < suggest.size(); ii++ ) {
                    std::string sName = ( (CVarUtils::CVar<int>*) suggest[ii]->m_pNodeData )->m_sVarName;
                    suggest_name_index_full.push_back( std::pair<std::string,int>( sName, ii ) );
                    if( _FindRecursionLevel( sName ) < nMinRecurLevel ) {
                        nMinRecurLevel = _FindRecursionLevel( sName );
                    }
                }
                // We need to know if there are many different roots for a given recursion
                std::set<std::string> sRoots;
                for( unsigned int ii = 0; ii < suggest_name_index_full.size() ; ii++ ) {
                    std::string sCurString = suggest_name_index_full[ii].first;
                    if( _FindRecursionLevel( sCurString ) == nMinRecurLevel ) {
                        std::string sCurLevel = FindLevel( suggest_name_index_full[ii].first, nMinRecurLevel-1 );
                        sRoots.insert( sCurLevel );
                    }
                }
                if( sRoots.size() > 1 ) {
                    nMinRecurLevel--;
                }

                // Sort alphabetically (this is useful for later removing
                // duplicate name at a given level and looks nice too...)
                std::sort( suggest_name_index_full.begin(), suggest_name_index_full.end(), 
                           _StringIndexPairGreater );

                // Remove suggestions at a higher level of recursion
                std::string sCurLevel = "";
                int nCurLevel;
                for( unsigned int ii = 0; ii < suggest_name_index_full.size() ; ii++ ) {
                    std::string sCurString = suggest_name_index_full[ii].first;
                    nCurLevel = _FindRecursionLevel( sCurString );
                    if( sCurLevel.length() == 0 ) {
                        if( nCurLevel == nMinRecurLevel ) {
                            sCurLevel = "";    
                            suggest_name_index_set.
                                push_back( std::pair<std::string,int>( sCurString,suggest_name_index_full[ii].second ) );
                        } 
                        else {
                            // Add new substring at given level
                            sCurLevel = FindLevel( sCurString, nMinRecurLevel );
                            suggest_name_index_set.push_back( std::pair<std::string,int>( sCurLevel,suggest_name_index_full[ii].second ) );
                        }
                    } 
                    else {
                        if( sCurString.find( sCurLevel ) == std::string::npos ) {
                            // Add new substring at given level
                            sCurLevel = FindLevel( sCurString, nMinRecurLevel );
                            suggest_name_index_set.push_back( std::pair<std::string,int>( sCurLevel,suggest_name_index_full[ii].second ) );
                        } 
                    }
                }

                // Get all commands and function separately
                // Print out all suggestions to the console
                std::string commands, functions; //collect each type separately

                unsigned int longest = 0;
                for( unsigned int ii = 0; ii < suggest_name_index_set.size(); ii++ ) {
                    if( suggest_name_index_set[ii].first.length() > longest ) {
                        longest = suggest_name_index_set[ii].first.length();
                    }
                }
                longest +=3;

                std::vector<std::string> cmdlines;
                std::vector<std::string> funclines;

                // add command lines
                for( unsigned int ii = 0; ii < suggest_name_index_set.size() ; ii++ ) {
                    std::string tmp = suggest_name_index_set[ii].first;
                    tmp.resize( longest, ' ' );

                    if( (commands+tmp).length() > nMaxNumCharactersPerLine ) {
                        cmdlines.push_back( commands );
                        commands.clear();
                    }
                    if( !IsConsoleFunc( suggest[suggest_name_index_set[ii].second] ) ) {
                        commands += tmp;
                    }
                }
                if( commands.length() ) cmdlines.push_back( commands );

                // add function lines
                for( unsigned int ii = 0; ii < suggest_name_index_set.size() ; ii++ ) {
                    std::string tmp = suggest_name_index_set[ii].first;
                    tmp.resize( longest, ' ' );
                    if( (functions+tmp).length() > nMaxNumCharactersPerLine ) {
                        funclines.push_back( functions );
                        functions.clear();
                    }
                    if( IsConsoleFunc( suggest[suggest_name_index_set[ii].second] ) ) {
                        functions += tmp;
                    }
                }
                if( functions.length() ) funclines.push_back( functions );

                // enter the results
                if( cmdlines.size() + funclines.size() > 0 ) {
                    //EnterLogLine( " ", LINEPROP_LOG );
                    vResult.push_back( " " );
                }
                for( unsigned int ii = 0; ii < cmdlines.size(); ii++ ) {
                    //EnterLogLine( cmdlines[ii].c_str(), LINEPROP_LOG );
                    vResult.push_back( cmdlines[ii] );
                }
                for( unsigned int ii = 0; ii < funclines.size(); ii++ ) {
                    //EnterLogLine( funclines[ii].c_str(), LINEPROP_FUNCTION );
                    vResult.push_back( funclines[ii] );
                }

                // Do partial completion - look for paths with one child down the trie
                int c = sCommand.length();
                while( node->m_children.size() == 1 ) {
                    node = node->m_children.front();
                    c++;
                }
                sCommand = suggest_name_index_set[0].first.substr( 0, c );
            } 
        }
        return true;
    }
}

