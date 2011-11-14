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



#include "CPhotoProperties.h"



///////////////////////////////////////////////////////////////////
//////////////////// CLASSE CPhotoProperties //////////////////////
///////////////////////////////////////////////////////////////////

/*******************************************************************
* CPhotoProperties( )
* ---------
* Constructeur par dfaut
********************************************************************/
CPhotoProperties::CPhotoProperties()
    : QObject()
{
    m_id = -1;
    m_f_processed = false;
}


/*******************************************************************
* CPhotoProperties( const CPhotoProperties &)
* ---------
* Constructeur par recopie, ncessaire pour les classes qui drivent de QObject
********************************************************************/
CPhotoProperties::CPhotoProperties( const CPhotoProperties &other )
    : QObject()
{
    this->m_caption = other.m_caption;
    this->m_exifTags = other.m_exifTags;
    this->m_fileInfo = other.m_fileInfo;
    this->m_fileName = other.m_fileName;
    this->m_id = other.m_id;
    this->m_f_processed = other.m_f_processed;
    this->m_lastModificationTime = other.m_lastModificationTime;
}


/*******************************************************************
* CPhotoProperties( const CPhotoProperties &)
* ---------
* Constructeur par recopie, ncessaire pour les classes qui drivent de QObject
********************************************************************/
CPhotoProperties & CPhotoProperties::operator=( const CPhotoProperties &other )
{
    if( this != &other )
    {
        this->m_caption = other.m_caption;
        this->m_exifTags = other.m_exifTags;
        this->m_fileInfo = other.m_fileInfo;
        this->m_fileName = other.m_fileName;
        this->m_id = other.m_id;
        this->m_f_processed = other.m_f_processed;
        this->m_lastModificationTime = other.m_lastModificationTime;
    }

    return *this;
}


/*******************************************************************
* operator==( )
* ---------
* Ncessaire pour tre utilis dans un QMap
********************************************************************/
bool CPhotoProperties::operator==( const CPhotoProperties &toCompare ) const
{
    bool f_result;
    if(    this->m_id == toCompare.m_id
        && this->m_f_processed == toCompare.m_f_processed
        && this->m_fileName == toCompare.m_fileName
        && this->m_lastModificationTime == toCompare.m_lastModificationTime
        && this->m_exifTags == toCompare.m_exifTags
        && this->m_caption == toCompare.m_caption )
    {
        f_result = true;
    }
    else {
        f_result = false;
    }

    return f_result;

}


/*******************************************************************
* isEquivalent( const CPhotoProperties &toCompare )
* ---------
* Ne compare que certains champs pertinants.
* En effet, certains tels m_exifTags, changent  la premire gnration
* mais ne sont pas sauvs dans le fichier projet, faisant apparatre la fentre
* de "projet modifi"  mauvais essient.
********************************************************************/
bool CPhotoProperties::isEquivalent( const CPhotoProperties &toCompare ) const
{
        bool f_result;
    if(    this->m_fileName == toCompare.m_fileName
        && this->m_lastModificationTime == toCompare.m_lastModificationTime
        && this->m_caption == toCompare.m_caption )
    {
        f_result = true;
    }
    else {
        f_result = false;
    }

    return f_result;

}


/*******************************************************************
* setId( int )
* ---------
* Mise  jour de l'id (numro) de la photo
* In : (int) id
********************************************************************/
void CPhotoProperties::setId( int id )
{
    m_id = id;
	m_caption.setId( id );
}


/*******************************************************************
* setFileInfo( const QFileInfo & )
* ---------
* Mise  jour du fileinfo dcrivant la photo
* In : (QFileInfo) fileInfo
********************************************************************/
void CPhotoProperties::setFileInfo( const QFileInfo &fileInfo )
{
    m_fileInfo = fileInfo;
	m_caption.setFileInfo( fileInfo );
}


/*******************************************************************
* setFileName( const QString & )
* ---------
* Mise  jour du filename
* In : (QString) filename
********************************************************************/
/*void CPhotoProperties::setFileName( const QString &fileName )
{
    m_fileName = fileName;
}*/

/*******************************************************************
* setLastModificationTime( const QDateTime &setLastModificationTime )
* ---------
* Mise  jour de la date de dernire modification du fichier
* In : (QDateTime) lastModificationTime
********************************************************************/
void CPhotoProperties::setLastModificationTime( const QDateTime &lastModificationTime )
{
    m_lastModificationTime = lastModificationTime;
}

/*******************************************************************
* setExifTags( const QMap<QString,QString> &exifTags )
* ---------
* Mise  jour des tags exifs
* In : (QMap<QString,QString>) exifTags
********************************************************************/
void CPhotoProperties::setExifTags( const QMap<QString,QString> &exifTags )
{
    m_exifTags = exifTags;
    m_caption.setExifTags( exifTags );
}

/*******************************************************************
* setCaption( const CCaption &caption )
* ---------
* Mise  jour de la lgende
* In : (CCaption) lgende
********************************************************************/
void CPhotoProperties::setCaption( const CCaption &caption )
{
    m_caption = caption;
    m_caption.setExifTags( m_exifTags );
}


/*******************************************************************
* setProcessed( bool processed )
* ---------
* Mise  jour du flag indiquant si la photo a t traite
* In : (bool) flag
********************************************************************/
void CPhotoProperties::setProcessed( bool processed )
{
    m_f_processed = processed;
}


/*******************************************************************
* fileInfo( )
* ---------
* Renvoie le QFileInfo dcrivant le fichier
* Retour (QFileInfo) descripteur du fichier
********************************************************************/
QFileInfo CPhotoProperties::fileInfo( ) const
{
    return m_fileInfo;
}


/*******************************************************************
* id( )
* ---------
* Renvoie le numro de la photo
* Retour (int) l'id de la photo
********************************************************************/
int CPhotoProperties::id( ) const
{
    return m_id;
}


/*******************************************************************
* fileName( )
* ---------
* Renvoie le nom du fichier
* Retour (QString) le nom du fichier
********************************************************************/
QString CPhotoProperties::fileName( ) const
{
    //return m_fileName;
    return m_fileInfo.fileName();
}

/*******************************************************************
* lastModificationTime( )
* ---------
* Renvoie la date de dernire modification de la photo
* Retour (QDateTime) date dedernire modification
********************************************************************/
QDateTime CPhotoProperties::lastModificationTime( ) const
{
    return m_lastModificationTime;
}

/*******************************************************************
* exifTags( )
* ---------
* Renvoie les tags exif du fichier
* Retour (QMap<QString,QString>) tags exif
********************************************************************/
QMap<QString,QString> CPhotoProperties::exifTags( ) const
{
    return m_exifTags;
}


/*******************************************************************
* caption( )
* ---------
* Renvoie la lgende de la photo
* Retour (CCaption) lgende
********************************************************************/
CCaption CPhotoProperties::caption( ) const
{
    return m_caption;
}


/*******************************************************************
* caption( )
* ---------
* Renvoie le flag indiquant si la photo a t traite
* Retour (bool) flag
********************************************************************/
bool CPhotoProperties::processed( ) const
{
    return m_f_processed;
}
