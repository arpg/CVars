#ifndef _CVAR_MAP_IO_H_
#define _CVAR_MAP_IO_H_

#include <map>
#include <sstream>
#include <string>
#include <iostream>
#include <tinyxml2.h>

namespace CVarUtils {
    // All types you wish to use with CVars must overload << and >>
    // This is a possible overloading for maps
    template<class K, class D>
        std::ostream &operator<<( std::ostream &stream, std::map<K,D>& mMap ) {

        stream << std::endl;
        stream << CVarUtils::CVarSpc() << "<map>\n";
        for( typename std::map<K,D>::iterator it = mMap.begin(); it != mMap.end(); ++it ) {
            CVarUtils::CVarIndent();
            stream << CVarUtils::CVarSpc() << "<Object>\n";
            CVarUtils::CVarIndent();
            stream << CVarUtils::CVarSpc() << "<Key>  " << it->first << "  </Key>" << std::endl;
            stream << CVarUtils::CVarSpc() << "<Data>  ";
            CVarUtils::CVarIndent();
            stream << it->second;
            CVarUtils::CVarUnIndent();
            stream << CVarUtils::CVarSpc() << "</Data>" << std::endl;
            CVarUtils::CVarUnIndent();
            stream << CVarUtils::CVarSpc() << "</Object>" << std::endl;
            CVarUtils::CVarUnIndent();
        }      
        stream << CVarUtils::CVarSpc() << "</map>" << std::endl;
        return stream;
    }

    template<class K, class D>
        std::istream &operator>>( std::istream &stream, std::map<K,D>& mMap ) {

        tinyxml2::XMLDocument doc;
        std::string s;
        stream >> s;
        doc.Parse(s.c_str());
        //doc.Print( stdout );
        tinyxml2::XMLNode* pCVarsNode = doc.RootElement();

        if( pCVarsNode == NULL ) {
            std::cerr <<  "ERROR: Could not find a <map> node." << std::endl;
            return stream;
        }

        for( tinyxml2::XMLNode* pNode = pCVarsNode->FirstChild();
             pNode != NULL;
             pNode = pNode->NextSibling() ) {

            std::string sObject( pNode->Value() );

            if( sObject != "Object" ) {
                std::cerr << "WARNING: got '" << sObject << "' value when expecting 'Object'." << std::endl;
                continue;
            }

            K KKey;
            D DData;

            ////////////////////////////////////////////////////////
            tinyxml2::XMLNode* pChild = pNode->FirstChild();
            ////////////////////////////////////////////////////////
            if( pChild == NULL ) {
                std::cerr << "ERROR parsing map, could not find first child (Key) of Object." << std::endl;
                return stream;
            }
            else {
                if( std::string( pChild->Value() ) != "Key" ) {
                    std::cerr << "ERROR parsing key in map, expecting 'Key' node, got "<< pChild->Value() << "." << std::endl;
                    return stream;
                }

                tinyxml2::XMLNode* pKeyChild = pChild->FirstChild();

                if( pKeyChild == NULL ) {
                    std::cerr << "ERROR parsing key value in map (empty Key ?).\n" << std::endl;
                    return stream;
                }           
                std::stringstream iKeyString(pKeyChild->ToText()->Value());
                iKeyString >> KKey;
                //std::cout << "Key value: " << pChild->FirstChild()->Value() << std::endl;
            }

            ////////////////////////////////////////////////////////
            pChild = pChild->NextSibling();
            ////////////////////////////////////////////////////////
            if( pChild == NULL ) {
                std::cerr << "ERROR parsing map, could not find 2nd child (Data) of Object." << std::endl;
                return stream;
            }
            else {
                if( std::string( pChild->Value() ) != "Data" ) {
                    std::cerr << "ERROR parsing key in map, expecting 'Data' node, got "<< pChild->Value() << "." << std::endl;
                    return stream;
                }

                tinyxml2::XMLNode* pDataChild = pChild->FirstChild();

                if( pDataChild == NULL ) {
                    std::cerr << "ERROR parsing Data value in map (empty Data ?).\n" << std::endl;
                    return stream;
                }
                std::stringstream iDataString(pDataChild->ToText()->Value());
                iDataString >> DData;         
                //std::cout << "Data value: " << pChild->FirstChild()->Value() << std::endl;
            }
            ////////////////////////////////////////////////////////
            //std::cout << "Found Key: " << KKey << " with data: " << DData << std::endl;

            mMap[ KKey ] = DData;
        }
        return stream;
    }
}

#endif
