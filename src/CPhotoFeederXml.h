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
 
#ifndef CPHOTOFEEDERXML_H
#define CPHOTOFEEDERXML_H

 #include <QDomElement>
 #include <QList>
 
 #include "IPhotoFeeder.h"
 #include "CPhotoPropertiesExtended.h"
 
 
/*****************************************
 * CPhotoFeederXml
 * ----------------------
 * Class feeding the application with
 * data parsed from an XML structure
 *****************************************/
 class CPhotoFeederXml : public IPhotoFeeder
 {
 public:
    //CPhotoFeederXml( void ){ };
    CPhotoFeederXml( const QDomElement & );
    ~CPhotoFeederXml( void ){ }
    bool isValid( void ){ return !m_propertiesList.isEmpty(); }
    QStringList getPhotoList( void );

private:
    static const QString PHOTOLIST;
    static const QString LASTMODIFICATIONTIME;
    static const QString FILEPATH;
    QList<CPhotoPropertiesExtended> m_propertiesList;
   
 };
 
#endif
