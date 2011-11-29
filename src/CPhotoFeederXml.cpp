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
 
#include "IPhotoFeeder.h"
#include "CPhotoFeederXml.h"

/********************** STATIC INSTANCES **************/

const QString CPhotoFeederXml::PHOTOLIST("Photo");
const QString CPhotoFeederXml::LASTMODIFICATIONTIME("LastModification");
const QString CPhotoFeederXml::FILEPATH("FilePath");

/*****************************************
 * CPhotoFeederXml( const QDomElement & )
 * ----------------------
 * Constructing from an XML element
 *****************************************/
CPhotoFeederXml::CPhotoFeederXml( const QDomElement & domElement )
{
    QDomNodeList photosNode = domElement.elementsByTagName( PHOTOLIST );
    for( unsigned int iteratorDomList = 0; iteratorDomList < photosNode.length() ; iteratorDomList++ )
    {        
        QDomNode node = photosNode.item( iteratorDomList );
        CPhotoPropertiesExtended properties( node );
        properties.setLastModificationTime( QDateTime::fromString( node.firstChildElement( LASTMODIFICATIONTIME ).text() ));
        properties.setFileInfo( QFileInfo( node.firstChildElement( FILEPATH ).text() ) );
        properties.setId( iteratorDomList );
        m_propertiesList << properties;
    }
}


QStringList CPhotoFeederXml::getPhotoList( void )
{
    QStringList filePathsList;
    foreach( CPhotoPropertiesExtended properties, m_propertiesList) {
        filePathsList << properties.fileInfo().absoluteFilePath();
    }
    return filePathsList;
}
