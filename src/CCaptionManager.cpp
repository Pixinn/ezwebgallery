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

#include "CCaptionManager.h"

/******************** PUBLIC *********************/
CCaptionManager::CCaptionManager( ) :
    QObject( ),
    m_photoDb( CPhotoDatabase::getInstance() )
{
    m_photoIndex = 0;
    //m_p_listView = NULL;
    m_f_captionsEdited = false;
}

//-----------------------
// operator=(const CCaptionManager & toCopy)
// ----------------
// Surcharge de l'assignation.
// Attention :  + on perd toutes les connections SIGNAUX/SLOTS dûes à QObject !!
//              + La classe ne sera pas connextée à une QListeView => utiliser setPhotoList( QListView* ) !!!
//----------------------
CCaptionManager::CCaptionManager( const CCaptionManager & toCopy ) :
    QObject( ), 
    m_photoDb( CPhotoDatabase::getInstance() )
{
    //this->m_p_listView = NULL;
   // this->m_captionMap = toCopy.m_captionMap;
    this->m_photoIndex = 0;
    this->m_f_captionsEdited = toCopy.m_f_captionsEdited;
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
    //this->m_p_listView = NULL;
  //  this->m_captionMap = toCopy.m_captionMap;
    this->m_photoIndex = 0;
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
    m_photoIndex = 0;
    remapCaptionList( );
    display( m_photoIndex );
}



//-----------------------
// photo( )
// ----------------
// Retourne le nom de la photo affichée actuellement
//----------------------
QString CCaptionManager::photo( )
{
/*    QStringListModel* model = (QStringListModel*)m_p_listView->model();
    QModelIndex indexPhotoSelected = model->index( m_photoIndex );
    QVariant photoSelected = model->data( indexPhotoSelected, Qt::DisplayRole );
    return photoSelected.toString();
*/
    return m_photoDb.filename( m_photoIndex );
}


//-----------------------
// captionList
// ----------------
// Retourne la liste de légendes sous forme d'une QList
// Ordonnée comme sur la QListView
// => Attention : l'instance doit être préalablement connectée à une QListView
//                voir setPhotoList( QListView* photoList )
//----------------------
/*QList<CCaption> CCaptionManager::captionList( )
{
   QStringListModel* model = (QStringListModel*)m_p_listView->model();
    QStringList photoList = model->stringList();
    QList<CCaption> captionList;
    CCaption emptyCaption;

    //La liste doit être générée sous forme de QList, ordonnée comme le seront les photos de la galerie
    foreach(QString photo, photoList )
    {
        captionList << m_captionMap.value( photo, emptyCaption );
    }

    return captionList;
}
*/

/*
//-----------------------
// captionList
// ----------------
// Retourne la liste de légendes
//----------------------
QMap<QString,CCaption> CCaptionManager::captionMap( )
{
    return m_captionMap;
}


//-----------------------
// setCaptionList
// ----------------
// Set la liste de légendes.
//----------------------
void CCaptionManager::setCaptionMap( QMap<QString,CCaption> & captionMap)
{    
    m_captionMap = captionMap;
}
*/

//-----------------------
// setExifTags
// ----------------
// Indique les <tags Exifs / metadata> à utiliser pour la légende de la photo spécifiée.
//----------------------
void CCaptionManager::setExifTags( const QString &photoName, const QMap<QString,QString> &exifTags )
{    
   /* CCaption caption;
    caption = m_captionMap.value( photoName );
    caption.setExifTags( exifTags );
    m_captionMap.insert( photoName, caption );*/

    CPhotoProperties* properties =  m_photoDb.properties( photoName );
    CCaption caption = properties->caption();
    caption.setExifTags( exifTags );
    properties->setCaption( caption );

}

//-----------------------
// setFileInfo
// ----------------
//Indique les infos fichier à utiliser par la légende de la photo spécifiée
//----------------------
void CCaptionManager::setFileInfo( const QString & photoName, const QFileInfo & fileInfo )
{
    /*CCaption caption;
    caption = m_captionMap.value( photoName );
    caption.setFileInfo( fileInfo );
    m_captionMap.insert( photoName, caption );
    */
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
        /*QVariant photoSelected = model.data( indexPhotoSelected, Qt::DisplayRole );
        caption = m_captionMap.value( photoSelected.toString() );
        caption.setId( nb + 1 ); //Maj du numéro de la photo affichée pour utilisation par la légende
        m_captionMap.insert( photoSelected.toString(), caption );*/
        CCaption caption = m_photoDb.properties( nb )->caption();
        emit displayCaptionSignal( caption.body() );
        emit displayPreviewSignal( caption.render( CTaggedString::PREVIEW ) );
        emit displayHighlightIndex( indexPhotoSelected );
    }
}

//-----------------------
// remapCaptionList( )
// ----------------
// Refabrique une nouvelle Map de légendes correspondant au modèle associé
// Retourne le nombre d'anciennes légendes n'ayant pu être gardées
//----------------------
int CCaptionManager::remapCaptionList( )
{
    /*
    int nbCaptionsRemoved = 0;
    QStringListModel* model = (QStringListModel*)m_p_listView->model();
    QStringList photoList = model->stringList();

    foreach(QString photoName, m_captionMap.keys() )
    {
        //Si une photo de l'ancienne liste n'est plus dans la nouvelle, supprimer sa légende
        if( !photoList.contains( photoName ) ){
            m_captionMap.remove( photoName );
            nbCaptionsRemoved++;
        }
    }

    return nbCaptionsRemoved;
    */
    return 0;
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
    if( m_photoIndex > 0 )
    {
        m_photoIndex--;        
        display( m_photoIndex );
    }
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

    if( m_photoIndex < model.stringList().size() - 1 )
    {
        m_photoIndex++;        
        display( m_photoIndex );
    }
}



/******************* SLOTS ************************/



void CCaptionManager::onListClicked( QModelIndex indexPhotoSelected)
{    
      m_photoIndex = indexPhotoSelected.row();
      display( m_photoIndex );
}


void CCaptionManager::onCaptionTextEdited( QString text )
{
    /*CCaption caption;
    caption = m_captionMap.value( this->photo() );
    caption.setBody( text );
    m_captionMap.insert( this->photo(), caption );*/
    CPhotoProperties* properties = m_photoDb.properties( m_photoIndex );
    CCaption caption = properties->caption();
    caption.setBody( text );
    properties->setCaption( caption );
    m_f_captionsEdited = true;

    emit displayPreviewSignal( caption.render( CTaggedString::PREVIEW ) );
}


void CCaptionManager::onCaptionHeaderEdited( QString text )
{
    /*QMutableMapIterator<QString,CCaption> i(m_captionMap);
    CCaption caption;
    //Modification de toutes les légendes
    while( i.hasNext() ){
        i.next( );
        caption = i.value();
        caption.setHeader( text );
        i.setValue( caption );
    }*/

    for( int i = 0; i < m_photoDb.size(); i++ ) {
        CPhotoProperties* properties = m_photoDb.properties( i );
        CCaption caption = properties->caption();
        caption.setHeader( text );
        properties->setCaption( caption );
    }
    m_f_captionsEdited = true;
   
    //Affichage de la preview
    //caption = m_captionMap.value( this->photo() );
    CPhotoProperties* properties = m_photoDb.properties( m_photoIndex );
    CCaption caption = properties->caption();
    emit displayPreviewSignal( caption.render( CTaggedString::PREVIEW ) );
}

void CCaptionManager::onCaptionEndingEdited( QString text )
{
 /*   QMutableMapIterator<QString,CCaption> i(m_captionMap);
    CCaption caption;
    //Modification de toutes les légendes
    while( i.hasNext() ){
        i.next( );
        caption = i.value();
        caption.setEnding( text );
        i.setValue( caption );
    }*/

    for( int i = 0; i < m_photoDb.size(); i++ ) {
        CPhotoProperties* properties = m_photoDb.properties( i );
        CCaption caption = properties->caption();
        caption.setEnding( text );
        properties->setCaption( caption );
    }
    m_f_captionsEdited = true;

    //Affichage de la preview
    //caption = m_captionMap.value( this->photo() );       
    CPhotoProperties* properties = m_photoDb.properties( m_photoIndex );
    CCaption caption = properties->caption();
    emit displayPreviewSignal( caption.render( CTaggedString::PREVIEW ) );
}
