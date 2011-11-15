/* 
 *  EZWebGallery:
 *  Copyright (C) 2011 Christophe Meneboeuf <dev@ezwebgallery.org>
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

IPhotoFeeder::IPhotoFeeder(){}

//-----------------------
// setDirectory( QString directoryPath )
// ----------------
// Set the directory path.
// In:      (QString)directoryPath
// Returns: (bool) true if the directory exists, false otherwise
//----------------------
 bool CPhotoFeederDirectory::setDirectory( QString directoryPath )
{
    m_directory.setPath( directoryPath );
    return m_directory.exists();
}

 //-----------------------
// setDirectory( QString directoryPath )
// ----------------
// Returns the photos contained in the directory
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
