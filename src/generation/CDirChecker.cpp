#include "CDirChecker.h"

/************************************************
* areFilesInDir( const QStringList& photos )
* returns true if the provided files are in the dir
* or all the contained subdir
*************************************************/
bool CDirChecker::areFilesInDir( const QStringList& photos )
{    
    QStringList files = m_dir.entryList( QDir::Files );
    foreach( QString photo, photos ) {
        if( !files.contains( photo ) ) {
            return false;
        }
    }
    return true;
}


/************************************************
* areFilesInSubdirs( const QStringList& photos )
* returns true if the provided files are in the dir
* or all the contained subdir
*************************************************/
bool CDirChecker::areFilesInSubdir( const QStringList& photos, const QStringList& subdirs )
{
    foreach( QString dir, subdirs ) {
        CDirChecker subdir( m_dir.absoluteFilePath( dir ) );
        if( !subdir.areFilesInDir( photos ) ) {
            return false;
        }
    }
    return true;
}