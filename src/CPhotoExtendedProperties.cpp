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



#include "CPhotoExtendedProperties.h"



///////////////////////////////////////////////////////////////////
//////////////////// CLASSE CPhotoExtendedProperties //////////////////////
///////////////////////////////////////////////////////////////////

/*******************************************************************
* CPhotoExtendedProperties( )
* ---------
* Constructeur par dfaut
********************************************************************/
CPhotoExtendedProperties::CPhotoExtendedProperties()
    : CPhotoProperties()
{

}


/*******************************************************************
* CPhotoExtendedProperties( const CPhotoExtendedProperties &)
* ---------
* Constructeur par recopie, ncessaire pour les classes qui drivent de QObject
********************************************************************/
CPhotoExtendedProperties::CPhotoExtendedProperties( const CPhotoExtendedProperties &other )
    : CPhotoProperties( static_cast<CPhotoProperties>(other) )
{
    this->m_lastModificationTime = other.m_lastModificationTime;
}


/*******************************************************************
* CPhotoExtendedProperties( const CPhotoExtendedProperties &)
* ---------
* Constructeur par recopie, ncessaire pour les classes qui drivent de QObject
********************************************************************/
CPhotoExtendedProperties & CPhotoExtendedProperties::operator=( const CPhotoExtendedProperties &other )
{
    if( this != &other )
    {
        /*this->setCaption( other.caption() );
        this->setExifTags( other.exifTags() );
        this->setFileInfo( other.fileInfo() );
        this->setId( other.id() );
        this->setProcessed( other.processed() );*/
        CPhotoProperties::operator=( other );
        this->m_lastModificationTime = other.m_lastModificationTime;
    }

    return *this;
}


/*******************************************************************
* operator==( )
* ---------
* Ncessaire pour tre utilis dans un QMap
********************************************************************/
bool CPhotoExtendedProperties::operator==( const CPhotoExtendedProperties &toCompare ) const
{
    bool f_result;
    if(     static_cast<CPhotoProperties>(*this) == static_cast<CPhotoProperties>(toCompare)
        &&   this->m_lastModificationTime == toCompare.m_lastModificationTime )
    {
        f_result = true;
    }
    else {
        f_result = false;
    }

    return f_result;

}


/*******************************************************************
* isEquivalent( const CPhotoExtendedProperties &toCompare )
* ---------
* Ne compare que certains champs pertinants.
* En effet, certains tels m_exifTags, changent à la premire génération
* mais ne sont pas sauvés dans le fichier projet, faisant apparatre la fentre
* de "projet modifié" à mauvais essient.
********************************************************************/
bool CPhotoExtendedProperties::isEquivalent( const CPhotoExtendedProperties &toCompare ) const
{
        bool f_result;
        if(   static_cast<CPhotoProperties>(*this).isEquivalent( static_cast<CPhotoProperties>(toCompare) )
             && this->m_lastModificationTime == toCompare.m_lastModificationTime
         )
    {
        f_result = true;
    }
    else {
        f_result = false;
    }

    return f_result;

}



/*******************************************************************
* setLastModificationTime( const QDateTime &setLastModificationTime )
* ---------
* Mise  jour de la date de dernire modification du fichier
* In : (QDateTime) lastModificationTime
********************************************************************/
void CPhotoExtendedProperties::setLastModificationTime( const QDateTime &lastModificationTime )
{
    m_lastModificationTime = lastModificationTime;
}

/*******************************************************************
* lastModificationTime( )
* ---------
* Renvoie la date de dernire modification de la photo
* Retour (QDateTime) date dedernire modification
********************************************************************/
QDateTime CPhotoExtendedProperties::lastModificationTime( ) const
{
    return m_lastModificationTime;
}

