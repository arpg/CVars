/*
    
    \file GLConsoleFunction.h

    Collection of ConsoleFunc's for GLConsole.

    $Id: GLConsoleFunction.h 162 2010-02-15 19:11:05Z gsibley $
 */

#ifndef __GL_CONSOLE_FUNCTION__
#define __GL_CONSOLE_FUNCTION__


#include <vector>
#include <cstdlib>

////////////////////////////////////////////////////////////////////////////////
/**
 * The current console version.
 */
inline bool ConsoleVersion( std::vector<std::string>* )
{
    GLConsole* pConsole = GetConsole();
    pConsole->Printf( "CVars v%d.%d.%d", 
                      CVARS_MAJOR_REV, 
                      CVARS_MINOR_REV, 
                      CVARS_PATCH_REV );
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Help for functions and variables or just general help.
 */
inline bool ConsoleHelp( std::vector<std::string> *vArgs )
{
    GLConsole* pConsole = GetConsole();
    return pConsole->Help( vArgs );
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Looks for the lists of substrings provided in vArgs in the CVarTrie.
 */
inline bool ConsoleFind( std::vector<std::string> *vArgs )
{
    GLConsole* pConsole = GetConsole();
    Trie& trie = CVarUtils::TrieInstance();

    if( vArgs != NULL && vArgs->size() > 0 ) {
        for( size_t i=0; i<vArgs->size(); i++ ) {
            std::vector<std::string> vCVarNames = trie.FindListSubStr( vArgs->at(i) );
            
            for( size_t j=0; j<vCVarNames.size(); j++ ) { 
                pConsole->Printf( "%s", vCVarNames[j].c_str() );       
            }
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * First argument indicates the file name, the following arguments are
 * used to choose a subset of variables to be saved using the provided
 * substrings.
 * Last argument can be used to be verbose when saving.
 */
inline bool ConsoleSave( std::vector<std::string> *vArgs )
{
    GLConsole* pConsole = GetConsole();
    std::string sFile = "cvars.xml";
    std::vector< std::string > vAcceptedSubstrings;

    if( vArgs != NULL && vArgs->size() > 0 ) {
        sFile = vArgs->at( 0 );
        for( size_t i=1; i<vArgs->size(); i++ ) {
            vAcceptedSubstrings.push_back( vArgs->at( i ) );
        }
    }

    pConsole->Printf( "Saving cvars to \"%s\".", sFile.c_str() );
    if( !CVarUtils::Save( sFile, vAcceptedSubstrings ) ) {
        pConsole->PrintError( "Error saving file.\n" );
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Load CVars from a file
 */
inline bool ConsoleLoad( std::vector<std::string> *vArgs )
{
    GLConsole* pConsole = GetConsole();
    std::string sFile = "cvars.xml";
    std::vector< std::string > vAcceptedSubstrings;
    
    if( vArgs != NULL && vArgs->size() > 0 ) {
        sFile = vArgs->at( 0 ); 
        for( size_t i=1; i<vArgs->size(); i++ ) {
            vAcceptedSubstrings.push_back( vArgs->at(i) );
        }
    }
    pConsole->Printf( "Loading file \"%s\".", sFile.c_str() );
    if( !CVarUtils::Load( sFile, vAcceptedSubstrings ) ) {
        pConsole->PrintError( "Error: Could not load \"%s\".", sFile.c_str() );
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Exits program from command line
 */
inline bool ConsoleExit( std::vector<std::string> * )
{
    exit( 0 );
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Save the console history to a file
 */
inline bool ConsoleHistorySave( std::vector<std::string> *vArgs )
{
    GLConsole* pConsole = GetConsole();
    if( vArgs != NULL && vArgs->size() != 0 ) {
        return pConsole->HistorySave( vArgs->at( 0 ) );
    }
    else {
        return pConsole->HistorySave();
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Load a previously saved console history from a file
 */
inline bool ConsoleHistoryLoad( std::vector<std::string> *vArgs )
{
    GLConsole* pConsole = GetConsole();
    if( vArgs != NULL && vArgs->size() != 0 ) {
        return pConsole->HistoryLoad( vArgs->at( 0 ) );
    }
    else {
        return pConsole->HistoryLoad();
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * clear the console history
 */
inline bool ConsoleHistoryClear( std::vector<std::string>* )
{
    GLConsole* pConsole = GetConsole();
    pConsole->HistoryClear();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Save the current script
 */
inline bool ConsoleScriptSave( std::vector<std::string> *vArgs )
{
    GLConsole* pConsole = GetConsole();
    if( vArgs != NULL && vArgs->size() != 0 ) {
        return pConsole->ScriptSave( vArgs->at( 0 ) );
    }
    else {
        return pConsole->ScriptSave();
    }

}

////////////////////////////////////////////////////////////////////////////////
/**
 * Load a script from a file
 */
inline bool ConsoleScriptLoad( std::vector<std::string> *vArgs )
{
    GLConsole* pConsole = GetConsole();
    if( vArgs != NULL && vArgs->size() != 0 ) {
        return pConsole->ScriptLoad( vArgs->at( 0 ) );
    }
    else {
        return pConsole->ScriptLoad();
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Start script recording
 */
inline bool ConsoleScriptRecordStart( std::vector<std::string> * )
{
    GLConsole* pConsole = GetConsole();
    pConsole->ScriptRecordStart();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Stop script recording
 */
inline bool ConsoleScriptRecordStop( std::vector<std::string> * )
{
    GLConsole* pConsole = GetConsole();
    pConsole->ScriptRecordStop();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Pause script recording
 */
inline bool ConsoleScriptRecordPause( std::vector<std::string> * )
{
    GLConsole* pConsole = GetConsole();
    pConsole->ScriptRecordPause();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Show the current script
 */
inline bool ConsoleScriptShow( std::vector<std::string> * )
{
    GLConsole* pConsole = GetConsole();
    pConsole->ScriptShow();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Run the current scipt or one from the named file
 */
inline bool ConsoleScriptRun( std::vector<std::string> *vArgs )
{
    GLConsole* pConsole = GetConsole();
    if( vArgs != NULL && vArgs->size() != 0 ) {
        return pConsole->ScriptRun( vArgs->at( 0 ) );
    }
    else {
        return pConsole->ScriptRun();
    }
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Save the console settings
 */
inline bool ConsoleSettingsSave( std::vector<std::string> *vArgs )
{
    GLConsole* pConsole = GetConsole();
    if( vArgs != NULL && vArgs->size() != 0 ) {
        return pConsole->SettingsSave( vArgs->at( 0 ) );
    }
    else {
        return pConsole->SettingsSave();
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
/**
 * Load console settings
 */
inline bool ConsoleSettingsLoad( std::vector<std::string> *vArgs )
{
    GLConsole* pConsole = GetConsole();
    if( vArgs != NULL && vArgs->size() != 0 ) {
        return pConsole->SettingsLoad( vArgs->at( 0 ) );
    }
    else {
        return pConsole->SettingsLoad();
    }
    return false;
}


#endif


