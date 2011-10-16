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

using namespace Magick;



///////////////////////////////////////////////////////////////////
//////////////////// CLASSE CGalleryGenerator /////////////////////
///////////////////////////////////////////////////////////////////

//------------------ DEFINITIONS ----------------------//

#define CSSSKINFILENAME     "skin.css"
#define SHARPENINGTHRESHOLD 0.04        /* quivalent  10 */

//-------------------- FONCTIONS -----------------------//

//CA_Interface_GalleryGenerator::~CA_Interface_GalleryGenerator(){ }// Implmentation du destructeur virtuel obligatoire

CGalleryGenerator::CGalleryGenerator( )
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
void CGalleryGenerator::generateGallery( CProjectParameters &projectParameters, const CSkinParameters &skinParameters )
{
	QStringList photoList;
	QString photoName;
    
    //Copie des paramtres en local avant lancement du thread
    if( !this->isGenerationInProgress() )
    {
        CCaption emptyCaption;
        CPhotoProperties photoProperties;
		QDir inputDir;
        QFileInfo fileInfo;
        int id = 1;

        //-- Init
        m_parameters = projectParameters;
        m_parameters.m_galleryConfig.description.replace("\n"," ");
        m_skinParameters = skinParameters;
		
		inputDir = m_parameters.m_galleryConfig.inputDir;

        displayProgressBar( 0, "green", "0%" );

        //Cration de la liste ordone des proprits des photos  utiliser
        //REMARQUE : pour le moment, la liste sera ordonne selon l'ordre alphabtique du nom des photos !
        //Dans le futur, il faudra que ce soit le Caption Manager qui ordonne les photos. Avec un rle tendu, changement de nom?
        m_photoPropertiesList.clear();
        photoList = m_parameters.m_photoPropertiesMap.keys();
	    qSort( photoList.begin(), photoList.end() ); //Le tri de la liste de photos !            
        foreach( photoName, photoList )
        {
            photoProperties = m_parameters.m_photoPropertiesMap.value( photoName );
			fileInfo = QFileInfo( inputDir.absoluteFilePath( photoName ) );
			photoProperties.setFileInfo( fileInfo );
            photoProperties.setId( id++ );
            m_photoPropertiesList.append( photoProperties );
        }
    
        emit startGenerationSignal(); //Dmarrage des traitements
    }

}

//Permet d'appeler onAbordGeneration de manire ASYNCHRONE  partir du thread de l'UI
void CGalleryGenerator::abordGeneration( )
{
    emit abordGenerationSignal( );
}

/*void CGalleryGenerator::connectToUi( IUserInterface* ui ){
    m_p_ui = ui;
    connect( this, SIGNAL( debugSignal(QString) ), m_p_ui, SLOT( onLogMsg(QString) ) );
    connect( this, SIGNAL( progressBarSignal( int, QString, QString ) ), m_p_ui, SLOT( onProgressBar( int, QString, QString ) ) );
    connect( this, SIGNAL( generationFinishedSignal(QList<CPhotoProperties> ) ), m_p_ui, SLOT( onGalleryGenerationFinished( QList<CPhotoProperties> ) ) );
    connect( this, SIGNAL( forceStoppedFinishedSignal( QStringList ) ), m_p_ui, SLOT( onForceStoppedFinished( QStringList ) ) );
}
*/

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

bool CGalleryGenerator::photosAlreadyExist(  )
{    
    QDir photosDir = m_parameters.m_galleryConfig.outputDir;
    QFile photoFile;
    int nbRes = m_parameters.m_photosConfig.nbIntermediateResolutions;

    photosDir.cd( PHOTOSPATH );
    for( int i = 1; i <= m_parameters.m_photoPropertiesMap.size(); i++)
    {
        QString photoName = PHOTOPREFIXE + QString::number(i) + ".jpg";
        for(int res=1; res <= nbRes; res++)
        {
            QString resDir = RESOLUTIONPATH + QString::number(res) + "/";
            photoFile.setFileName( photosDir.absoluteFilePath( resDir + photoName ) );
            if( !photoFile.exists() ){
                return false;
            }
        }
    }

    return true;
}


bool CGalleryGenerator::thumbsAlreadyExist()
{
    QDir thumbsDir = m_parameters.m_galleryConfig.outputDir;
    QFile thumbFile;

    thumbsDir.cd( THUMBSPATH );
    for( int i = 1; i <= m_parameters.m_photoPropertiesMap.size(); i++)
    {
        QString thumbName = QString(THUMBPREFIXE) + QString(PHOTOPREFIXE) + QString::number(i) + QString(".jpg");
        thumbFile.setFileName( thumbsDir.absoluteFilePath( thumbName ) );
            if( !thumbFile.exists() ){
                return false;
            }
    }

    return true;
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
        m_msgErrorList.append( CErrorMessages::error(DirectoryCreation) + thumbsFilesPath );
        emit abordGenerationSignal( );
        return false;
    }
    for(int i = 1; i <= m_parameters.m_photosConfig.nbIntermediateResolutions; i++){
        if( !outPath.mkpath( photosFilesPath + QString::number(i) ) ){
            m_msgErrorList.append( CErrorMessages::error(DirectoryCreation) + photosFilesPath + QString::number(i) );
            emit abordGenerationSignal( );
            return false;
        }
    }

    //Copie des fichiers ressources (ie non modifiable, ne faisant pas partie des skins)
    outPath = QDir( m_parameters.m_galleryConfig.outputDir );
    srcPath = CPlatform::applicationDirPath();
    if( !srcPath.cd( QString(RESSOURCES) ) ){
        m_msgErrorList.append(  CErrorMessages::error(SourceFileNotFound) );
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
        photosAlreadyExist() &&
        thumbsAlreadyExist() )
    {
        emit jobDone();
        return 0;
    }//Attention cela ne vrifie pas la prsence de la vignette reprsentant la gallerie

//    QMapIterator<QString,QDateTime> photoListIterator( m_parameters.m_photosList );
    QQueue<QSize> sizesList;
    QQueue<int> qualityList;
    QDir outPath( m_parameters.m_galleryConfig.outputDir );
    QDir inPath( m_parameters.m_galleryConfig.inputDir );


    //Gnration de la vignette reprsentant la galerie
    //Cette gnration est effectue  part de celle des autres vignettes et des photos
    if( !m_parameters.m_galleryConfig.thumbPhoto.isEmpty() )
    {
        QImage photo;
        QImage thumbnail;
        QString photoName = inPath.absoluteFilePath( m_parameters.m_galleryConfig.thumbPhoto );
        outPath.cd( PHOTOSPATH );
        if( photo.load( photoName )){
            thumbnail = photo.scaled( QSize(250,250),
                                      Qt::KeepAspectRatio,
                                      Qt::SmoothTransformation );
            thumbnail.save( outPath.absoluteFilePath( GALLERYTHUMBFILENAME ), 0, 77 );
        }
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
                m_msgErrorList.append( CErrorMessages::error(WatermarkInvalid) + watermarkImage.error() );
                emit abordGenerationSignal( );
                return false;
            }
        }
        
/*
        else{
            watermarkImage = CWatermark( watermarkParams.text, QFont( watermarkParams.fontName ), QColor( watermarkParams.colorName ) );
        }
*/

    }
    
    m_photoSizes.clear();//Les tailles des photos qui seront gnres
    CPhotoProcessor* photoToProcess;
    int idPhotoToProcess = 1;
    
    //- Cration de la file des tailles des photos  gnrer:
    //- IMPORTANT : L'ordre doit correspondre  l'ordre des traitements au sein de CPhotoProcessor
    //- tailleMax, ..., tailleMin, tailleVignette
    sizesList.enqueue( QSize( m_parameters.m_photosConfig.maxSizeW, m_parameters.m_photosConfig.maxSizeH ) );
    if( m_parameters.m_photosConfig.nbIntermediateResolutions > 2){
        int spaceW;
        int spaceH;
        int nbRes = m_parameters.m_photosConfig.nbIntermediateResolutions - 2;
        spaceW = (m_parameters.m_photosConfig.maxSizeW - m_parameters.m_photosConfig.minSizeW)/(nbRes+1);
        spaceH = (m_parameters.m_photosConfig.maxSizeH - m_parameters.m_photosConfig.minSizeH)/(nbRes+1);
        for(int i = 1; i <= nbRes; i++){
            sizesList.enqueue( QSize( m_parameters.m_photosConfig.maxSizeW - i*spaceW,
                                      m_parameters.m_photosConfig.maxSizeH - i*spaceH ) );
        }
    }
    sizesList.enqueue( QSize( m_parameters.m_photosConfig.minSizeW, m_parameters.m_photosConfig.minSizeH ) );
    sizesList.enqueue( QSize( m_parameters.m_thumbsConfig.size, m_parameters.m_thumbsConfig.size ) ); //Thumbnails toujours en dernier

    //- Cration de la file des qualits
    for(int i = 1; i <= m_parameters.m_photosConfig.nbIntermediateResolutions; i++){
        qualityList.enqueue( m_parameters.m_photosConfig.quality );
    }
    qualityList.enqueue( m_parameters.m_thumbsConfig.quality ); //Thumbnails toujours en dernier !

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
                                              sizesList,
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
    //Constantes : Noms de fichiers
    const QString jsGalleryConfigurationFileName( JSCONFFILENAME );
    const QString jsGalleryPresentationFileName( JSPRESFILENAME );
    QDir path( m_parameters.m_galleryConfig.outputDir );   

    //////////// Configuration ///////////////
    //---- Ouverture du fichier
    path.cd( JSPATH );
    QFile file_jsGalleryConfiguration( path.absoluteFilePath( jsGalleryConfigurationFileName ) );
    if (!file_jsGalleryConfiguration.open(QIODevice::WriteOnly | QIODevice::Text)){
         m_msgErrorList.append( CErrorMessages::error(FileOpening) +  file_jsGalleryConfiguration.fileName() );
         emit abordGenerationSignal( );
         return false;
    }
    file_jsGalleryConfiguration.setPermissions( QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::ReadOther );
    QTextStream jsGalleryConfigurationStream( &file_jsGalleryConfiguration );
    jsGalleryConfigurationStream.setCodec( "UTF-8" );
    jsGalleryConfigurationStream.setGenerateByteOrderMark (true);
    //---- proprits
    //Ecriture du fichier JS
    jsGalleryConfigurationStream
            << "var TAILLECACHEPHOTO = " << m_parameters.m_galleryConfig.prefetchCacheSize << ";" << endl
            << "var NBPHOTOSAPRECHARGER = " << m_parameters.m_galleryConfig.nbPhotosToPrefetch << ";" << endl
            << "var F_PHOTO_RIGHTCLICKENABLED = " << m_parameters.m_galleryConfig.f_rightClickEnabled << ";" << endl
            // ********* EN DUR -> TEMPORAIRE !! ************ //
            << "var PHOTOFILEPREFIXE = " << "\"Photo_\"" << ";" << endl /* ATTENTION : doit correspondre  GlobalDefinitions.h !!! */            
            << "var NUMPREMIEREPHOTO = 1;" << endl
            << "var NUMPREMIEREPAGEINDEX = 1;"	 << endl;

    //---- photos
    QMapIterator<int, QQueue<QSize> > i( m_photoSizes );
    QQueue<QSize> localSizeQueue;
    QSize localSize;
    int nbPhotos = m_photoSizes.size();
    int numPhoto;
    int nbRes;
    int numRes;

    jsGalleryConfigurationStream << "listePhotos =  { " << endl;

    while(i.hasNext())
    {
       localSizeQueue =  (i.next()).value();
       numPhoto = i.key( );
       nbRes = localSizeQueue.size();

       jsGalleryConfigurationStream << QString::number(numPhoto) <<  ":{fileName:\"" << PHOTOPREFIXE << QString::number(numPhoto) << ".jpg\",\tres:{\t";
       numRes = 1;
       while( !localSizeQueue.isEmpty() )
       {
            localSize = localSizeQueue.dequeue();
            if( numRes < nbRes){
                jsGalleryConfigurationStream << QString::number(numRes++) << " : {width:" << QString::number(localSize.width()) << ",\theight:" << QString::number(localSize.height()) << "},\t";
            }

       }//fin while

       //dernire rsolution
       jsGalleryConfigurationStream << QString::number(numRes) << " : {width:" << QString::number(localSize.width()) << ",\theight:" << QString::number(localSize.height()) << "}";

       if( numPhoto < nbPhotos ){
           jsGalleryConfigurationStream << "} }," << endl;
       }

    }//fin while

    //dernire photo
    jsGalleryConfigurationStream << "} }" << endl << "};" << endl;

    //---- Lgendes
    int iteratorCaptions;
    CCaption caption;
    QString renderedCaption;
    CPhotoProperties photoProperties;
    jsGalleryConfigurationStream << "listeCaptions =  {" << endl;
    for( iteratorCaptions = 1; iteratorCaptions < m_photoPropertiesList.size(); iteratorCaptions++ ) {
        photoProperties = m_photoPropertiesList.at( iteratorCaptions - 1);
		caption = photoProperties.caption();
        renderedCaption = caption.render();
        jsGalleryConfigurationStream << QString::number(iteratorCaptions) << "\t:\"" << renderedCaption << "\"," << endl;
    }
    //dernire lgende, spare car il ne doit pas y avoir de virgule finale dans la liste
    photoProperties = m_photoPropertiesList.at( iteratorCaptions - 1);
    caption = photoProperties.caption();
    renderedCaption = caption.render();
    jsGalleryConfigurationStream << QString::number(iteratorCaptions) << "\t:\"" << renderedCaption << "\"" << endl << "};" << endl;

    //---- Fermeture du fichier
    file_jsGalleryConfiguration.close();




    //////////// Prsentation ///////////////
   // Attention : certaines proprits doivent tre en phase avec les css
   
   // -> Dbut du process de skinning

    //---- Ouverture du fichier
    QFile file_jsGalleryPresentation( path.absoluteFilePath( jsGalleryPresentationFileName ) );
    if (!file_jsGalleryPresentation.open(QIODevice::WriteOnly | QIODevice::Text)){
         m_msgErrorList.append( CErrorMessages::error(FileOpening) +  file_jsGalleryPresentation.fileName() );
         emit abordGenerationSignal( );
         return false;
    }
    file_jsGalleryPresentation.setPermissions( QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::ReadOther );
    QTextStream jsGalleryPresentationStream( &file_jsGalleryPresentation );
    jsGalleryPresentationStream.setCodec( "UTF-8" );
    jsGalleryPresentationStream.setGenerateByteOrderMark (true);


    //---- Image
    jsGalleryPresentationStream << "var URL_QUALITY  = " << m_parameters.m_photosConfig.quality << ";" << endl
                                << "var MAXPHOTOWIDTH  = " << m_parameters.m_photosConfig.maxSizeW << ";" << endl
                                << "var MAXPHOTOWHEIGHT = " << m_parameters.m_photosConfig.maxSizeH << ";" << endl;


   //---- Fermeture du fichier
    file_jsGalleryPresentation.close();
    
    emit jobDone();
    return true;
}



///////////////////////////////////////////////////
///////////////// cssSkinning /////////////////////
///////////////////////////////////////////////////

// -> Fin du process de skinning aprs 

bool CGalleryGenerator::skinning( )
{
    QDir imgPath( m_parameters.m_galleryConfig.outputDir );
    QImage buttonIndex;
    const int offset = 33;

	//-------- Copie des fichiers de la skin --------//
        QDir outSkinPath( m_parameters.m_galleryConfig.outputDir );

        displayProgressBar( 100, "green", tr("Skinning...") );
	
        //---------- COPIE DES FICHIERS RESSOURCES --------//
        outSkinPath = QDir( m_parameters.m_galleryConfig.outputDir );        
        if( !outSkinPath.mkpath( RESIMGPATH ) ){
            m_msgErrorList.append( CErrorMessages::error(FileCreation) + outSkinPath.absolutePath() + QString("/") + QString(RESIMGPATH) );
            emit abordGenerationSignal( );
            return false;
        }
        outSkinPath.cd( RESIMGPATH );
        if( !m_skinParameters.copyRessources(outSkinPath/*, m_msgErrorList*/) ){
            m_msgErrorList.append( m_skinParameters.errors() );
            emit abordGenerationSignal( );
            return false;
        }
        //
        //
        //
        //
        //
        //---------- CREATION DU CSS --------//
        //--- Ouverture du fichier
        outSkinPath = QDir( m_parameters.m_galleryConfig.outputDir );
        if( !outSkinPath.mkpath( CSSPATH ) ){
            m_msgErrorList.append( CErrorMessages::error(FileCreation) + outSkinPath.absolutePath() + QString("/") + QString(CSSPATH) );
            emit abordGenerationSignal( );
            return false;
        }
        outSkinPath.cd( CSSPATH );
        QFile skinCssFile( outSkinPath.absoluteFilePath( CSSSKINFILENAME ) );
        if (!skinCssFile.open(QIODevice::WriteOnly | QIODevice::WriteOnly | QIODevice::Text)){
                m_msgErrorList.append( CErrorMessages::error(FileOpening) +  skinCssFile.fileName() );
                emit abordGenerationSignal( );
                return false;
        }
        skinCssFile.setPermissions( QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::ReadOther );
        QTextStream cssSkinStream( &skinCssFile );
        cssSkinStream.setCodec( "UTF-8" );
        cssSkinStream.setGenerateByteOrderMark (true);
        
        //--- Rcupration des paramtres de la skin
        CCssSheet skinCssSheet = m_skinParameters.toCss( );
        
         //--- Modification du Css: prise en compte des paramtres pour
         //    les tailles de la mosaque de vignettes.
        int thumbBoxW = m_parameters.m_thumbsConfig.size + 2*m_skinParameters.thumbImgBorderSize;
        int thumbBoxH = thumbBoxW;
        int wrapperW = m_parameters.m_thumbsConfig.nbColumns * ( thumbBoxW + 2*m_skinParameters.thumbBoxBorderSize );
        int wrapperH = m_parameters.m_thumbsConfig.nbRows * ( thumbBoxH + 2*m_skinParameters.thumbBoxBorderSize ); 
        
      	CCssSelection * selection;
        CCssSelection * subselection;
        QStringList selector;
        
        selection = new CCssSelection( "#indexSliderContainer" );
            selection->setProperty( "width", QString::number(wrapperW) + "px" );
            skinCssSheet.addSelection( *selection );
        delete selection;
        selection = new CCssSelection( ".thumbsWrapper" );
            selection->setProperty( "width", QString::number(wrapperW) + "px" );
            selection->setProperty( "height", QString::number(wrapperH) + "px" );
        skinCssSheet.addSelection( *selection );
        delete selection;
        selection = new CCssSelection( ".scrollContainer" );
            selection->setProperty( "width", QString::number(wrapperW) + "px" );
            selection->setProperty( "height", QString::number(wrapperH) + "px" );		
            subselection = new CCssSelection( "div.slidingPanel" );
                subselection->setProperty( "width", QString::number(wrapperW) + "px" );
            selection->addSubSelection( *subselection );	
        skinCssSheet.addSelection( *selection );	
        delete selection;
        delete subselection;   
        selector << ".thumbsWrapperContainer"; //Existe dj: on rcupre la slection de la skin -> sinon crasement !
        CCssSelection thumbWrapperSelection = skinCssSheet.selection( selector );
            thumbWrapperSelection.setProperty( "width", QString::number(wrapperW) + "px" );
            thumbWrapperSelection.setProperty( "height", QString::number(wrapperH) + "px" );
        skinCssSheet.addSelection( thumbWrapperSelection );
        selector.clear();        
        selector << ".thumbBox";            
        CCssSelection thumBoxSelection = skinCssSheet.selection( selector );
            thumBoxSelection.setProperty( "width", QString::number(thumbBoxW) + "px" );
            thumBoxSelection.setProperty( "height", QString::number(thumbBoxH) + "px" );
        skinCssSheet.addSelection( thumBoxSelection );
     
        //--- Ecriture du fichier sur le disque
        skinCssSheet.toStream( cssSkinStream );
        skinCssFile.close();
	//
	//
	//
	//
	//
        //-------- MODIFICATION DU JAVASCRIPT --------//
        /*const QString jsGalleryConfigurationFileName( JSCONFFILENAME );*/
    	const QString jsGalleryPresentationFileName( JSPRESFILENAME );
        const QString jsFilesPath( JSPATH );
        outSkinPath = QDir( m_parameters.m_galleryConfig.outputDir );

    	outSkinPath.cd( jsFilesPath );		
    	
    	//---- Ouverture du fichier
    	QFile file_jsGalleryPresentation( outSkinPath.absoluteFilePath( jsGalleryPresentationFileName ) );
        if (!file_jsGalleryPresentation.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text)){
                m_msgErrorList.append( CErrorMessages::error(FileOpening) +  file_jsGalleryPresentation.fileName() );
                emit abordGenerationSignal( );
         	return false;
    	}
        file_jsGalleryPresentation.setPermissions( QFile::ReadOwner | QFile::WriteOwner | QFile::ReadUser | QFile::WriteUser | QFile::ReadGroup | QFile::ReadOther );
    	QTextStream jsGalleryPresentationStream( &file_jsGalleryPresentation );
    	jsGalleryPresentationStream.setCodec( "UTF-8" );
        jsGalleryPresentationStream.setGenerateByteOrderMark (true);
    	
    	//---- Index
        jsGalleryPresentationStream << "var NBTHUMBSBYPAGE = " <<  QString::number(m_parameters.m_thumbsConfig.nbColumns * this->m_parameters.m_thumbsConfig.nbRows) << ";" << endl;
    	//---- RESIZE ET RECENTRAGE
        /* //Lecture de la taille du bouton index
        imgPath.cd( RESIMGPATH );
        if( buttonIndex.load( imgPath.absoluteFilePath( m_skinParameters.buttonImage(CSkinParameters::buttonIndex) ) ) ) {
            offset = buttonIndex.height() + 10;
        }
        else { 
            offset = 75;
        }*/
    	jsGalleryPresentationStream << "var NBOFFSETHAUT = " << QString::number( offset ) << ";" << endl
                                    << "var NBOFFSETBAS  = " << QString::number( offset ) << ";" << endl
                                    << "var IMAGEQUALITYSTRATEGY = " << QString::number(m_parameters.m_photosConfig.imageOptimizationStrategy) << ";" << endl;
        //---- Photo
        jsGalleryPresentationStream << "var MAINPHOTOPADDING = " << QString::number(m_skinParameters.photoPaddingSize) << endl;

        //---- Fermeture du fichier
        file_jsGalleryPresentation.close();
    	//
    	//
    	//
    	//
    	//
        //
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
            htmlString.replace( "[EZWEBGALLERY_LOGO]", QString("<img src=\"ressources/images/EZWebGallery.gif\" id=\"logo\" title=\"")
                                                                + tr("Photo gallery designed and generated using EZWebGallery.")
                                                                + QString("\" alt=\"EZWebGallery\"/>") );
			//Add-this
            if( m_parameters.m_galleryConfig.f_shareOnSocialNetworks ){
                QString addthisString = QString("<div class=\"addthis_toolbox addthis_default_style\" style=\"margin-left:50px\">\n")+
                                        QString("\t<span class=\"addthis_separator\"></span>\n")+
                                        QString("\t<a href=\"http://addthis.com/bookmark.php?v=250\" class=\"addthis_button_compact\"></a>\n")+
                                        QString("\t<a class=\"addthis_button_facebook\"></a>\n")+
                                        QString("\t<a class=\"addthis_button_twitter\"></a>\n")+
                                        QString("\t<a class=\"addthis_button_googlebuzz\"></a>\n")+
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
            m_msgErrorList.append( CErrorMessages::error(FileOpening) +  htmlFile.fileName() );
            emit abordGenerationSignal( );
            return false;
        }
        htmlFile.close();
		
		
        //-- FIN DE LA GENERATION --//
        m_mutex.lock();
        m_f_WorkInProgress = false;        
        emit generationFinishedSignal( /*true*/m_photoPropertiesList ); //Information de l'UI
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
    int idPhotoDone;
    bool f_wip;
    bool f_stopRequested;
    m_mutex.lock();
    f_wip = m_f_WorkInProgress;
    m_mutex.unlock();
    CPhotoProperties photoProperties;

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
        
        idPhotoDone = generatedPhotoParams.idPhoto();
        //Rcupration des tailles gnres par le process
        m_photoSizes.insert( idPhotoDone, generatedPhotoParams.generatedSizesQueue() );
        //Mise  jour des proprits de la photo
        photoProperties = m_photoPropertiesList.at( idPhotoDone - 1 );
        photoProperties.setExifTags( generatedPhotoParams.exifTags() );
        photoProperties.setProcessed( true );
        m_photoPropertiesList.replace( idPhotoDone - 1, photoProperties );

        //Fin nominale du process des photos??
        if( nbPhotosProcessed + nbPhotoProcessFailed == m_nbPhotosToProcess)
        {
            if( nbPhotoProcessFailed == 0){
                emit jobDone();
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
            f_stopRequested = m_fStopRequested;
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
        m_p_photoProcessorPool->waitForDone(); 	//Pour que les dernires threads du pool aient le temps de se terminer proprement
												//Dangereux si plus de deux cpus??
        emit jobDone();
        m_mutex.lock();
            m_f_WorkInProgress = false;
        m_mutex.unlock();
        this->debugDisplay("emit forcedFinished");
        emit forceStoppedFinishedSignal( m_msgErrorList ); //Asynchrone
    }

}
