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

const QString CPhotoDatabase::XMLTAG_PHOTOSDB("PhotoList");
const QString CPhotoDatabase::XMLTAG_PHOTOS("Photo");
const QString CPhotoDatabase::XMLTAG_LASTMODIFICATIONTIME("LastModification");
const QString CPhotoDatabase::XMLTAG_FILEPATH("FilePath");
const QString CPhotoDatabase::XMLTAG_CAPTIONBODY("Caption");
const QString CPhotoDatabase::XMLTAG_CAPTIONHEADER("CaptionHeader");
const QString CPhotoDatabase::XMLTAG_CAPTIONENDING("CaptionEnding");
const QString CPhotoDatabase::XMLTAG_DEPRECATED_FILENAME("PhotoName");

//SINGLE STATIC INSTANCE
CPhotoDatabase CPhotoDatabase::s_instance;

/***************************************** FUNCTIONS ************************************/


/*******************************************************************
* build( const QStringList& photoList )
* ---------
* Build from a list of absolute file paths. All previous data are erased.
* Returns the list of invalid files
********************************************************************/
QStringList CPhotoDatabase::build( const QStringList& photoList )
{
    
    clear();
    QStringList invalidFiles = appendPhotoList( photoList );
    //m_model.setStringList( m_orderedKeys );
    return invalidFiles;
}



/*******************************************************************
* build( const QDomElement & )
* ---------
* Build from an xml dom element. All previous data are erased.
* Returns the list of invalid files
********************************************************************/
QStringList CPhotoDatabase::build( const QDomElement & xmlElem )
{
    QStringList invalidFiles;
    clear();
    QDomNodeList photosNode = xmlElem.elementsByTagName( XMLTAG_PHOTOS );
    for( unsigned int iteratorDomList = 0; iteratorDomList < photosNode.length() ; iteratorDomList++ )
    {        
        QDomNode node = photosNode.item( iteratorDomList );
        CPhotoPropertiesExtended properties( node ); //this constructor sets the caption
        //file properties
        QFileInfo fileInfo( node.firstChildElement( XMLTAG_FILEPATH ).text() );
        properties.setLastModificationTime( QDateTime::fromString( node.firstChildElement( XMLTAG_LASTMODIFICATIONTIME ).text() ));
        properties.setFileInfo( fileInfo );
        properties.setId( iteratorDomList );
        //caption
        QString body = node.firstChildElement(XMLTAG_CAPTIONBODY).text();
        QString header = node.firstChildElement(XMLTAG_CAPTIONHEADER).text();
        QString ending = node.firstChildElement(XMLTAG_CAPTIONENDING).text();
        CCaption caption( header, body, ending );
        caption.setFileInfo( fileInfo );
        caption.setId( iteratorDomList + 1 );
        properties.setCaption( caption );
        if ( !add( properties ) )   {
            invalidFiles << properties.fileInfo().absoluteFilePath();
        }
    } 
//    emit updatedProperties( propertiesList() );
    //m_model.setStringList( m_orderedKeys );
    return invalidFiles;
}


/*******************************************************************
* importDeprecated( const QDomElement &, const QString & )
* ---------
* Build from a deprecated xml element
* In (QDomElement) dom element to parse
* In (QString) input directpory containing the photos
* Return (QStringList) list of invalid files
********************************************************************/
QStringList CPhotoDatabase::importDeprecated( const QDomElement & xmlElem, const QString & inDir )
{
    clear();
    QStringList invalidFiles;
    QDomNodeList photosNode = xmlElem.elementsByTagName( XMLTAG_PHOTOS );
    QDir inputDir( inDir );
    for( unsigned int iteratorDomList = 0; iteratorDomList < photosNode.length() ; iteratorDomList++ )
    {        
        QDomNode node = photosNode.item( iteratorDomList );
        CPhotoPropertiesExtended properties( node ); //this constructor sets the caption
        //file properties
         QFileInfo fileInfo( inputDir.absoluteFilePath( node.firstChildElement( XMLTAG_DEPRECATED_FILENAME ).text() ) );
        properties.setLastModificationTime( QDateTime::fromString( node.firstChildElement( XMLTAG_LASTMODIFICATIONTIME ).text() ));        
        properties.setFileInfo( fileInfo );
        properties.setId( iteratorDomList );
        //caption
        QString body = node.firstChildElement(XMLTAG_CAPTIONBODY).text();
        QString header = node.firstChildElement(XMLTAG_CAPTIONHEADER).text();
        QString ending = node.firstChildElement(XMLTAG_CAPTIONENDING).text();
        CCaption caption( header, body, ending );
        caption.setFileInfo( fileInfo );
        caption.setId( iteratorDomList + 1);
        properties.setCaption( caption );
        if ( !add( properties ) )   {
            invalidFiles << properties.fileInfo().absoluteFilePath();
        }
    } 
//    emit updatedProperties( propertiesList() );
    //m_model.setStringList( m_orderedKeys );
    return invalidFiles;
}



/*******************************************************************
* xml( QDomElement& document )
* ---------
* Builds an Xml representation of the database
* In/Out: (QDomDocument&) Xml document where to build the elements
* Return: (QDomElement) the dom element generated
********************************************************************/
QDomElement CPhotoDatabase::xml( QDomDocument& document ) const
{
    QDomElement database = document.createElement( XMLTAG_PHOTOSDB );
  //  QDomElement root = document.documentElement();
    
  //  root.appendChild( database );
    foreach( CPhotoPropertiesExtended* properties, m_db )
    {
        QDomElement photoElement = document.createElement( XMLTAG_PHOTOS );
        database.appendChild( photoElement );
        //filePath
        QDomElement filePath = document.createElement( XMLTAG_FILEPATH );
        photoElement.appendChild( filePath );
        filePath.appendChild( document.createTextNode( properties->fileInfo().absoluteFilePath() ) );
        //date
        QDomElement date = document.createElement( XMLTAG_LASTMODIFICATIONTIME );
        photoElement.appendChild( date );
        date.appendChild( document.createTextNode( properties->lastModificationTime().toString() ) );
        //caption
        QDomElement captionElem = document.createElement( XMLTAG_CAPTIONBODY );
        photoElement.appendChild( captionElem );
        captionElem.appendChild( document.createTextNode( properties->caption().body() ) );
        QDomElement captionHeaderElem = document.createElement( XMLTAG_CAPTIONHEADER );
        photoElement.appendChild( captionHeaderElem );
        captionHeaderElem.appendChild( document.createTextNode( properties->caption().header() ) );
        QDomElement captionEndingElem = document.createElement( XMLTAG_CAPTIONENDING );
        photoElement.appendChild( captionEndingElem );
        captionEndingElem.appendChild( document.createTextNode( properties->caption().ending() ) );
    }
    return database;
}


/*******************************************************************
* CPhotoDatabase( )
* ---------
* The layout of the model has been updated outside
********************************************************************/
void CPhotoDatabase::modelLayoutChanged( void )
{
    CPhotoDatabaseElem* elem;
    int id = 0;
    QStringList orderedKeys = m_model.stringList();
    foreach( QString key, orderedKeys ) {
        elem = m_db.value( key );
        elem->setId( id );
        //keeping the caption in sync
        CCaption caption = elem->caption();
        caption.setId( id + 1);
        elem->setCaption( caption );
        id++;
    }
}


/*******************************************************************
* appendPhotoList(const QStringList &)
* ---------
* Appends a list of photo to end of the database. Duplicates are not appended.
* In : (QStringList) absolute filepath to the photos
* Return : (QStringList) paths of invalid files
********************************************************************/
QStringList CPhotoDatabase::appendPhotoList(const QStringList &photoList )
{
    QStringList invalidFiles;
    foreach( QString file, photoList ){
        if( !add( file ) ) {
            invalidFiles << file;
        }
    }

    //emit updatedProperties( propertiesList() );
    //m_model.setStringList( m_orderedKeys );
    return invalidFiles;
}


/*******************************************************************
* refresh( const QStringList &)
* ---------
* Refresh the db using this new set of files. Captions and ids of the
* files already present are preserved. Files not present in the provided
* list are removed from the database.
* In: (QStringList&) absolute file paths composing the new file set
* Returns invalid files.
********************************************************************/
QStringList CPhotoDatabase::refresh( const QStringList & fileSet )
{
    QStringList invalidFiles;
    QStringList removedfiles;
    QStringList fileNameSet;

    //Extracting the file names from the file paths
    foreach( QString filePath, fileSet )  {
        fileNameSet << QFileInfo( filePath ).fileName( );
    }

    //1 - Removing non-provided files from the db
    foreach( CPhotoProperties* properties, m_db )  {
        QString fileName = properties->fileInfo().fileName();
        if( !fileNameSet.contains( fileName ) ) {
            remove( fileName );
            removedfiles << fileName;
        }
    }
    //2 - adding provided files. Some properties of preexisting files are preserved.
    //    see add( QString ) definition.  
    foreach( QString filePath, fileSet )    {
        if( !add( filePath) ) {
            invalidFiles << filePath;
        }
    }

    //3 - Emit an info for the file removed from the db
    if( !removedfiles.isEmpty() )
    {
        QString details;
        foreach( QString file, removedfiles ) {
            details.append( file + QString('\n') );
        }
        CMessage info( CMessage::message(CMessage::Info_RemovingPhotos), QString(), details );
        emit message( info );
    }
    
    //4 - Emit an error if there are invalid Files
    if( !invalidFiles.isEmpty() )
    {
        QString details;
        foreach( QString file, invalidFiles ) {
            details.append( file + QString('\n') );
        }
        CMessage err( tr("Some photos are invalid."), QString(), details );
        emit error( err );
    }


    consolidate(); //removes invalidFiles
    //m_model.setStringList( m_orderedKeys );
    return invalidFiles;
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
        if( !fileInfo.isReadable() ) {
            missingPhotos << fileInfo.absoluteFilePath();
        }
    }

    if( !missingPhotos.isEmpty() ){
        QString summary = CError::error(CError::InvalidFiles);
        QString details;
        foreach( QString file, missingPhotos ){
            details += file + QString("\n");
        }
        emit error( CMessage(summary, QString(), details) );
    }
    
    return missingPhotos;
}


/*******************************************************************
* add( const QString & ); 
* ---------
* adds a photo at the end of the ordered list. If the filename is already
* present, updates its files properties but keep the other properties.
* In : (QString) absolute file path
* Returns false if the file is invalid or non-readable
********************************************************************/
bool CPhotoDatabase::add( const QString& photoPath)
{
    QFileInfo fileInfo(photoPath);    
    bool returnedValue = ( fileInfo.exists() &&  fileInfo.isReadable() );

    if( returnedValue )
    {
        QStringList orderedKeys = m_model.stringList();
        QString filename = fileInfo.fileName();
        if( !orderedKeys.contains(filename)  )
        {
            CPhotoDatabaseElem* photoElem = new CPhotoDatabaseElem();
            int id = orderedKeys.size();    
            
            CCaption caption;
            caption.setFileInfo( fileInfo );
            caption.setId( id + 1);
            
            photoElem->setCaption( caption );
            photoElem->setId( id );
            photoElem->setFileInfo( fileInfo );
            photoElem->setLastModificationTime( fileInfo.lastModified() );
            m_db.insert( fileInfo.fileName(), photoElem );
            orderedKeys << fileInfo.fileName();
            m_model.setStringList( orderedKeys );
        }
        else { //update of the file's properties
            CPhotoDatabaseElem* photoElem = m_db.value( filename );
            photoElem->setFileInfo( fileInfo );
            photoElem->setLastModificationTime( fileInfo.lastModified() );
            //keep the caption in sync with new properties
            CCaption caption = photoElem->caption();
            caption.setFileInfo( fileInfo );
            photoElem->setCaption( caption );
        }
    }

    return returnedValue;
}


/*******************************************************************
* add( const CPhotoPropertiesExtended & ); 
* ---------
* adds a photo at the end of the ordered list. If the filename is already
* present, erase it before adding the new properties WITH THE SAME ID.
* In : (CPhotoPropertiesExtended) properties of the photo
* Returns false if the file is invalid or non-readable
********************************************************************/
bool CPhotoDatabase::add( const CPhotoPropertiesExtended& properties)
{
    QFileInfo fileInfo = properties.fileInfo();
    bool returnedValue = ( fileInfo.exists() &&  fileInfo.isReadable() );

    if( returnedValue ) 
    {
        int id;
        QString filename = fileInfo.fileName();
        QStringList orderedKeys = m_model.stringList();
        
        if( orderedKeys.contains( filename ) )
        {
            CPhotoDatabaseElem* oldElem = m_db.value( filename );
            id = oldElem->id();
            m_db.remove( filename );
            delete oldElem;
        }
        else {
             id = orderedKeys.size();
             orderedKeys << fileInfo.fileName();
             m_model.setStringList( orderedKeys );
        }
        
        CPhotoDatabaseElem* photoElem = new CPhotoDatabaseElem( properties );
        photoElem->setId( id );
        //keeping the caption in sync with new properties
        CCaption caption = photoElem->caption();
        caption.setId( id + 1);
        photoElem->setCaption( caption );
        //adding to db
        m_db.insert( filename, photoElem );        
    }

    return returnedValue;
}



/*******************************************************************
* consolidate(); 
* ---------
* Removes files present in the db but not on the disk
********************************************************************/
void CPhotoDatabase::consolidate( void )
{
    QStringList missingFiles = checkPhotosInDb();
    foreach( QString file, missingFiles ) {
        remove( QFile( file ).fileName() );
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
    if( m_db.contains( filename ) ) 
    {
        QStringList orderedKeys = m_model.stringList();
        CPhotoDatabaseElem* elem = m_db.value( filename );
        int id = elem->id();
        orderedKeys.removeAt( id );
        //Removing the element from the db and decrementing subsequent ids
        QMap<QString,CPhotoDatabaseElem*>::iterator it = m_db.find( filename );        
        while( ++it != m_db.end() ){
            CPhotoPropertiesExtended* properties = it.value();
            int id = properties->id() - 1;
            properties->setId( id );
            //keeping the caption in sync
            CCaption caption = properties->caption();
            caption.setId( id + 1);
            properties->setCaption( caption );
        }
        m_db.remove( filename );        
        //Deleting the element itself
        delete elem;
        m_model.setStringList( orderedKeys );
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
    QStringList orderedKeys = m_model.stringList();
    const QString filename = orderedKeys.at( id ); //we could use m_model.data( QModelIndex ).toString() but I don't understand how to use QModelIndex
    remove( filename );
    //emit updatedProperties( propertiesList() );
}


/*******************************************************************
* remove( int ); 
* ---------
* remove one photo from the db
* In : (int) id
********************************************************************/
void CPhotoDatabase::clear( )
{
    QStringList orderedKeys = m_model.stringList();
    foreach( CPhotoDatabaseElem* element, m_db) {
        delete element;
    }
    m_db.clear();
    orderedKeys.clear();
    m_model.setStringList( orderedKeys ); //direct use of m_model.reset() possible ??
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
        //Syncing the captions properties
        CCaption caption =  elem1->caption();
        caption.setId( id2 + 1);
        elem1->setCaption( caption );
        caption =  elem2->caption();
        caption.setId( id1 + 1);
        elem2->setCaption( caption );
        //Swaping in model
        QStringList orderedKeys = m_model.stringList();
        orderedKeys.swap( id1, id2 );
        m_model.setStringList( orderedKeys );

     }
     //emit updatedProperties( propertiesList() );
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
    QStringList orderedKeys = m_model.stringList();
    const QString& filename1 = orderedKeys.at( id1 );
    const QString& filename2 = orderedKeys.at( id2 );
     if( m_db.contains( filename1 ) && m_db.contains( filename2 )  )
     {
        //Swaping in main db
        CPhotoDatabaseElem* elem1 = m_db.value( filename1 );
        CPhotoDatabaseElem* elem2 = m_db.value( filename2 );
        elem1->setId( id1 );
        elem2->setId( id2 );
        //Syncing the captions properties
        CCaption caption =  elem1->caption();
        caption.setId( id2 + 1);
        elem1->setCaption( caption );
        caption =  elem2->caption();
        caption.setId( id1 + 1);
        elem2->setCaption( caption );
        //Swaping in keylist
        orderedKeys.swap( id1, id2 );
        m_model.setStringList( orderedKeys );
     }
     //emit updatedProperties( propertiesList() );
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
                CMessage msg( QString("[Thumbnail]." ) + err );
                emit error( msg );
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
    QStringList orderedKeys = m_model.stringList();
    const QString& filename = orderedKeys.at( id ); //m_model.data() ??
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
    QStringList orderedKeys = m_model.stringList();
    const QString& filename = orderedKeys.at( id ); //m_model.data() ???
    if( m_db.contains( filename ) ) {
       CPhotoDatabaseElem* elem = m_db.value( filename );
       return elem->m_thumbnail;
    }
    else {
       return s_defaultThumbnail;
    }
}


 /*******************************************************************
* properties( int ); 
* ---------
* Returns the properties of the specified photo
* Returns : (CPhotoProperties*)
********************************************************************/
CPhotoProperties* CPhotoDatabase::properties( int id )
{
    return m_db.value( m_model.data( m_model.index( id ), Qt::DisplayRole ).toString() );
}


/*******************************************************************
* properties( const QString & );
* ---------
* Returns the properties of the specified photo
* Returns : (CPhotoProperties*)
********************************************************************/
CPhotoProperties* CPhotoDatabase::properties( const QString & fileName )
{
   return m_db.value( fileName );
}
