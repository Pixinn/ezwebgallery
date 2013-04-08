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

#include <QDebug>
#include <QColor>
#include <QFont>
#include <QPainter>
#include <QRect>
#include <QPen>
#include <QColor>

#include "CPlatform.h"
#include "CPhotoDatabase.h"
#include "CPhoto.h"
#include "CError.h"
#include "CWarning.h"



/***************************************** CDefaultThumbnail CLASS ************************************/

CPhotoDatabase::CPhotoDatabaseElem::CDefaultThumbnail CPhotoDatabase::CPhotoDatabaseElem::CDefaultThumbnail::s_instance( QSize(THUMBNAIL_WIDTH,THUMBNAIL_HEIGHT) );
const int CPhotoDatabase::CPhotoDatabaseElem::CDefaultThumbnail::s_fontSize = 20;
CPhotoDatabase::CPhotoDatabaseElem* CPhotoDatabase::s_defaultElem = NULL;

//Constructor
CPhotoDatabase::CPhotoDatabaseElem::CDefaultThumbnail::CDefaultThumbnail( QSize size ) :
    QImage( size, QImage::Format_ARGB32 )
{

}

void CPhotoDatabase::CPhotoDatabaseElem::CDefaultThumbnail::init( void )
{
    fill( Qt::transparent );

    QFont font = CPlatform::defaultFont();
    font.setPointSize( s_fontSize );

    QPainter painter( this );
    painter.setFont( font );
    painter.setPen( QPen( QColor( Qt::darkBlue ) ) );

    QRect rect( 0, 0, this->width(), this->height() );
    painter.drawText( rect, Qt::AlignCenter | Qt::AlignVCenter, tr("Loading preview...") );
}

/***************************************** STATIC VARS ************************************/

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
* init( )
* ---------
* Inits the database.
* Calling this function before using the singleton is MANDATORY !
* Executes code that cannot be executed during *static* instanciation
********************************************************************/
void CPhotoDatabase::init( )
{
    if( !f_initialized )
    {
        f_initialized = true;
        connect(  &m_model, SIGNAL(rowsRemoved (  QModelIndex , int,  int )), \
                    this, SLOT(rowRemoved ( QModelIndex , int,  int ) ) );
        CPhotoDatabaseElem::CDefaultThumbnail::getInstance().init();
        s_defaultElem = new CPhotoDatabaseElem();
        m_loader.init();
        connect( &m_loader, SIGNAL( loaded(const CLoadedThumbnail & ) ),
                 this, SLOT( thumbnailLoaded( const CLoadedThumbnail & ) ) );
    }
}




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
    saveState();
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
        CPhotoProperties properties( node ); //this constructor sets the caption!
        QFileInfo fileInfo( node.firstChildElement( XMLTAG_FILEPATH ).text() );
        //properties.setLastModificationTime( QDateTime::fromString( node.firstChildElement( XMLTAG_LASTMODIFICATIONTIME ).text() ));
        properties.setFileInfo( fileInfo );
        properties.setId( iteratorDomList );
        if ( !add( properties ) )   {
            invalidFiles << properties.fileInfo().absoluteFilePath();
        }
    } 
    saveState();
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
        CPhotoProperties properties( node ); //this constructor sets the caption
        //file properties
        QFileInfo fileInfo( inputDir.absoluteFilePath( node.firstChildElement( XMLTAG_DEPRECATED_FILENAME ).text() ) );
       // properties.setLastModificationTime( QDateTime::fromString( node.firstChildElement( XMLTAG_LASTMODIFICATIONTIME ).text() ));        
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
    saveState();
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
    QStringList orderedList = m_model.stringList();

    foreach( QString photoName, orderedList )
    {
        CPhotoProperties* properties = m_db.value( photoName );
        QDomElement photoElement = document.createElement( XMLTAG_PHOTOS );
        database.appendChild( photoElement );
        //filePath
        QDomElement filePath = document.createElement( XMLTAG_FILEPATH );
        photoElement.appendChild( filePath );
        filePath.appendChild( document.createTextNode( properties->fileInfo().absoluteFilePath() ) );
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

void CPhotoDatabase::rowRemoved( const QModelIndex&, int, int )
{
    CPhotoDatabaseElem* elem;
    int id = 0;
    QStringList orderedKeys = m_model.stringList();
    foreach( QString key, orderedKeys ) 
    {
        elem = m_db.value( key );
        elem->setId( id );
        //keeping the caption in sync
        CCaption caption = elem->caption();
        caption.setId( id + 1);
        elem->setCaption( caption );
        id++;
    }
    emit layoutChanged();
}


/*******************************************************************
* thumbnailLoaded( const CLoadedThumbnail )
* ---------
* A thumbnail as been loaded by the asynchronous loader
********************************************************************/
void CPhotoDatabase::thumbnailLoaded( const CLoadedThumbnail & thumbnail )
{
    QFileInfo fileInfo( thumbnail.getFilePath() );
    QString fileName = fileInfo.fileName();
    CPhotoDatabaseElem* elem = m_db.value( fileName, NULL );
    if( elem != NULL ) { //Keeping this test is ESSENTIAL: as a flushed loader can emit its last thumbnail afterwards
        elem->setExifTags( thumbnail.getExifTags() );
        elem->setFileInfo( fileInfo );
        elem->m_thumbnail = thumbnail.getImage();
        emit thumbnailLoaded( elem->id() );
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
    QStringList newFiles;
    QStringList removedfiles;
    QStringList fileNameSet;



    //Extracting the file names from the file paths
    foreach( QString filePath, fileSet )  {
        fileNameSet << QFileInfo( filePath ).fileName( );
    }

    //1 - Removing non-provided files from the db
    //Building a list of the photos missing from the new fileset
    foreach( CPhotoProperties* properties, m_db )  {
        QString fileName = properties->fileInfo().fileName();
        if( !fileNameSet.contains( fileName ) ) {
            removedfiles << fileName; 
        }
    }
    //Removing these photos
    remove( removedfiles );

    //2 - adding provided files. Some properties of preexisting files are preserved.
    //    see add( QString ) definition.  
    foreach( QString filePath, fileSet )
    {
        QString fileName = QFileInfo( filePath ).fileName( );
        if( !m_db.contains( fileName ) ) {
            newFiles << fileName;
            if( !add( filePath ) ) {
                invalidFiles << filePath;
            }
        }

    }

    //3 - Emit a warning for the file removed from the db
    if( !removedfiles.isEmpty() )
    {
        QString details;
        foreach( QString file, removedfiles ) {
            details.append( file + QString('\n') );
        }
        details.remove( details.lastIndexOf('\n'), 1);
        emit warning( PtrMessage( new CWarning( CMessage::message(CMessage::Info_RemovingPhotos), details ) ) );
    }
    
    //4 - Emit an error if there are invalid Files
    if( !invalidFiles.isEmpty() )
    {
        QString details;
        foreach( QString file, invalidFiles ) {
            details.append( file + QString('\n') );
        }
        details.remove( details.lastIndexOf('\n'), 1);
        emit error( PtrMessage(new CError( CError::error(CError::InvalidFiles), details )) );
    }


    consolidate(); //removes invalidFiles
    //m_model.setStringList( m_orderedKeys );
    return newFiles;
}



/*******************************************************************
*  updateFileInfo( const QString & )
* ---------
* Updates the file info of the specified element.
* Returns true if success, false otherwise (ie element not present in db)
********************************************************************/
bool CPhotoDatabase::refreshFileInfo( const QString & filename )
{
    bool f_result = false;

    if( m_db.contains( filename ) ) {
        CPhotoDatabaseElem* elem = m_db.value( filename );
        elem->refreshFileInfo();
        f_result = true;
    }

    return f_result;
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
        emit error( PtrMessage( new CError(summary, details) ) );
    }
    
    return missingPhotos;
}



/*******************************************************************
* photosModified( void )
* ---------
* Returns a list of the photos removed or modified
* Return : (QStringList)
********************************************************************/
QStringList CPhotoDatabase::photosModified( void ) const
{
    QStringList modifiedPhotos;

    //Missing and modified photos
    foreach( CPhotoProperties* properties, m_db )
    {

        QFileInfo fileInfo( properties->fileInfo().absoluteFilePath() ); 

        QString dbg1 = fileInfo.lastModified().toString();
        QString dbg2 = properties->lastModified().toString();

        if( !fileInfo.exists()  ||                                                             //File removed
            ( fileInfo.lastModified().toString() != properties->lastModified().toString() ) )  //File modified
        {
            modifiedPhotos << fileInfo.fileName();
        }
    }
    
    return modifiedPhotos;
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

            m_db.insert( fileInfo.fileName(), photoElem );
            orderedKeys << fileInfo.fileName();
            m_model.setStringList( orderedKeys );

            loadThumbnail( filename );
        }

    }

    return returnedValue;
}


/*******************************************************************
* add( const CPhotoProperties & ); 
* ---------
* adds a photo at the end of the ordered list. If the filename is already
* present, erase it before adding the new properties WITH THE SAME ID.
* In : (CPhotoProperties) properties of the photo
* Returns false if the file is invalid or non-readable
********************************************************************/
bool CPhotoDatabase::add( const CPhotoProperties& properties )
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
        //Instanciating the element
        CPhotoDatabaseElem* photoElem = new CPhotoDatabaseElem( properties );
        photoElem->setId( id );

        //adding to db
        m_db.insert( filename, photoElem );

        //async thumbnail loading
        loadThumbnail( filename ); //MUST be placed afert insert: the element has ti be present in the db
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

        //Updating subsequent ids: processing in order!
        QStringList::Iterator it = orderedKeys.begin() + id;
        while( ++it < orderedKeys.end() )
        {
            QString filename = *it;
            CPhotoProperties* properties = m_db.value( filename );
            int newId = properties->id() - 1;
            properties->setId( newId );
            CCaption caption = properties->caption();
            caption.setId( newId + 1);
            properties->setCaption( caption );
        }

        //removing from db
        m_db.remove( filename );
        orderedKeys.removeAt( id );  
        m_model.setStringList( orderedKeys );  
        //removing from the thumb loader
        m_loader.stopLoading( elem->fileInfo().absoluteFilePath() );
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
    QStringList orderedKeys = m_model.stringList();
    const QString filename = orderedKeys.at( id ); //we could use m_model.data( QModelIndex ).toString() but I don't understand how to use QModelIndex
    remove( filename );
    //emit updatedProperties( propertiesList() );
}


/*******************************************************************
* remove( const QString & ); 
* ---------
* remove a list of photos from the db
* In : (QStringList) fileList
********************************************************************/
void CPhotoDatabase::remove( const QStringList & fileList )
{
    QStringList orderedKeys = m_model.stringList();
    foreach( QString photo, fileList )
    {
       if( m_db.contains( photo ) )
       {
           //removing corresponding properties from the database
           orderedKeys.removeAll( photo );
           CPhotoProperties* properties = m_db.value( photo );                      
           m_loader.stopLoading( properties->fileInfo().absoluteFilePath() );// Stop thumbnail loading 
           m_db.remove( photo );
           delete properties;
       }
    }
    m_model.setStringList( orderedKeys );

    //Reordering the remaining properties
    int id = 0;
    QStringList::Iterator it = orderedKeys.begin();
    while( it < orderedKeys.end() )
    {
         //updating id
         CPhotoProperties* properties = m_db.value( *it );
         properties->setId( id );
         CCaption caption = properties->caption();
         caption.setId( id + 1);
         properties->setCaption( caption );
         //next
         id++;
         it++;
    }

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
    m_loader.flush();
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
bool CPhotoDatabase::loadThumbnail( const QString & filename, CThumbnailLoadingManager::e_Priority priority )
{
    CPhoto photo;    
    bool f_success = true;
    CPhotoDatabaseElem* elem = m_db.value( filename, NULL );
    
    if( elem == NULL ) {
        return false;
    }

    //Load only if the thumbnail is not already loaded
    if( elem->m_thumbnail == CPhotoDatabaseElem::CDefaultThumbnail::getInstance() )
    {
        QString absoluteFilePath = elem->fileInfo().absoluteFilePath();
        m_loader.load( absoluteFilePath, priority );
        /*f_success = photo.load( elem->fileInfo().absoluteFilePath() );
    
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
            if( !photo.errors().isEmpty() )
            {
                foreach( QString err, photo.errors() ){
                    CMessage msg( QString("[Thumbnail]." ) + err );
                    emit error( msg );
                }
                f_success = false;
            }
        }
        */
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
bool CPhotoDatabase::loadThumbnail( int id, CThumbnailLoadingManager::e_Priority priority )
{
    QStringList orderedKeys = m_model.stringList();
    const QString& filename = orderedKeys.at( id ); //m_model.data() ??
    return loadThumbnail( filename, priority );
} 
 
 

 /*******************************************************************
* thumbnail( const QString & filename ); 
* ---------
* Returns the thumnail of the element defined by its filename
* In : (int) element id
* Returns : thumbnail
********************************************************************/
QImage& CPhotoDatabase::thumbnail( const QString & filename  )
{
     if( m_db.contains( filename ) ) {
        CPhotoDatabaseElem* elem = m_db.value( filename );
        return elem->m_thumbnail;
     }
     else {
        return CPhotoDatabaseElem::CDefaultThumbnail::getInstance();
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
       //We should avoid entering here!
       return CPhotoDatabaseElem::CDefaultThumbnail::getInstance();
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
    return m_db.value( m_model.data( m_model.index( id ), Qt::DisplayRole ).toString(), s_defaultElem );
}


/*******************************************************************
* properties( const QString & );
* ---------
* Returns the properties of the specified photo
* Returns : (CPhotoProperties*)
********************************************************************/
CPhotoProperties* CPhotoDatabase::properties( const QString & fileName )
{
   return m_db.value( fileName, s_defaultElem );
}


/*******************************************************************
* id( const QString & );
* ---------
* Returns the id of the specified photo. -1 if not present in the db
* Returns : (int)
********************************************************************/
int CPhotoDatabase::id( const QString & fileName ) const
{
    int id = -1;
    if( m_db.contains( fileName ) ) {
        id = m_db.value( fileName )->id();
    }
    return id;
}
