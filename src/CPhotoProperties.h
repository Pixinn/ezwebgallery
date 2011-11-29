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



#ifndef CPhotoProperties_H
#define CPhotoProperties_H

#include <QDomDocument>
#include <QString>
#include <QMap>
#include <QFileInfo>

#include "CCaption.h"


/*****************************************************************
 * CPhotoProperties
 * ----------------------
 * Base proposeties of a photo ;
 * required for gallery generation
 ****************************************************************/
class CPhotoProperties : public QObject 
{

    Q_OBJECT

public:
    CPhotoProperties( void ) :
        QObject(),
        m_id(-1)
        //m_f_processed(false)
    {   }
    CPhotoProperties( const CPhotoProperties & other) :
        QObject(),
        m_id( other.m_id ),
        //m_f_processed( other.m_f_processed ),
        m_fileInfo( other.m_fileInfo ),
        m_exifTags( other.m_exifTags ),
        m_caption( other.m_caption )
    {   }
    CPhotoProperties( const QDomNode & );
    ~CPhotoProperties( void ){ }

    CPhotoProperties & operator=( const CPhotoProperties & other);
    bool operator==( const CPhotoProperties &) const;
    
    void setId( int id ){
        m_id = id;
        m_caption.setId( id );
    }
    /*void setProcessed( bool processed ){
        m_f_processed = processed;
    }*/
    void setFileInfo( const QFileInfo & fileInfo){
        m_fileInfo = fileInfo;
        m_caption.setFileInfo( fileInfo );
    }
    void setExifTags( const QMap<QString,QString> & exifTags){
        m_exifTags = exifTags;
        m_caption.setExifTags( exifTags );
    }

    void setCaption( const CCaption & caption){
        m_caption = caption;
        m_caption.setExifTags( m_exifTags );
    }
    
    int id( ) const {
        return m_id;
    }
    /*bool processed( ) const {
        return m_f_processed;
    }*/
    QFileInfo fileInfo( ) const {
        return m_fileInfo;
    }
    QMap<QString,QString> exifTags( ) const {
        return m_exifTags;
    }
    CCaption caption() const {
        return m_caption;
    }

    bool isEquivalent( const CPhotoProperties & );

    QString fileName( void ) const { return m_fileInfo.fileName(); } //A RETIRER !!!
    
private:
    static const QString CAPTION;
    static const QString CAPTIONHEADER;
    static const QString CAPTIONENDING;
    int m_id;	//NECESSAIRE ???
    //bool m_f_processed;
    QFileInfo m_fileInfo;
    QMap<QString,QString> m_exifTags;
    CCaption m_caption;
};

#endif
