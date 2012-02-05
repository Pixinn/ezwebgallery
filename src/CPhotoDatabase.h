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

#include "CPhotoProperties.h"
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
            // ---- PRIVATE CLASSES ---- //

     //Private type of element to store in the DB
    class CPhotoDatabaseElem : public CPhotoProperties
    {
        public:
            CPhotoDatabaseElem( void ) : 
                CPhotoProperties( )  {     }

            CPhotoDatabaseElem( const CPhotoDatabaseElem & other ) :
                CPhotoProperties( other ),
                m_thumbnail( other.m_thumbnail )    {     }

            CPhotoDatabaseElem( const CPhotoProperties & other ) :
                CPhotoProperties( other )   {     }

            ~CPhotoDatabaseElem( void ) {       }

            CPhotoDatabaseElem & operator=( const CPhotoDatabaseElem &  source)     {
                m_thumbnail = source.m_thumbnail;
                return *this;
            }            
        public:
            QImage m_thumbnail;
    };//class end

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
            m_thumbnailsSize( QSize(320,200) ),
            f_initialized( false )
        {
           /* connect( &m_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), \
                     this, SLOT( modelLayoutChanged(QModelIndex,QModelIndex) )     );*/
        }
        ~CPhotoDatabase( )  {
            clear();
        }
 
 public:
        static CPhotoDatabase& getInstance( void ) { return s_instance; }
        void init( void )
        {
            if( !f_initialized ) {
                f_initialized = true;
                connect(  &m_model, SIGNAL(rowsRemoved (  QModelIndex , int,  int )), \
                          this, SLOT(rowRemoved ( QModelIndex , int,  int ) ) );
            }
        }
    
        QStringList checkPhotosInDb( void ); //Returns a list of the photos present in the DB but not on the disk
        QStringList photosModified( void ) const; //List of the removed or modified files
        
        QStringList appendPhotoList( const QStringList & ); //Add new properties to the db. Return a list of invalid files.
        //QStringList appendPhotoList( const CPhotoProperties & ); //Add new properties to the db. Return a list of invalid files.

        QStringList refresh( const QStringList & ); //Updates the database using this new list of files.
        bool refreshFileInfo( const QString & ); //Updates the file info of the element

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
        int id( const QString & filename) const;
        
        const QStringListModel& model( void ) const { return m_model; } //returns a reference to a QModel representation of the filenames

    signals:
        void warning( CMessage );   //a warning occured
        void error( CMessage );   //an error occured
        void message( CMessage );   //send an informative message
        void layoutChanged( ); //The layout of the db changed

    public slots:
        QStringList build( const QDomElement & ); //from Xml. Returns the list of invalid files
        QStringList build( const QStringList & ); //from a list of absolute file paths. Returns the list of invalid files
        QStringList importDeprecated( const QDomElement &, const QString & ); //from a deprecated Xml. Returns the list of invalid files
        QDomElement xml( QDomDocument& document ) const; //Constructs an Xml representation of the in the provided document
        //link with the model, which layout is directly updated by the ui
        void rowRemoved(const QModelIndex & parent, int start, int end );
     
    private: //nb private memebers do not emit updatedProperties()
        void clear( void );
        bool add( const QString& photoPath ); //adds a photo photoproperties at the end of the ordered list
        bool add( const CPhotoProperties& ); //adds a photo photoproperties at the end of the ordered list
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
         const static QImage s_defaultThumbnail; //Thumbnail for a not yet loaded photo
         //Static instance for singleton
         static CPhotoDatabase s_instance;
          //CORE DB:
         //The two containers that must be in sync form the DB.
         //Allowing access by filename and number.
         QMap<QString,CPhotoDatabaseElem*> m_db; //key : FILENAME
         CPhotoDatabaseModel m_model; //model syncing the db and the ui
 };
 
#endif
