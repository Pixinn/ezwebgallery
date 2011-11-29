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
 
 #include <QString>
 #include <QDir> 
 
 #include "IPhotoFeeder.h"
 
#ifndef CPHOTOFEEDERDIRECTORY_H
#define CPHOTOFEEDERDIRECTORY_H
 
/*****************************************
 * CPhotoFeederDirectory
 * ----------------------
 * Class feeding the application with
 * photos contained in a single directory
 *****************************************/
 class CPhotoFeederDirectory : public IPhotoFeeder
 {
 public:
    //constructor and destructor
    CPhotoFeederDirectory( void ) :
        IPhotoFeeder(),
        m_directory("")
    {   }
    CPhotoFeederDirectory( const QString& directoryPath ) :
        IPhotoFeeder(),
        m_directory( directoryPath )
    {   }
    ~CPhotoFeederDirectory( void ) { }
    //public functions
    void refresh( void );
    bool isValid( void ) { return m_directory.exists(); }
    //accessors
    bool setDirectory( const QString& directoryPath );
    bool setDirectory( const QDir& directory );
    QStringList getPhotoList( void );
    QDir& getDirectory( void ){ return m_directory; }
    QString getDirectoryPath( void ){ return m_directory.absolutePath(); }


private:
    //static members
    
    //members
    QDir m_directory;
    
 };
 
#endif

