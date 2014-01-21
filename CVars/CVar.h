/*

    Cross platform "CVars" functionality.

    This Code is covered under the LGPL.  See COPYING file for the license.

    $Id: CVar.h 201 2012-10-17 18:43:27Z effer $

 */


////////////////////////////////////////////////////////////////////////////////
// Facilities to get and set a variable of any type from the command line uses
// a Trie to store strings for easy lookup have specialized cast and assignment
// operators to fascilitate little code change They can also be Serialized and
// Restored on app startup and shutdown
// Example:
//  "int height = 2;"
// would be written as
//  "int& height = CVarUtils::CreateCVar( "height", 2);"
// Saving can be done in the Console with:
// save cvars.xml
// or for verbose:
// save cvars.xml true
// or to save just the values starting by "cons" with verbose:
// save cvars.xml cons true

#ifndef CVAR_H__
#define CVAR_H__

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <typeinfo>
#include <algorithm>
#include <memory>

#include <CVars/CVarVersion.h>
#include <CVars/Trie.h>
#include <CVars/TrieNode.h>
#include <CVars/CVarVectorIO.h>

////////////////////////////////////////////////////////////////////////////////
// A global Trie structure holds all of the variables organized by their string
// name.  He lives in Trie.cpp.
//extern Trie CVarTrie;

extern std::unique_ptr<Trie> g_pCVarTrie;

void InitCVars();

// Console functions must have the following signature
typedef bool (*ConsoleFunc)( std::vector<std::string> *args);

namespace CVarUtils
{
    ////////////////////////////////////////////////////////////////////////////////
	template <typename T>
	class CVarRef
	{
	public:
		CVarRef(T* reference=0) : var(reference) {}
		~CVarRef() {}
		T* var;
	};

	// overload the stream operators (so operator << and >> are used on type T when using a CVarRef<T>)
	template <typename T> std::ostream& operator<<(std::ostream &out, const CVarRef<T> &v) { out << *v.var; return out; }
	template <typename T> std::istream& operator>>(std::istream &in, CVarRef<T> &v)        { in >> *v.var;  return in;  }

    inline std::ostream &operator<<(std::ostream &stream, ConsoleFunc &)
    {
        return stream;
    }

    inline std::istream &operator>>(std::istream &stream, ConsoleFunc &)
    {
        return stream;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// Color class: useful in practice for most consoles
    struct Color {
        Color( float tr = 1.0f,
               float tg = 1.0f,
               float tb = 1.0f,
               float ta = 1.0f ) {
            r = tr; g = tg; b = tb; a = ta;
        }
        Color( int tr,
               int tg,
               int tb,
               int ta = 255
               ) {
            r = tr/255.0f; g = tg/255.0f; b = tb/255.0f; a = ta/255.0f;
        }
        //float fColor[0];
        struct { float r; float g; float b; float a; };
    };

    ////////////////////////////////////////////////////////////////////////////////
    /// All types you wish to use with CVars must overload << and >>.
    inline std::ostream &operator<<( std::ostream &stream, Color &color ) {
        int r = int( 255*color.r );
        int g = int( 255*color.g );
        int b = int( 255*color.b );
        int a = int( 255*color.a );
        stream << "[ " << r << ",  " << g << ",  " << b << ",  " << a << " ]";
        return stream;
    }

    ////////////////////////////////////////////////////////////////////////////////
    /// All types you wish to use with CVars must overload << and >>.
    inline std::istream &operator>>( std::istream &stream, Color &color ) {
        int r=0, g=0, b=0, a=0;
        const int CVAR_NAME_MAX = 1024; char str[CVAR_NAME_MAX] = {0};
        stream.readsome( str, CVAR_NAME_MAX );
        sscanf( str, "[ %d, %d, %d, %d ]", &r, &g, &b, &a );
        color.r = (float)r/255.0f;
        color.g = (float)g/255.0f;
        color.b = (float)b/255.0f;
        color.a = (float)a/255.0f;
        return stream;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // This is our generic data-to-string function, there is one instantiated for
    // every kind of CVar type that gets declared.  To print, the CVar type merely
    // has to overload <<.  To support reading from the console, just overload >>.
    template <class T>
        std::string CVarValueString( T *t )
        {
            std::ostringstream oss;
            oss << *t;
            return oss.str();
        }

    ////////////////////////////////////////////////////////////////////////////////
    // Each time a CVar is constructed we register it's type so we can recover the
    // original type after being cast about (esp. within the Trie data structure).
    template <class T>
        std::string CVarTypeString( T *t )
        {
            std::ostringstream oss;
            oss << typeid( *t ).name();
            return oss.str();
        }

    ////////////////////////////////////////////////////////////////////////////////
    // Each time a CVar is constructed we register it's type so we can recover the
    // original type after being cast about (esp. within the Trie data structure).
    template <class T>
        void StringToCVarValue( T *t, const std::string &sValue )
        {
            std::istringstream iss( sValue );
            iss >> *t;
        }

    ////////////////////////////////////////////////////////////////////////////////
    /** These functions must be called to create a CVar, they return a reference to
     *  the value saved.
     *  A default parameter makes it possible to avoid this variable
     *  from being saved.
     *  eg. int& nGUIWidth = CVarUtils::CreateCVar<int>( "gui.Width", 10 );
     */
    template <class T> T& CreateCVar(
            const std::string& s,
            T val,
            std::string sHelp = "No help available",
            std::ostream& (*pSerialisationFuncPtr)( std::ostream &, T ) = NULL,
            std::istream& (*pDeserialisationFuncPtr)( std::istream &, T ) = NULL
            );

    template <class T> T& CreateGetCVar(
            const std::string& s,
            T val,
            std::string sHelp = "No help available",
            std::ostream& (*pSerialisationFuncPtr)( std::ostream &, T ) = NULL,
            std::istream& (*pDeserialisationFuncPtr)( std::istream &, T ) = NULL
            );

    template <class T> T& CreateUnsavedCVar(
            const std::string& s,
            T val,
            const std::string& sHelp = "No help available",
            std::ostream& (*pSerialisationFuncPtr)( std::ostream &, T ) = NULL,
            std::istream& (*pDeserialisationFuncPtr)( std::istream &, T ) = NULL
            );

    template <class T> T& CreateGetUnsavedCVar(
            const std::string& s,
            T val,
            const std::string& sHelp = "No help available",
            std::ostream& (*pSerialisationFuncPtr)( std::ostream &, T ) = NULL,
            std::istream& (*pDeserialisationFuncPtr)( std::istream &, T ) = NULL
            );

    ////////////////////////////////////////////////////////////////////////////////
    /** These functions must be called to attach a CVar to a variable.
     *  Use these functions if you do not want to use references to CVars (as
     *  created with \c CreateCVar() ).
     *  \code
     *    int nGUIWidth = 100; // declare a variable as you usually do
     *    AttachCVar<int>( "gui.Wdith", &nGuiWidth ); // attach the variable
     *
     *    nGUIWidth = 200;     // change the variable as you usually do
     *  \endcode
     */
    template <typename T>
    void AttachCVar(const std::string& s,
                    T* ref,
                    const std::string& sHelp = "No help available");

    ////////////////////////////////////////////////////////////////////////////////
    /** These functions can be called to obtain a reference to a previously
     *  created CVar.
     *
     *  The exception "CVarUtils::CVarNonExistant" will be thrown if the value
     *  does not exist.
     *  eg. int& nGUIWidth = CVarUtils::GetCVarRef<int>( "gui.Width" );
     */
    template <class T> T& GetCVarRef( const char* s );
    template <class T> T& GetCVarRef( std::string s );

    ////////////////////////////////////////////////////////////////////////////////
    /** These functions can be called to obtain the value of a previously
     *  created CVar.
     *
     *  The exception "CVarUtils::CVarNonExistant" will be thrown if the value
     *  does not exist.
     *  eg. int nGUIWidth = CVarUtils::GetCVar<int>( "gui.Width" );
     */
    template <class T> T GetCVar( const char* s );
    template <class T> T GetCVar( std::string s );

    ////////////////////////////////////////////////////////////////////////////////
    /** This function can be called to determine if a particular CVar exists.
      */
    bool CVarExists( std::string s );

    ////////////////////////////////////////////////////////////////////////////////
    /** These functions can be called to change the value of a previously
     *  created CVar.
     *
     *  The exception "CVarUtils::CVarNonExistant" will be thrown if
     *  the value does not exist.
     *  eg. CVarUtils::SetCVar<int>( "gui.Width", 20 );
     */
    template <class T> void SetCVar( const char* s, T val );
    template <class T> void SetCVar( std::string s, T val );

    ////////////////////////////////////////////////////////////////////////////////
    /** These functions can be called to obtain the help value associated with a
     *  previously created CVar.
     *
     *  The exception "CVarUtils::CVarNonExistant" will be thrown if
     *  the value does not exist.
     *  eg. printf( "GUI help: %s\n", CVarUtils::GetHelp( "gui.Width" ) );
     */
    const std::string& GetHelp( const char* s );
    const std::string& GetHelp( std::string s );

    ////////////////////////////////////////////////////////////////////////////////
    /** This function prints all the CVars out, along with their default values and
     *  help message -- useful for creating documentation.
     */
 //    void PrintAllCVars();

    ////////////////////////////////////////////////////////////////////////////////
    /// Changes the input/output types when calling Save and Load, options:
    /// - CVARS_XML_STREAM is the default
    /// - TXT_XML_STREAM is another option where the format is 'cvar_name = cvar_value' per line
    /// with commented lines starting by '#' or '//'
    inline void SetStreamType( const CVARS_STREAM_TYPE& stream_type );

    ////////////////////////////////////////////////////////////////////////////////
    /** This function saves the CVars to "sFileName", it takes an optional
     *  argument that is a vector of substrings indicating the CVars that should
     *  or should not be saved.
     *
     *  If this vector is empty, all the CVars are saved.
     *  If "not" is add to the list, all the following substrings will not be saved.
     *  If "true" is used as the last argument, the saving will be verbose.
     */
    inline bool Save( const std::string& sFileName,
                      std::vector<std::string> vFilterSubstrings=std::vector<std::string>() );

    ////////////////////////////////////////////////////////////////////////////////
    /** This function loads the CVars from "sFileName", it takes an optional
     *  argument that is a vector of substrings indicating the CVars that should
     *  or should not be loaded.
     *
     *  If this vector is empty, all the CVars are loaded.
     *  If "not" is add to the list, all the following substrings will not be loaded.
     *  If "true" is used as the last argument, the loading will be verbose.
     */
    inline bool Load( const std::string& sFileName,
                      std::vector<std::string> vFilterSubstrings=std::vector<std::string>() );

    /** Utilities for the indentation of XML output */
    inline std::string CVarSpc();
    inline void CVarIndent();
    inline void CVarUnIndent();
    inline void CVarResetSpc();
}


////////////////////////////////////////////////////////////////////////////////
namespace CVarUtils {
    enum CVarException {
        CVarsNotInitialized,
        CVarNonExistant,
        CVarAlreadyCreated,
        ReservedName
    };
}

////////////////////////////////////////////////////////////////////////////////
namespace CVarUtils {
    template <class T> class CVar
        {
        public:
            ////////////////////////////////////////////////////////////////////////////////
            CVar( std::string sVarName,
                  T TVarValue, std::string sHelp="No help available",
                  bool bSerialise=true,   /**< Input: if false, this CVar will not be taken into account when serialising (eg saving) the Trie */
                  std::ostream& (*pSerialisationFuncPtr)( std::ostream &, T ) = NULL,
                  std::istream& (*pDeserialisationFuncPtr)( std::istream &, T ) = NULL ) {

                if( g_pCVarTrie.get() == NULL ){
                    InitCVars();
                }
                if( g_pCVarTrie->Exists( sVarName ) ) {
                    throw CVarUtils::CVarAlreadyCreated;
                }

                //std::cout << TToStream( std::cout, TVarValue );
                m_pValueStringFuncPtr = CVarValueString; // template pointer to value string func
                m_pTypeStringFuncPtr = CVarTypeString; // template pointer to type string func
                m_pSetValueFuncPtr = StringToCVarValue;

                m_pSerialisationFuncPtr   = pSerialisationFuncPtr;
                m_pDeserialisationFuncPtr = pDeserialisationFuncPtr;

                m_pVarData = new T;
                *m_pVarData = TVarValue;
                m_sVarName = sVarName;
                m_bSerialise = bSerialise;
                m_sHelp = sHelp;
            }

            ////////////////////////////////////////////////////////////////////////////////
            ~CVar() {
              delete m_pVarData;
            }

            ////////////////////////////////////////////////////////////////////////////////
            // Convert value to string representation
            // Call the original function that was installed at object creation time,
            // regardless of current object class type T.
            std::string GetValueAsString() {
                if( m_pSerialisationFuncPtr != NULL ) {
                    std::stringstream sStream( "" );
                    m_pSerialisationFuncPtr( sStream, *m_pVarData );
                    return sStream.str();
                }
                else {
                    return (*m_pValueStringFuncPtr)( m_pVarData );
                }
            }

            ////////////////////////////////////////////////////////////////////////////////
            // Convert string representation to value
            // Call the original function that was installed at object creation time,
            // regardless of current object class type T.
            void SetValueFromString( const std::string &sValue ) {
                if( m_pDeserialisationFuncPtr != NULL ) {
                    std::stringstream sStream( sValue );
                    m_pDeserialisationFuncPtr( sStream, *m_pVarData );
                }
                else {
                    if( m_pSetValueFuncPtr != NULL ) {
                        (*m_pSetValueFuncPtr)( m_pVarData, sValue );
                    }
                }
            }

            ////////////////////////////////////////////////////////////////////////////////
            // Convert type to string representation
            // Call the original function that was installed at object creation time,
            // regardless of current object class type T.
            std::string type() {
                return (*m_pTypeStringFuncPtr)( m_pVarData );
            }

            ////////////////////////////////////////////////////////////////////////////////
            // Get values to and from a string representation (used for
            // serialization and console interaction)
            bool FromString( std::string s ); //return true if successful

            const std::string& GetHelp() {
                return m_sHelp;
            }


        public: // Public data
            std::string   m_sVarName;
            T            *m_pVarData;
            bool m_bSerialise;

        private:
            std::string         m_sHelp;

            // pointer to func to get CVar type as a string
            std::string (*m_pTypeStringFuncPtr)( T *t );

            // pointer to func to get CVar value as a string
            std::string (*m_pValueStringFuncPtr)( T *t );

            // pointer to func to set CVar Value from a string
            void (*m_pSetValueFuncPtr)( T *t, const std::string & );

            std::ostream& (*m_pSerialisationFuncPtr)( std::ostream &, T );
            std::istream& (*m_pDeserialisationFuncPtr)( std::istream &, T ) ;
        };
}

namespace CVarUtils {
    ////////////////////////////////////////////////////////////////////////////////
    template <class T> T& CreateCVar(
            const std::string& s,
            T val,
            std::string sHelp,
            std::ostream& (*pSerialisationFuncPtr)( std::ostream &, T ),
            std::istream& (*pDeserialisationFuncPtr)( std::istream &, T )
            )
    {
        if( g_pCVarTrie.get() == NULL ){
            InitCVars();
        }
        if( g_pCVarTrie.get()->Exists( s ) ) {
            throw CVarAlreadyCreated;
        }
        if( std::string( s ) == "true" ||
                std::string( s ) == "false" ||
                std::string( s ) == "not") {
            throw ReservedName;
        }
#ifdef DEBUG_CVAR
        printf( "Creating variable: %s.\n", s  );
#endif
        CVarUtils::CVar<T> *pCVar = new CVarUtils::CVar<T>(
                s, val, sHelp, true, pSerialisationFuncPtr, pDeserialisationFuncPtr );
        g_pCVarTrie->Insert( s, (void *) pCVar );
        return *(pCVar->m_pVarData);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T> T& CreateGetCVar(
            const std::string& s,
            T val,
            std::string sHelp,
            std::ostream& (*pSerialisationFuncPtr)( std::ostream &, T ),
            std::istream& (*pDeserialisationFuncPtr)( std::istream &, T )
            )
    {
        try {
            return CreateCVar( s, val, sHelp, pSerialisationFuncPtr, pDeserialisationFuncPtr );
        }
        catch( CVarUtils::CVarException e  ){
            switch( e ) {
            case CVarUtils::CVarAlreadyCreated:
                break;
            default:
                throw e;
                break;
            }
        }
        return CVarUtils::GetCVarRef<T>( s );
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T> T& CreateGetUnsavedCVar(
            const std::string& s,
            T val,
            const std::string& sHelp,
            std::ostream& (*pSerialisationFuncPtr)( std::ostream &, T ),
            std::istream& (*pDeserialisationFuncPtr)( std::istream &, T )
            )
    {
        try {
            return CreateUnsavedCVar( s, val, sHelp, pSerialisationFuncPtr, pDeserialisationFuncPtr );
        }
        catch( CVarUtils::CVarException e  ){
            switch( e ) {
            case CVarUtils::CVarAlreadyCreated:
                break;
            default:
                throw e;
                break;
            }
        }
        return CVarUtils::GetCVarRef<T>( s );
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T> T& CreateUnsavedCVar(
            const std::string& s,
            T val,
            const std::string& sHelp,
            std::ostream& (*pSerialisationFuncPtr)( std::ostream &, T ),
            std::istream& (*pDeserialisationFuncPtr)( std::istream &, T )
            )
    {
        if( g_pCVarTrie.get() == NULL ){
            InitCVars();
        }
        if( g_pCVarTrie.get()->Exists( s ) ) {
            throw CVarAlreadyCreated;
        }
        if( std::string( s ) == "true" ||
            std::string( s ) == "false" ||
            std::string( s ) == "not") {
            throw ReservedName;
        }
#ifdef DEBUG_CVAR
        printf( "Creating variable: %s.\n", s  );
#endif
        CVarUtils::CVar<T> *pCVar = new CVarUtils::CVar<T>( s, val, sHelp, false, pSerialisationFuncPtr, pDeserialisationFuncPtr );
        g_pCVarTrie->Insert( s, (void *) pCVar );
        return *(pCVar->m_pVarData);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <typename T>
	void AttachCVar(const std::string& s,
                    T* ref,
                    const std::string& sHelp)
    {
        CreateCVar<CVarRef<T> >(s, CVarRef<T>(ref), sHelp, 0, 0);
    }

   ////////////////////////////////////////////////////////////////////////////////
    template <class T> T& GetCVarRef( const char* s ) {
        if( g_pCVarTrie.get() == NULL ){
            InitCVars();
        }
        if( !g_pCVarTrie.get()->Exists( s ) ) {
            throw CVarNonExistant;
        }
        return *(((CVar<T>*)g_pCVarTrie.get()->Find(s)->m_pNodeData)->m_pVarData);
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T> T& GetCVarRef( std::string s ) {
        return CVarUtils::GetCVarRef<T>( s.c_str() );
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T> T GetCVar( const char* s ) {
        return CVarUtils::GetCVarRef<T>( s );
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T> T GetCVar( std::string s ) {
        return CVarUtils::GetCVarRef<T>( s.c_str() );
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline bool CVarExists( std::string s ){
        return g_pCVarTrie->Exists( s );
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline std::string GetCVarString( std::string s ) {
        if( g_pCVarTrie.get() == NULL ){
            InitCVars();
        }
        if( !g_pCVarTrie.get()->Exists( s ) ) {
            throw CVarNonExistant;
        }
        return
            ((CVar<int>*)g_pCVarTrie.get()->Find(s)->m_pNodeData)->GetValueAsString();
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T> void SetCVar( const char* s, T val ) {
        if( g_pCVarTrie.get() == NULL ){
            InitCVars();
        }
        if( !g_pCVarTrie.get()->Exists( s ) ) {
            throw CVarNonExistant;
        }
        *(((CVar<T>*)g_pCVarTrie.get()->Find(s)->m_pNodeData)->m_pVarData) = val;
    }

    ////////////////////////////////////////////////////////////////////////////////
    template <class T> void SetCVar( std::string s, T val ) {
        CVarUtils::SetCVar( s.c_str(), val );
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline const std::string& GetHelp( const char* s ) {
        if( g_pCVarTrie.get() == NULL ){
            InitCVars();
        }
        if( !g_pCVarTrie.get()->Exists( s ) ) {
            throw CVarNonExistant;
        }
        return ((CVar<int>*)g_pCVarTrie.get()->Find(s)->m_pNodeData)->GetHelp();
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline const std::string& GetHelp( std::string s ) {
        return CVarUtils::GetHelp( s.c_str() );
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline std::string GetValueAsString( void* cvar ) {
        return ((CVar<int>*) cvar)->GetValueAsString();
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline void SetValueFromString( void* cvar, const std::string &sValue ) {
        ((CVar<int>*) cvar)->SetValueFromString( sValue );
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline void PrintCVars(
            const char* sRowBeginTag = "",
            const char* sRowEndTag = "",
            const char* sCellBeginTag = "",
            const char* sCellEndTag = ""
            )
    {
        TrieNode* node = g_pCVarTrie->FindSubStr("");
        if( !node ) {
            return;
        }
        std::cout << "CVars:" << std::endl;
        // Retrieve suggestions (retrieve all leaves by traversing from current node)
        std::vector<TrieNode*> suggest = g_pCVarTrie->CollectAllNodes( node );
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
//                sName.resize( nLongestName, ' ' );
//                sVal.resize( nLongestVal, ' ' );
//                printf( "%-s: Default value = %-30s   %-50s\n", sName.c_str(), sVal.c_str(), sHelp.empty() ? "" : sHelp.c_str() );
                printf( "%s%s%-s%s%s  %-30s %s%s  %-50s%s%s\n",
                        sRowBeginTag,
                        sCellBeginTag, sName.c_str(), sCellEndTag,
                        sCellBeginTag, sVal.c_str(), sCellEndTag,
                        sCellBeginTag, sHelp.empty() ? "" : sHelp.c_str(), sCellEndTag,
                        sRowEndTag );
                printf( "%s", sRowEndTag );
            }
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline void SetStreamType( const CVARS_STREAM_TYPE& stream_type )
    {
        g_pCVarTrie->SetStreamType( stream_type );
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline bool Save( const std::string& sFileName, std::vector<std::string> vAcceptedSubstrings ) {
        std::ofstream sOut( sFileName.c_str() );
        if( sOut.is_open() ) {
            g_pCVarTrie->SetVerbose( false );
            g_pCVarTrie->SetAcceptedSubstrings ( vAcceptedSubstrings );
            sOut << *g_pCVarTrie;
            sOut.close();
            return true;
        }
        else {
            //            std::cerr << "ERROR opening cvars file for saving." << std::endl;
            return false;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline bool Load( const std::string& sFileName, std::vector<std::string> vAcceptedSubstrings ) {
        std::ifstream sIn( sFileName.c_str() );
        if( sIn.is_open() ) {
            g_pCVarTrie->SetVerbose( false );
            g_pCVarTrie->SetAcceptedSubstrings ( vAcceptedSubstrings );
            sIn >> *g_pCVarTrie;
            sIn.close();
            return true;
        }
        else {
            //            std::cerr << "ERROR opening cvars file for loading." << std::endl;
            return false;
        }
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline std::string CVarSpc() {
        if( CVarUtils::GetCVar<int>( "console.CVarIndent" ) <= 0 ) {
            return "";
        }
        return std::string( CVarUtils::GetCVar<int>( "console.CVarIndent" ), ' ' );
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline void CVarIndent() {
        CVarUtils::GetCVarRef<int>( "console.CVarIndent" ) += CVarUtils::GetCVar<int>( "console.CVarIndentIncr" );
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline void CVarUnIndent() {
        CVarUtils::GetCVarRef<int>( "console.CVarIndent" ) -= CVarUtils::GetCVar<int>( "console.CVarIndentIncr" );
    }

    ////////////////////////////////////////////////////////////////////////////////
    inline void CVarResetSpc() {
        CVarUtils::SetCVar<int>( "console.CVarIndent", 0 );
    }

    ////////////////////////////////////////////////////////////////////////////////



    ////////////////////////////////////////////////////////////////////////////////
    /// This function parses console input for us -- useful function for glconsole, textconsole, etc
    bool ProcessCommand(
            const std::string& sCommand,   //< Input:
            std::string& sResult,          //< Output:
            bool bExecute = 1              //< Input:
            );

    ////////////////////////////////////////////////////////////////////////////////
    bool ExecuteFunction(
            const std::string& sCommand,            //< Input:
            CVarUtils::CVar<ConsoleFunc> *cvar,     //< Input:
            std::string& sResult,                   //< Output:
            bool bExecute = 1                       //< Input:
            );

    ////////////////////////////////////////////////////////////////////////////////
    bool IsConsoleFunc(
            TrieNode *node  //< Input:
            );

    ////////////////////////////////////////////////////////////////////////////////
    bool IsConsoleFunc(
            const std::string sCmd      //< Input
            );

    ////////////////////////////////////////////////////////////////////////////////
    /// If 'sCommand' is
    /// - part of an existing unique variable, it will be completed and 'sCommand'
    ///   will be the complete variable, sResult will be empty
    /// - part of multiple possible variables, it will be completed up to the common
    ///   factor and possible solutions will be put in sResult (TO IMPROVE: differentiate type of
    ///   results: command, variable, ...)
    /// - a full variable: 'sCommand' will be completed with '= variable_value' and
    ///   sResult will also contain the variable name and its value
    bool TabComplete( const unsigned int nMaxNumCharactersPerLine, ///< Input: use for formatting the sResult when it returns the list of possible completions
                      std::string& sCommand,
                      std::vector<std::string>& vResult );


}

#endif
