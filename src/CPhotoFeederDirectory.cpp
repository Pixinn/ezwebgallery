/* 
 *  EZWebGallery:
 *  Copyright (C) 2011 Christophe Meneboeuf <christophe@xtof.info>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
 
#include "IPhotoFeeder.h"
#include "CPhotoFeederDirectory.h"
#include "CError.h"


CPhotoFeederDirectory CPhotoFeederDirectory::s_instance( QDir::homePath() );


//-----------------------
// setDirectory( QString directoryPath )
// ----------------
// Set the directory path.
// In:      (QString)directoryPath
// Returns: (bool) true if the directory exists, false otherwise
//----------------------
 bool CPhotoFeederDirectory::setDirectory( const QString& directoryPath )
{
    if( QDir( directoryPath ).exists() ) {
        m_directory.setPath( directoryPath );
//        emit update( getPhotoList() );
    }
    else {
        emit error( PtrMessage (new CError( CError::error( CError::DirectoryCreation ), directoryPath)) );
    }

    return m_directory.exists();
}


 //-----------------------
// setDirectory( const QDir& directory )
// ----------------
// Set the directory path.
// In:      (const QDir&)directoryPath
// Returns: (bool) true if the directory exists, false otherwise
//----------------------
 bool CPhotoFeederDirectory::setDirectory( const QDir& directory )
{
    if( directory.exists() ) {
        m_directory = directory;
        emit update( getPhotoList() );
    }
    else {
        emit error( PtrMessage( new CError(CError::error( CError::DirectoryCreation ), directory.absolutePath() )) );
    }

    return m_directory.exists();
}

//-----------------------
// getPhotoList( void )
// ----------------
// Returns the photos contained in the directory
// Return: (QStringList) absolute file paths
//----------------------
QStringList CPhotoFeederDirectory::getPhotoList( void )
{
    QStringList supportedFileTypes;
    QStringList shortNameList;
    QStringList fullPathList;
        
    supportedFileTypes << "*.jpeg" << "*.jpg" << "*.tiff" << "*.tif"; //supported file types filter    
    shortNameList = m_directory.entryList( supportedFileTypes );
    
    foreach( QString shortName, shortNameList ) {
        fullPathList << m_directory.absoluteFilePath( shortName );
    }
    
    return fullPathList;
}

