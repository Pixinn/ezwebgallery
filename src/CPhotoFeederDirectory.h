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
   
    static CPhotoFeederDirectory& getInstance( void ) { return s_instance; }
    CPhotoFeederDirectory& operator=( const CPhotoFeederDirectory& source)  {
        m_directory = source.m_directory;
        return *this;
    }
    bool operator==( const CPhotoFeederDirectory& other ) { return m_directory == other.m_directory; }
    bool operator!=( const CPhotoFeederDirectory& other ) { return m_directory != other.m_directory; }
    //public functions
    void clear( void ) { m_directory = QDir(); }
    bool isValid( void ) { return m_directory.exists(); }
    //accessors
    bool setDirectory( const QString& directoryPath );
    bool setDirectory( const QDir& directory );
    QStringList getPhotoList( void );
    QDir& getDirectory( void ){ return m_directory; }
    QString getDirectoryPath( void ){ return m_directory.absolutePath(); }

 private:
    //constructor and destructor
    CPhotoFeederDirectory( void ) :
        IPhotoFeeder(),
        m_directory( QDir::homePath() )
    {   }
    CPhotoFeederDirectory( const QString& directoryPath ) :
        IPhotoFeeder(),
        m_directory( directoryPath )
    {   }
    ~CPhotoFeederDirectory( void ) { }

private:
    //members
    QDir m_directory;
    static CPhotoFeederDirectory s_instance;
    
 };
 
#endif

