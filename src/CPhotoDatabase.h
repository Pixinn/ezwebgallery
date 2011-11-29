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



#ifndef CPHOTODATABASE_H
#define CPHOTODATABASE_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QImage>
#include <QSize>
#include <QDomElement>

#include "CPhotoPropertiesExtended.h"
#include "CCaption.h"
#include "CError.h"



/***************************************************************************
 * CPhotoDatabase
 * ----------------------
* Database including the properties of all the photos to process
 **************************************************************************/
 class CPhotoDatabase : public QObject
 {
     Q_OBJECT   
    
 private:
    //Private type of element to store in the DB
    class CPhotoDatabaseElem : public CPhotoPropertiesExtended
    {
        public:
            CPhotoDatabaseElem( void ) : 
                CPhotoPropertiesExtended( )
            {     }
            CPhotoDatabaseElem( const CPhotoDatabaseElem & other ) :
                CPhotoPropertiesExtended( other ),
                m_thumbnail( other.m_thumbnail )
            {     }               
            ~CPhotoDatabaseElem( void ){  }
            CPhotoDatabaseElem & operator=( const CPhotoDatabaseElem &  source)
            {
                m_thumbnail = source.m_thumbnail;
                return *this;
            }            
        public:
            QImage m_thumbnail;
    };//class end
    
 public:
        CPhotoDatabase( void ) :
            QObject(),
            m_thumbnailsSize( QSize(320,200) )
        {    }
        CPhotoDatabase( const CPhotoDatabase &  other) :
            QObject(),
            m_db( other.m_db ),
            m_orderedKeys( other.m_orderedKeys )
        {   }
        ~CPhotoDatabase( ){    }
        
        QDomElement toXml( void ); //serialize
        QStringList& build( const QDomElement & ); //from Xml. Returns the list of invalid files
        QStringList& build( const QStringList & ); //from a list of absolute file paths. Returns the list of invalid files
        
        inline QStringList checkPhotosInDb( void ); //Returns a list of the photos present in the DB but not on the disk
        QStringList photosModified( void ) const; //List of the added or modified files since the last call of the constructors, buil or photosModified
        QStringList setPhotoList( const QStringList & ); //Add new pictures to the db. Signal all photos present in the db but not in the list.
                
        void remove( int );
        void remove( const QString &);
        
        void swap( int id1, int id2 );
        void swap( const QString & name1, const QString & name2 );
       
        void setThumbnailsSize( const QSize &size ) { m_thumbnailsSize = size; }
        bool loadThumbnail( int );  // ** For the times being thumbnails are loaded synchronously via an external request.
        bool loadThumbnail( const QString& ); // ** In the future: a background process could load them asynconously
        const QImage& thumbnail( int ) const; //Returns a thumbnail
        const QImage& thumbnail( const QString & ) const; //Returns a thumbnail
        
        QList<CPhotoProperties> properties( void ) const; //Returns an ordered list of base properties

    signals:
        void error( CError );
        void somePhotosAreMissing( QStringList ); //signals photos present on the database but not on the disk

    public slots:
        void feed( QStringList photoList ) { 
            clear();
            setPhotoList(photoList);
        }
     
    private:
        void clear( void );
        void add( const QString& photoPath ); //adds a photo at the end of the ordered list
        
    private:
         //The two containers that must be in sync form the DB.
         //Allowing access by filename and number.
         QMap<QString,CPhotoDatabaseElem*> m_db; //key : filename
         QStringList m_orderedKeys;//element: filename. Allows an ordered search of elements in m_db;
         const static QImage s_defaultThumbnail; //Thumbnail for a not yet loaded photo
         QSize m_thumbnailsSize;
 };
 
#endif
