﻿/*
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


#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QImageReader>
#include <QImage>
#include <QQueue>
#include <QMetaType>
#include <QMapIterator>
#include <QMessageBox>
#include <QtAlgorithms>

#include <cmath>


#include "CGalleryGenerator.h"
#include "CPhoto.h"
#include "CTaggedString.h"
#include "CPlatform.h"
#include "CCss.h"
#include "CDirChecker.h"

using namespace Magick;
using namespace JSON;


///////////////////////////////////////////////////////////////////
//////////////////// CLASSE CGalleryGenerator /////////////////////
///////////////////////////////////////////////////////////////////


//------------------ DEFINITIONS ----------------------//

#define CSSSKINFILENAME     "skin.css"
#define SHARPENINGTHRESHOLD 0.04        /* équivalent à 10 */

//Thubnail mosaic targeted sizes
const CGalleryGenerator::t_thumbSize CGalleryGenerator::s_thumbMosaicSizes[ s_nbMosaicSizes ] = {
    {900,568},
    {1130,670},
    {1270,780},
    {1690,1140}
};

//-------------------- FONCTIONS -----------------------//


CGalleryGenerator::CGalleryGenerator( ) :
    QThread()
{
    qRegisterMetaType<CGeneratedPhotoSetParameters> ( "CGeneratedPhotoSetParameters" );

    //--- Init de la machine Etat
    //Etats
    m_p_waitingForOrders = new QState();
    m_p_galleryGeneration = new QState();
    m_p_abording = new QState();
    m_p_init = new QState( m_p_galleryGeneration );
    m_p_generatingFileStructure = new QState( m_p_galleryGeneration );
    m_p_generatingPhotos = new QState( m_p_galleryGeneration );
    m_p_generatingJSFiles = new QState( m_p_galleryGeneration );
    m_p_skinning = new QState( m_p_galleryGeneration );
    m_p_galleryGeneration->setInitialState( m_p_init );   
    //Transitions
    m_p_galleryGeneration->addTransition( this, SIGNAL( abordGenerationSignal( )), m_p_abording);
    m_p_waitingForOrders->addTransition( this, SIGNAL(startGenerationSignal()), m_p_galleryGeneration);
    m_p_init->addTransition( this, SIGNAL(jobDone()), m_p_generatingFileStructure);
    m_p_generatingFileStructure->addTransition( this, SIGNAL(jobDone()), m_p_generatingPhotos);
    m_p_generatingPhotos->addTransition( this, SIGNAL(jobDone()), m_p_generatingJSFiles);
    m_p_generatingJSFiles->addTransition( this, SIGNAL(jobDone()), m_p_skinning);
    m_p_skinning->addTransition( this, SIGNAL(jobDone()), m_p_waitingForOrders);    
    m_p_abording->addTransition( this, SIGNAL(abordGenerationSignal( )), m_p_abording);
    m_p_abording->addTransition( this, SIGNAL(jobDone()), m_p_waitingForOrders);
    //Connections Etats<->Traitements
    connect( m_p_init, SIGNAL(entered()), this, SLOT( initGeneration() ) );
    connect( m_p_generatingFileStructure, SIGNAL(entered()), this, SLOT( generateFileStructure() ) );
    connect( m_p_generatingPhotos, SIGNAL(entered()), this, SLOT( generatePhotos() ) );
    connect( m_p_generatingJSFiles, SIGNAL(entered()), this, SLOT( generateJsFiles() ) );
    connect( m_p_skinning, SIGNAL(entered()), this, SLOT( skinning() ) );
    connect( m_p_abording, SIGNAL(entered()), this, SLOT( onAbordGeneration() ) );
    //Finalisation    
    m_stateMachine.addState( m_p_waitingForOrders );
    m_stateMachine.addState( m_p_galleryGeneration );
    m_stateMachine.addState( m_p_abording );
    m_stateMachine.setInitialState( m_p_waitingForOrders );
    //---

    m_nbPhotosToProcess = 0;
    m_nbPhotosProcessed = 0;
    m_nbPhotoProcessFailed = 0;
    m_f_WorkInProgress = false;
    m_fStopRequested = false;
    m_p_photoProcessorPool = new QThreadPool( this );
    //m_p_photoProcessorPool->setMaxThreadCount( std::min(4,QThread::idealThreadCount() ) ); //above 4 threads, might be counterprodictive SMT
    m_errorMsg = tr("No error.");

    start();	//Lancement du thread
}

CGalleryGenerator::~CGalleryGenerator( )
{
	m_stateMachine.stop();
    delete m_p_photoProcessorPool;
}


/************ Interfaage avec UI ************/


/************************************
* REMARQUE : c'est ici qu'est ordonne la liste
* des photos  traiter, qui donnera l'ordre d'affichage
* des photos de la galerie
************************************/
bool CGalleryGenerator::generateGallery( CProjectParameters &projectParameters, const CSkinParameters &skinParameters, QList<CPhotoProperties*> photoProperties )
{
	QStringList photoList;
	QString photoName;
    bool f_success = false;
    
    //Copie des paramtres en local avant lancement du thread
    if( !this->isGenerationInProgress() && !photoProperties.isEmpty() )
    {
        CCaption emptyCaption;
		QDir inputDir;
        QFileInfo fileInfo;

        //-- Init
        m_parameters = projectParameters;
        m_parameters.m_galleryConfig.description.replace("\n"," ");
        m_skinParameters = skinParameters;
		
		inputDir = m_parameters.m_galleryConfig.inputDir;

        displayProgressBar( 0, "green", "0%" );

        m_photoPropertiesList.clear();
        foreach( CPhotoProperties* properties, photoProperties )    {
            m_photoPropertiesList << *properties;
        }
    
        emit startGenerationSignal(); //Dmarrage des traitements

        f_success = true;
    }
    
    return f_success;

}

//Permet d'appeler onAbordGeneration de manire ASYNCHRONE  partir du thread de l'UI
void CGalleryGenerator::abordGeneration( )
{
    emit abordGenerationSignal( );
}


bool CGalleryGenerator::isGenerationInProgress( )
{
    bool isRunning;
    m_mutex.lock();
    isRunning = m_f_WorkInProgress;
    m_mutex.unlock();
    return isRunning;
}

void CGalleryGenerator::debugDisplay( QString msg )
{
    emit debugSignal( msg );
}

void CGalleryGenerator::displayProgressBar( int completion, QString color, QString message )
{
    emit progressBarSignal( completion, color, message );
}

/***********************************************************************/

/**************************************
* checkImages(  )
*
* Returns true if the photos and the thumbnails tobe generated
* are present in the proper dirs
***************************************/
bool CGalleryGenerator::areImageAndThumbs(  )
{       
    QStringList photosNames;
    foreach( CPhotoProperties prop, m_photoPropertiesList ) {
        photosNames << prop.encodedFilename();
    }
    QStringList photoSubDirs;
    for( int i = 1; i <= m_parameters.m_photosConfig.nbIntermediateResolutions; i++ ) {
        photoSubDirs << ( RESOLUTIONPATH + QString::number(i) );
    }
    QStringList thumbSubDirs;
    for( int i = 1; i <= s_nbthumbRes; i++ ) {
        thumbSubDirs << ( RESOLUTIONPATH + QString::number(i) );
    }
    QDir photosDir( m_parameters.m_galleryConfig.outputDir );
    CDirChecker photoCheck( photosDir.absoluteFilePath( PHOTOSPATH ) );
    QDir thumbsDir( m_parameters.m_galleryConfig.outputDir );
    CDirChecker thumbsCheck( thumbsDir.absoluteFilePath( THUMBSPATH ) );

    return ( photoCheck.areFilesInSubdir( photosNames, photoSubDirs ) &&  thumbsCheck.areFilesInSubdir( photosNames, thumbSubDirs ) );
}



/**************************************
* computeThumbSizes(  )
*
* Computes the size of the thumbnails to be generated
* Returns: Computed sizes. Key: folder to save.
***************************************/
QMap<QString,QSize> CGalleryGenerator::computeThumbSizes( void )
{
  QList<int> sizeList; //in this "size" list width = height
  unsigned int nbCols =  m_parameters.m_thumbsConfig.nbColumns;
  unsigned int nbRows = m_parameters.m_thumbsConfig.nbRows;
  QSize unavailableSpace = m_skinParameters.unavailableSpace( nbCols, nbRows );
  //Iterating on the mosaic size constraints
  for( int i = 0; i < s_nbMosaicSizes; i++ ) {
      sizeList << ( s_thumbMosaicSizes[i].width - unavailableSpace.width() ) / nbCols;
      sizeList << ( s_thumbMosaicSizes[i].height - unavailableSpace.height() ) / nbRows;
  }
  //Downward sorting sizes 
  qSort( sizeList.begin(), sizeList.end(), qGreater<int>() );
  //returning sizes
  int n = 1;  //folder appendix
  QMap<QString,QSize> sizes;
  foreach( int size, sizeList ) {
      sizes.insert( QString(RESOLUTIONPATH) + QString::number(n++), QSize( size, size ) );
  }

  return sizes;
}


/**************************************
* computePhotoSizes(  )
*
* Computes the size of the photos to be generated
* Returns: Computed sizes. Key: folder to save.
***************************************/
QMap<QString,QSize> CGalleryGenerator::computePhotoSizes( void )
{
    int n = 1; //folder appendix
    QMap<QString,QSize> sizes;
    //max size
    sizes.insert( QString(RESOLUTIONPATH) + QString::number(n++),
                  QSize( m_parameters.m_photosConfig.maxSizeW, m_parameters.m_photosConfig.maxSizeH ) );
    //intermediate sizes
    if( m_parameters.m_photosConfig.nbIntermediateResolutions > 2){
        int spaceW;
        int spaceH;
        int nbRes = m_parameters.m_photosConfig.nbIntermediateResolutions - 2;
        spaceW = (m_parameters.m_photosConfig.maxSizeW - m_parameters.m_photosConfig.minSizeW)/(nbRes+1);
        spaceH = (m_parameters.m_photosConfig.maxSizeH - m_parameters.m_photosConfig.minSizeH)/(nbRes+1);
        for(int i = 1; i <= nbRes; i++){
            sizes.insert( QString(RESOLUTIONPATH) + QString::number(n++),
                          QSize( m_parameters.m_photosConfig.maxSizeW - i*spaceW, m_parameters.m_photosConfig.maxSizeH - i*spaceH ) );
        }
    }
    //min size
    sizes.insert( QString(RESOLUTIONPATH) + QString::number(n++),
                  QSize( m_parameters.m_photosConfig.minSizeW, m_parameters.m_photosConfig.minSizeH ) );

    return sizes;
}


/***************************************************************************************************/
//////////////////////////////////////////////////////////
///////////////////////// RUN ////////////////////////////
//////////////////////////////////////////////////////////
void CGalleryGenerator::run( )
{
    m_stateMachine.start();
    exec();
}


//////////////////////////////////////////////////////////
//////////////// initGeneration  /////////////////////////
//////////////////////////////////////////////////////////
void CGalleryGenerator::initGeneration( )
{
	//Normalement les mutex ne servent  rien ici
    m_mutex.lock();
		m_f_WorkInProgress = true;
		m_nbPhotosToProcess = 0;
		m_nbPhotosProcessed = 0;
		m_nbPhotoProcessFailed = 0;                
		m_msgErrorList.clear();
    m_mutex.unlock();
    m_mutexControlProcessors.lock();
		m_fStopRequested = false;
    m_mutexControlProcessors.unlock();        

    emit jobDone();
}


//////////////////////////////////////////////////////////
//////////////// generateFileStructure ///////////////////
//////////////////////////////////////////////////////////
bool CGalleryGenerator::generateFileStructure( )
{
   displayProgressBar( 0, "green", tr("Generating the file structure...") );

    //--- Photos
    const QString photosFilesPath( QString(PHOTOSPATH) + "/res" );
    const QString thumbsFilesPath( THUMBSPATH );
    QDir outPath( m_parameters.m_galleryConfig.outputDir );
    QDir srcPath( CPlatform::applicationDirPath()  );

    if ( !outPath.mkpath( thumbsFilesPath ) ){
        m_msgErrorList.append( CError::error(CError::DirectoryCreation) + thumbsFilesPath );
        emit abordGenerationSignal( );
        return false;
    }
    for(int i = 1; i <= m_parameters.m_photosConfig.nbIntermediateResolutions; i++){
        if( !outPath.mkpath( photosFilesPath + QString::number(i) ) ){
            m_msgErrorList.append( CError::error(CError::DirectoryCreation) + photosFilesPath + QString::number(i) );
            emit abordGenerationSignal( );
            return false;
        }
    }

    //Copie des fichiers ressources (ie non modifiable, ne faisant pas partie des skins)
    outPath = QDir( m_parameters.m_galleryConfig.outputDir );
    srcPath = CPlatform::applicationDirPath();
    if( !srcPath.cd( QString(RESSOURCES) ) ){
        m_msgErrorList.append(  CError::error(CError::SourceFileNotFound) );
        emit abordGenerationSignal( );
        return false;
    }
    QString copyErrorMessage;
    if( !CPlatform::copyDirectory( srcPath, outPath, copyErrorMessage ) ){
        m_msgErrorList.append( copyErrorMessage );
        emit abordGenerationSignal( );
        return false;
    }

    emit jobDone();
    
    return true;

}



///////////////////////////////////////////////////
//////////////// generatePhotos ///////////////////
///////////////////////////////////////////////////
int CGalleryGenerator::generatePhotos( )
{
    //Pas besoin de regnrer les photos
    if( !m_parameters.m_photosConfig.f_regeneration &&
        !m_parameters.m_thumbsConfig.f_regeneration &&
        areImageAndThumbs() )
    {
        emit jobDone();
        return 0;
    }//Attention cela ne vrifie pas la prsence de la vignette reprsentant la gallerie

    QMap<QString,QSize> photoSizes;
    QQueue<int> qualityList;
    QDir outPath( m_parameters.m_galleryConfig.outputDir );

    //Gnration de la vignette reprsentant la galerie
    //Cette gnration est effectue  part de celle des autres vignettes et des photos
    QImage photo;
    QImage thumbnail;
    QString galleryThumbnailFile =  m_parameters.m_galleryConfig.thumbPhoto;
    if( !QFileInfo( m_parameters.m_galleryConfig.thumbPhoto ).exists() )  {
        galleryThumbnailFile = m_photoPropertiesList.at(0).fileInfo().absoluteFilePath();
    }
    outPath.cd( PHOTOSPATH );
    if( photo.load( galleryThumbnailFile )){
        thumbnail = photo.scaled( QSize(250,250),
                                  Qt::KeepAspectRatio,
                                  Qt::SmoothTransformation );
        thumbnail.save( outPath.absoluteFilePath( GALLERYTHUMBFILENAME ), 0, 85 );
    }
    else {
        //TODO : log an error
    }
    

    //Gnration du watermark si besoin
    CWatermark watermarkImage;
    t_watermark watermarkParams = m_parameters.m_photosConfig.watermark;
    watermarkImage.setParameters( watermarkParams );
    if( watermarkParams.enabled )
    {        
        //Cration du watermark. Deux possibilits : une image ou du texte
        //Si image: on le cr ici une unique fois
        //Si texte, le photoProcessor le verra via les paramtres et le gnrera afin d'avoir accs aux tags exif et autres        
        if( watermarkParams.type == t_watermark::IMAGE ){
            watermarkImage.setImage( watermarkParams.imageFile );
            //Vrification de la validit du watermark
            if( !watermarkImage.isValid() ){
                m_msgErrorList.append( CError::error(CError::WatermarkInvalid) + watermarkImage.error() );
                emit abordGenerationSignal( );
                return false;
            }
        }       

    }
    
    m_photoSizes.clear();//Les tailles des photos qui seront gnres
    CPhotoProcessor* photoToProcess;
    int idPhotoToProcess = 1;
    
    //- Cration de la file des tailles des photos  gnrer:
    //- IMPORTANT : L'ordre doit correspondre  l'ordre des traitements au sein de CPhotoProcessor
    //- tailleMax, ..., tailleMin, tailleVignette  
    photoSizes = computePhotoSizes();
    m_thumbSizes = computeThumbSizes();

    //- Cration de la file des qualits
    for(int i = 1; i <= m_parameters.m_photosConfig.nbIntermediateResolutions; i++){
        qualityList.enqueue( m_parameters.m_photosConfig.quality );
    }

    //- Sharpening options cf. http://redskiesatnight.com/2005/04/06/sharpening-using-image-magick/
    // Seulement si OPTIMIZE_QUALITY
    t_sharpening sharpening; //Mmes paramtres de sharpening pour toutes les photos (non appliqu aux vignettes)
    if( m_parameters.m_photosConfig.imageOptimizationStrategy == t_photosConf::OPTIMIZE_QUALITY )
    {
        sharpening.amount = ((double)m_parameters.m_photosConfig.sharpeningAmount)/100.0;
        sharpening.radius =  m_parameters.m_photosConfig.sharpeningRadius;
        sharpening.threshold = SHARPENINGTHRESHOLD; // quivalent de 10
        if( m_parameters.m_photosConfig.sharpeningRadius <= 1.0 ){
            sharpening.sigma = m_parameters.m_photosConfig.sharpeningRadius;
        }
        else{
            sharpening.sigma = sqrt( m_parameters.m_photosConfig.sharpeningRadius );
        }
    }
    else
    {
        sharpening.amount = 0.0;
        sharpening.radius =  0.0;
        sharpening.threshold = 1.0;
    }

    //- Cration  et lancement de la file de traitements photos
    outPath = m_parameters.m_galleryConfig.outputDir;
    displayProgressBar( 0, "green", tr("Generating the photos : ")+QString::number(0)+"%" );
    m_mutexControlProcessors.lock(); //Pour ne pas que les threads dmarrent trop tt
    
    foreach( CPhotoProperties photoProperties, m_photoPropertiesList )
    {
        photoToProcess = new CPhotoProcessor( photoProperties,
                                              outPath,
                                              photoSizes,
                                              m_thumbSizes,
                                              qualityList,
                                              sharpening,
                                              watermarkImage,
                                              &m_fStopRequested,
                                              &m_mutexControlProcessors  );

        if( !connect( photoToProcess, SIGNAL( processCompleted(CGeneratedPhotoSetParameters) ), this, SLOT( onPhotoProcessDone(CGeneratedPhotoSetParameters) ), Qt::QueuedConnection ) )
        {
            this->debugDisplay("Erreur connection #" + QString::number(idPhotoToProcess) );
            return m_nbPhotosToProcess; //AJOUTER UNE SORTIE PAR ERREUR !!!
        }

        m_p_photoProcessorPool->start( photoToProcess );
        m_nbPhotosToProcess++;
        idPhotoToProcess++;
    }


    if( m_nbPhotosToProcess == 0){
        emit jobDone();
    }
    m_mutexControlProcessors.unlock(); //Les threads peuvent dmarrer

    return m_nbPhotosToProcess; //La tche suivante sera lance lorsque tous les photoProcessors auront fini
}




///////////////////////////////////////////////////
/////////////// generateJsFiles ///////////////////
///////////////////////////////////////////////////
bool CGalleryGenerator::generateJsFiles( )
{
    m_jsonRoot.clear(); //json root will be rebuilt

    QDir path( m_parameters.m_galleryConfig.outputDir );   

    //////////// Configuration ///////////////

    // --- JSON    
    // -- technical properties
    Object& jsonPhotos = m_jsonRoot.addObject( "photos" );
    Object& jsonPhotosProperties = jsonPhotos.addObject( "technical" );
    jsonPhotosProperties.addNumber( "cacheSize", m_parameters.m_galleryConfig.prefetchCacheSize );
    jsonPhotosProperties.addNumber( "prefetchSize", m_parameters.m_galleryConfig.nbPhotosToPrefetch );
    jsonPhotosProperties.addBoolean( "rightClickEnabled", m_parameters.m_galleryConfig.f_rightClickEnabled );
    jsonPhotosProperties.addNumber( "qualityStrategy", m_parameters.m_photosConfig.imageOptimizationStrategy );
    jsonPhotosProperties.addNumber( "first", 1 );
    jsonPhotosProperties.addString( "smallestSet", QString(RESOLUTIONPATH) + QString::number( m_parameters.m_photosConfig.nbIntermediateResolutions ) );
    jsonPhotosProperties.addString( "largetSet", QString(RESOLUTIONPATH) + QString::number( 1 ) );
    Object& maxSize = jsonPhotosProperties.addObject( "maxSize" );
    maxSize.addNumber("width", m_parameters.m_photosConfig.maxSizeW );
    maxSize.addNumber("height", m_parameters.m_photosConfig.maxSizeH );
    Object& decoration = jsonPhotosProperties.addObject( "decoration" );
    decoration.addNumber( "padding", m_skinParameters.photoPaddingSize );

    // -- photo properties
    int numPhoto = 1; //m_photoPropertiesList and m_photoSizes must be coherent. Bad design!
    Array& jsonPhotoList = jsonPhotos.addArray( "list" );
    foreach( CPhotoProperties properties, m_photoPropertiesList )
    {
        Object& photo = jsonPhotoList.appendObject();
        photo.addString("filename",properties.encodedFilename() );
        photo.addString("caption",properties.caption().render() );
        Object& photoSizes = photo.addObject( "sizes" );
        QMap<QString,QSize> sizes = m_photoSizes.value( numPhoto );        
        foreach(QString res, sizes.keys() )
        {
            QSize size = sizes.value( res );
            Object& photoSize = photoSizes.addObject( res );
            photoSize.addNumber( "width", size.width() );
            photoSize.addNumber( "height", size.height() );
        }
        numPhoto++;
    }


    //////////// Presentation ///////////////
   // Attention : certaines proprits doivent tre en phase avec les css
   
    //---- JSON index      
    Object& index = m_jsonRoot.addObject( "index" );
   
    //-- Mosaic
    Object& mosaic = index.addObject( "mosaic" );
    //-- Technical
    mosaic.addNumber( "first", 1 );
    //-- Display
    const unsigned int nbCols =  m_parameters.m_thumbsConfig.nbColumns;
    const unsigned int nbRows = m_parameters.m_thumbsConfig.nbRows;
    const QSize unavailableSpace = m_skinParameters.unavailableSpace( nbCols, nbRows );
    mosaic.addNumber( "nbRows",  nbRows);
    mosaic.addNumber( "nbCols",  nbCols);
    mosaic.addString( "defaultSet", QString(RESOLUTIONPATH) + ("8") );  
    Object& sizes = mosaic.addObject( "sizes" );
    foreach( QSize size, m_thumbSizes ) {
        sizes.addNumber( m_thumbSizes.key(size), size.width() );
    }
    Object& unavailable = mosaic.addObject( "unavailable" );
    unavailable.addNumber( "horizontal", unavailableSpace.width() );
    unavailable.addNumber( "vertical", unavailableSpace.height() );  

    // Writing document to disk
    QDir jsonPath( m_parameters.m_galleryConfig.outputDir );
    jsonPath.cd( RESPATH );
    QFile jsonFile( jsonPath.absoluteFilePath( "parameters.json" ) );
    if (!jsonFile.open(QIODevice::WriteOnly)){ //omitting QIODevice::Text to get UNIX end-of-line even on Windows
         m_msgErrorList.append( CError::error(CError::FileOpening) +  jsonFile.fileName() );
         emit abordGenerationSignal( );
         return false;
    }
    jsonFile.setPermissions( QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::ReadOther );
    QTextStream jsonStream( &jsonFile );
    jsonStream.setCodec( "UTF-8" );
    jsonStream.setGenerateByteOrderMark (true);
    jsonStream << m_jsonRoot.serialize();
    jsonFile.close();   
    
    
    emit jobDone();
    return true;
}



////////////////////////////////////////////////////
///////////////// css Skinning /////////////////////
///////////////////////////////////////////////////

// -> Fin du process de skinning aprs 

bool CGalleryGenerator::skinning( )
{
 	//-------- Copie des fichiers de la skin --------//
    QDir outSkinPath( m_parameters.m_galleryConfig.outputDir );

    displayProgressBar( 100, "green", tr("Skinning...") );
	
    //---------- COPIE DES FICHIERS RESSOURCES --------//
    outSkinPath = QDir( m_parameters.m_galleryConfig.outputDir );        
    if( !outSkinPath.mkpath( RESIMGPATH ) ){
        m_msgErrorList.append( CError::error(CError::FileCreation) + outSkinPath.absolutePath() + QString("/") + QString(RESIMGPATH) );
        emit abordGenerationSignal( );
        return false;
    }
    outSkinPath.cd( RESIMGPATH );
    if( !m_skinParameters.copyRessources(outSkinPath/*, m_msgErrorList*/) ){
        m_msgErrorList.append( m_skinParameters.errors() );
        emit abordGenerationSignal( );
        return false;
    }

    //---------- CREATION DU CSS --------//
    //--- Ouverture du fichier
    outSkinPath = QDir( m_parameters.m_galleryConfig.outputDir );
    if( !outSkinPath.mkpath( CSSPATH ) ){
        m_msgErrorList.append( CError::error(CError::FileCreation) + outSkinPath.absolutePath() + QString("/") + QString(CSSPATH) );
        emit abordGenerationSignal( );
        return false;
    }
    outSkinPath.cd( CSSPATH );
    QFile skinCssFile( outSkinPath.absoluteFilePath( CSSSKINFILENAME ) );
    if (!skinCssFile.open(QIODevice::WriteOnly | QIODevice::WriteOnly | QIODevice::Text)){
            m_msgErrorList.append( CError::error(CError::FileOpening) +  skinCssFile.fileName() );
            emit abordGenerationSignal( );
            return false;
    }
    skinCssFile.setPermissions( QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::ReadOther );
    QTextStream cssSkinStream( &skinCssFile );
    cssSkinStream.setCodec( "UTF-8" );
    cssSkinStream.setGenerateByteOrderMark (true);
        
    //--- Récupration des paramtres de la skin
    CCssSheet skinCssSheet = m_skinParameters.toCss( );

    //--- Ecriture du fichier sur le disque
    skinCssSheet.toStream( cssSkinStream );
    skinCssFile.close();


//-------- MODIFICATIONS HTML -------//
    QDir outPath( m_parameters.m_galleryConfig.outputDir );
    QFile htmlFile( outPath.absoluteFilePath( QString("index.html")) );
    htmlFile.setPermissions( QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::ReadOther );
    if( htmlFile.open( QIODevice::Text|QIODevice::ReadWrite ) )
    {
        QTextStream htmlTextStream( &htmlFile );
        htmlTextStream.setCodec( "UTF-8" );
        htmlTextStream.setGenerateByteOrderMark (false); //False sinon pb avec ie6 car les premiers caractres ne sont pas le doctype...
        QString htmlString;
        QString verticalTitle(  "<br />" );
			
        htmlString = htmlTextStream.readAll() ; //Lecture du fichier

        //------ HEADER -----//
        //Metatag generator
        htmlString.replace( "[META_GENERATOR]", QString("<meta name=\"generator\" content=\"EZWebGallery r")+CPlatform::revision()+QString("\" />") );
        //Metatags Content, title &  Description
        htmlString.replace( "[META_TITLE]", QString("<meta name=\"title\" content=\"")+m_parameters.m_galleryConfig.title+QString("\" />") );
        htmlString.replace( "[META_DESCRIPTION]", QString("<meta name=\"description\" content=\"")+m_parameters.m_galleryConfig.description+QString("\" />") );
        htmlString.replace( "[META_CONTENT]", QString("<meta name=\"content\" content=\"")+m_parameters.m_galleryConfig.description+QString("\" />") );
        //Facebook's OpenGraph
        QChar slash = '/';
        QString galleryThumbURL = m_parameters.m_galleryConfig.url.remove(' '); //Mise en forme de l'url de la vignette reprsentant la galerie
        if( !galleryThumbURL.isEmpty() && galleryThumbURL.at( galleryThumbURL.size() - 1 ) != slash ){ //Ajout d'un / final si il n'y en a pas
            galleryThumbURL += slash;
        }
        galleryThumbURL += PHOTOSPATH;
        galleryThumbURL += slash;
        galleryThumbURL += GALLERYTHUMBFILENAME;
        QString openGraphString = QString("<meta property=\"og:image\" content=\"")+galleryThumbURL+QString("\" / >\n") \
                                    +QString("<meta property=\"og:title\" content=\"")+m_parameters.m_galleryConfig.title+QString("\" / >\n") \
                                    +QString("<meta property=\"og:description\" content=\"")+m_parameters.m_galleryConfig.description+QString("\" / >\n")/*\
                                    +QString("<meta property=\"fb:admins\" content=\"786810484\"/>" )*/;
        htmlString.replace( "[META_OPENGRAPH]",	openGraphString );
        //------ FOOTER -------//
        //EZWebGallery Logo
        htmlString.replace( "[EZWEBGALLERY_LOGO]", QString("<img src=\"ressources/images/EZWebGallery.png\" id=\"logo\" title=\"")
                                                            + tr("Photo gallery designed and generated using EZWebGallery.")
                                                            + QString("\" alt=\"EZWebGallery\"/>") );
		//Add-this
        if( m_parameters.m_galleryConfig.f_shareOnSocialNetworks ){
            QString addthisString = QString("<div class=\"addthis_toolbox addthis_default_style\" style=\"margin-left:50px\">\n")+
                                    QString("\t<span class=\"addthis_separator\"></span>\n")+
                                    QString("\t<a href=\"http://addthis.com/bookmark.php?v=250\" class=\"addthis_button_compact\"></a>\n")+
                                    QString("\t<a class=\"addthis_button_facebook\"></a>\n")+
                                    QString("\t<a class=\"addthis_button_twitter\"></a>\n")+
                                    /*QString("\t<a class=\"addthis_button_googlebuzz\"></a>\n")+*/
                                    QString("\t<a class=\"addthis_button_email\"></a>\n")+
                                    QString("</div>\n")+
                                    QString("<script type=\"text/javascript\" src=\"http://s7.addthis.com/js/250/addthis_widget.js#username=xa-4c0eb6595435a765\"></script>");
            htmlString.replace( "[BUTTON_ADDTHIS]", addthisString );
        }else{
            htmlString.remove( "[BUTTON_ADDTHIS]" );
        }

        //------ TITRES -----//
        //Titre de la page
        htmlString.replace( "[HTML_TITLE]", QString("<title>")+m_parameters.m_galleryConfig.title+QString("</title>") );
        //Titre vertical
        for( int i = 0; i < m_parameters.m_galleryConfig.title.size(); i++ ) {
            verticalTitle.append( "<br />" );
            verticalTitle.append( m_parameters.m_galleryConfig.title.at(i) );
        }
        htmlString.replace( "[CONTENT_VERTICALTITLE]", verticalTitle );
        //Titre au dessus des photos
        htmlString.replace( "[CONTENT_PHOTOTITLE]", m_parameters.m_galleryConfig.title );

        //------ BOUTONS (SCREEN PHOTO) -------//
        //Previous
        htmlString.replace( "[BUTTON_PREVIOUSPHOTO]", QString("<img alt=\"") + tr("Previous photo")
                                                                +QString("\" class=\"photoButtonEnabled\" id=\"boutonPrevious\" src=\"ressources/images/")
                                                                +m_skinParameters.buttonImage( CSkinParameters::buttonPrevious )+QString("\" />") );
        //Next
        htmlString.replace( "[BUTTON_NEXTPHOTO]", QString("<img alt=\"") + tr("Next photo")
                                                            +QString("\" class=\"photoButtonEnabled\" id=\"boutonNext\" src=\"ressources/images/")
                                                            +m_skinParameters.buttonImage( CSkinParameters::buttonNext )+QString("\" />") );
        //Index
        htmlString.replace( "[BUTTON_INDEX]", QString("<img alt=\"") + tr("Browse the gallery")
                                                        +QString("\" class=\"photoButtonEnabled\" id=\"boutonIndex\" src=\"ressources/images/")
                                                        +m_skinParameters.buttonImage( CSkinParameters::buttonIndex )+QString("\" />") );

        //---------------------//

        htmlTextStream.seek( 0 ); //Reposition  0 pour remplacer le fichier et pas faire un append
        htmlTextStream << htmlString;

    } //fin modifications html
    else{
        m_msgErrorList.append( CError::error(CError::FileOpening) +  htmlFile.fileName() );
        emit abordGenerationSignal( );
        return false;
    }
    htmlFile.close();
		
		
    //-- FIN DE LA GENERATION --//
    m_mutex.lock();
    m_f_WorkInProgress = false;        
    emit generationFinishedSignal( m_photoPropertiesList ); //Information de l'UI
    emit jobDone();
    m_mutex.unlock();
	
	return true;
}


/*********************************************************************/


/************* SLOTS **************/

void CGalleryGenerator::onPhotoProcessDone( CGeneratedPhotoSetParameters generatedPhotoParams )
{	    	
    int completion = 0;
    int nbPhotosProcessed;
    int nbPhotoProcessFailed;
    //int idPhotoDone;
    bool f_wip;

    m_mutex.lock();
    f_wip = m_f_WorkInProgress;
    m_mutex.unlock();
    //CPhotoProperties photoProperties;

    CPhotoProperties photoProperties = generatedPhotoParams.photoProperties();
    switch( generatedPhotoParams.exitStatus() )
    {
	
    case success:    
        m_mutex.lock();
			nbPhotoProcessFailed = m_nbPhotoProcessFailed;
			nbPhotosProcessed = ++m_nbPhotosProcessed;
		m_mutex.unlock();
		
		//Affichage de l'avancement
        completion = (int)(((float)nbPhotosProcessed/(float)m_nbPhotosToProcess)*100);
        if( nbPhotoProcessFailed == 0 && f_wip ){
            displayProgressBar( completion, "green", tr("Generating the photos : ")+QString::number(completion)+"%" );
        }
       
        //idPhotoDone = generatedPhotoParams.idPhoto();
        //Rcupration des tailles gnres par le process
        m_photoSizes.insert( photoProperties.id() + 1,  generatedPhotoParams.generatedSizes() );
        //Updating photoproperties to insert the read ExifTags
        //photoProperties = m_photoPropertiesList.at( photoProperties.id() );
        //photoProperties.setExifTags( generatedPhotoParams.exifTags() );
        m_photoPropertiesList.replace( photoProperties.id(), photoProperties );

        //Fin nominale du process des photos??
        if( nbPhotosProcessed + nbPhotoProcessFailed == m_nbPhotosToProcess)
        {
            if( nbPhotoProcessFailed == 0){
                emit jobDone( );
            }
            else{
                this->debugDisplay("UNSPECIFIED FAILURE !!!");
                emit abordGenerationSignal( );
                break; //On ne devrait vraiment pas se trouver ici...
            }
        }
     break;
	 
     case failure:
            m_msgErrorList << generatedPhotoParams.message();
     case stopped:
     default:
        m_mutex.lock();
            m_nbPhotoProcessFailed++;
        m_mutex.unlock();
        emit abordGenerationSignal( );
     break;
    }//switch

}

void CGalleryGenerator::onAbordGeneration( )
{
    int nbPhotosProcessed;
    int nbPhotoProcessFailed;
    m_mutex.lock();
        nbPhotosProcessed = m_nbPhotosProcessed;
        nbPhotoProcessFailed = m_nbPhotoProcessFailed;
	m_mutex.unlock();
    m_mutexControlProcessors.lock();
        m_fStopRequested = true;
    m_mutexControlProcessors.unlock();

    //Finalisation de l'annulation si tout est arrt
    if( nbPhotosProcessed + nbPhotoProcessFailed == m_nbPhotosToProcess )
	{
        m_p_photoProcessorPool->waitForDone(); 	//Pour que les derniers threads du pool aient le temps de se terminer proprement
												//Dangereux si plus de deux cpus??
        emit jobDone();
        m_mutex.lock();
            m_f_WorkInProgress = false;
        m_mutex.unlock();
        this->debugDisplay("emit forcedFinished");
        emit forceStoppedFinishedSignal( m_msgErrorList ); //Asynchrone
    }

}