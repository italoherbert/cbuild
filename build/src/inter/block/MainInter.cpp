
#include "MainInter.h"
#include "it/StringIterator.h"
#include "../inter.h"
#include "../interexcept.h"

#include <fstream>
#include <sstream>
#include <iostream>

MainInter::MainInter( MainInterDriver* drv ) {
    this->drv = drv;
}

InterResult* MainInter::interpreta( BlockIterator* blockIt, int lineNumber ) {
    vector<string> validCMDs = drv->validMainCMDNames();

    string line;
    int numberOfLines = 0;
    while( blockIt->hasNextLine() ) {
        string line = blockIt->nextLine();

        if ( line.length() == 0 ) {
            numberOfLines++;
            continue;
        }
        if ( line[ 0 ] == ' ' || line[ 0 ] == '\n' || line[ 0 ] == '\t' || line[ 0 ] == '\r' || line[ 0 ] == '#' ) {
            numberOfLines++;
            continue;
        }

        bool isCmd = false;

        size_t i = line.find( ' ' );
        if ( i != string::npos ) {
            string cmd = line.substr( 0, i );
            int len = validCMDs.size();
            for( int k = 0; !isCmd && k < len; k++ ) {
                if ( cmd == validCMDs[ k ] ) {
                    line = inter::replaceVars( line, lineNumber, this );
                    isCmd = true;
                }
            }
        }

        if ( isCmd ) {
            CMDInter* cmd = new CMDInter();
            InterResult* result = cmd->interpreta( line, lineNumber + numberOfLines );
            numberOfLines += result->getNumberOfLines();
            if ( result->isOk() ) {
                cmdIntersVect.push_back( cmd );
            } else {
                return new InterResult( numberOfLines, result->getErrorMsg() );
            }
        } else {
            i = line.find( '=' );
            if ( i == string::npos ) {
                stringstream ss;
                ss << "Erro na linha: " << ( lineNumber + numberOfLines ) << ";\nPropriedade em formato inv�lido: \"" << line << "\"";
                return new InterResult( numberOfLines, ss.str() );
            }

            string name = line.substr( 0, i );
            string value = line.substr( i+1, line.length()-i );

            value = inter::replaceVars( value, lineNumber, this );

            propertiesMap[ name ] = value;
        }

        numberOfLines++;
    }

    return new InterResult( numberOfLines );
}

int MainInter::getPropertiesLength() {
    return propertiesMap.size();
}

string MainInter::getPropertyValue( string name ) {
    return propertiesMap[ name ];
}

bool MainInter::existsProperty( string name ) {
    return propertiesMap.find( name ) != propertiesMap.end();
}

vector<string> MainInter::propertyNames()  {
    vector<string> props;
    for( const auto& pair : propertiesMap )
        props.push_back( pair.first );
    return props;
}

int MainInter::getCMDIntersLength() {
    return cmdIntersVect.size();
}

CMDInter* MainInter::getCMDInter( string name ) {
    for( CMDInter* inter : cmdIntersVect )
        if ( inter->getName() == name )
            return inter;
    return nullptr;
}

CMDInter* MainInter::getCMDInterByIndex( int i ) {
    int k = 0;
    for( CMDInter* inter : cmdIntersVect ) {
        if ( k == i )
            return inter;
        k++;
    }
    return nullptr;
}

vector<CMDInter*> MainInter::cmdInters() {
    return cmdIntersVect;
}
