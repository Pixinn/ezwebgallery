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



#ifndef CPHOTODATABASE_H
#define CPHOTODATABASE_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QImage>
#include <QSize>
#include <QDomElement>
#include <QStringListModel>

#include "CPhotoProperties.h"
#include "CCaption.h"
#include "CMessage.h"
#include "CThumbnailLoader.h"


/***************************** FUNCTIONS *****************************/


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
            // ---- PRIVATE CLASSES ---- //

     //Private type of element to store in the DB
    class CPhotoDatabaseElem : public CPhotoProperties
    {
    public:


        class CDefaultThumbnail : public QImage
        {
        public:
            CDefaultThumbnail( QSize size );
            void init( void );
            static CDefaultThumbnail& getInstance( void ){ return s_instance; }

        private:
            static CDefaultThumbnail s_instance;
            const static int s_fontSize; //font size for text replacing not loaded thumb
        };//End class


        CPhotoDatabaseElem( void ) :
            CPhotoProperties( ),
            m_thumbnail( CDefaultThumbnail::getInstance() )
        {   }

        CPhotoDatabaseElem( const CPhotoProperties & other ) :
            CPhotoProperties( other ),
            m_thumbnail( CDefaultThumbnail::getInstance() )
        {   }

        ~CPhotoDatabaseElem( void ) {       }

        CPhotoDatabaseElem & operator=( const CPhotoDatabaseElem &  source)     {
            m_thumbnail = source.m_thumbnail;
            return *this;
         }

    public:
            QImage m_thumbnail;
    };//class CPhotoDatabaseElem end


    //Model ordering the elements and linked to the UI (see Model/View pattern)
    class CPhotoDatabaseModel : public QStringListModel
    {
    public:
        CPhotoDatabaseModel( void ) : QStringListModel( ) {     }

        Qt::ItemFlags flags ( const QModelIndex & index ) const {
            if( index.isValid() ) {
                return ( QStringListModel::flags(index) & ~Qt::ItemIsDropEnabled & ~Qt::ItemIsEditable );
            }
            else {
                return ( Qt::ItemIsDropEnabled );
            }
        }

    };

    
 private:
        //Private constructor
        CPhotoDatabase( void ) :
            QObject(),
            m_thumbnailsSize( QSize(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT) ),
            f_initialized( false )
        {    }
        ~CPhotoDatabase( )  {
            clear();
            delete s_defaultElem;
        }
 
 public:
        static CPhotoDatabase& getInstance( void ) { return s_instance; }
        void init( void );

        inline bool contains( const QString& filename ) { return m_db.contains( filename ); }
    
        QStringList checkPhotosInDb( void ); //Returns a list of the photos present in the DB but not on the disk
        QStringList photosModified( void ) const; //List of the removed or modified files
        
        QStringList appendPhotoList( const QStringList & ); //Add new properties to the db. Return a list of invalid files.

        QStringList refresh( const QStringList & ); //Updates the database using this new list of files.
        bool refreshFileInfo( const QString & ); //Updates the file info of the element

        void remove( int );
        void remove( const QString &);
        void remove( const QStringList & );
        
        void swap( int id1, int id2 );
        void swap( const QString & name1, const QString & name2 );

        void saveState( void ) {
            m_savedState = m_model.stringList();
        }
        bool hasChanged( void ) {
            return !( m_savedState == m_model.stringList() );
        }
       
        void setThumbnailsSize( const QSize &size ) { m_thumbnailsSize = size; }
        bool loadThumbnail( int, CThumbnailLoadingManager::e_Priority = CThumbnailLoadingManager::NORMAL );  // ** For the times being thumbnails are loaded synchronously via an external request.
        bool loadThumbnail( const QString&, CThumbnailLoadingManager::e_Priority = CThumbnailLoadingManager::NORMAL ); // ** In the future: a background process could load them asynconously
        const QImage& thumbnail( int ) const; //Returns a thumbnail
        QImage& thumbnail( const QString & ); //Returns a thumbnail

        int size( void ) const { return m_db.size(); }
        CPhotoProperties* properties( int );
        CPhotoProperties* properties( const QString & );
        QString filename( int id ) const { return  m_model.data( m_model.index( id ), Qt::DisplayRole ).toString(); }
        int id( const QString & filename) const;
        
        const QStringListModel& model( void ) const { return m_model; } //returns a reference to a QModel representation of the filenames

    signals:
        void warning( PtrMessage );   //a warning occured
        void error( PtrMessage );   //an error occured
        void message( PtrMessage );   //send an informative message
        void layoutChanged( ); //The layout of the db changed
        void thumbnailLoaded( int ); //A thumbnail has been loaded and added to the db

    public slots:
        QStringList build( const QDomElement & ); //from Xml. Returns the list of invalid files
        QStringList build( const QStringList & ); //from a list of absolute file paths. Returns the list of invalid files
        QStringList importDeprecated( const QDomElement &, const QString & ); //from a deprecated Xml. Returns the list of invalid files
        QDomElement xml( QDomDocument& document ) const; //Constructs an Xml representation of the in the provided document
        //link with the model, which layout is directly updated by the ui
        void rowRemoved(const QModelIndex & parent, int start, int end );
        void thumbnailLoaded( const CLoadedThumbnail & );
     
    private: //nb private memebers do not emit updatedProperties()
        void clear( void );
        bool add( const QString& photoPath ); //adds a an element at the end of the ordered list
        bool add( const CPhotoProperties& ); //adds an element at the end of the ordered list
        void consolidate( void ); //Removes files present in the db but not on the disk
        
    private:
         QSize m_thumbnailsSize;
         bool f_initialized;
         static const QString XMLTAG_PHOTOSDB;
         static const QString XMLTAG_PHOTOS;
         static const QString XMLTAG_LASTMODIFICATIONTIME;
         static const QString XMLTAG_FILEPATH;
         static const QString XMLTAG_CAPTIONBODY;
         static const QString XMLTAG_CAPTIONHEADER;
         static const QString XMLTAG_CAPTIONENDING;
         static const QString XMLTAG_DEPRECATED_FILENAME;
         
         static CPhotoDatabase s_instance;  //Static instance for singleton
         static CPhotoDatabaseElem* s_defaultElem; //Default element, returned when the queried elem was not found

          //CORE DB:
         //The two containers that must be in sync form the DB.
         //Allowing access by filename and number.
         QMap<QString,CPhotoDatabaseElem*> m_db; //key : FILENAME
         CPhotoDatabaseModel m_model; //model syncing the db and the ui
         //Saved state to test if the db content or order has changed
         QStringList m_savedState;

         //Thumbnail loader
         CThumbnailLoadingManager m_loader;
 };
 
#endif
