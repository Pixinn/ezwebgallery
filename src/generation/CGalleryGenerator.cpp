﻿/*
 *  EZWebGallery:
 *  Copyright (C) 2011-2017 Christophe Meneboeuf <dev@ezwebgallery.org>
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
#include <stdexcept>


#include "CGalleryGenerator.h"
#include "CPhoto.h"
#include "CTaggedString.h"
#include "CPlatform.h"
#include "CCss.h"
#include "CToolbar.h"
#include "CLogger.h"
#include "CMessage.h"
#include "CError.h"

using namespace Magick;
using namespace JSON;


///////////////////////////////////////////////////////////////////
//////////////////// CLASSE CGalleryGenerator /////////////////////
///////////////////////////////////////////////////////////////////


//------------------ DEFINITIONS ----------------------//

#define JSTOOLBARFILENAME   "Toolbar.js"
#define CSSSKINFILENAME     "skin.css"
#define SHARPENINGTHRESHOLD 0.04        /* équivalent à 10 */

//-------------------- FONCTIONS -----------------------//


CGalleryGenerator::CGalleryGenerator( ) :
    QThread()
{
    qRegisterMetaType<CGeneratedPhotoSetParameters> ( "CGeneratedPhotoSetParameters" );

    //--- Init de la machine Etat
    //Etats
    m_p_waitingForOrders = std::make_unique<QState>();
    m_p_galleryGeneration = std::make_unique<QState>();
    m_p_abording = std::make_unique<QState>();
    m_p_init = std::make_unique<QState>(m_p_galleryGeneration.get());
    m_p_generatingFileStructure = std::make_unique<QState>(m_p_galleryGeneration.get());
    m_p_generatingPhotos = std::make_unique<QState>(m_p_galleryGeneration.get());
    m_p_generatingJSFiles = std::make_unique<QState>(m_p_galleryGeneration.get());
    m_p_optionalBehaviors = std::make_unique<QState>(m_p_galleryGeneration.get());
    m_p_skinning = std::make_unique<QState>(m_p_galleryGeneration.get());
    m_p_galleryGeneration->setInitialState( m_p_init.get() );
    //Transitions
    m_p_galleryGeneration->addTransition( this, SIGNAL( abordGenerationSignal( )), m_p_abording.get());
    m_p_waitingForOrders->addTransition( this, SIGNAL(startGenerationSignal()), m_p_galleryGeneration.get());
    m_p_init->addTransition( this, SIGNAL(jobDone()), m_p_generatingFileStructure.get());
    m_p_generatingFileStructure->addTransition( this, SIGNAL(jobDone()), m_p_generatingPhotos.get());
    m_p_generatingPhotos->addTransition( this, SIGNAL(jobDone()), m_p_generatingJSFiles.get());
    m_p_generatingJSFiles->addTransition( this, SIGNAL(jobDone()), m_p_optionalBehaviors.get());
    m_p_optionalBehaviors->addTransition(this, SIGNAL(jobDone()), m_p_skinning.get());
    m_p_skinning->addTransition( this, SIGNAL(jobDone()), m_p_waitingForOrders.get());
    m_p_abording->addTransition( this, SIGNAL(abordGenerationSignal( )), m_p_abording.get());
    m_p_abording->addTransition( this, SIGNAL(jobDone()), m_p_waitingForOrders.get());
    //Connections Etats<->Traitements
    connect( m_p_init.get(), SIGNAL(entered()), this, SLOT( initGeneration() ) );
    connect( m_p_generatingFileStructure.get(), SIGNAL(entered()), this, SLOT( generateFileStructure() ) );
    connect( m_p_generatingPhotos.get(), SIGNAL(entered()), this, SLOT( generatePhotos() ) );
    connect( m_p_generatingJSFiles.get(), SIGNAL(entered()), this, SLOT( generateJsFiles() ) );
    connect(m_p_optionalBehaviors.get(), SIGNAL(entered()), this, SLOT(optionnalBehaviors()));
    connect( m_p_skinning.get(), SIGNAL(entered()), this, SLOT( skinning() ) );
    connect( m_p_abording.get(), SIGNAL(entered()), this, SLOT( onAbordGeneration() ) );
    //Finalisation
    m_stateMachine.addState( m_p_waitingForOrders.get() );
    m_stateMachine.addState( m_p_galleryGeneration.get());
    m_stateMachine.addState( m_p_abording.get());
    m_stateMachine.setInitialState( m_p_waitingForOrders.get());
    //---

    m_nbPhotosToProcess = 0;
    m_nbPhotosProcessed = 0;
    m_nbPhotoProcessFailed = 0;
    m_f_WorkInProgress = false;
    m_fStopRequested = false;
    m_p_photoProcessorPool = new QThreadPool( this ); 
    m_p_photoProcessorPool->setMaxThreadCount(QThread::idealThreadCount());
    
    m_errorMsg = tr("No error.");

    start();	//Lancement du thread
}

CGalleryGenerator::~CGalleryGenerator( )
{
	m_stateMachine.stop();
    delete m_p_photoProcessorPool;
}


/************ Interfacage avec UI ************/


/************************************
* REMARQUE : c'est ici qu'est ordonne la liste
* des photos  traiter, qui donnera l'ordre d'affichage
* des photos de la galerie
************************************/
bool CGalleryGenerator::generateGallery( const CGalleryGeneratorFeeder & galleryFeeder  )
{
	QStringList photoList;
	QString photoName;
    m_feeder = galleryFeeder;
    bool f_success = false;

    //Copie des paramtres en local avant lancement du thread
    if( !this->isGenerationInProgress() && !m_feeder.getPhotoProperties().isEmpty() )
    {
        CCaption emptyCaption;
        QFileInfo fileInfo;

        //-- Init
        m_skinParameters = m_feeder.getSkinParams();
        const auto& param = m_feeder.getProjectParams();
        m_toolbarButtons.fullscreen = param.m_galleryConfig.f_buttonFullscreen;
        displayProgressBar( 0, "green", PtrMessage(new CMessage( tr("Initialization...") )) );

        //-- Start
        emit startGenerationSignal(); //Demarrage des traitements

        f_success = true;
    }

    return f_success;

}

//Permet d'appeler onAbordGeneration de manire ASSYNCHRONE  partir du thread de l'UI
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


void CGalleryGenerator::displayProgressBar( int completion, QString color, const PtrMessage &message )
{
    emit progressBarSignal( completion, color, message );
}



std::unique_ptr<QTextStream> CGalleryGenerator::readHtml(QFile& input) const
{
    if (input.open(QIODevice::Text | QIODevice::ReadWrite))
    {
        std::unique_ptr<QTextStream> htmlTextStream = std::make_unique<QTextStream>(&input);
        htmlTextStream->setCodec("UTF-8");
        htmlTextStream->setGenerateByteOrderMark(false); //False sinon pb avec ie6 car les premiers caractres ne sont pas le doctype...
        return htmlTextStream;
    }
    else {
        throw std::runtime_error{ "Cannot open the file" };
    }
}

/***********************************************************************/




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
	//Normalement les mutex ne servent a rien ici
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
   displayProgressBar( 0, "green", PtrMessage(new CMessage(tr("Generating the file structure..."))) );

    //--- Photos
    const QString photosFilesPath( QString(PHOTOSPATH) + "/res" );
    const QString thumbsFilesPath( THUMBSPATH );
    QDir outPath( m_feeder.getProjectParams().m_galleryConfig.outputDir );

    if ( !outPath.mkpath( thumbsFilesPath ) ){
        m_msgErrorList.append( PtrMessage(new CError( CError::error(CError::DirectoryCreation), thumbsFilesPath)) );
        emit abordGenerationSignal( );
        return false;
    }
    for(int i = 1; i <= m_feeder.getPhotoSizes().size(); i++){
        if( !outPath.mkpath( photosFilesPath + QString::number(i) ) ){
            m_msgErrorList.append( PtrMessage(new CError(CError::error(CError::DirectoryCreation), photosFilesPath + QString::number(i))) );
            emit abordGenerationSignal( );
            return false;
        }
    }

    //Copie des fichiers ressources (ie non modifiables, ne faisant pas partie des skins)
    outPath = QDir( m_feeder.getProjectParams().m_galleryConfig.outputDir );
    QDir srcPath( CPlatform::resourceDirPath()  );
    if( !srcPath.exists() ){
        m_msgErrorList.append(  PtrMessage(new CError(CError::error(CError::SourceFileNotFound), srcPath.absolutePath())) );
        emit abordGenerationSignal( );
        return false;
    }
    PtrMessage copyError( new CError( CPlatform::copyDirectory( srcPath, outPath ) ) ); //Copying directory
    if( !copyError->isEmpty() ){
        m_msgErrorList.append(copyError);
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
    if( !m_feeder.getProjectParams().m_photosConfig.f_regeneration &&
        !m_feeder.getProjectParams().m_thumbsConfig.f_regeneration &&
        m_feeder.isOutFolderUpToDate() )
    {
        emit jobDone();
        return 0;
    }//Attention cela ne vrifie pas la prsence de la vignette reprsentant la gallerie

    QDir outPath( m_feeder.getProjectParams().m_galleryConfig.outputDir );

    //Gnration de la vignette reprsentant la galerie
    //Cette gnration est effectue  part de celle des autres vignettes et des photos
    QImage photo;
    QImage thumbnail;
    QString galleryThumbnailFile =  m_feeder.getProjectParams().m_galleryConfig.thumbPhoto;
    if( !QFileInfo( m_feeder.getProjectParams().m_galleryConfig.thumbPhoto ).exists() )  {
        galleryThumbnailFile = m_feeder.getPhotoProperties().at(0).fileInfo().absoluteFilePath();
    }
    outPath.cd( PHOTOSPATH );
    if( photo.load( galleryThumbnailFile )){
        thumbnail = photo.scaled(QSize{ 640,640 },
                                  Qt::KeepAspectRatio,
                                  Qt::SmoothTransformation );
        thumbnail.save( outPath.absoluteFilePath( GALLERYTHUMBFILENAME ), 0, 85 );
        m_feeder.setThumbGallerySize(QSize{ thumbnail.size()});
    }
    else {
        //TODO : log an error
    }


    //Gnration du watermark si besoin
    CWatermark watermarkImage;
    t_watermark watermarkParams = m_feeder.getProjectParams().m_photosConfig.watermark;
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
                m_msgErrorList.append( PtrMessage(new CError( CError::WatermarkInvalid, watermarkImage.error() )) );
                emit abordGenerationSignal( );
                return false;
            }
        }

    }

    m_photoSizes.clear();//Les tailles des photos qui seront gnres
    CPhotoProcessor* photoToProcess;
    int idPhotoToProcess = 1;

    //- Sharpening options cf. http://redskiesatnight.com/2005/04/06/sharpening-using-image-magick/
    // Seulement si OPTIMIZE_QUALITY
    t_sharpening sharpening; //Mmes paramtres de sharpening pour toutes les photos (non appliqu aux vignettes)
    if( m_feeder.getProjectParams().m_photosConfig.imageOptimizationStrategy == t_photosConf::OPTIMIZE_QUALITY )
    {
        sharpening.amount = ((double)m_feeder.getProjectParams().m_photosConfig.sharpeningAmount)/100.0;
        sharpening.radius =  m_feeder.getProjectParams().m_photosConfig.sharpeningRadius;
        sharpening.threshold = SHARPENINGTHRESHOLD; // quivalent de 10
        if( m_feeder.getProjectParams().m_photosConfig.sharpeningRadius <= 1.0 ){
            sharpening.sigma = m_feeder.getProjectParams().m_photosConfig.sharpeningRadius;
        }
        else{
            sharpening.sigma = sqrt( m_feeder.getProjectParams().m_photosConfig.sharpeningRadius );
        }
    }
    else
    {
        sharpening.amount = 0.0;
        sharpening.radius =  0.0;
        sharpening.threshold = 1.0;
    }

    //- Cration  et lancement de la file de traitements photos
    outPath = m_feeder.getProjectParams().m_galleryConfig.outputDir;
    displayProgressBar( 0, "green", PtrMessage(new CMessage(tr("Generating the photos : "), QString::number(0)+"%" )) );
    m_mutexControlProcessors.lock(); //Pour ne pas que les threads dmarrent trop tt

    foreach( CPhotoProperties photoProperties, m_feeder.getPhotoProperties() )
    {
        photoToProcess = new CPhotoProcessor( photoProperties,
                                              outPath,
                                              m_feeder.getPhotoSizes(),
                                              m_feeder.getThumbSizes(),
                                              m_feeder.getProjectParams().m_photosConfig.quality,
                                              sharpening,
                                              watermarkImage,
                                              &m_fStopRequested,
                                              &m_mutexControlProcessors  );

        if( !connect( photoToProcess, SIGNAL( processCompleted(CGeneratedPhotoSetParameters) ), this, SLOT( onPhotoProcessDone(CGeneratedPhotoSetParameters) ), Qt::QueuedConnection ) )
        {
            CLogger::getInstance().log(PtrMessage( new CError( CError::error(CError::Internal), tr("Cannot connect photoprocessor: ") + QString::number(idPhotoToProcess)) ));
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

    QDir path( m_feeder.getProjectParams().m_galleryConfig.outputDir );

    //////////// Configuration ///////////////

    // --- JSON    
    // -- technical properties
    Object& jsonPhotos = m_jsonRoot.addObject( "photos" );
    Object& jsonPhotosProperties = jsonPhotos.addObject( "technical" );
    jsonPhotosProperties.addNumber( "cacheSize", m_feeder.getProjectParams().m_galleryConfig.prefetchCacheSize );
    jsonPhotosProperties.addNumber( "prefetchSize", m_feeder.getProjectParams().m_galleryConfig.nbPhotosToPrefetch );
    jsonPhotosProperties.addBoolean( "rightClickEnabled", m_feeder.getProjectParams().m_galleryConfig.f_rightClickEnabled );
    jsonPhotosProperties.addNumber( "qualityStrategy", m_feeder.getProjectParams().m_photosConfig.imageOptimizationStrategy );
    jsonPhotosProperties.addNumber( "first", 1 );
    jsonPhotosProperties.addString( "smallestSet", QString(RESOLUTIONPATH) + QString::number( m_feeder.getPhotoSizes().size() ) );
    jsonPhotosProperties.addString( "largetSet", QString(RESOLUTIONPATH) + QString::number( 1 ) );
    Object& maxSize = jsonPhotosProperties.addObject( "maxSize" );
    maxSize.addNumber("width", m_feeder.getProjectParams().m_photosConfig.maxSizeW );
    maxSize.addNumber("height", m_feeder.getProjectParams().m_photosConfig.maxSizeH );
    Object& decorationPhoto = jsonPhotosProperties.addObject( "decoration" );
    decorationPhoto.addNumber( "padding", m_skinParameters.photoPaddingSize );


    // -- photo properties
    int numPhoto = 1; //m_photoPropertiesList and m_photoSizes must be coherent. Bad design!
    Array& jsonPhotoList = jsonPhotos.addArray( "list" );
    foreach( CPhotoProperties properties, m_feeder.getPhotoProperties() )
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
   // Attention : certaines proprites doivent etre en phase avec les css

    //---- JSON index
    Object& index = m_jsonRoot.addObject( "index" );

    //-- Mosaic
    Object& mosaic = index.addObject( "mosaic" );
    //-- Technical
    mosaic.addNumber( "first", 1 );
    //-- Display
    const unsigned int nbCols =  m_feeder.getProjectParams().m_thumbsConfig.nbColumns;
    const QSize unavailableSpace = m_skinParameters.mosaicDecoration( nbCols );
    mosaic.addNumber( "nbCols",  nbCols);
    mosaic.addString( "defaultSet", QString(RESOLUTIONPATH) + ("1") );
    Object& sizes = mosaic.addObject( "sizes" );
    foreach( QSize size, m_feeder.getThumbSizes() ) {
        sizes.addNumber( m_feeder.getThumbSizes().key(size), size.width() );
    }
    Object& unavailable = mosaic.addObject( "unavailable" );
    unavailable.addNumber( "horizontal", unavailableSpace.width() );
    unavailable.addNumber( "vertical", unavailableSpace.height() );
    Object& decorationMosaic = mosaic.addObject("decoration");
    decorationMosaic.addNumber("borderBox", m_skinParameters.thumbBoxBorderSize);
    decorationMosaic.addNumber("borderPhoto", m_skinParameters.thumbImgBorderSize);

    // Writing document to disk
    QDir jsonPath( m_feeder.getProjectParams().m_galleryConfig.outputDir );
    jsonPath.cd( RESPATH );
    QFile jsonFile( jsonPath.absoluteFilePath( "parameters.json" ) );
    if (!jsonFile.open(QIODevice::WriteOnly)){ //omitting QIODevice::Text to get UNIX end-of-line even on Windows
         m_msgErrorList.append( PtrMessage(new CError(CError::FileOpening,  jsonFile.fileName())) );
         emit abordGenerationSignal( );
         return false;
    }
    jsonFile.setPermissions( QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::ReadOther );
    QTextStream jsonStream( &jsonFile );
    jsonStream.setCodec( "UTF-8" );
    jsonStream.setGenerateByteOrderMark (true);
    jsonStream << m_jsonRoot.serialize( "g_properties" );
    jsonFile.close();


    emit jobDone();
    return true;
}




////////////////////////////////////////////////////
///////////////// Adds the optionnal behaviors /////////////////////
///////////////////////////////////////////////////

bool CGalleryGenerator::optionnalBehaviors()
{
    QDir outPath{ m_feeder.getProjectParams().m_galleryConfig.outputDir };
    QFile htmlFile{ outPath.absoluteFilePath(QString("index.html")) };
    htmlFile.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::ReadOther);
    
    try 
    {
        auto streamHtml = readHtml(htmlFile);
        auto html = streamHtml->readAll();
        const auto& behaviors = m_feeder.getOptionnalBehaviors();
        for (const auto& behavior : behaviors) {
            behavior->embedInHtml(html);
        }
        streamHtml->seek(0); //Reposition  0 pour remplacer le fichier et pas faire un append
        *streamHtml << html;
    }
    catch (const std::runtime_error&) {
        m_msgErrorList.append(PtrMessage(new CError(CError::FileOpening, htmlFile.fileName())));
        emit abordGenerationSignal();
        return false;
    }
 
    htmlFile.close();
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
    QDir outSkinPath( m_feeder.getProjectParams().m_galleryConfig.outputDir );

    displayProgressBar( 100, "green", PtrMessage(new CMessage(tr("Skinning..."))) );

    //---------- COPIE DES FICHIERS RESSOURCES --------//
    outSkinPath = QDir( m_feeder.getProjectParams().m_galleryConfig.outputDir );
    if( !outSkinPath.mkpath( RESIMGPATH ) ){
        m_msgErrorList.append( PtrMessage(new CError(CError::FileCreation, outSkinPath.absolutePath() + QString("/") + QString(RESIMGPATH))) );
        emit abordGenerationSignal( );
        return false;
    }
    outSkinPath.cd( RESIMGPATH );
    if( !m_skinParameters.copyResources(outSkinPath) ){
        foreach( CError error,  m_skinParameters.errors() ) {
            m_msgErrorList.append( PtrMessage(new CError(error)) );
        }
        emit abordGenerationSignal( );
        return false;
    }

    //---------- CREATION DU CSS --------//
    //--- Ouverture du fichier
    outSkinPath = QDir( m_feeder.getProjectParams().m_galleryConfig.outputDir );
    if( !outSkinPath.mkpath( CSSPATH ) ){
        m_msgErrorList.append( PtrMessage(new CError(CError::FileCreation, outSkinPath.absolutePath() + QString("/") + QString(CSSPATH))) );
        emit abordGenerationSignal( );
        return false;
    }
    outSkinPath.cd( CSSPATH );
    QFile skinCssFile( outSkinPath.absoluteFilePath( CSSSKINFILENAME ) );
    if (!skinCssFile.open(QIODevice::WriteOnly | QIODevice::WriteOnly | QIODevice::Text)){
            m_msgErrorList.append( PtrMessage(new CError(CError::FileOpening, skinCssFile.fileName())) );
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


    //--------------- TOOLBAR --------------//
    //--- Ouverture du fichier
    QDir outJsToolbarPath( m_feeder.getProjectParams().m_galleryConfig.outputDir );
    if( !outJsToolbarPath.mkpath( JSPATH ) ){
        m_msgErrorList.append( PtrMessage(new CError(CError::FileCreation, outSkinPath.absolutePath() + QString("/") + QString(JSPATH))) );
        emit abordGenerationSignal( );
        return false;
    }
    outJsToolbarPath.cd( JSPATH );
    QFile toolbarJsFile( outJsToolbarPath.absoluteFilePath( JSTOOLBARFILENAME ) );
    if (!toolbarJsFile.open(QIODevice::WriteOnly | QIODevice::WriteOnly | QIODevice::Text)){
            m_msgErrorList.append( PtrMessage(new CError(CError::FileOpening, toolbarJsFile.fileName())) );
            emit abordGenerationSignal( );
            return false;
    }
    toolbarJsFile.setPermissions( QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::ReadOther );
    QTextStream toolbarJsStream( &toolbarJsFile );
    toolbarJsStream.setCodec( "UTF-8" );
    toolbarJsStream.setGenerateByteOrderMark (true);
    CToolbarBehavior toolbar( m_toolbarButtons );
    toolbarJsStream << toolbar.getJavascript();

//-------- MODIFICATIONS HTML -------//
    QDir outPath( m_feeder.getProjectParams().m_galleryConfig.outputDir );
    QFile htmlFile( outPath.absoluteFilePath( QString("index.html")) );
    htmlFile.setPermissions( QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::ReadOther );

    QString htmlString;
    std::unique_ptr<QTextStream> htmlTextStream;
    try    {
        htmlTextStream = readHtml(htmlFile);
        htmlString = htmlTextStream->readAll();
    }
    catch (const std::runtime_error&) {
        m_msgErrorList.append(PtrMessage(new CError(CError::FileOpening, htmlFile.fileName())));
        emit abordGenerationSignal();
        return false;
    }


    //------ HEADER -----//
    //Metatag generator
    htmlString.replace( "[META_GENERATOR]", QString("<meta name=\"generator\" content=\"EZWebGallery r")+CPlatform::revision()+QString("\" />") );
    //Metatags Content, title &  Description
    htmlString.replace( "[META_TITLE]", QString("<meta name=\"title\" content=\"")+m_feeder.getProjectParams().m_galleryConfig.title+QString("\" />") );
    htmlString.replace( "[META_DESCRIPTION]", QString("<meta name=\"description\" content=\"")+m_feeder.getProjectParams().m_galleryConfig.description+QString("\" />") );
    htmlString.replace( "[META_CONTENT]", QString("<meta name=\"content\" content=\"")+m_feeder.getProjectParams().m_galleryConfig.description+QString("\" />") );
    //Facebook's OpenGraph
    QChar slash = '/';
    QString galleryThumbURL = m_feeder.getProjectParams().m_galleryConfig.url; //Mise en forme de l'url de la vignette reprsentant la galerie
    if( !galleryThumbURL.isEmpty() && galleryThumbURL.at( galleryThumbURL.size() - 1 ) != slash ){ //Ajout d'un / final si il n'y en a pas
        galleryThumbURL += slash;
    }
    galleryThumbURL += PHOTOSPATH;
    galleryThumbURL += slash;
    galleryThumbURL += GALLERYTHUMBFILENAME;
    m_feeder.getThumbGallerySize().width();
    QString openGraphString = QString{ "<meta property=\"og:image\" content=\"" } +galleryThumbURL + QString{ "\" />\n" } \
        + QString{ "<meta property=\"og:image:width\" content=\"" } + QString::number( m_feeder.getThumbGallerySize().width() ) +QString{ "px\" />\n" } \
        + QString{ "<meta property=\"og:image:height\" content=\"" } +QString::number(m_feeder.getThumbGallerySize().height() ) +QString{ "px\" />\n" } \
        + QString{ "<meta property=\"og:title\" content=\"" } +m_feeder.getProjectParams().m_galleryConfig.title + QString{ "\" />\n" } \
        + QString{ "<meta property=\"og:description\" content=\"" } +m_feeder.getProjectParams().m_galleryConfig.description + QString{ "\" />\n" } \
        + QString{ "<meta property=\"og:url\" content=\"" } +m_feeder.getProjectParams().m_galleryConfig.url + QString{ "\" />\n" };


    htmlString.replace( "[META_OPENGRAPH]",	openGraphString );
    //------ FOOTER -------//
    //EZWebGallery Logo
    htmlString.replace( "[EZWEBGALLERY_LOGO]", QString("<img src=\"resources/images/EZWebGallery.png\" id=\"logo\" title=\"")
                                                        + tr("Photo gallery designed and generated using EZWebGallery.")
                                                        + QString("\" alt=\"EZWebGallery\"/>") );


    //------ TITRES -----//
    //Titre de la page
    htmlString.replace( "[HTML_TITLE]", QString("<title>")+m_feeder.getProjectParams().m_galleryConfig.title+QString("</title>") );
    htmlString.replace( "[CONTENT_TITLE]", m_feeder.getProjectParams().m_galleryConfig.title );

    //------ BOUTONS (SCREEN PHOTO) -------//
    //Toolbar
    htmlString.replace( "[TOOLBAR]", m_skinParameters.toolbar() );
    //Previous
    htmlString.replace( "[BUTTON_PREVIOUSPHOTO]", QString("<img alt=\"") + tr("Previous photo")
                                                            +QString("\" class=\"photoButtonEnabled\" id=\"boutonPrevious\" src=\"resources/images/")
                                                            +m_skinParameters.buttonImage( CSkinParameters::buttonPrevious )+QString("\" />") );
    //Next
    htmlString.replace( "[BUTTON_NEXTPHOTO]", QString("<img alt=\"") + tr("Next photo")
                                                        +QString("\" class=\"photoButtonEnabled\" id=\"boutonNext\" src=\"resources/images/")
                                                        +m_skinParameters.buttonImage( CSkinParameters::buttonNext )+QString("\" />") );
    //---------------------//

    //------- OPTIONNAL BEHAVIORS ---------//

    htmlTextStream->seek( 0 ); //Reposition  0 pour remplacer le fichier et pas faire un append
    *htmlTextStream << htmlString;



    //-- FIN DE LA GENERATION --//
    m_mutex.lock();
    m_f_WorkInProgress = false;
    emit generationFinishedSignal( m_feeder.getPhotoProperties() ); //Information de l'UI
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
            displayProgressBar( completion, "green", PtrMessage(new CMessage( tr("Generating the photos : "), QString::number(completion)+"%")) );
        }

        //Rcupration des tailles gnres par le process
        m_photoSizes.insert( photoProperties.id() + 1,  generatedPhotoParams.generatedSizes() );
        
        //Updating photoproperties to insert the read ExifTags
        //m_photoPropertiesList.replace( photoProperties.id(), photoProperties ); // ?????

        //Fin nominale du process des photos??
        if( nbPhotosProcessed + nbPhotoProcessFailed == m_nbPhotosToProcess)
        {
            if( nbPhotoProcessFailed == 0){
                emit jobDone( );
            }
            else{
                CLogger::getInstance().log(PtrMessage( new CError(CError::error(CError::Internal), tr("Unspecified.")) ));
                emit abordGenerationSignal( );
                break; //On ne devrait vraiment pas se trouver ici...
            }
        }
     break;

     case failure:
         m_msgErrorList.append( generatedPhotoParams.message() );
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
        emit jobDone();
        m_mutex.lock();
            m_f_WorkInProgress = false;
        m_mutex.unlock();

        emit forceStoppedFinishedSignal( m_msgErrorList ); //Asynchrone
    }

}
