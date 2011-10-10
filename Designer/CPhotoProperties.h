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



#ifndef CPHOTOPROPERTIES_H
#define CPHOTOPROPERTIES_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QMap>
#include <QFileInfo>

#include "CCaption.h"

/*****************************
 * CPhotoProperties
 * ----------------------
 * Classe dcrivant les propits d'une photo  traiter
 * Permet de centraliser toutes ces infos
 ********************************************/
class CPhotoProperties : public QObject //Drive de QObject pour pouvoir tre transmis via un signal
{        

    Q_OBJECT

public:
    CPhotoProperties();
    CPhotoProperties( const CPhotoProperties & );
    CPhotoProperties & operator=( const CPhotoProperties & );
    bool operator==( const CPhotoProperties &) const;
    bool isEquivalent( const CPhotoProperties & ) const; //Ne compare que les champs intressants 
    void setId( int );
    void setFileInfo( const QFileInfo & );
    void setLastModificationTime( const QDateTime & );
    void setExifTags( const QMap<QString,QString> & );
    void setCaption( const CCaption & );
    void setProcessed( bool );
    int id( ) const;
    QString fileName( ) const;
	QFileInfo fileInfo( ) const;
    QDateTime lastModificationTime( ) const;
    QMap<QString,QString> exifTags( ) const;
    CCaption caption() const;
    bool processed( ) const;

private:
    int m_id;
    bool m_f_processed; //La photo a t traite
	QFileInfo m_fileInfo;
    QString m_fileName;
    QDateTime m_lastModificationTime;
    QMap<QString,QString> m_exifTags;
    CCaption m_caption;

};

#endif
