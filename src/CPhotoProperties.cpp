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


#include "GlobalDefinitions.h"
#include "CPhotoProperties.h"



///////////////////////////////////////////////////////////////////
//////////////////// CLASSE CPhotoProperties //////////////////////
///////////////////////////////////////////////////////////////////


/************************ STATIC INSTANCES *********************/
const QString CPhotoProperties::CAPTION("Caption");
const QString CPhotoProperties::CAPTIONHEADER("CaptionHeader");
const QString CPhotoProperties::CAPTIONENDING("CaptionEnding");


/*******************************************************************
* CPhotoProperties( const QDomNode & )
* ---------
* Constructs from an XML structure
********************************************************************/
CPhotoProperties::CPhotoProperties( const QDomNode & node ) :
    QObject(),
    m_id(-1),
    m_fileInfo(),
    m_exifTags(),
    m_caption()
{   
    m_caption.setBody( node.firstChildElement(CAPTION).text() );
    m_caption.setHeader( node.firstChildElement(CAPTIONHEADER).text() );
    m_caption.setEnding( node.firstChildElement(CAPTIONENDING).text() );
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
        this->m_lastModified = other.m_lastModified;
        this->m_id = other.m_id;     
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
        && this->m_fileInfo.absoluteFilePath() == toCompare.m_fileInfo.absoluteFilePath() //Doesn't work? Cache effects?
        && this->m_lastModified.toString() == toCompare.m_lastModified.toString()
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
* En effet, certains tels m_exifTags, changent à la premire génération
* mais ne sont pas sauvés dans le fichier projet, faisant apparatre la fentre
* de "projet modifié" à mauvais essient.
********************************************************************/
bool CPhotoProperties::isEquivalent( const CPhotoProperties &toCompare )
{
        bool f_result;
    if(    this->m_fileInfo == toCompare.m_fileInfo  
        && this->m_lastModified == toCompare.m_lastModified
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
* encodedFilename( )
* ---------
* Returns the filename encoded to be part of URL (no forbidden characters)
********************************************************************/
QString CPhotoProperties::encodedFilename( void ) const
{    
    QByteArray encoded = fileName().remove( IMAGEFILEEXTENSION ).toUtf8().toBase64();
    encoded.replace('=','-');
    encoded.replace('/','_');
    encoded.append( IMAGEFILEEXTENSION );
    return QString( encoded );
}
