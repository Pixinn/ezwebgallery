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

#include <QDebug>

#include "CPhoto.h"
#include "CThumbnailLoader.h"



/******************************* FUNCTIONS ***************************************/


/*****************************************************************
 *
 * CLASS CThumbnailLoadingManager
 *
 ****************************************************************/


//-----------------------
// load( const QString & filePath )
// ----------------
// Add the specified file to the loading queue and
// load it when possible
//----------------------
void CThumbnailLoadingManager::load( const QString & filePath )
{
    //Updating the file queue
    if( !m_loadingQueue.contains( filePath ) ) {
        m_loadingQueue.enqueue( filePath );
        qDebug() << filePath << ", not in queue.";
    }
    else //Already in the queue, put in front
    {
        if( m_currentlyLoading != filePath ) //m_currentlyLoading had been extracted from the queue
        {
            if( m_loadingQueue.at(0) != filePath ) {
                qDebug() << m_loadingQueue.at(0) << ", removing from position " << QString::number(m_loadingQueue.indexOf( filePath ));
                m_loadingQueue.removeAt( m_loadingQueue.indexOf( filePath ) ); //removing current occurence
                m_loadingQueue.prepend( filePath ); //put in front                
            }
        }
    }

    //-- Load next thumbnail
    m_mutex.lock();
    bool f_isThreadRunning = m_loader.isRunning();
    m_mutex.unlock();
    //If thread is not running: ask to load next
    if( !f_isThreadRunning ) { //If it's running, next thumb will be loaded after it finishes
        loadNext();
    }  
}


//-----------------------
// thumbLoaded( const CLoadedThumbnail thumb )
// ----------------
// SIGNAL: a thumbnail has been loaded
//----------------------
void CThumbnailLoadingManager::thumbLoaded( CLoadedThumbnail thumb )
{
    emit loaded( thumb );
}


//-----------------------
// loadNext( )
// ----------------
// SIGNAL: load next thumbnail
//----------------------
void CThumbnailLoadingManager::loadNext( )
{
    if( !m_loadingQueue.isEmpty() ) {
        m_currentlyLoading = m_loadingQueue.dequeue();
        m_loader.setJob( m_currentlyLoading );
        m_loader.start();
    }
    else {
        qDebug() << "Queue empty.";
    }
}



/*****************************************************************
 *
 * CLASS CThumbnailLoader
 *
 ****************************************************************/

void CThumbnailLoader::setJob( const QString & filePath )
{
    m_ptrMutex->lock();
        m_filePath = filePath;
    m_ptrMutex->unlock();        
}



//-----------------------
// run( )
// ----------------
// Load a thumbnail in another thread
//----------------------
void CThumbnailLoader::run( void )
{
    qDebug() << "Loading Thread started.";
 
    m_ptrMutex->lock();        
        QString filePath = m_filePath;
    m_ptrMutex->unlock();
    const QSize thumbSize = m_thumbSize;

    CPhoto photo;    
    
    if( photo.load( m_filePath ) ) //loading photo
    {
        photo.zoom( thumbSize, Qt::FastTransformation ); //resize
        photo.orientationExif(); //correct orientation
        CLoadedThumbnail loadedThumb( filePath, photo.qimage(), photo.exifTags() );

        emit jobDone( loadedThumb );
    }
 /*   else
    {
        //Loging messages if an error occured
        if( !photo.errors().isEmpty() )
        {
            foreach( QString err, photo.errors() ){
                CMessage msg( QString("[Thumbnail]." ) + err );
                emit error( msg );
            }
            f_success = false;
        }
    }*/

    qDebug() << "Loading Thread finished.";
}
