/*
    
    \file TextConsoleFunction.h

    Collection of ConsoleFunc's for TextConsole.

    $Id: TextConsoleFunction.h 164 2010-03-01 20:19:13Z gsibley $
 */

#ifndef __TEXT_CONSOLE_FUNCTION__
#define __TEXT_CONSOLE_FUNCTION__


#include <vector>
#include <cstdlib>

////////////////////////////////////////////////////////////////////////////////
/// Print the current CVars version.
inline bool ConsoleVersion( std::vector<std::string>* )
{
    printf( "CVars v%d.%d.%d", 
                      CVARS_MAJOR_REV, 
                      CVARS_MINOR_REV, 
                      CVARS_PATCH_REV );
    return true;
}

////////////////////////////////////////////////////////////////////////////////
///  Help for functions and variables or just general help.
inline bool ConsoleHelp( std::vector<std::string> *vArgs )
{
    TextConsoleInstance* pConsole = GetConsole();
    return pConsole->Help( vArgs );
}

////////////////////////////////////////////////////////////////////////////////
/// Looks for the lists of substrings provided in vArgs in the CVarTrie.
inline bool ConsoleFind( std::vector<std::string> *vArgs )
{
    if( vArgs != NULL && vArgs->size() > 0 ) {
        for( size_t i=0; i<vArgs->size(); i++ ) {
            std::vector<std::string> vCVarNames = g_pCVarTrie->FindListSubStr( vArgs->at(i) );
            
            for( size_t j=0; j<vCVarNames.size(); j++ ) { 
                printf( "%s", vCVarNames[j].c_str() );       
            }
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// First argument indicates the file name, the following arguments are
//  used to choose a subset of variables to be saved using the provided
//  substrings.
//  Last argument can be used to be verbose when saving.
inline bool ConsoleSave( std::vector<std::string> *vArgs )
{
    std::string sFile = "cvars.xml";
    std::vector< std::string > vAcceptedSubstrings;

    if( vArgs != NULL && vArgs->size() > 0 ) {
        sFile = vArgs->at( 0 );
        for( size_t i=1; i<vArgs->size(); i++ ) {
            vAcceptedSubstrings.push_back( vArgs->at( i ) );
        }
    }

    printf( "Saving cvars to \"%s\".", sFile.c_str() );
    if( !CVarUtils::Save( sFile, vAcceptedSubstrings ) ) {
        printf( "Error saving file.\n" );
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Load CVars from a file.
inline bool ConsoleLoad( std::vector<std::string> *vArgs )
{
    TextConsoleInstance* pConsole = GetConsole();
    std::string sFile = "cvars.xml";
    std::vector< std::string > vAcceptedSubstrings;
    
    if( vArgs != NULL && vArgs->size() > 0 ) {
        sFile = vArgs->at( 0 ); 
        for( size_t i=1; i<vArgs->size(); i++ ) {
            vAcceptedSubstrings.push_back( vArgs->at(i) );
        }
    }
    printf( "Loading file \"%s\".", sFile.c_str() );
    if( !CVarUtils::Load( sFile, vAcceptedSubstrings ) ) {
        printf( "Error: Could not load \"%s\".", sFile.c_str() );
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Exits program from command line.
inline bool ConsoleExit( std::vector<std::string> * )
{
    exit( 0 );
}

////////////////////////////////////////////////////////////////////////////////
/// Save the console history to a file.
inline bool ConsoleHistorySave( std::vector<std::string> *vArgs )
{
    TextConsoleInstance* pConsole = GetConsole();
    if( vArgs != NULL && vArgs->size() != 0 ) {
        return pConsole->HistorySave( vArgs->at( 0 ) );
    }
    else {
        return pConsole->HistorySave();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Load a previously saved console history from a file.
inline bool ConsoleHistoryLoad( std::vector<std::string> *vArgs )
{
    TextConsoleInstance* pConsole = GetConsole();
    if( vArgs != NULL && vArgs->size() != 0 ) {
        return pConsole->HistoryLoad( vArgs->at( 0 ) );
    }
    else {
        return pConsole->HistoryLoad();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// clear the console history.
inline bool ConsoleHistoryClear( std::vector<std::string>* )
{
    TextConsoleInstance* pConsole = GetConsole();
    pConsole->HistoryClear();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Save the current script.
inline bool ConsoleScriptSave( std::vector<std::string> *vArgs )
{
    TextConsoleInstance* pConsole = GetConsole();
    if( vArgs != NULL && vArgs->size() != 0 ) {
        return pConsole->ScriptSave( vArgs->at( 0 ) );
    }
    else {
        return pConsole->ScriptSave();
    }

}

////////////////////////////////////////////////////////////////////////////////
/// Load a script from a file.
inline bool ConsoleScriptLoad( std::vector<std::string> *vArgs )
{
    TextConsoleInstance* pConsole = GetConsole();
    if( vArgs != NULL && vArgs->size() != 0 ) {
        return pConsole->ScriptLoad( vArgs->at( 0 ) );
    }
    else {
        return pConsole->ScriptLoad();
    }
}

////////////////////////////////////////////////////////////////////////////////
/// Start script recording
inline bool ConsoleScriptRecordStart( std::vector<std::string> * )
{
    TextConsoleInstance* pConsole = GetConsole();
    pConsole->ScriptRecordStart();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
/// Stop script recording.
inline bool ConsoleScriptRecordStop( std::vector<std::string> * )
{
    TextConsoleInstance* pConsole = GetConsole();
    pConsole->ScriptRecordStop();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
///  Pause script recording.
inline bool ConsoleScriptRecordPause( std::vector<std::string> * )
{
    TextConsoleInstance* pConsole = GetConsole();
    pConsole->ScriptRecordPause();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
///  Show the current script.
inline bool ConsoleScriptShow( std::vector<std::string> * )
{
    TextConsoleInstance* pConsole = GetConsole();
    pConsole->ScriptShow();
    return true;
}

////////////////////////////////////////////////////////////////////////////////
///  Run the current scipt or one from the named file.
inline bool ConsoleScriptRun( std::vector<std::string> *vArgs )
{
    TextConsoleInstance* pConsole = GetConsole();
    if( vArgs != NULL && vArgs->size() != 0 ) {
        return pConsole->ScriptRun( vArgs->at( 0 ) );
    }
    else {
        return pConsole->ScriptRun();
    }
}

////////////////////////////////////////////////////////////////////////////////
///  Save the console settings.
inline bool ConsoleSettingsSave( std::vector<std::string> *vArgs )
{
    TextConsoleInstance* pConsole = GetConsole();
    if( vArgs != NULL && vArgs->size() != 0 ) {
        return pConsole->SettingsSave( vArgs->at( 0 ) );
    }
    else {
        return pConsole->SettingsSave();
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
/// Load console settings.
inline bool ConsoleSettingsLoad( std::vector<std::string> *vArgs )
{
    TextConsoleInstance* pConsole = GetConsole();
    if( vArgs != NULL && vArgs->size() != 0 ) {
        return pConsole->SettingsLoad( vArgs->at( 0 ) );
    }
    else {
        return pConsole->SettingsLoad();
    }
    return false;
}



#endif

