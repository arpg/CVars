#ifndef _CVAR_VECTOR_IO_H_
#define _CVAR_VECTOR_IO_H_

#include <vector>
#include <sstream>

#define VECTOR_NAME_MAX 1000

namespace CVarUtils {
    // All types you wish to use with CVars must overload << and >>
    // This is a possible overloading for vectors
    template<class T>
        std::ostream &operator<<(std::ostream &stream, std::vector<T>& vT ) {
        if( vT.size() == 0 ) {
            stream << "[ ]";
            return stream;
        }

        stream << "[ " << vT[0]; 
        for( size_t i=1; i<vT.size(); i++ ) {
            stream << " " << vT[i];
        }
        stream << " ]";
        
        return stream;
    }


    template<class T>
        std::istream &operator>>(std::istream &stream, std::vector<T>& vT ) {

        std::string sBuf; // Have a buffer string
  
        vT.clear();
        
        // Only works for splitting with whitespaces
        while( stream >> sBuf ) {
            if( sBuf.find( "[" ) == std::string::npos &&
                sBuf.find( "]" ) == std::string::npos ) {
                
                T TVal;
                std::stringstream( sBuf ) >> TVal;
                std::stringstream sCheck;
                sCheck << TVal;
                if( sBuf != sCheck.str() ) {
                    printf( "ERROR deserialising vector, ignoring \"%s\" value.\n" , 
                            sBuf.c_str() );
                    continue;
                }

                vT.push_back( TVal );
            }
        }
        
        return stream;
    }
}

#endif
