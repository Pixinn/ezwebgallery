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

#include "CPhotoDatabase.h"
#include "CPhoto.h"


/***************************************** STATIC VARS ************************************/
const QImage CPhotoDatabase::s_defaultThumbnail;    //Needs to be initialized


/***************************************** FUNCTIONS ************************************/

/*******************************************************************
* setPhotoList(const QStringList &)
* ---------
* Add a list of photo to the database
* In : (QStringList) absolute filepath to the photos
* Return : (QStringList) absolute file path to photos present in the db but not on disk
********************************************************************/
QStringList CPhotoDatabase::setPhotoList(const QStringList &photoList )
{
    foreach( QString photo, photoList ){
        add( photo );
    }
    QStringList missingFiles = checkPhotosInDb();
    if( !missingFiles.isEmpty() ) {
        QString summary = CError::error(CError::missingFiles);
        QString details;
        foreach( QString file, missingFiles ){
            details += file + QString("\n");
        }
        emit error( CError(summary,details) );
    }

    return missingFiles;
}



/*******************************************************************
* checkPhotosInDb( void )
* ---------
* Returns a list of the photos present in the DB but not on the disk
* Return : (QStringList)
********************************************************************/
QStringList CPhotoDatabase::checkPhotosInDb( void )
{
    QStringList missingPhotos;
    
    foreach( CPhotoDatabaseElem* elem, m_db)  {
        QFileInfo fileInfo = elem->fileInfo();
        if( !fileInfo.exists() ) {
            missingPhotos << fileInfo.absoluteFilePath();
        }
    }

    if( !missingPhotos.isEmpty() ){
        emit somePhotosAreMissing( missingPhotos );
    }
    
    return missingPhotos;
}


/*******************************************************************
* addBack( const QString & ); 
* ---------
* adds a photo at the end of the ordered list
* In : (QString) absolute file path
********************************************************************/
void CPhotoDatabase::add( const QString& photoPath)
{
    QFileInfo fileInfo(photoPath);
    if( !m_orderedKeys.contains( fileInfo.fileName() ) )
    {
        CPhotoDatabaseElem* photoElem = new CPhotoDatabaseElem();
        int id = m_orderedKeys.size();    

        photoElem->setId( id );
        photoElem->setFileInfo( fileInfo );
        photoElem->setLastModificationTime( fileInfo.lastModified() );
        m_db.insert( fileInfo.fileName(), photoElem );
        m_orderedKeys.insert( id, fileInfo.fileName() );
    }    
}


/*******************************************************************
* remove( const QString & ); 
* ---------
* remove one photo from the db
* In : (QString) filename
********************************************************************/
void CPhotoDatabase::remove( const QString & filename )
{
    if( m_db.contains( filename ) ) {
        CPhotoDatabaseElem* elem = m_db.value( filename );
        int id = elem->id();
        m_orderedKeys.removeAt( id );
        //Removing the element from the db and decrementing subsequent ids
        QMap<QString,CPhotoDatabaseElem*>::iterator it = m_db.find( filename );        
        while( ++it != m_db.end() ){
            int id = it.value()->id();
            it.value()->setId( --id );
        }
        m_db.remove( filename );        
        //Deleting the element itself
        delete elem;
    }
}

/*******************************************************************
* remove( int ); 
* ---------
* remove one photo from the db
* In : (int) id
********************************************************************/
void CPhotoDatabase::remove( int id )
{
    const QString filename = m_orderedKeys.at( id );
    remove( filename );
}

/*******************************************************************
* remove( int ); 
* ---------
* remove one photo from the db
* In : (int) id
********************************************************************/
void CPhotoDatabase::clear( )
{
    foreach( CPhotoDatabaseElem* element, m_db) {
        delete element;
    }
    m_db.clear();
    m_orderedKeys.clear();
}

/*******************************************************************
* swap( const QString &, const QString & ); 
* ---------
* Swap to element in the database, defined by their filename
* In : (QString) element filename1
* In : (QString) element filename2
********************************************************************/
void CPhotoDatabase::swap( const QString & filename1, const QString & filename2 )
{
     if( m_db.contains( filename1 ) && m_db.contains( filename2 )  )
     {
        //Swaping in main db
        CPhotoDatabaseElem* elem1 = m_db.value( filename1 );
        CPhotoDatabaseElem* elem2 = m_db.value( filename2 );
        int id1 = elem1->id();
        int id2 = elem2->id();
        elem1->setId( id1 );
        elem2->setId( id2 );
        //Swaping in keylist
        m_orderedKeys.swap( id1, id2 );
     }
 }

/*******************************************************************
* swap( int, int  ); 
* ---------
* Swap to element in the database, defined by their id
* In : (int) element id1
* In : (int) element id2
********************************************************************/
void CPhotoDatabase::swap( int id1, int id2 )
{
    const QString& filename1 = m_orderedKeys.at( id1 );
    const QString& filename2 = m_orderedKeys.at( id2 );
     if( m_db.contains( filename1 ) && m_db.contains( filename2 )  )
     {
        //Swaping in main db
        CPhotoDatabaseElem* elem1 = m_db.value( filename1 );
        CPhotoDatabaseElem* elem2 = m_db.value( filename2 );
        elem1->setId( id1 );
        elem2->setId( id2 );
        //Swaping in keylist
        m_orderedKeys.swap( id1, id2 );
     }
 }
 
 /*******************************************************************
* loadThumbnail( const QString &  ); 
* ---------
* For the times being thumbnails are loaded synchronously via an external request.
* In the future: a background process could load them asynconously
* In : (QString) filename from which to generate the thumbnail
* Returns : (bool) true if success, false otherwise
********************************************************************/
bool CPhotoDatabase::loadThumbnail( const QString & filename )
{
    CPhoto photo;    
    CPhotoDatabaseElem* elem = m_db.value( filename );

    QString absoluteFilePath = elem->fileInfo().absoluteFilePath();
    bool f_success = photo.load( elem->fileInfo().absoluteFilePath() );
    
    if( f_success )
    {
        photo.zoom( m_thumbnailsSize, Qt::FastTransformation ); //resize
        photo.orientationExif(); //correct orientation
        //Writing  into the db
        elem->setExifTags( photo.exifTags() );
        elem->setFileInfo( absoluteFilePath );
        elem->m_thumbnail = photo.qimage();
    }
    else
    {
        //Loging messages if an error occured
        if( !photo.errors().isEmpty() ){
            foreach( QString err, photo.errors() ){
                CError cerror( QString("[Thumbnail]." ) + err );
                emit error( cerror );
            }
            f_success = false;
        }
    }
    
    return f_success;
}
 

 /*******************************************************************
* loadThumbnail( in id ); 
* ---------
* For the times being thumbnails are loaded synchronously via an external request.
* In the future: a background process could load them asynconously
* In : (QString) filename from which to generate the thumbnail
* Returns : (bool) true if success, false otherwise
********************************************************************/
bool CPhotoDatabase::loadThumbnail( int id )
{
    const QString& filename = m_orderedKeys.at( id );
    return loadThumbnail( filename );
} 
 
 

 /*******************************************************************
* thumbnail( const QString & filename ); 
* ---------
* Returns the thumnail of the element defined by its filename
* In : (int) element id
* Returns : thumbnail
********************************************************************/
const QImage& CPhotoDatabase::thumbnail( const QString & filename  ) const
{
     if( m_db.contains( filename ) ) {
        CPhotoDatabaseElem* elem = m_db.value( filename );
        return elem->m_thumbnail;
     }
     else {
        return s_defaultThumbnail;
     }
}
 
 /*******************************************************************
* thumbnail( int ); 
* ---------
* Returns the thumnail of the element defined by id
* In : (int) element id
* Returns : thumbnail
********************************************************************/
const QImage& CPhotoDatabase::thumbnail( int id  ) const
{
     const QString& filename = m_orderedKeys.at( id );
     if( m_db.contains( filename ) ) {
        CPhotoDatabaseElem* elem = m_db.value( filename );
        return elem->m_thumbnail;
     }
     else {
        return s_defaultThumbnail;
     }
}


 /*******************************************************************
* properties(  ); 
* ---------
* Returns an ordered list of base properties
* Returns : (QList<CPhotoProperties>)
********************************************************************/
QList<CPhotoProperties> CPhotoDatabase::properties( void ) const
{
    QList<CPhotoProperties> list;
    foreach( QString filename, m_orderedKeys ) {
        list << *(m_db.value( filename ));
    }
    
    return list;
}

