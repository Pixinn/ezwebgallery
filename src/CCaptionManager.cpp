/* 
 *  EZWebGallery:
 *  Copyright (C) 2011 Christophe Meneboeuf@ezwebgallery.org>
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



#include <QModelIndex>
#include <QStringListModel>
#include <QStringList>
#include <QMutableMapIterator>
#include <QDebug>

#include "CCaptionManager.h"

/******************** PUBLIC *********************/
CCaptionManager::CCaptionManager( ) :
    QObject( ),
    m_photoDb( CPhotoDatabase::getInstance() )
{
    //m_photoIndex = 0;
    m_f_captionsEdited = false;

    connect( &m_photoDb, SIGNAL(layoutChanged()), this, SLOT(onListUpdated()) );
}

//-----------------------
// CCaptionManager(const CCaptionManager & toCopy)
// ----------------
// Surcharge de l'assignation.
// Attention :  + on perd toutes les connections SIGNAUX/SLOTS dûes à QObject !!
//              + La classe ne sera pas connextée à une QListeView => utiliser setPhotoList( QListView* ) !!!
//----------------------
CCaptionManager::CCaptionManager( const CCaptionManager & toCopy ) :
    QObject( ), 
    m_photoDb( CPhotoDatabase::getInstance() ),
    m_photoSelected( toCopy.m_photoSelected )
{
    //m_photoIndex = 0;
    m_f_captionsEdited = toCopy.m_f_captionsEdited;
    
    connect( &m_photoDb, SIGNAL(layoutChanged()), this, SLOT(onListUpdated()) );
}

//-----------------------
// operator=(const CCaptionManager & toCopy)
// ----------------
// Surcharge de l'assignation.
// Attention :  + on perd toutes les connections SIGNAUX/SLOTS dûes à QObject !!
//              + La classe ne sera pas connextée à une QListeView => utiliser setPhotoList( QListView* ) !!!
//----------------------
CCaptionManager CCaptionManager::operator=(const CCaptionManager & toCopy)
{
    //this->m_photoIndex = 0;
    m_photoSelected = toCopy.m_photoSelected;
    this->m_f_captionsEdited = toCopy.m_f_captionsEdited;

    return *this;
}

//-----------------------
// reset( )
// ----------------
// Reset (reinit) the manager
//----------------------
void CCaptionManager::reset( )
{
    //m_photoIndex = 0;
    m_photoSelected.setIndex( 0 );
    display( m_photoSelected.index() );
    //duplicate the first header and ending to all the captions
    if( m_photoDb.size() > 0 )
    {
        CCaption ref = ( m_photoDb.properties(0) )->caption();
        for( int i = 1; i < m_photoDb.size(); i++ )
        {
            CPhotoProperties* properties = m_photoDb.properties( i );
            CCaption caption = properties->caption();
            caption.setEnding( ref.ending() );
            caption.setHeader( ref.header() );
            properties->setCaption( caption );
        }
    }
}



//-----------------------
// photo( )
// ----------------
// Retourne le nom de la photo affichée actuellement
//----------------------
QString CCaptionManager::selectedPhoto( )
{
    return m_photoSelected.name();
}


//-----------------------
// setExifTags
// ----------------
// Indique les <tags Exifs / metadata> à utiliser pour la légende de la photo spécifiée.
//----------------------
void CCaptionManager::setExifTags( const QString &photoName, const QMap<QString,QString> &exifTags )
{    
    CPhotoProperties* properties =  m_photoDb.properties( photoName );
    properties->setExifTags( exifTags );

}

//-----------------------
// setFileInfo
// ----------------
//Indique les infos fichier à utiliser par la légende de la photo spécifiée
//----------------------
void CCaptionManager::setFileInfo( const QString & photoName, const QFileInfo & fileInfo )
{
    CPhotoProperties* properties =  m_photoDb.properties( photoName );
    CCaption caption = properties->caption();
    caption.setFileInfo( fileInfo );
    properties->setCaption( caption );
}


/******************* PRIVATE **********************/

//-----------------------
// display( int )
// ----------------
// Demande l'affichage de la légende et de la vignette correspondante
//----------------------
void CCaptionManager::display( int nb )
{   
    if( m_photoDb.size() != 0 ) //Fails if db is empty
    {
        const QStringListModel& model = m_photoDb.model();
        QModelIndex indexPhotoSelected = model.index( nb );

        //Affichage vignette
        emit displayThumbnailSignal( indexPhotoSelected );  //Affichage vignette à réaliser en premier
                                                        //Les données exifs lues à ce moment permettront la preview correcte de la légende
        //Affichage légende
        CCaption caption = m_photoDb.properties( nb )->caption();
        emit displayCaptionSignal( caption.body() );
        emit displayPreviewSignal( caption.render( CTaggedString::PREVIEW ) );
        emit displayHighlightIndex( indexPhotoSelected );
    }
}



//-----------------------
// captionsEdited( )
// ----------------
// Retourne un booléen indiquant si les légendes ont été modifiées
// depuis le dernier appel de captionsEditedReset()
//----------------------
bool CCaptionManager::captionsEdited()
{
    return m_f_captionsEdited;
}


//-----------------------
// captionsEditedReset( )
// ----------------
// Remet l'indicateur de modification des légendes sur false
// Utilisé vec remapLCaptionist(), permet de savoir si les légendes ont
// été modifiées.
//----------------------
void CCaptionManager::captionsEditedReset()
{
    m_f_captionsEdited = false;
}


//-----------------------
// previous( QModelIndex& )
// ----------------
// Previous photo requested by the ui
//----------------------
void CCaptionManager::onPrevious(  )
{
    if( m_photoSelected.index() > 0 )  {
       //m_photoIndex--;
        m_photoSelected--;
        display( m_photoSelected.index() );
    }
    //m_photoDisplayed = m_photoDb.filename( m_photoIndex );
}

//-----------------------
// next( QModelIndex& )
// ----------------
// Next photo requested by the ui
// Out: (QModelIndex&) QModelIndex of the next photo
// Returns: true if operation succedeed, false otherwise 
//----------------------
void CCaptionManager::onNext(  )
{
    const QStringListModel& model = m_photoDb.model();

    if( m_photoSelected.index() < model.stringList().size() - 1 )    {
        //m_photoIndex++;
        m_photoSelected++;
        display( m_photoSelected.index() );
    }
    //m_photoDisplayed = m_photoDb.filename( m_photoIndex );
}



/******************* SLOTS ************************/



void CCaptionManager::onListPressed( QModelIndex indexPhotoSelected )
{    
    int newPhotoIndex = indexPhotoSelected.row();
    if( m_photoSelected.index() != newPhotoIndex )   {
      //m_photoIndex = newPhotoIndex;
      //m_photoDisplayed = m_photoDb.filename( m_photoIndex );
        m_photoSelected.setIndex( newPhotoIndex );
        display( m_photoSelected.index() );
    }
}


//-----------------------
// onListUpdated( )
// ----------------
// The photolist was updated 
//----------------------
void CCaptionManager::onListUpdated( )
{
    qDebug() << "CaptionManager: list updated.";
    int newPhotoIndex = m_photoDb.id( m_photoSelected.name() );
    if( newPhotoIndex != m_photoSelected.index() ) {
        if( newPhotoIndex == -1 ) { //Photo is no longer present in the db
            //m_photoIndex = 0;
            m_photoSelected.setIndex(0);
        }
        else {
            //m_photoIndex = newPhotoIndex;
            m_photoSelected.setIndex( newPhotoIndex );
        }
        display( m_photoSelected.index() );
    }    
}


void CCaptionManager::onCaptionTextEdited( QString text )
{
    CPhotoProperties* properties = m_photoDb.properties( m_photoSelected.index() );
    CCaption caption = properties->caption();
    caption.setBody( text );
    properties->setCaption( caption );
    m_f_captionsEdited = true;

    emit displayPreviewSignal( caption.render( CTaggedString::PREVIEW ) );
}


void CCaptionManager::onCaptionHeaderEdited( QString text )
{
    for( int i = 0; i < m_photoDb.size(); i++ ) {
        CPhotoProperties* properties = m_photoDb.properties( i );
        CCaption caption = properties->caption();
        caption.setHeader( text );
        properties->setCaption( caption );
    }
    m_f_captionsEdited = true;
   
    //Affichage de la preview
    CPhotoProperties* properties = m_photoDb.properties( m_photoSelected.index() );
    CCaption caption = properties->caption();
    emit displayPreviewSignal( caption.render( CTaggedString::PREVIEW ) );
}

void CCaptionManager::onCaptionEndingEdited( QString text )
{
    for( int i = 0; i < m_photoDb.size(); i++ ) {
        CPhotoProperties* properties = m_photoDb.properties( i );
        CCaption caption = properties->caption();
        caption.setEnding( text );
        properties->setCaption( caption );
    }
    m_f_captionsEdited = true;

    //Affichage de la preview    
    CPhotoProperties* properties = m_photoDb.properties( m_photoSelected.index() );
    CCaption caption = properties->caption();
    emit displayPreviewSignal( caption.render( CTaggedString::PREVIEW ) );
}
