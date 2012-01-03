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
#include <QStringListModel>

#include "CPhotoPropertiesExtended.h"
#include "CCaption.h"
#include "CMessage.h"



/***************************************************************************
 * CPhotoDatabase
 * ----------------------
 * Database including the properties of all the photos to process
 * Pattern singleton
 * It is built on an internal QMap<filename,properties>, the db itself,
 * and a QStringListModel linked to the ui which orders the properties.
 * These two elements must be kept in sync.
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
            CPhotoDatabaseElem( const CPhotoPropertiesExtended & other ) :
                CPhotoPropertiesExtended( other )
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
    
 private:
        //Private constructor
        CPhotoDatabase( void ) :
            QObject(),
            m_thumbnailsSize( QSize(320,200) )
        {    }
        ~CPhotoDatabase( )  {
            clear();
        }
 
 public:
        static CPhotoDatabase& getInstance( void ) { return s_instance; }
    
        QStringList checkPhotosInDb( void ); //Returns a list of the photos present in the DB but not on the disk
        QStringList photosModified( void ) const; //List of the added or modified files since the last call of the constructors, buil or photosModified
        
        QStringList appendPhotoList( const QStringList & ); //Add new properties to the db. Return a list of invalid files.
        QStringList appendPhotoList( const CPhotoPropertiesExtended & ); //Add new properties to the db. Return a list of invalid files.

        QStringList refresh( const QStringList & ); //Update the database using this new list of files.

        void remove( int );
        void remove( const QString &);
        
        void swap( int id1, int id2 );
        void swap( const QString & name1, const QString & name2 );
       
        void setThumbnailsSize( const QSize &size ) { m_thumbnailsSize = size; }
        bool loadThumbnail( int );  // ** For the times being thumbnails are loaded synchronously via an external request.
        bool loadThumbnail( const QString& ); // ** In the future: a background process could load them asynconously
        const QImage& thumbnail( int ) const; //Returns a thumbnail
        const QImage& thumbnail( const QString & ) const; //Returns a thumbnail

        int size( void ) const { return m_db.size(); }
        CPhotoProperties* properties( int );
        CPhotoProperties* properties( const QString & );
        QString filename( int id ) const { return  m_model.data( m_model.index( id ), Qt::DisplayRole ).toString(); }
        
        const QStringListModel& model( void ) const { return m_model; } //returns a reference to a QModel representation of the filenames

    signals:
        void warning( CMessage );   //a warning occured
        void error( CMessage );   //an error occured
        void message( CMessage );   //send an informative message

    public slots:
        QStringList build( const QDomElement & ); //from Xml. Returns the list of invalid files
        QStringList build( const QStringList & ); //from a list of absolute file paths. Returns the list of invalid files
        QStringList importDeprecated( const QDomElement &, const QString & ); //from a deprecated Xml. Returns the list of invalid files
        QDomElement xml( QDomDocument& document ) const; //Constructs an Xml representation of the in the provided document
        //link with the model, which layout is directly updated by the ui
        void modelLayoutChanged( void );

     
    private: //nb private memebers do not emit updatedProperties()
        void clear( void );
        bool add( const QString& photoPath ); //adds a photo photoproperties at the end of the ordered list
        bool add( const CPhotoPropertiesExtended& ); //adds a photo photoproperties at the end of the ordered list
        void consolidate( void ); //Removes files present in the db but not on the disk
        
    private:
         static const QString XMLTAG_PHOTOSDB;
         static const QString XMLTAG_PHOTOS;
         static const QString XMLTAG_LASTMODIFICATIONTIME;
         static const QString XMLTAG_FILEPATH;
         static const QString XMLTAG_CAPTIONBODY;
         static const QString XMLTAG_CAPTIONHEADER;
         static const QString XMLTAG_CAPTIONENDING;
         static const QString XMLTAG_DEPRECATED_FILENAME;
         const static QImage s_defaultThumbnail; //Thumbnail for a not yet loaded photo
         QSize m_thumbnailsSize;
         //Static instance for singleton
         static CPhotoDatabase s_instance;
          //CORE DB:
         //The two containers that must be in sync form the DB.
         //Allowing access by filename and number.
         QMap<QString,CPhotoDatabaseElem*> m_db; //key : FILENAME
         //QStringList m_orderedKeys;//element: FILENAME. Allows an ordered search of elements in m_db;
         QStringListModel m_model; //model syncing the db and the ui
 };
 
#endif
