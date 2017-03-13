/*
    Cross platform OpenGL console using the "CVars" functionality.

    This Code is covered under the LGPL.  See COPYING file for the license.

    $Id: GLConsole.h 192 2012-03-06 01:12:01Z gsibley $
 */

#ifndef __GLCONSOLE_H__
#define __GLCONSOLE_H__


#include <cvars/CVar.h>
#include <cvars/Timestamp.h>
#include <cvars/glplatform.h>

#include <GLConsole/GLFont.h>
//#include <GLConsole/GLColor.h>

#include <deque>
#include <string>



#define CVAR_DEL_KEY 127
#define GLCONSOLE_KEY 96

#define GLCONSOLE_HELP_FILE "helpfile.txt"
#define GLCONSOLE_HISTORY_FILE ".cvar_history"
#define GLCONSOLE_SETTINGS_FILE ".glconsole_settings"
#define GLCONSOLE_SCRIPT_FILE "default.script"
#define GLCONSOLE_INITIAL_SCRIPT_FILE "initial.script"


////////////////////////////////////////////////////////////////////////////////
/// The type of line entered. Used to determine how each line is treated.
enum LineProperty
{
    LINEPROP_LOG,         // text coming from a text being logged to the console
    LINEPROP_COMMAND,     // command entered at the console
    LINEPROP_FUNCTION,    // a function
    LINEPROP_ERROR,       // an error
    LINEPROP_HELP         //help text
};

class GLConsole;

////////////////////////////////////////////////////////////////////////////////
///  A line of text contained in the console can be either inputted commands or
//  log text from the application
class ConsoleLine
{
    public:
        ConsoleLine(std::string t, LineProperty p = LINEPROP_LOG, bool display = true ){
            m_sText = t;
            m_nOptions = p;
            m_bDisplay = display;
        }
        std::string m_sText;        //actual text
        LineProperty m_nOptions;    //see LineProperty
        bool m_bDisplay;            // display on the console screen?
};


////////////////////////////////////////////////////////////////////////////////
/// This function returns a pointer to the very first GLConsole ever created.
//  As there should only be one, this is ok.
//  This is a workaround for header only programming.
inline GLConsole* GetConsole( GLConsole* pFirstConsole = NULL )
{
    static GLConsole* pSavedConsole = NULL;
    if( pSavedConsole ){
        return pSavedConsole;
    }
    if( pFirstConsole == NULL ){
        // if pFirstConsole is NULL (e.g. user is asking for this first console), then
        // pSavedConsole BETTER not also be NULL;
        fprintf( stderr, "ERROR: GLConsole has not been initialized!\n" );
    }
    else{
        pSavedConsole = pFirstConsole;
    }
    return pSavedConsole;
}

////////////////////////////////////////////////////////////////////////////////
/// This function calls "GetConsole" to set the static variable pSavedConsole so
//  we can get access to the console globally.
//  This is a workaround for header only programming.
inline void SetConsole( GLConsole* pFirstConsole )
{
    GetConsole( pFirstConsole );
}

////////////////////////////////////////////////////////////////////////////////
/// Return whether first element is greater than the second.
inline bool StringIndexPairGreater( std::pair<std::string,int> e1, std::pair<std::string,int> e2 )
{
    return e1.first < e2.first;
}

////////////////////////////////////////////////////////////////////////////////
/// Utility function.
inline std::string FindLevel( std::string sString, int iMinRecurLevel )
{
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
// remove all spaces from the front and back...
inline std::string& RemoveSpaces( std::string &str )
{
    // remove them off the front
    int idx = str.find_first_not_of( ' ' );
    if( idx > 0 && idx != 0 ) {
        str = str.substr( idx, str.length() );
    }

    // remove them off the back
    idx = str.find_last_not_of(' ');
    if( idx != -1 ) {
        str = str.substr( 0, idx+1 );
    }

    return str;
}



////////////////////////////////////////////////////////////////////////////////
///  The GLConsole class.
class GLConsole
{
    friend void GLConsoleCheckInit( GLConsole* pConsole );

    public:
         GLConsole();
        ~GLConsole();

        // call this after OpenGL is up
        void Init();

        //Prints to console using familiar printf style
        void Printf(const char *msg, ...);
        void Printf_All(const char *msg, ...);

        //error and help versions of printf.
        void PrintError(const char *msg, ...);
        void PrintHelp(const char *msg, ...);

        //commands to the console...
        void ToggleConsole();
        bool IsOpen();
		bool IsChanging();
        void OpenConsole();
        void CloseConsole();

        //scrolling text up and down in the console
        void ScrollUp(int pixels);
        void ScrollDown(int pixels);
        void ScrollUpLine();
        void ScrollDownLine();
        void CursorLeft();
        void CursorRight();
        void CursorToBeginningOfLine();
        void CursorToEndOfLine();
        void ScrollUpPage();
        void ScrollDownPage();
        void SpecialFunc( int key );

        void PrintAllCVars();

        /// Add a character to the command line.
        void KeyboardFunc( unsigned char key );

        /// Clear the current command.
        void ClearCurrentCommand();

        /// Clear backwards until the last dot
        void ClearCurrentWord();

        /// enter a full line of text to the log text.
        void EnterLogLine(const char *line, LineProperty prop = LINEPROP_LOG, bool display = true);

        /// display previous command in history on command line.
        void HistoryBack();

        /// go forward in the history.
        void HistoryForward();

        /// clears all of the console's history.
        void HistoryClear();

        bool HistorySave( std::string sFileName = "" );
        bool HistoryLoad( std::string sFileName = "" );

        bool SettingsSave(std::string sFileName = "");
        bool SettingsLoad(std::string sFileName = "");

        //script interface
        void ScriptRecordStart();
        void ScriptRecordStop();
        void ScriptRecordPause();
        void ScriptShow();
        bool ScriptRun( std::string sFileName = "" );
        bool ScriptSave( std::string sFileName = "" );
        bool ScriptLoad( std::string sFileName = "" );

        //help
        bool Help(std::vector<std::string> *vArgs);

        GLFont * GetFont() const { return m_pGLFont; }

        void SetLogColor(float r, float g, float b) {
            m_logColor.r = r; m_logColor.g = g; m_logColor.b = b;
        }

        void SetCommandColor( float r, float g, float b ) {
            m_commandColor.r = r; m_commandColor.g = g; m_commandColor.b = b;
        }

        void SetFunctionColor(float r, float g, float b) {
            m_functionColor.r = r; m_functionColor.g = g; m_functionColor.b = b;
        }

        void SetErrorColor(float r, float g, float b) {
            m_errorColor.r = r; m_errorColor.g = g; m_errorColor.b = b;
        }

        void SetHelpColor(float r, float g, float b) {
            m_helpColor.r = r; m_helpColor.g = g; m_helpColor.b = b;
        }
        void RenderConsole();

        // member cvars accessible from console
        float& m_fConsoleBlinkRate;
        float& m_fConsoleAnimTime;
        int&   m_nConsoleMaxHistory;
        int&   m_nConsoleLineSpacing;
        int&   m_nConsoleLeftMargin;
        int&   m_nConsoleVerticalMargin;
        int&   m_nConsoleMaxLines;
        float& m_fOverlayPercent;
        std::string& m_sHistoryFileName;
        std::string& m_sScriptFileName;
        std::string& m_sSettingsFileName;
        std::string& m_sInitialScriptFileName;

    protected:
        void _CheckInit();

        /// Height of the console in pixels (even if it is currently animating)
        int  _GetConsoleHeight();
        void _RenderText();
        void _TabComplete();
        bool _ProcessCurrentCommand( bool bExecute = true );
        bool _ExecuteFunction( CVarUtils::CVar<ConsoleFunc> * cvar, bool bExecute );
        bool _IsCursorOn();
        bool _IsConsoleFunc( TrieNode *node );
        int  _FindRecursionLevel( std::string sCommand );
        bool _LoadExecuteHistory( std::string sFileName = "", bool bExecute=false );

        std::string _GetHistory();

    protected:
        bool          m_bExecutingHistory; //Are we executing a script or not.
        bool          m_bSavingScript; // Are we saving a script
        bool          m_bConsoleOpen; // whether the console is drawing or not
        bool          m_bIsChanging; // whether the console is currently transitioning
        TimeStamp     m_Timer;
        TimeStamp     m_BlinkTimer;
        int           m_nWidth;
        int           m_nHeight;
        int           m_nViewportX;
        int           m_nViewportY;
        int           m_nViewportWidth;
        int           m_nViewportHeight;
        int           m_nTextHeight;
        int           m_nScrollPixels;  //the number of pixels the text has been scrolled "up"
        int           m_nCommandNum;
        GLFont*       m_pGLFont;

        // Text colors
        CVarUtils::Color&      m_logColor;
        CVarUtils::Color&      m_commandColor;
        CVarUtils::Color&      m_functionColor;
        CVarUtils::Color&      m_errorColor;
        CVarUtils::Color&      m_helpColor;

        //CVar<GLColor> m_consoleColor;
        CVarUtils::Color&      m_consoleColor;

        //history variables
        std::string   m_sOldCommand;
        char          m_cLastChar;
        unsigned int  m_nSuggestionNum;

        // simplify getting gl viewport
        struct {
            int x;
            int y;
            int width;
            int height;
        } m_Viewport;

        //std::string m_sMaxPrefix; // max prefix
        std::string m_sCurrentCommandBeg;      //current command being typed
        std::string m_sCurrentCommandEnd;      //current command being typed
        std::deque<ConsoleLine> m_consoleText; // all the console text
        std::deque<ConsoleLine> m_ScriptText; // all the console text
};






////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// INLINE DEFINITIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <algorithm>
#include <cstring>

#include <cvars/CVar.h>
#include <cvars/TrieNode.h>


/// Include a collection of useful "ConsoleFunc" functiions:
#include <GLConsole/GLConsoleFunction.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor
 */
inline GLConsole::GLConsole() :
    // Init our member cvars  (can't init the names in the class decleration)
    m_fConsoleBlinkRate( CVarUtils::CreateCVar<float>(    "console.BlinkRate", 4.0 ) ), // cursor blinks per sec
    m_fConsoleAnimTime( CVarUtils::CreateCVar<float>(     "console.AnimTime", 0.1 ) ),     // time the console animates
    m_nConsoleMaxHistory( CVarUtils::CreateCVar<int>(     "console.history.MaxHistory", 100 ) ), // max lines ofconsole history
    m_nConsoleLineSpacing( CVarUtils::CreateCVar<int>(    "console.LineSpacing", 2 ) ), // pixels between lines
    m_nConsoleLeftMargin( CVarUtils::CreateCVar<int>(     "console.LeftMargin", 5 ) ),   // left margin in pixels
    m_nConsoleVerticalMargin( CVarUtils::CreateCVar<int>( "console.VertMargin", 8 ) ),
    m_nConsoleMaxLines( CVarUtils::CreateCVar<int>(       "console.MaxLines", 200 ) ),
    m_fOverlayPercent( CVarUtils::CreateCVar<float>(      "console.OverlayPercent", 0.75 ) ),
    m_sHistoryFileName( CVarUtils::CreateCVar<> (         "console.history.HistoryFileName", std::string( GLCONSOLE_HISTORY_FILE ) ) ),
    m_sScriptFileName( CVarUtils::CreateCVar<> (          "script.ScriptFileName", std::string( GLCONSOLE_SCRIPT_FILE ) ) ),
    m_sSettingsFileName( CVarUtils::CreateCVar<> (        "console.settings.SettingsFileName", std::string( GLCONSOLE_SETTINGS_FILE ) ) ),
    m_sInitialScriptFileName( CVarUtils::CreateCVar<> (   "console.InitialScriptFileName", std::string( GLCONSOLE_INITIAL_SCRIPT_FILE ) ) ),
    m_logColor( CVarUtils::CreateCVar<CVarUtils::Color>(           "console.colors.LogColor", CVarUtils::Color( 255, 255, 64 ) ) ),
    m_commandColor( CVarUtils::CreateCVar<CVarUtils::Color>(       "console.colors.CommandColor", CVarUtils::Color( 255, 255, 255 ) ) ),
    m_functionColor( CVarUtils::CreateCVar<CVarUtils::Color>(      "console.colors.FunctionColor", CVarUtils::Color( 64, 255, 64 ) ) ),
    m_errorColor( CVarUtils::CreateCVar<CVarUtils::Color>(         "console.colors.ErrorColor", CVarUtils::Color( 255, 128, 64 ) ) ),
    m_helpColor( CVarUtils::CreateCVar<CVarUtils::Color>(          "console.colors.HelpColor", CVarUtils::Color( 110, 130, 200 ) ) ),
    m_consoleColor( CVarUtils::CreateCVar<CVarUtils::Color>(       "console.colors.ConsoleColor", CVarUtils::Color( 25, 60, 130, 120 ) ) )
{

    SetConsole( this );

    //pConsole = this;
    m_Viewport.width = 0;
    m_bConsoleOpen = 0;
    m_bSavingScript = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
inline GLConsole::~GLConsole()
{
    delete m_pGLFont;
    //HistorySave();
    //SettingsSave();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Initialise the console
 * Sets up all the default values.
 */
inline void GLConsole::Init()
{
    m_bExecutingHistory = false;
    m_bSavingScript = false;
    m_bConsoleOpen = false;
    m_bIsChanging = false;
    m_nTextHeight = 12;  // Hard coded for now???
    m_nScrollPixels = 0;
    m_nCommandNum = 0;

    // setup member CVars
    m_Timer.Stamp();
    m_BlinkTimer.Stamp();
    m_pGLFont = new GLFont();

    // if the width and height ptrs aren't supplied then just extract the info
    // from GL
    glGetIntegerv( GL_VIEWPORT, &m_Viewport.x );

    // add basic functions to the console
    CVarUtils::CreateCVar( "console.version", ConsoleVersion, "The current version of GLConsole" );
    CVarUtils::CreateCVar( "help", ConsoleHelp, "Gives help information about the console or more specifically about a CVar." );
    CVarUtils::CreateCVar( "find", ConsoleFind, "find 'name' will return the list of CVars containing 'name' as a substring." );
    CVarUtils::CreateCVar( "exit", ConsoleExit, "Close the application" );
    CVarUtils::CreateCVar( "quit", ConsoleExit, "Close the application" );
    CVarUtils::CreateCVar( "save", ConsoleSave, "Save the CVars to a file" );
    CVarUtils::CreateCVar( "load", ConsoleLoad, "Load CVars from a file" );

    CVarUtils::CreateCVar( "console.history.load", ConsoleHistoryLoad, "Load console history from a file" );
    CVarUtils::CreateCVar( "console.history.save", ConsoleHistorySave, "Save the console history to a file" );
    CVarUtils::CreateCVar( "console.history.clear", ConsoleHistoryClear, "Clear the current console history" );

    CVarUtils::CreateCVar( "console.settings.load", ConsoleSettingsLoad, "Load console settings from a file" );
    CVarUtils::CreateCVar( "console.settings.save", ConsoleSettingsSave, "Save the console settings to a file" );

    CVarUtils::CreateCVar( "script.record.start", ConsoleScriptRecordStart );
    CVarUtils::CreateCVar( "script.record.stop", ConsoleScriptRecordStop );
    CVarUtils::CreateCVar( "script.record.pause", ConsoleScriptRecordPause );
    CVarUtils::CreateCVar( "script.show", ConsoleScriptShow );
    CVarUtils::CreateCVar( "script.run", ConsoleScriptRun );
    CVarUtils::CreateCVar( "script.save", ConsoleScriptSave );
    CVarUtils::CreateCVar( "script.load", ConsoleScriptLoad );

    //load the default settings file
    SettingsLoad();

    //load the history file
    HistoryLoad();

    //load the initial execute script
    std::ifstream ifs( m_sInitialScriptFileName.c_str() );

    if( ifs.is_open() ) {
        ifs.close();
        ScriptRun(m_sInitialScriptFileName);
    }
    else
    {
//        std::cout << "Info: Initial script file, " << m_sInitialScriptFileName << ", not found." << std::endl;
        ifs.clear(std::ios::failbit);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Check that Init() has run. If not run.
 */
inline void GLConsole::_CheckInit()
{
    if( m_Viewport.width == 0 ) {
        Init();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Toggle the console open or closed
 */
inline void GLConsole::ToggleConsole()
{
    _CheckInit();
    if( m_bConsoleOpen ) {
        CloseConsole();
    }
    else {
        OpenConsole();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Is the console open?
 */
inline bool GLConsole::IsOpen()
{
    return m_bConsoleOpen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline bool GLConsole::IsChanging()
{
    return m_bIsChanging;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::OpenConsole()
{
    m_bConsoleOpen = true;
    m_bIsChanging = true;
    m_Timer.Stamp();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::CloseConsole()
{
    m_bConsoleOpen = false;
    m_bIsChanging = true;
    m_Timer.Stamp();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * This will output text to the GL console
 */
inline void GLConsole::Printf(const char *msg, ... )
{
    _CheckInit();
    char msgBuf[1024];
    va_list va_alist;

    if (!msg) return;

    va_start( va_alist, msg );
    vsnprintf( msgBuf, 1024, msg, va_alist );
    va_end( va_alist );
    msgBuf[1024 - 1] = '\0';

    EnterLogLine( msgBuf );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * same as Printf() except it also prints to the terminal
 */
inline void GLConsole::Printf_All(const char *msg, ... )
{
    _CheckInit();
    char msgBuf[1024];
    va_list va_alist;

    if (!msg) return;

    va_start( va_alist, msg );
    vsnprintf( msgBuf, 1024, msg, va_alist );
    va_end( va_alist );
    msgBuf[1024 - 1] = '\0';

    EnterLogLine( msgBuf );
    printf( "%s", msgBuf );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * This will output help formatted text to the GL console
 */
inline void GLConsole::PrintHelp(const char *msg, ... )
{
    _CheckInit();
    char msgBuf[1024];
    va_list va_alist;

    if (!msg) return;

    va_start( va_alist, msg );
    vsnprintf( msgBuf, 1024, msg, va_alist );
    va_end( va_alist );
    msgBuf[1024 - 1] = '\0';

    EnterLogLine( msgBuf, LINEPROP_HELP );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * This will output error formatted text to the GL console
 */
inline void GLConsole::PrintError(const char *msg, ... )
{
    _CheckInit();
    char msgBuf[1024];
    va_list va_alist;

    if (!msg) return;

    va_start( va_alist, msg );
    vsnprintf( msgBuf, 1024, msg, va_alist );
    va_end( va_alist );
    msgBuf[1024 - 1] = '\0';

    EnterLogLine( msgBuf, LINEPROP_ERROR );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gives you the height of the console in pixels (even if it is currently animating)
 */
inline int GLConsole::_GetConsoleHeight()
{
    //determine dimensions of scissor region
    float fConsoleHeight = m_Viewport.height * m_fOverlayPercent;
    if( m_Timer.Elapsed() > m_fConsoleAnimTime ) {
        m_bIsChanging = false;
        if( m_bConsoleOpen ) { // closing motion
            fConsoleHeight = m_Viewport.height * m_fOverlayPercent;
        }
        else{
            fConsoleHeight = 0;
        }
    }
    // animating
    if( m_bIsChanging ) {
        float elapsed = m_Timer.Elapsed();
        if( m_bConsoleOpen ) { // closing motion
            fConsoleHeight = m_Viewport.height * (elapsed / m_fConsoleAnimTime)
                * m_fOverlayPercent;
        }
        else {
            fConsoleHeight =  m_Viewport.height *  (1-(elapsed /
                        m_fConsoleAnimTime)) * m_fOverlayPercent;
        }
    }
    else {
    }
    return (int)fConsoleHeight;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Clears all of the console's history
 */
inline void GLConsole::HistoryClear()
{
    m_consoleText.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Save the console history to the named file.
 * If no file is specified the name held in the history.HistoryFileName CVar will be used.
 * @param sFilename save history to this file.
 * @return successor failure
 */
inline bool GLConsole::HistorySave( std::string sFileName )
{
    ///@TODO check filenames for validity - no spaces or illegal characters.

    if( !m_bExecutingHistory ) {
        if( sFileName == ""){
            if(m_sHistoryFileName != "") {
                sFileName = m_sHistoryFileName;
            }
            else {
                PrintError( "Warning: No default name. Resetting history filename to: \"%s\".", GLCONSOLE_HISTORY_FILE );
                sFileName = m_sHistoryFileName = GLCONSOLE_HISTORY_FILE;
            }
        }

        std::ofstream ofs( sFileName.c_str() );

        if( !ofs.is_open() ) {
            PrintError( "Error: could not open \"%s\" for saving.", sFileName.c_str() );
            m_bExecutingHistory = false;
            return false;
        }

        unsigned int nTextSize = m_consoleText.size();
        for( int ii = nTextSize-1; ii >= 0 ; --ii ) {
            if( m_consoleText[ii].m_nOptions == LINEPROP_COMMAND ) {
                ofs << m_consoleText[ii].m_sText << "\n";
            }
        }
        ofs.close();
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Start script recording
 */
inline void GLConsole::ScriptRecordStart()
{
    m_ScriptText.clear();
    m_bSavingScript = true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Stop script recording
 */
inline void GLConsole::ScriptRecordStop()
{
    if( !m_bSavingScript ) {
        return;
    }
    m_ScriptText.pop_front();
    m_ScriptText.pop_front();
    m_bSavingScript = false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Pause script recording
 */
inline void GLConsole::ScriptRecordPause()
{
    //unpause
    if( !m_bSavingScript && !m_ScriptText.empty() )
    {
        m_ScriptText.pop_front();
        m_ScriptText.pop_front();
        m_bSavingScript = true;
        return;
    }
    else  //pause
    {
        m_ScriptText.pop_front();
        m_ScriptText.pop_front();
        m_bSavingScript = false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Print the script to the console
 */
inline void GLConsole::ScriptShow()
{
    if( m_bSavingScript ) {
        m_ScriptText.pop_front();
        m_ScriptText.pop_front();
    }
    bool bWasSavingScript = m_bSavingScript;
    m_bSavingScript = false;
    for( int ii = m_ScriptText.size()-1; ii >= 0 ; --ii ) {
        if( m_ScriptText[ii].m_nOptions == LINEPROP_COMMAND ) {
            EnterLogLine( m_ScriptText[ii].m_sText.c_str(), LINEPROP_COMMAND );
        }
    }
    m_bSavingScript = bWasSavingScript;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Run the current script or the one specified on disk
 * @TODO currently overwrites the script in memory. need to allow multiple scripts to be held and ran for recursion
 */
inline bool GLConsole::ScriptRun( std::string sFileName )
{
    if(!sFileName.empty()){
        bool bsuccess = ScriptLoad(sFileName);
        if(!bsuccess){
            PrintError("Aborting script run");
            return false;
        }
    }

    if( m_bSavingScript ) {
        m_ScriptText.pop_front();
        m_ScriptText.pop_front();
    }
    bool bWasSavingScript = m_bSavingScript;
    m_bSavingScript = false;
    for( int ii = m_ScriptText.size()-1; ii >= 0 ; --ii ) {
        if( m_ScriptText[ii].m_nOptions == LINEPROP_COMMAND ) {
            m_sCurrentCommandBeg = m_ScriptText[ii].m_sText;
            m_sCurrentCommandEnd = "";
            _ProcessCurrentCommand( true );
            m_sCurrentCommandBeg = "";
        }
    }
    m_bSavingScript = bWasSavingScript;

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Save the current script to a file
 */
inline bool GLConsole::ScriptSave( std::string sFileName )
{
    if( !m_bExecutingHistory ) {
        m_bExecutingHistory = true;

        if( m_bSavingScript ) {
            m_ScriptText.pop_front();
            m_ScriptText.pop_front();
        }

        if( sFileName == "" ) {
            sFileName = CVarUtils::GetCVar<std::string>( "script.ScriptFileName" );
        }

        std::ofstream ofs( sFileName.c_str() );

        if( !ofs.is_open() ) {
            PrintError( "Error: could not open \"%s\" for saving.", sFileName.c_str() );
            m_bExecutingHistory = false;
            return false;
        }

        unsigned int nTextSize = m_ScriptText.size();

        for( int ii = nTextSize-1; ii >= 0 ; --ii ) {
            if( m_ScriptText[ii].m_nOptions == LINEPROP_COMMAND ) {
                ofs << m_ScriptText[ii].m_sText << "\n";
            }
        }

        ofs.close();
        m_bExecutingHistory = false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Load history from a file.
 * @param sFileName Load history from this file, else use default
 * @return sucess or failure
 */
inline bool GLConsole::HistoryLoad( std::string sFileName )
{
    if( sFileName == "" ) {
        if(m_sHistoryFileName != "")
            sFileName = m_sHistoryFileName;
        else {
            PrintError("Warning: No default name. Resetting history filename to: \"%s\".", GLCONSOLE_HISTORY_FILE);
            sFileName = m_sHistoryFileName = GLCONSOLE_HISTORY_FILE;
        }
    }

    //test if file exists
    std::ifstream ifs( sFileName.c_str() );

    if( ifs.is_open() ) {
        ifs.close();
        return _LoadExecuteHistory( sFileName, false );
    }
    else {
//        std::cout << "Info: History file, " << sFileName << ", not found." << std::endl;
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Load a script from a file
 */
inline bool GLConsole::ScriptLoad( std::string sFileName )
{
    if( sFileName == "") {
        if(m_sScriptFileName != "") {
            sFileName = m_sScriptFileName;
        }
        else {
            PrintError("Warning: No default name. Resetting script filename to: \"%s\".", GLCONSOLE_SCRIPT_FILE);
            sFileName = m_sScriptFileName = GLCONSOLE_SCRIPT_FILE;
        }
    }

    //test if file exists
    std::ifstream ifs( sFileName.c_str() );

    if( ifs.is_open() ) {
        ifs.close();
        return _LoadExecuteHistory( sFileName, true );
    }
    else {
//        std::cout << "Info: Script file, " << sFileName << ", not found." << std::endl;
        return false;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Load history from a file and execute it if desired
 */
inline bool GLConsole::_LoadExecuteHistory( std::string sFileName, bool bExecute )
{
    if( sFileName == "" ) {
        std::cerr << "_LoadExecuteHistory: No file specified. There is a bug in GLConsole. Please report it." << std::endl;
        return false;
    }

    if( !m_bExecutingHistory ) {
        m_bExecutingHistory = true;

        std::ifstream ifs( sFileName.c_str() );

        if( !ifs.is_open() ) {
            PrintError("Error: could not open \"%s\" for loading.", sFileName.c_str());
            m_bExecutingHistory = false;
            return false;
        }

        while( ifs.good() ) {
            char linebuf[1024];
            ifs.getline(linebuf, 1024);
            if( ifs.good() ) {
                m_sCurrentCommandBeg = linebuf;
                m_sCurrentCommandEnd = "";
                _ProcessCurrentCommand( bExecute );
                m_sCurrentCommandBeg = "";
            }
        }
        m_bExecutingHistory = false;

        ifs.close();
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Render the console
 */
inline void GLConsole::RenderConsole()
{
    _CheckInit();
    if( m_bConsoleOpen || m_bIsChanging ) {
        glPushAttrib(GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT | GL_SCISSOR_BIT | GL_TRANSFORM_BIT );

        glDisable(GL_LIGHTING);

        //blend function based on source alpha
        glEnable(GL_BLEND);
        glBlendFunc( GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA );

        //get the width and heigtht of the viewport
        glGetIntegerv(GL_VIEWPORT, &m_Viewport.x );

        //reset matrices and switch to ortho view
        glDisable(GL_DEPTH_TEST );
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0,m_Viewport.width,0,m_Viewport.height,-1,1);
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        //set up a scissor region to draw the console in
        GLfloat bottom = m_Viewport.height - _GetConsoleHeight();
        glScissor(0, bottom, //bottom coord
                m_Viewport.width, //width
                m_Viewport.height); //top coord
        glEnable(GL_SCISSOR_TEST);

        //render transparent background
        glDisable(GL_DEPTH_TEST); //for transparency
        glEnable(GL_BLEND);
        glColor4f( m_consoleColor.r,
                m_consoleColor.g,
                m_consoleColor.b,
                m_consoleColor.a );

        GLfloat verts[] = { 0.0f, bottom,
                            (GLfloat)m_Viewport.width, bottom,
                            (GLfloat)m_Viewport.width, (GLfloat)m_Viewport.height,
                            0.0f, (GLfloat)m_Viewport.height };
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, verts);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
        glDisableClientState(GL_VERTEX_ARRAY);

        //draw text
        _RenderText();

        //restore old matrices and properties...
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();

        glPopAttrib();
    }
}

////////////////////////////////////////////////////////////////////////////////
inline bool GLConsole::_IsCursorOn()
{
    float elapsed = m_BlinkTimer.Elapsed();
    if(elapsed > (1.0 / m_fConsoleBlinkRate)) {
        m_BlinkTimer.Stamp();
        return true;
    }
    else if( elapsed > 0.50*(1.0 / m_fConsoleBlinkRate) ) {
        return false;
    }
    else{
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::_RenderText()
{
    int consoleHeight = _GetConsoleHeight();

    if( consoleHeight - m_nConsoleVerticalMargin < 0 ) {
        return;
    }

    //set up a scissor region to draw the text in
    glScissor( 1 ,m_Viewport.height - _GetConsoleHeight() + 1, //bottom coord
            m_Viewport.width, //width
            consoleHeight - m_nConsoleVerticalMargin ); //top coord
    glEnable( GL_SCISSOR_TEST ); {
        int lines = (consoleHeight / m_nTextHeight);
        int scrollLines = (m_nScrollPixels / m_nTextHeight);
        lines += scrollLines;

        //start drawing from bottom of console up...
        int lineLoc = m_Viewport.height-1 - consoleHeight + m_nConsoleVerticalMargin;

        //draw command line first
        char blink = ' ';
        //figure out if blinking cursor is on or off
        if( _IsCursorOn() ) {
            blink = '_';
        }
        glColor3f(m_commandColor.r, m_commandColor.g, m_commandColor.b);
        m_pGLFont->glPrintf( m_nConsoleLeftMargin, lineLoc - m_nScrollPixels,
                "> " + m_sCurrentCommandBeg );
        int size = m_sCurrentCommandBeg.length();
        std::string em = "";
        for(int i=0;i<size;i++) {
            em = em+" ";
        }
        m_pGLFont->glPrintf( m_nConsoleLeftMargin, lineLoc - m_nScrollPixels,
                "> " + em + blink );
        m_pGLFont->glPrintf( m_nConsoleLeftMargin, lineLoc - m_nScrollPixels,
                "> " + em + m_sCurrentCommandEnd );

        lineLoc += m_nTextHeight + m_nConsoleLineSpacing;

        int count = 0;
        for(  int i = 1 ; i < lines; i++ ) {
            if( count >= m_nConsoleMaxLines)
                continue;
            if( (int)m_consoleText.size() > i - 1 ) {
                //skip this line if it was marked not to be displayed
                if( !(m_consoleText.begin() + (i-1))->m_bDisplay) {
                    lines++;
                    continue;
                }

                std::deque<ConsoleLine>::iterator it = m_consoleText.begin() + i - 1;
                std::string fulltext = (*it).m_sText;

                //set the appropriate color
                switch((*it).m_nOptions)
                {
                    case LINEPROP_FUNCTION:
                        glColor3f(m_functionColor.r, m_functionColor.g, m_functionColor.b);
                        break;
                    case LINEPROP_ERROR:
                        glColor3f(m_errorColor.r, m_errorColor.g, m_errorColor.b);
                        break;
                    case LINEPROP_HELP:
                        glColor3f(m_helpColor.r, m_helpColor.g, m_helpColor.b);
                        break;
                    default:
                        //regular log line...
                        glColor3f(m_logColor.r, m_logColor.g, m_logColor.b);
                        break;
                }

                //wrap text to multiple lines if necessary
                int chars_per_line = (int)(1.65*m_Viewport.width / m_nTextHeight);
                if( chars_per_line == 0 ) {
                    // What should we do if the window has width == 0 ?
                    return;
                }

                int iterations = (fulltext.length() / chars_per_line) + 1;
                for(int j = iterations -1; j >= 0 ; j-- ) {
                    //print one less line now that I have wrapped to another line
                    if( j < iterations - 1)
                    {
                        lines--;
                        lineLoc += m_nTextHeight + m_nConsoleLineSpacing;
                    }
                    count++;
                    int start = fulltext.substr(j*chars_per_line, chars_per_line).find_first_not_of( ' ' );
                    if( start >= 0  ) {
                        m_pGLFont->glPrintfFast(m_nConsoleLeftMargin, lineLoc - m_nScrollPixels,
                                fulltext.substr(j*chars_per_line+start, chars_per_line) );
                    }
                }
            }
            else
                break;

            lineLoc += m_nTextHeight + m_nConsoleLineSpacing;
        }
    }
    glDisable(GL_SCISSOR_TEST);
}

////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::ScrollDown(int pixels)
{
    _CheckInit();
    if( m_bConsoleOpen ) {
        m_nScrollPixels += pixels;
    }
}

////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::ScrollUp(int pixels)
{
    _CheckInit();
    if( m_bConsoleOpen ) {
        m_nScrollPixels -= pixels;
        if( m_nScrollPixels < 0 ) {
            m_nScrollPixels = 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::ScrollUpLine()
{
    ScrollUp( m_nTextHeight + m_nConsoleLineSpacing );
}

////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::ScrollDownLine()
{
    ScrollDown(m_nTextHeight + m_nConsoleLineSpacing);
}

////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::CursorLeft()
{
    _CheckInit();
    if( m_bConsoleOpen ) {
        if( m_sCurrentCommandBeg.length()>0 ) {
            m_sCurrentCommandEnd = m_sCurrentCommandBeg.substr( m_sCurrentCommandBeg.length()-1, m_sCurrentCommandBeg.length() )
                + m_sCurrentCommandEnd;
            m_sCurrentCommandBeg = m_sCurrentCommandBeg.substr( 0, m_sCurrentCommandBeg.length()-1 );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::CursorRight()
{
    _CheckInit();
    if( m_bConsoleOpen ) {
        if( m_sCurrentCommandEnd.length()>0 ) {
            m_sCurrentCommandBeg += m_sCurrentCommandEnd.substr( 0, 1 );
            m_sCurrentCommandEnd = m_sCurrentCommandEnd.substr( 1, m_sCurrentCommandEnd.length() );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::CursorToBeginningOfLine()
{
    _CheckInit();
    if( m_bConsoleOpen ) {
        m_sCurrentCommandEnd = m_sCurrentCommandBeg+m_sCurrentCommandEnd;
        m_sCurrentCommandBeg = "";
    }
}

////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::CursorToEndOfLine()
{
    _CheckInit();
    if( m_bConsoleOpen ) {
        m_sCurrentCommandBeg += m_sCurrentCommandEnd;
        m_sCurrentCommandEnd = "";
    }
}

////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::ScrollUpPage()
{
    ScrollUp( (int)((m_Viewport.height*m_fOverlayPercent) - 2*m_pGLFont->CharHeight()));
}

////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::ScrollDownPage()
{
    ScrollDown( (int)( (m_Viewport.height*m_fOverlayPercent) - 2*m_pGLFont->CharHeight() ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * What to do when certain special keys are pressed
 * such as arrow keys, home, end, page up, page down
 *
 */
inline void GLConsole::SpecialFunc( int key )
{
    if(!IsOpen())
        return;

    _CheckInit();

    int nGlutModifiers = glutGetModifiers();

    //   unsigned char ckey = key;
    //   std::cerr << "sf: key: " << ckey << ", " << key << " mod: " << nGlutModifiers << std::endl;


    switch(nGlutModifiers)
    {
        case GLUT_ACTIVE_SHIFT:
            switch( key ) {
                case GLUT_KEY_UP:
                    ScrollDownLine();
                    break;
                case GLUT_KEY_DOWN:
                    ScrollUpLine();
                    break;
                default:
                    break;
            }
            break;

        case GLUT_ACTIVE_CTRL:
            switch (key) {
                case GLUT_KEY_LEFT:
                    CursorToBeginningOfLine();
                    break;
                case GLUT_KEY_RIGHT:
                    CursorToEndOfLine();
                    break;
            }
            break;

        case GLUT_ACTIVE_ALT:
            switch (key) {
                case GLUT_KEY_LEFT:
                    ClearCurrentWord();
                    break;
            }
            break;

        default:
            switch (key) {
                case GLUT_KEY_LEFT:
                    CursorLeft();
                    break;
                case GLUT_KEY_RIGHT:
                    CursorRight();
                    break;
                case GLUT_KEY_PAGE_UP:
                    ScrollDownPage();
                    break;
                case GLUT_KEY_PAGE_DOWN:
                    ScrollUpPage();
                    break;
                case GLUT_KEY_UP:
                    HistoryBack();
                    break;
                case GLUT_KEY_DOWN:
                    HistoryForward();
                    break;
                case GLUT_KEY_HOME:
                    CursorToBeginningOfLine();
                    break;
                case GLUT_KEY_END:
                    CursorToEndOfLine();
                    break;
                default:
                    break;
            }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Process key presses
 */
inline void GLConsole::KeyboardFunc( unsigned char key)
{
    if(!IsOpen())
        return;

    _CheckInit();

    int nGlutModifiers = glutGetModifiers();
    //   int ikey = key;
    //   std::cerr << "kf: key: " << key << ", " << ikey << " mod: " << nGlutModifiers << std::endl;

    switch(nGlutModifiers)
    {
        case GLUT_ACTIVE_CTRL:
            //Yay for GLUT. when control is active the key codes change for some but not all keys, and appears not to be documented.
            //a-z and A-Z are now 1-26 and not 97-122 and 65-90
            switch( key ) {
                case ('a' - 96):
                    CursorToBeginningOfLine();
                    break;
                case ('e' - 96):
                    CursorToEndOfLine();
                    break;
                case('c' - 96):
                    ClearCurrentCommand();
                    break;
                case('w' - 96):
                    ClearCurrentWord();
                    break;
                default:
                    break;
            }
            break;

        case GLUT_ACTIVE_ALT:

            break;

        case GLUT_ACTIVE_SHIFT:
        default:
            switch(key)
            {
                case '\r':
                    //user pressed "enter"
                    _ProcessCurrentCommand();
                    m_sCurrentCommandBeg = "";
                    m_sCurrentCommandEnd = "";
                    m_nCommandNum = 0; //reset history
                    m_nScrollPixels = 0; //reset scrolling
                    break;

                case '\t':
                    //tab complete
                    _TabComplete();
                    break;

                case '\b':
                    // backspace
                    if( m_sCurrentCommandBeg.size() > 0 ) {
                        m_sCurrentCommandBeg = m_sCurrentCommandBeg.substr(0, m_sCurrentCommandBeg.size() - 1);
                    }
                    break;

                case CVAR_DEL_KEY:
                    // delete
                    if( m_sCurrentCommandEnd.size() > 0 ) {
                        m_sCurrentCommandEnd = m_sCurrentCommandEnd.substr(1, m_sCurrentCommandEnd.size() );
                    }
                    break;
#if 0
            } else if( chr == ' ' && m_sCurrentCommandEnd.size() > 0 ) { // space clear the right bit of the cursor
                m_sCurrentCommandEnd = "";
#endif
                default:
                m_sCurrentCommandBeg += key;
                m_nCommandNum = 0; //reset history
                m_nScrollPixels = 0; //reset scrolling
            }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::ClearCurrentCommand()
{
    m_sCurrentCommandBeg = "";
    m_sCurrentCommandEnd = "";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::ClearCurrentWord()
{
  // Remove a level from tab-completion
  const size_t nPosEqual = m_sCurrentCommandBeg.rfind('=');
  if(nPosEqual != m_sCurrentCommandBeg.npos) {
    m_sCurrentCommandBeg = m_sCurrentCommandBeg.substr(0, nPosEqual);
  }else{
    const size_t nPosDot = m_sCurrentCommandBeg.rfind('.');
    if(nPosDot != m_sCurrentCommandBeg.npos) {
      m_sCurrentCommandBeg = m_sCurrentCommandBeg.substr(0, nPosDot);
    }else{
      m_sCurrentCommandBeg = "";
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Add a line to the history log
 */
inline void GLConsole::EnterLogLine(const char *line, const LineProperty prop, bool display)
{
    _CheckInit();
    if( (int)m_consoleText.size() >= m_nConsoleMaxHistory ) {
        m_consoleText.pop_back();
    }

    if( line != NULL ) {
        m_consoleText.push_front( ConsoleLine(std::string(line), prop, display) );

        if( m_bSavingScript && prop != LINEPROP_ERROR ) {
            m_ScriptText.push_front( ConsoleLine(std::string(line), prop, display) );
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
//retreives command in history, if m_nCommandNum is invalid then m_sCurrentCommand is returned
inline std::string GLConsole::_GetHistory()
{
    _CheckInit();
    std::deque<ConsoleLine>::iterator it;
    int commandCount = 1;

    if( m_nCommandNum <= 0 ) {
        return m_sCurrentCommandBeg+m_sCurrentCommandEnd;
    }

    for( it = m_consoleText.begin() ; it != m_consoleText.end() ; it++ ) {
        if( it->m_nOptions == LINEPROP_COMMAND ) {
            if( commandCount == m_nCommandNum ) {
                return it->m_sText;
            }
            else {//not the right command
                commandCount++;
            }
        }
    }
    if(  m_nCommandNum > commandCount ) {
        m_nCommandNum = commandCount-1;
    }

    return m_sCurrentCommandBeg+m_sCurrentCommandEnd;
}

////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::HistoryBack()
{
    _CheckInit();
    if(m_nCommandNum <= 0 ) {
        m_sOldCommand = m_sCurrentCommandBeg+m_sCurrentCommandEnd;
    }
    m_nCommandNum++;
    std::string temp(m_sCurrentCommandBeg+m_sCurrentCommandEnd);
    m_sCurrentCommandBeg = _GetHistory();
    m_sCurrentCommandEnd = "";
}

////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::HistoryForward()
{
    _CheckInit();
    if( m_nCommandNum > 0 ) {
        m_nCommandNum--;
        if( m_nCommandNum == 0 ) { //restore old command line
            m_sCurrentCommandBeg = m_sOldCommand;
            m_sCurrentCommandEnd = "";
        }
        else{
            m_sCurrentCommandBeg = _GetHistory();
            m_sCurrentCommandEnd = "";
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
inline bool GLConsole::_IsConsoleFunc( TrieNode *node )
{
    if( typeid( ConsoleFunc ).name()
            == ((CVarUtils::CVar<int>*)node->m_pNodeData)->type() ) {
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
inline int GLConsole::_FindRecursionLevel( std::string sCommand )
{
    int level = 0;
    for( unsigned int ii = 0; ii < sCommand.length(); ii++ ) {
        if( sCommand.c_str()[ii]=='.' ) {
            level ++;
        }
    }
    return level;
}



/*
////////////////////////////////////////////////////////////////////////////////
void GLConsole::PrintAllCVars()
{
    Trie& trie = CVarUtils::TrieInstance();
    TrieNode* node = trie.FindSubStr(  RemoveSpaces( m_sCurrentCommandBeg ) );
    if( !node ) {
        return;
    }

    std::cout << "CVars:" << std::endl;

    // Retrieve suggestions (retrieve all leaves by traversing from current node)
    std::vector<TrieNode*> suggest = trie.CollectAllNodes( node );
    std::sort( suggest.begin(), suggest.end() );
    //output suggestions
    unsigned int nLongestName = 0;
    unsigned int nLongestVal = 0;
    for( unsigned int ii = 0; ii < suggest.size(); ii++ ){
        std::string sName = ( (CVarUtils::CVar<int>*) suggest[ii]->m_pNodeData )->m_sVarName;
        std::string sVal = CVarUtils::GetValueAsString( suggest[ii]->m_pNodeData );
        if( sName.length() > nLongestName ){
            nLongestName = sName.length();
        }
        if( sVal.length() > nLongestVal ){
            nLongestVal = sVal.length();
        }
     }

    if( suggest.size() > 1) {
        for( unsigned int ii = 0; ii < suggest.size(); ii++ ){
            std::string sName = ( (CVarUtils::CVar<int>*) suggest[ii]->m_pNodeData )->m_sVarName;
            std::string sVal = CVarUtils::GetValueAsString( suggest[ii]->m_pNodeData );
            std::string sHelp = CVarUtils::GetHelp( sName );
            sName.resize( nLongestName, ' ' );
            sVal.resize( nLongestVal, ' ' );
            printf( "%-s: Default value = %-30s   %-50s\n", sName.c_str(), sVal.c_str(), sHelp.empty() ? "" : sHelp.c_str() );
        }
    }
}
*/

////////////////////////////////////////////////////////////////////////////////
inline void GLConsole::_TabComplete()
{
    Trie& trie = CVarUtils::TrieInstance();
    TrieNode* node = trie.FindSubStr(  RemoveSpaces( m_sCurrentCommandBeg ) );
    if( !node ) {
        // Attempt to strip away '=' so that the value can be re-completed
        const size_t nEquals = m_sCurrentCommandBeg.rfind( "=" );
        if(nEquals != m_sCurrentCommandBeg.npos) {
            std::string sCommandStripEq = m_sCurrentCommandBeg.substr( 0, nEquals );
            node = trie.FindSubStr( RemoveSpaces( sCommandStripEq ) );
            if( node != NULL ) { m_sCurrentCommandBeg = sCommandStripEq; }
        }
    }
    if( !node ) {
        return;
    }
    else if( node->m_nNodeType == TRIE_LEAF || (node->m_children.size() == 0) ) {
        node = trie.Find(m_sCurrentCommandBeg);
        if( !_IsConsoleFunc( node ) ) {
            m_sCurrentCommandBeg += " = " + CVarUtils::GetValueAsString(node->m_pNodeData);
        }
    } else {
        // Retrieve suggestions (retrieve all leaves by traversing from current node)
        std::vector<TrieNode*> suggest = trie.CollectAllNodes(node);
        //output suggestions
        if( suggest.size() > 1) {
            std::vector<std::pair<std::string,int> > suggest_name_index_full;
            std::vector<std::pair<std::string,int> > suggest_name_index_set;
            // Build list of names with index from suggest
            // Find lowest recursion level
            int iMinRecurLevel = 100000;
            for( unsigned int ii = 0; ii < suggest.size(); ii++ ) {
                std::string sName = ( (CVarUtils::CVar<int>*) suggest[ii]->m_pNodeData )->m_sVarName;
                suggest_name_index_full.push_back( std::pair<std::string,int>( sName, ii ) );
                if( _FindRecursionLevel( sName ) < iMinRecurLevel ) {
                    iMinRecurLevel = _FindRecursionLevel( sName );
                }
            }
            // Sort alphabetically (this is useful for later removing
            // duplicate name at a given level and looks nice too...)
            std::sort( suggest_name_index_full.begin(), suggest_name_index_full.end(), StringIndexPairGreater );

            // Remove suggestions at a higher level of recursion
            std::string sCurLevel = "";
            int iCurLevel;
            for( unsigned int ii = 0; ii < suggest_name_index_full.size() ; ii++ ) {
                std::string sCurString = suggest_name_index_full[ii].first;
                iCurLevel = _FindRecursionLevel( sCurString );
                if( sCurLevel.length()==0 ) {
                    if( iCurLevel == iMinRecurLevel ) {
                        sCurLevel = "";
                        suggest_name_index_set.
                            push_back( std::pair<std::string,int>( sCurString,suggest_name_index_full[ii].second ) );
                    } else {
                        // Add new substring at given level
                        sCurLevel = FindLevel( sCurString, iMinRecurLevel );
                        suggest_name_index_set.push_back( std::pair<std::string,int>( sCurLevel,suggest_name_index_full[ii].second ) );
                    }
                } else {
                    if( sCurString.find( sCurLevel ) == std::string::npos ) {
                        // Add new substring at given level
                        sCurLevel = FindLevel( sCurString, iMinRecurLevel );
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

                if( (commands+tmp).length() > m_Viewport.width/m_pGLFont->CharWidth() ) {
                    cmdlines.push_back( commands );
                    commands.clear();
                }
                if( !_IsConsoleFunc( suggest[suggest_name_index_set[ii].second] ) ) {
                    commands += tmp;
                }
            }
            if( commands.length() ) cmdlines.push_back( commands );

            // add function lines
            for( unsigned int ii = 0; ii < suggest_name_index_set.size() ; ii++ ) {
                std::string tmp = suggest_name_index_set[ii].first;
                tmp.resize( longest, ' ' );
                if( (functions+tmp).length() > m_Viewport.width/m_pGLFont->CharWidth() ) {
                    funclines.push_back( functions );
                    functions.clear();
                }
                if( _IsConsoleFunc( suggest[suggest_name_index_set[ii].second] ) ) {
                    functions += tmp;
                }
            }
            if( functions.length() ) funclines.push_back( functions );

            // enter the results
            if( cmdlines.size() + funclines.size() > 0 ) {
                EnterLogLine( " ", LINEPROP_LOG );
            }
            for( unsigned int ii = 0; ii < cmdlines.size(); ii++ ) {
                EnterLogLine( cmdlines[ii].c_str(), LINEPROP_LOG );
            }
            for( unsigned int ii = 0; ii < funclines.size(); ii++ ) {
                EnterLogLine( funclines[ii].c_str(), LINEPROP_FUNCTION );
            }


            //do partial completion - look for paths with one child down the trie
            int c = m_sCurrentCommandBeg.length();
            while(node->m_children.size() == 1) {
                node = node->m_children.front();
                c++;
            }
            m_sCurrentCommandBeg = suggest_name_index_set[0].first.substr(0, c);
        } else if( suggest.size() == 1 ) {
            // Is this what the use wants? Clear the left bit...
            m_sCurrentCommandEnd = "";
            m_sCurrentCommandBeg = ((CVarUtils::CVar<int>*) suggest[0]->m_pNodeData)->m_sVarName;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// commands are in the following form:
// [Command] = value //sets a value
// or
// [Command] //prints out the command's value
inline bool GLConsole::_ProcessCurrentCommand( bool bExecute )
{
    //trie version
    //int error = 0;
    TrieNode*node;
    std::string sRes;
    bool bSuccess = true;

    Trie& trie = CVarUtils::TrieInstance();
    std::string m_sCurrentCommand = m_sCurrentCommandBeg+m_sCurrentCommandEnd;

    // remove leading and trailing spaces
    int pos = m_sCurrentCommand.find_first_not_of( " ", 0 );
    if( pos >= 0 ) {
        m_sCurrentCommand = m_sCurrentCommand.substr( pos );
    }
    pos = m_sCurrentCommand.find_last_not_of( " " );
    if( pos >= 0 ) {
        m_sCurrentCommand = m_sCurrentCommand.substr( 0, pos+1 );
    }

    // Make sure the command gets added to the history
    if( !m_sCurrentCommand.empty() ) {
        EnterLogLine( m_sCurrentCommand.c_str(), LINEPROP_COMMAND, false );
    }

    // Simply print value if the command is just a variable
    if( ( node = trie.Find( m_sCurrentCommand ) ) ) {
        //execute function if this is a function cvar
        if( _IsConsoleFunc( node ) ) {
            bSuccess &= CVarUtils::ExecuteFunction( m_sCurrentCommand, (CVarUtils::CVar<ConsoleFunc>*) node->m_pNodeData, sRes, bExecute );
            EnterLogLine( m_sCurrentCommand.c_str(), LINEPROP_FUNCTION );
        }
        else { //print value associated with this cvar
            EnterLogLine( ( m_sCurrentCommand + " = " +
                        CVarUtils::GetValueAsString(node->m_pNodeData)).c_str(), LINEPROP_LOG );
        }
    }
    //see if it is an assignment or a function execution (with arguments)
    else {
        int eq_pos; //get the position of the equal sign
        //see if this an assignment
        if( ( eq_pos = m_sCurrentCommand.find( "=" ) ) != -1 ) {
            std::string command, value;
            std::string tmp = m_sCurrentCommand.substr(0, eq_pos ) ;
            command = RemoveSpaces( tmp );
            value = m_sCurrentCommand.substr( eq_pos+1, m_sCurrentCommand.length() );
            if( !value.empty() ) {
                value = RemoveSpaces( value );
                if( ( node = trie.Find(command) ) ) {
                    if( bExecute ) {
                        CVarUtils::SetValueFromString( node->m_pNodeData, value );
                    }
                    EnterLogLine( ( command + " = " + value ).c_str(), LINEPROP_LOG );
                }
            }
            else {
                if( bExecute ) {
                    std::string out = "-glconsole: " + command + ": command not found";
                    EnterLogLine( out.c_str(), LINEPROP_ERROR );
                }
                bSuccess = false;
            }
        }
        //check if this is a function
        else if( ( eq_pos = m_sCurrentCommand.find(" ") ) != -1 ) {
            std::string function;
            function = m_sCurrentCommand.substr( 0, eq_pos );
            //check if this is a valid function name
            if( ( node = trie.Find( function ) ) && _IsConsoleFunc( node ) ) {
                bSuccess &= CVarUtils::ExecuteFunction( m_sCurrentCommand, (CVarUtils::CVar<ConsoleFunc>*)node->m_pNodeData, sRes, bExecute );
                EnterLogLine( m_sCurrentCommand.c_str(), LINEPROP_FUNCTION );
            }
            else {
                if( bExecute ) {
                    std::string out = "-glconsole: " + function + ": function not found";
                    EnterLogLine( out.c_str(), LINEPROP_ERROR );
                }
                bSuccess = false;
            }
        }
        else if( !m_sCurrentCommand.empty() ) {
            if( bExecute ) {
                std::string out = "-glconsole: " + m_sCurrentCommand + ": command not found";
                EnterLogLine( out.c_str(), LINEPROP_ERROR );
            }
            bSuccess = false;
        }
        else { // just pressed enter
            EnterLogLine( " ", LINEPROP_LOG );
        }
    }

    return bSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Help interpreter function
 * With no arguments prints out help from GLCONSOLE_HELPFILE
 */
inline bool GLConsole::Help(std::vector<std::string> *vArgs)
{
    if( vArgs && vArgs->size() != 0 )
    {
        for( size_t i = 0; i < vArgs->size(); i++ ) {
            try {
                PrintHelp( "help for %s", vArgs->at(i).c_str() );
                if(CVarUtils::GetHelp( vArgs->at(i) ).empty())
                    PrintHelp( "No help available." );
                else
                    PrintHelp( "%s", CVarUtils::GetHelp( vArgs->at(i) ).c_str() );
            }
            catch( CVarUtils::CVarException e ) {
                PrintHelp( "Unknown variable %s.", vArgs->at(i).c_str() );
                return false;
            }
        }
    }
    else {
        //try opening a default helpfile
        //else use the built in help
        std::ifstream sIn(GLCONSOLE_HELP_FILE  );
        if( sIn.is_open() ) {
            char s[1024];
            while(sIn.good()) {
                sIn.getline(s, 1024);
                PrintHelp(s);
            }
            sIn.close();
        }
        else{
            std::cerr << "WARNING: No custom " << GLCONSOLE_HELP_FILE << ". Using default GLConsole help." << std::endl;

            PrintHelp("");
            PrintHelp("----------------- HELP -----------------");
            PrintHelp("Press ~ key to open and close console");
            PrintHelp("Press TAB to see the available commands and functions");
            PrintHelp("Functions are shown in green and variables in yellow");
            PrintHelp("Setting a value: [command] = value");
            PrintHelp("Getting a value: [command]");
            PrintHelp("Functions: [function] [arg1] [arg2] ...");
            PrintHelp("Entering just the function name will give a description.");
            PrintHelp("History: Up and Down arrow keys move through history.");
            PrintHelp("Tab Completion: TAB does tab completion and makes suggestions.");
            PrintHelp("----------------- HELP -----------------");
            PrintHelp("");
        }
    }
    return true;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Save settings current console settings to a file
 * @param sFileName the file to save to. if none given the one specified in console.SettingsFileName is used
 * @return sucess or failure
 */
inline bool GLConsole::SettingsSave(std::string sFileName)
{
    if( !m_bExecutingHistory ) {
        if( sFileName == ""){
            if(m_sSettingsFileName != "") {
                sFileName = m_sSettingsFileName;
            }
            else {
                PrintError( "Warning: No default name. Resetting settings filename to: \"%s\".", GLCONSOLE_SETTINGS_FILE );
                sFileName = m_sHistoryFileName = GLCONSOLE_SETTINGS_FILE;
            }
        }

        std::ofstream ofs( sFileName.c_str() );

        if( !ofs.is_open() ) {
            PrintError("Error: could not open \"%s\" for saving.", sFileName.c_str());
            return false;
        }

        std::vector<std::string> vSave;
        vSave.push_back(sFileName);
        vSave.push_back("console");
        vSave.push_back("script");
        ConsoleSave( &vSave );

        ofs.close();
    }
    return true;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Load console settings from a file
 * @param sFileName the file to load from. if none given the one specified in console.SettingsFileName is used
 * @return sucess or failure
 */
inline bool GLConsole::SettingsLoad( std::string sFileName )
{
    if( sFileName == "" ) {
        if( m_sSettingsFileName != "" ) {
            sFileName = m_sSettingsFileName;
        }
        else {
            PrintError( "Warning: No default name. Resetting settigns filename to: \"%s\".", GLCONSOLE_SETTINGS_FILE );
            sFileName = m_sSettingsFileName = GLCONSOLE_SETTINGS_FILE;
        }
    }

    //test if file exists
    std::ifstream ifs( sFileName.c_str() );

    if( ifs.is_open() ) {
        ifs.close();
        std::vector<std::string> v;
        v.push_back(sFileName);
        ConsoleLoad(&v);
    }
    else {
//        std::cout << "Info: Settings file, " << sFileName << ", not found." << std::endl;
        return false;
    }

    return true;
}

#endif
