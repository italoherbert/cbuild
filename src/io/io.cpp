
#include "io.h"
#include "../util/strutil.h"

#include "../error_messages.h"

#include <fstream>
#include <sstream>
#include <filesystem>

#include <sys/stat.h>

using std::stringstream;
using std::ifstream;
using std::ofstream;

namespace filesystem = std::filesystem;

io_error::io_error( string msg ) : runtime_error( msg ) {}
joker_error::joker_error( string msg ) : io_error( msg ) {}

namespace io {

    ByNameFileFilter* by_name_file_filter( string fileName ) {
        return new ByNameFileFilter( fileName );
    }

    void writeInTextFile( string file, string text ) {
        ofstream ofs( file );

        ofs << text;
        ofs.close();
    }

    string readFromTextFile( string file ) {
        ifstream ifs( file );

        stringstream ss;
        if ( ifs.is_open() ) {
            string line;
            while( getline( ifs, line, '\n' ) )
                ss << line << "\n";
        }
        
        return ss.str();
    }

    int countFilesAndDirs( string dir ) {
        try {
            string dirpath = path::makePreferred( dir );
            if ( !fileExists( dirpath ) )
                throw io_error( errors::io::DIR_NOT_FOUND );
            if ( !isDir( dirpath ) )
                throw io_error( errors::io::IS_NOT_A_DIR );

            int count = 0;

            for( const auto& entry : filesystem::recursive_directory_iterator( dirpath ) )                
                count++;                                    
            return count;
        } catch ( const filesystem::filesystem_error& e ) {
            throw io_error( e.what() );
        }
    }

    int recursiveDeleteDir( string path ) {
        try {
            string p = path::makePreferred( path );
            if ( filesystem::exists( p ) ) {
                uintmax_t removedCount = filesystem::remove_all( p );
                return removedCount;
            }
            return 0;
        } catch ( const filesystem::filesystem_error& e ) {
            throw io_error( e.what() );
        }
    }

    int deleteFileOrDir( string path, bool isRecursive ) {
        try {
            string p = path::makePreferred( path );
            
            if ( !fileExists( p ) ) 
                throw io_error( errors::io::FILE_OR_FOLDER_NOT_FOUND );

            if ( isDir( p ) ) {
                if ( isRecursive ) {
                    return recursiveDeleteDir( p );
                } else {
                    if ( hasNoEmptyDir( p, by_name_file_filter( "*" ), false ) )
                        throw io_error( errors::io::FOLDER_WITH_NOT_EMPTY_FOLDER );

                    bool removed = filesystem::remove_all( p );
                    return ( removed ? 1 : 0 );                    
                }
            } else {                
                bool removed = filesystem::remove( p );
                return ( removed ? 1 : 0 );
            }
        } catch ( const filesystem::filesystem_error& e ) {
            throw io_error( e.what() );
        }
    }

    bool hasNoEmptyDir( string path, FileFilter* filter, bool isRecursive ) {
        string preferredDir = path::makePreferred( path );

        if ( !fileExists( preferredDir ) )
            throw io_error( errors::io::DIR_NOT_FOUND );
        if ( !isDir( preferredDir ) )
            throw io_error( errors::io::IS_NOT_A_DIR );

        if ( isRecursive ) {
            for( const auto& entry : filesystem::recursive_directory_iterator( preferredDir ) ) {
                string file = entry.path().string();

                if ( filter != nullptr )
                    if ( !filter->match( file ) )
                        continue;

                if ( filesystem::is_directory( file ) )
                    if ( !isEmptyDir( file ) )
                        return true;                                       
            }
        } else {
            for( const auto& entry : filesystem::directory_iterator( preferredDir ) ) {
                string file = entry.path().string();

                if ( filter != nullptr )
                    if ( !filter->match( file ) )
                        continue;

                if ( filesystem::is_directory( file ) )
                    if ( !isEmptyDir( file ) )                    
                        return true;              
            }
        }

        return false;
    }

    int deleteFiles( string dir, FileFilter* filter, bool isRecursive ) {
        int removedCount = 0;
        try {
            string preferredDir = path::makePreferred( dir );

            if ( !fileExists( preferredDir ) )
                throw io_error( errors::io::DIR_NOT_FOUND );
            if ( !isDir( preferredDir ) )
                throw io_error( errors::io::IS_NOT_A_DIR );

            if ( isRecursive ) {
                for( const auto& entry : filesystem::directory_iterator( preferredDir ) ) {
                    string file = path::makePreferred( entry.path().string() );

                    if ( filesystem::is_directory( file ) )
                        removedCount += deleteFiles( file, filter, true );
                        
                    if ( filter != nullptr )
                        if ( !filter->match( file ) )
                            continue;

                    removedCount += deleteFileOrDir( file, true );                                                    
                }
            } else {
                for( const auto& entry : filesystem::directory_iterator( preferredDir ) ) {
                    string file = path::makePreferred( entry.path().string() );

                    if ( filter != nullptr )
                        if ( !filter->match( file ) )
                            continue;
                    
                    removedCount += deleteFileOrDir( file, isRecursive );
                }       
            }     
        } catch ( const filesystem::filesystem_error& e ) {
            throw io_error( e.what() );
        }
        return removedCount;
    }     

    void copyFile( string srcFile, string targetPath, bool isOverwriteExisting ) {
        try {
            string preferredSrc = path::makePreferred( srcFile );
            string preferredTarget = path::makePreferred( targetPath );

            if ( !fileExists( preferredSrc ) )
                throw io_error( errors::io::SRC_DIR_NOT_FOUND );
            
            string unixTarget = path::makeUnixPreferred( targetPath );
            string unixSrc = path::makeUnixPreferred( srcFile );

            string srcFName = path::fileOrDirName( unixSrc );
            string targetFName = path::fileOrDirName( unixTarget );

            if ( srcFName != targetFName && isDir( preferredTarget ) ) {
                unixTarget = path::addSeparatorIfNeed( unixTarget );
                unixTarget += srcFName;
            }

            preferredSrc = path::makePreferred( unixSrc );
            preferredTarget = path::makePreferred( unixTarget );

            if ( isOverwriteExisting && filesystem::exists( preferredTarget ) )
                filesystem::remove( preferredTarget );

            filesystem::copy_file( preferredSrc, preferredTarget );
        } catch ( const filesystem::filesystem_error& e ) {
            throw io_error( e.what() );
        }
    }

    void __copyFile( string file, string target, string replacePath, bool isOverwriteExisting ) {
        string unixFile = path::makeUnixPreferred( file );
        string unixReplacePath = path::makeUnixPreferred( replacePath );

        string fname = strutil::replace( unixFile, unixReplacePath, "" );

        string unixTarget = path::makeUnixPreferred( target );
        unixTarget = path::addSeparatorIfNeed( unixTarget );
        unixTarget = unixTarget + fname;

        string preferredDir = path::dirPath( unixTarget );
        createDirs( preferredDir );

        unixTarget = path::makePreferred( unixTarget );

        string preferredFile = path::makePreferred( file );
        string preferredTarget = path::makePreferred( unixTarget );

        if ( isOverwriteExisting && filesystem::exists( preferredTarget ) )
            filesystem::remove( preferredTarget );

        filesystem::copy_file( preferredFile, preferredTarget );
    }

    void copyDir( string srcDir, string targetDir, bool isOverwriteExisting, bool isRecursive ) {
        try {
            string preferredSrcDir = path::makePreferred( srcDir );
            string preferredTargetDir = path::makePreferred( targetDir );            
            
            if ( !fileExists( preferredSrcDir ) )
                throw io_error( errors::io::SRC_DIR_NOT_FOUND ); 
            if ( !isDir( preferredSrcDir ) )
                throw io_error( errors::io::SRC_IS_NOT_A_DIR );
            if ( fileExists( preferredTargetDir ) && !isDir( preferredTargetDir ) )
                throw io_error( errors::io::TARGET_IS_NOT_A_DIR );

            string fsrcName = path::fileOrDirName( srcDir );

            string forReplace = path::makeUnixPreferred( srcDir );
            forReplace = path::addSeparatorIfNeed( forReplace );
            
            bool srcExists = fileExists( preferredSrcDir );
            bool targetExists = fileExists( preferredTargetDir );
          
            if ( srcExists ) {                
                if ( targetExists ) {
                    string unixSrcDir = path::makeUnixPreferred( srcDir );
                    string unixTargetDir = path::makeUnixPreferred( targetDir );

                    string srcFolder = path::fileOrDirName( unixSrcDir );

                    unixTargetDir = path::addSeparatorIfNeed( unixTargetDir );
                    unixTargetDir += srcFolder;

                    preferredTargetDir = path::makePreferred( unixTargetDir );

                    createDir( preferredTargetDir );
                } else {
                    string unixTargetDir = path::makeUnixPreferred( targetDir );
                    string parentDir = path::parentPath( unixTargetDir );
                    bool parentExists = io::fileExists( parentDir );
                    if ( parentExists )
                        createDir( preferredTargetDir );
                }
            }

            if ( isRecursive ) {
                for( const auto& entry : filesystem::recursive_directory_iterator( preferredSrcDir ) ) {
                    string preferredFile = entry.path().string();
                    if ( filesystem::is_directory( preferredFile ) )
                        continue;
                        
                    __copyFile( preferredFile, preferredTargetDir, forReplace, isOverwriteExisting );
                }
            } else {
                for( const auto& entry : filesystem::directory_iterator( preferredSrcDir ) ) {
                    string preferredFile = entry.path().string();
                    if ( filesystem::is_directory( preferredFile ) )
                        continue;
                        
                    __copyFile( preferredFile, preferredTargetDir, forReplace, isOverwriteExisting );                         
                }
            }
        } catch ( const filesystem::filesystem_error& e ) {
            throw io_error( e.what() );
        }
    }

    void copyFileOrDir( string srcPath, string targetDir, bool isOverwriteExisting, bool isRecursive ) {
        if ( isDir( srcPath ) ) {
            copyDir( srcPath, targetDir, isOverwriteExisting, isRecursive );
        } else {
            copyFile( srcPath, targetDir, isOverwriteExisting );
        }
    }
    
    void copyFiles( 
                string srcDir, 
                string targetDir, 
                FileFilter* filter, 
                bool isOverwriteExisting, 
                bool isRecursive ) {
    
        string forReplacePath = path::makeUnixPreferred( srcDir );
        forReplacePath = path::addSeparatorIfNeed( forReplacePath );

        copyFiles( srcDir, targetDir, forReplacePath, filter, isOverwriteExisting, isRecursive );
    }

    void copyFiles( 
                string srcDir, 
                string targetDir, 
                string forReplacePath, 
                FileFilter* filter, 
                bool isOverwriteExisting,
                bool isRecursive ) {
        try {
            string src = path::makePreferred( srcDir );
            string target = path::makePreferred( targetDir );

            if ( !fileExists( src ) )
                throw io_error( errors::io::SRC_DIR_NOT_FOUND );
            if ( !fileExists( target ) )
                throw io_error( errors::io::TARGET_DIR_NOT_FOUND );

            if ( isRecursive ) {
                for( const auto& entry : filesystem::recursive_directory_iterator( src ) ) {
                    string file = path::makePreferred( entry.path().string() );

                    bool isCopyFile = true;
                    if ( filter != nullptr )
                        isCopyFile = filter->match( file );

                    if ( isCopyFile && !filesystem::is_directory( file ) )
                        __copyFile( file, target, forReplacePath, isOverwriteExisting );
                }
            } else {
                for( const auto& entry : filesystem::directory_iterator( src ) ) {
                    string file = path::makePreferred( entry.path().string() );

                    bool isCopyFile = true;
                    if ( filter != nullptr )
                        isCopyFile = filter->match( file );

                    if ( isCopyFile && !filesystem::is_directory( file ) )
                        __copyFile( file, target, forReplacePath, isOverwriteExisting );
                }
            }
        } catch ( const filesystem::filesystem_error& e ) {
            throw io_error( e.what() );
        }
    }

    bool createDir( string path ) {
        string p = path::makePreferred( path );
        try {
            return filesystem::create_directory( p );
        } catch ( const filesystem::filesystem_error& e ) {
            throw io_error( e.what() );
        }
    }

    bool createDirs( string path ) {
        string p = path::makePreferred( path );
        try {
            return filesystem::create_directories( p );
        } catch ( const filesystem::filesystem_error& e ) {
            throw io_error( e.what() );
        }
    }

    bool fileExists( string path ) {
        return filesystem::exists( path );
    }

    bool isDir( string path ) {
        return filesystem::is_directory( path );
    }

    bool isFile( string path ) {
        return filesystem::is_regular_file( path );
    }

    bool isEmptyDir( string dir ) {
        return filesystem::is_empty( dir );
    }

    long lastWriteTime( string path ) {
        struct stat fileStat;
        int result = stat( path.c_str(), &fileStat );
        if ( result != 0 )
            return -1;

        return fileStat.st_mtime;
    }

namespace path {

    string dirPath( string path ) {
        string p = makeUnixPreferred( path );
        p = removeSeparatorFromDirIfNeed( p );

        if ( filesystem::is_directory( path ) )
            return p;
        
        size_t i = p.find_last_of( '/' );
        size_t j = p.find( ':' );
        if ( i == string::npos && j == string::npos )
            return p;

        if ( i == string::npos && j != string::npos ) {
            if ( p.length() > j+1 )
                return p.substr( 0, j+1 );            
            return "";           
        }
            
        if ( i == 0 )
            return "/";        
        return p.substr( 0, i );
    }

    string jokerJokerDirPath( string path ) {
        string p = makeUnixPreferred( path );
        p = removeJokerJoker( p );
        return dirPath( p );
    }

    string fileOrDirName( string path ) {
        string p = makeUnixPreferred( path );

        size_t i = p.find_last_of( '/' );
        if ( i == string::npos )
            return p;

        if ( p.length() > 0 ) {
            if ( p[ p.length()-1 ] == '/' ) {
                size_t j = p.find_last_of( '/', i-1 );
                size_t k = ( j == string::npos ? 0 : j+1 );
                return p.substr( k, p.length()-k-1 );
            }
        }

        return p.substr( i+1, p.length()-i );
    }

    string fileOrDirNameWithoutExtension( string pathOrName ) {
        string name = makeUnixPreferred( pathOrName );
        name = fileOrDirName( name );

        size_t i = name.find( '.' );
        if ( i == string::npos )
            return name;
        return name.substr( 0, i );
    }

    char preferredSeparator() {
        return filesystem::path::preferred_separator;
    }

    string currentPath() {
        filesystem::path currPathFS = filesystem::current_path();
        return makeUnixPreferred( currPathFS.string() );
    }

    string tempDirPath() {
        filesystem::path tempDirPath = filesystem::temp_directory_path();
        return makeUnixPreferred( tempDirPath.string() );
    }

    string makePreferred( string path ) {
        filesystem::path p( path );
        p = p.make_preferred();
        return p.string();
    }

    string makeUnixPreferred( string path ) {
        return strutil::replaceAll( path, '\\', '/' );
    }

    string addSeparatorIfNeed( string dir ) {
        string d = makeUnixPreferred( dir );
        if ( d.length() > 0 )
            if( d[ d.length()-1 ] != '/' )
                d += "/";
        return d;
    }

    string removeSeparatorFromDirIfNeed( string dir ) {
        string d = makeUnixPreferred( dir );

        int len = d.length();
        if ( len > 0 ) {
            if( d[ len-1 ] == '/' ) {
                if ( len == 1 )
                    return "/";
                return d = d.substr( 0, d.length()-1 );
            }
        }
        return d;
    }

    string parentPath( string path ) {
        string p = makeUnixPreferred( path );
        p = removeSeparatorFromDirIfNeed( p );
        if ( p.length() > 0 ) {
            size_t i = p.find_last_of( '/' );
            if ( i != string::npos ) {                
                if ( i == 0 ) {
                    if ( p.length() == 1 )
                        p = "";
                    else p = "/";
                } else {
                    p = p.substr( 0, i );
                }                
            } else {
                p = "";                
            }
        }
        return p;
    }

    string absolutePath( string path ) {
        if ( path == "." )
            return makeUnixPreferred( currentPath() );

        string path2 = makeUnixPreferred( path );
        if ( strutil::startsWith( path2, "./" ) )
            path2 = path2.substr( 2, path2.length()-2 );
           
        if ( strutil::startsWith( path2, "/" ) )
            return path2;

        size_t i = path2.find( ':' );
        if ( i != string::npos )
            return path2;    

        filesystem::path p( path2 );
        if ( p.is_absolute() ) {
            return path2;
        } else {
            string pp = currentPath();
            pp = makeUnixPreferred( pp );

            if ( path2 != "" )
                pp = addSeparatorIfNeed( pp );
                
            pp += path2;
            return pp;
        }
    }

    string relativePath( string path ) {
        if ( path == "." )
            return "";

        string path2 = makeUnixPreferred( path );
        if ( strutil::startsWith( path2, "./" ) )
            path2 = path2.substr( 2, path2.length()-2 );

        filesystem::path p( path2 );
        if ( p.is_absolute() ) {
            path2 = filesystem::relative( path2, currentPath() ).string();
            return makeUnixPreferred( path2 ); 
        } else {
            return path2;
        }
    }

    string extension( string path ) {
        string name = fileOrDirName( path );
        size_t i = name.find( '.' );
        if ( i == string::npos )
            return "";
        return name.substr( i+1, name.length()-i );
    }

    /*
    Retorna o path antes do **
    */
    string jokerJokerBeforePath( string path ) {
        string p = makeUnixPreferred( path );
        size_t i = p.find( "**" );
        if ( i == string::npos ) {
            return "";
        } else {
            if ( i == 0 ) {
                p = "";
            } else if ( i == 1 ) {
                p = "/";
            } else {
                p = p.substr( 0, i );
            }
            return p;
        }
    }

    /*
    Retorna o path após o **.
    */
    string jokerJokerAfterPath( string path ) {
        string p = makeUnixPreferred( path );

        size_t i = p.find( "**" );
        if ( i == string::npos ) {
            return fileOrDirName( p );
        } else {
            i += 2;
            if ( i < p.length() )
                i++;
            return p.substr( i, p.length()-i+1 );
        }
    }

    string removeJokerJoker( string path ) {
        string p = makeUnixPreferred( path );
        if ( p.find( "**/" ) == string::npos )
            return strutil::replace( p, "**", "" );
        return strutil::replace( p, "**/", "" );
    }

    bool isFileNameStartsWithJoker( string path ) {
        string file = fileOrDirName( path );
        if ( file.length() > 1 ) {
            if ( file[ 0 ] == '*' && file[ 1 ] != '*' )
                return true;
            return false;            
        }

        if ( file.length() > 0 )
            if ( file[ 0 ] == '*' )
                return true;

        return false;
    }

    void countTwoDotsAndSlash( string path, int index, size_t& count, size_t& i, size_t& j, size_t& k ) {
        string rpath = path.substr( index, path.length()-index );
        rpath = makeUnixPreferred( rpath );

        size_t len = rpath.length();

        if ( rpath == ".." ) {
            k = 2;
            i = index;
            j = index+2;
            count = 1;
        } else {
            i = rpath.find( "../", index );
            j = ( i != string::npos ? i : index );
            k = ( i != string::npos ? i : index );
            count = 0;
            bool isContinue = i != string::npos;
            while( isContinue ) {
                count++;
                j = k+3;
                k += 3;
                if ( k+2 < len ) {
                    isContinue = rpath[ k ] == '.';
                    isContinue &= rpath[ k+1 ] == '.';
                    isContinue &= rpath[ k+2 ] == '/';
                } else {
                    isContinue = false;
                }
            }
        }
    }

    string removeInitTwoDotsAndSlashes( string relativePath ) {
        if ( relativePath == "." )
            return "";

        string rpath = makeUnixPreferred( relativePath );
        if ( strutil::startsWith( rpath, "./" ) )
            rpath = strutil::replace( rpath, "./", "" );
        
        size_t count, i, j, k;    
        countTwoDotsAndSlash( rpath, 0, count, i, j, k );
        return rpath.substr( k, rpath.length()-k );
    }

    string absoluteResolvePath( string path ) {
        return resolvePath( absolutePath( path ) );
    }

    string resolvePath( string path ) {
        if ( path == "." )
            return "";

        string rpath = makeUnixPreferred( path );
        if ( strutil::startsWith( rpath, "./" ) )
            rpath = strutil::replace( rpath, "./", "" );

        size_t count, i, j, k;
        countTwoDotsAndSlash( rpath, 0, count, i, j, k );

        if( count == 0 )
            return rpath;
        
        string basePath = rpath.substr( 0, i );
        string relativePath = rpath.substr( i, rpath.length()-i ).replace( 0, j-i, "" );

        for( size_t l = 0; l < count; l++ )
            basePath = parentPath( basePath );        
        basePath = addSeparatorIfNeed( basePath );
        
        return basePath + relativePath;
    }

    string resolvePath( string currDir, string path ) {
        string resolvedPath = makeUnixPreferred( path );
        
        if ( strutil::startsWith( resolvedPath, "./" ) )
            resolvedPath = strutil::replace( resolvedPath, "./", "" );

        size_t i = resolvedPath.find( "../" );
        bool isRepeat = i != string::npos;
        string dir = addSeparatorIfNeed( currDir );
        while( isRepeat ) {
            dir = parentPath( dir );
            dir = addSeparatorIfNeed( dir );

            size_t j = resolvedPath.find( "../", i+3 );
            if ( j == string::npos ) {
                isRepeat = false;
            } else {
                i = j;
            }
        }

        size_t k = ( i == string::npos ? 0 : i+3 );
        return resolvedPath.replace( 0, k, dir );
    }

}

}
