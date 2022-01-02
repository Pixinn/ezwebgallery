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
// Add the specified file to the loading queue: FILO or FIFO depending on the priority
// Async loading
//----------------------
void CThumbnailLoadingManager::load( const QString & filePath, e_Priority priority )
{
    //Updating the file queue
    if( !m_loadingQueue.contains( filePath ) ) {
        if( priority == HIGH ) {
            m_loadingQueue.prepend( filePath );
        }
        else {
             m_loadingQueue.append( filePath );
        }
        //qDebug() << filePath << ", not in queue.";
    }
    else //Already in the queue, put in front
    {
        if( m_currentlyLoading != filePath ) //m_currentlyLoading had been extracted from the queue
        {
            if( m_loadingQueue.at(0) != filePath ) {
                //qDebug() << m_loadingQueue.at(0) << ", removing from position " << QString::number(m_loadingQueue.indexOf( filePath ));
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
// stopLoading( const QString & filePath )
// ----------------
// Stop loading the specified thumbnail
// NOTE: if already in the loading thread, it will be loaded then emitted
// anyway.
//----------------------
void CThumbnailLoadingManager::stopLoading( const QString & filePath )
{
    m_loadingQueue.removeOne( filePath );
}


//-----------------------
// flush( )
// ----------------
// Flushing (reinit)
// NOTE: if the loading thread is running, it will finish and the
// loaded thumbnail will be emitted
//----------------------
void CThumbnailLoadingManager::flush( )
{
    m_loadingQueue.clear();
    m_currentlyLoading.clear(); 
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
        //qDebug() << "Queue empty.";
        m_currentlyLoading.clear();
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
    else
    {
        //TO BE DONE
    }

}
