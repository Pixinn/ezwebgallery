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

#ifndef CCAPTION_H
#define CCAPTION_H


#include <QString>
#include <QMap>
#include <QFileInfo>

#include "CTaggedString.h"


class CCaption
{
public:
    CCaption( );
    bool operator==( const CCaption & ) const;
    QString render( int preview = CTaggedString::NOPREVIEW );          //Calcule la lgende sous forme de QString
    void setHeader( const CTaggedString& );
    void setHeader( const QString& );
    void setBody( const CTaggedString& );
    void setBody( const QString& );
    void setEnding( const CTaggedString& );    
    void setEnding( const QString& );
    void setExifTags( const QMap<QString,QString> & ); //Indique les <tags Exifs / metadata>  utiliser
    void setFileInfo( const QFileInfo & );             //Indique les infos fichier  utiliser
    void setId( int id );                              //Indique un id entier
    CTaggedString header();
    CTaggedString body();
    CTaggedString ending();

private:
    CTaggedString m_header;
    CTaggedString m_ending;
    CTaggedString m_body;    
};




#endif
