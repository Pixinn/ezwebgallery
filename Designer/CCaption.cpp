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

/*
* This header is mandatory on all files of the project in order to track the global svn revision usin svnrev
* $Id
*/


#include "CCaption.h"


//Membres public
CCaption::CCaption( )
{
    setId( -1 );
}

bool CCaption::operator==( const CCaption &b ) const
{
    bool f_returned = false;
    
    if(     m_header == b.m_header
        &&  m_body == b.m_body
        &&  m_ending == b.m_ending
        ) {
        f_returned = true;
    }

    return f_returned;
}


/*******************************************************************
* setHeader( const CTaggedString &header )
* ---------
* Renseigne le préfixe de la légende
* In : (CTaggedString) préfixe
********************************************************************/
void CCaption::setHeader( const CTaggedString &header )
{   
    m_header = header;
}

/*******************************************************************
* setHeader( const QString &header )
* ---------
* Renseigne le préfixe de la légende
* In : (QString) préfixe
********************************************************************/
void CCaption::setHeader( const QString &header )
{   
    m_header.setString( header );
}


/*******************************************************************
* setBody( const CTaggedString &body )
* ---------
* Renseigne le corps de la légende
* In : (CTaggedString) corps
********************************************************************/
void CCaption::setBody( const CTaggedString &body )
{   
    m_body = body;
}

/*******************************************************************
* setBody( const QString &body )
* ---------
* Renseigne le corps de la légende
* In : (CTaggedString) corps
********************************************************************/
void CCaption::setBody( const QString &body )
{   
    m_body.setString( body );
}

/*******************************************************************
* setEnding( const CTaggedString &ending )
* ---------
* Renseigne le suffixe de la légende
* In : (CTaggedString) suffixe
********************************************************************/
void CCaption::setEnding( const CTaggedString &ending )
{   
    m_ending = ending;
}

/*******************************************************************
* setEnding( const QString &ending )
* ---------
* Renseigne le suffixe de la légende
* In : (CTaggedString) suffixe
********************************************************************/
void CCaption::setEnding( const QString &ending )
{   
    m_ending.setString(ending);
}


/*******************************************************************
* header( )
* ---------
* Retourne le préfixe de la légende
* Return : (CTaggedString) préfixe
********************************************************************/
CTaggedString CCaption::header(  )
{   
    return m_header;
}

/*******************************************************************
* body( )
* ---------
* Retourne le corps de la légende
* Return : (CTaggedString) corps
********************************************************************/
CTaggedString CCaption::body( )
{   
    return m_body;
}

/*******************************************************************
* ending( )
* ---------
* Retourne le suffixe de la légende
* Return : (CTaggedString) suffixe
********************************************************************/
CTaggedString CCaption::ending(  )
{   
    return m_ending;
}


/*******************************************************************
* setExifTags( const QMap<QString,QString> & )
* ---------
* Indique les <tags Exifs / metadata> à utiliser
* In : (QMap<QString,QString>) tag exifs
********************************************************************/
void CCaption::setExifTags( const QMap<QString,QString> &exifTags )
{   
    m_header.setExifTags( exifTags );
    m_body.setExifTags( exifTags );
    m_ending.setExifTags( exifTags );
}

/*******************************************************************
* setFileInfo( const QQFileInfo & )
* ---------
* Indique le fileinfo à utiliser
* In : (QFileInfo) file info
********************************************************************/
void CCaption::setFileInfo( const QFileInfo &fileInfo )
{
    m_header.setFileInfo( fileInfo );
    m_body.setFileInfo( fileInfo );
    m_ending.setFileInfo( fileInfo );
}


/*******************************************************************
* setId( int )
* ---------
* Indique l'id de la photo
* In : (int) id
********************************************************************/
void CCaption::setId( int id )
{
    m_header.setId( id );
    m_body.setId( id );
    m_ending.setId( id );
}


/*******************************************************************
* render( int preview = NOPREVIEW )
* ---------
* Calcule la légende avec tous les tags remplacés par les données correspondantes
* In: (int) indique si le rendu est envue d'une preview (par faut : NON )
* return : le QString résultant
********************************************************************/
QString CCaption::render( int preview )
{
    QString rendered = m_header.render( preview ) + m_body.render( preview ) + m_ending.render( preview );

    //Il faut remplacer les caractères dangereux pour le html / javascript
    //Mais pas si preview ;)
    if( preview == CTaggedString::NOPREVIEW ) {
        rendered.replace( QString("\""), QString("\\\"") );
    }

    return rendered;
}


