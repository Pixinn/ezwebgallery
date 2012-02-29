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


#ifndef CTHUMBNAILLOADER_H
#define CTHUMBNAILLOADER_H

#include <QObject>
#include <QMetaType>
#include <QSize>
#include <QQueue>
#include <QMutex>
#include <QThread>
#include <QImage>
#include <QMap>

/******************************* DEFINES *********************************/
#define THUMBNAIL_WIDTH     420
#define THUMBNAIL_HEIGHT    360


/******************************** CLASSES ********************************/


/*****************************************************************
 * CLoadedThumbnail
 * ----------------------
 * Description of a loaded thumbnail, to be passed via a signal
 ****************************************************************/
class CLoadedThumbnail : public QObject
{
    Q_OBJECT

public:
    CLoadedThumbnail( void ) :
        QObject()
    {   }
        
    CLoadedThumbnail( const QString & filePath, const QImage & image,  const QMap<QString,QString> & tags ) :
        QObject(),
        m_filePath( filePath ),
        m_image( image ),
        m_exifTags( tags )
    {   }

     CLoadedThumbnail( const CLoadedThumbnail & other ) :
        QObject(),
        m_filePath( other.m_filePath ),
        m_image( other.m_image ),
        m_exifTags( other.m_exifTags )
    {   }

     inline const QString& getFilePath( void ) const { return m_filePath; }
     inline const QImage& getImage( void ) const { return m_image; }
     inline const QMap<QString,QString>& getExifTags( void ) const { return m_exifTags; }

private:
    const QString m_filePath;
    const QImage m_image;
    const QMap<QString,QString> m_exifTags;
};


/*****************************************************************
 * CThumbnailLoader
 * ----------------------
 * Thumbnail loader working in a separate thread
 ****************************************************************/
class CThumbnailLoader : public QThread
{
    Q_OBJECT

public:
    CThumbnailLoader( const QSize& thumbSize, QMutex* mutex, QObject* parent = 0 ) :
        QThread( parent ),
        m_ptrMutex( mutex ),
        m_thumbSize( thumbSize )
    {   }
    ~CThumbnailLoader( void )
    {   }

    void setJob( const QString & filePath );
    void run( void );

signals:
    void jobDone( CLoadedThumbnail );


private:
    QString m_filePath;
    QMutex* m_ptrMutex;
    const QSize m_thumbSize;

};


/*****************************************************************
 * CThumbnailLoadingManager
 * ----------------------
 * Managing thumbnails loading
 ****************************************************************/
class CThumbnailLoadingManager : public QObject
{
    Q_OBJECT

public:
    typedef enum {
         NORMAL,
         HIGH
     }e_Priority;

    explicit CThumbnailLoadingManager( QObject *parent = 0 ) :
        QObject( parent ),
        m_loader( QSize(THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT), &m_mutex )
    {   }  
    ~CThumbnailLoadingManager( void )
    {   }
    void init( void )   //As a member of a static instance, connections cannot be performed in constructor
    {
        qRegisterMetaType<CLoadedThumbnail> ( "CLoadedThumbnail" );
        connect( &m_loader, SIGNAL( jobDone( CLoadedThumbnail ) ), this, SLOT(thumbLoaded( CLoadedThumbnail ) ) );
        connect( &m_loader, SIGNAL( finished() ), this, SLOT( loadNext() ) );
    }

    void load( const QString& filePath, e_Priority priority = NORMAL );
    void stopLoading( const QString& filePath );
    void flush( void );

public slots:
    void thumbLoaded( CLoadedThumbnail );
    void loadNext( void );

signals:
    void loaded( const CLoadedThumbnail & );

private:
    CThumbnailLoader m_loader;
    QQueue<QString> m_loadingQueue;
    QMutex m_mutex;
    QString m_currentlyLoading;   
};

#endif // CTHUMBNAILLOADER_H
