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



#ifndef CPHOTOEXTENDEDPROPERTIES_H
#define CPHOTOEXTENDEDPROPERTIES_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QMap>
#include <QFileInfo>

#include "CPhotoProperties.h"
#include "CCaption.h"

/*****************************
 * CPhotoExtendedProperties
 * ----------------------
 * Classe décrivant les propits d'une photo  traiter
 * Permet de centraliser toutes ces infos
 ********************************************/
class CPhotoExtendedProperties : public CPhotoProperties
{        

    Q_OBJECT

public:
    CPhotoExtendedProperties();
    CPhotoExtendedProperties( const CPhotoExtendedProperties & );
    CPhotoExtendedProperties & operator=( const CPhotoExtendedProperties & );
    bool operator==( const CPhotoExtendedProperties &) const;
    bool isEquivalent( const CPhotoExtendedProperties & ) const; //Ne compare que les champs intressants 
    void setLastModificationTime( const QDateTime & );
    QDateTime lastModificationTime( ) const;

private:
    QDateTime m_lastModificationTime;
};

#endif
