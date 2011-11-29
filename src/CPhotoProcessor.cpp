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



#include <QImage>
#include <QFile>

#include <string>
#include <QImageReader>

#include "GlobalDefinitions.h"
#include "CPhotoProcessor.h"

using namespace std;

///////////////////////////////////////////////////////////////////
//////////////////// CLASSE CPhotoProcessor ///////////////////////
///////////////////////////////////////////////////////////////////

//--- Variable statique
QMutex CPhotoProcessor::m_mutexFileReading;  //Mutex partag pour viter les acces disques concurrents
const CError CPhotoProcessor::MsgError;



CPhotoProcessor::CPhotoProcessor(   CPhotoPropertiesExtended photoProperties,
                                    QDir outPath,           //Path de la gallerie gnre
                                    QQueue<QSize> &sizes,   //Fifo des tailles  gnrer. Au moins deux: thumb + 1 taille de sortie
                                    QQueue<int> &quality,   //Qualit des Jpegs gnrs. Au moins deux: thumb + 1 jpeg de sortie                                    
                                    t_sharpening &sharpening, //paramtres d'accentuation
                                    const CWatermark &watermark,
                                    volatile bool* fStopRequested,    //Boolen demandant l'arrt des traitements
                                    QMutex* remoteControl )
{
    m_photoProperties = photoProperties,
//    m_idPhoto = photoProperties.id();
    m_generatedParameters.setIdPhoto( photoProperties.id() );
    m_inFilePath = photoProperties.fileInfo().absoluteFilePath();
    m_outPath = outPath;
    m_qualityQueue = quality;
    m_sizesQueue = sizes;
    m_watermark = watermark;
    m_sharpening = sharpening;
//    m_fCancellationRequested = false;
    m_fStopRequested = fStopRequested;
    m_p_mutexRemoteControl = remoteControl;
}

CPhotoProcessor::~CPhotoProcessor(){ }



////////////// SLOTS /////////////////////


////////////////// RUN ////////////////////
void CPhotoProcessor::run()
{
    bool  fCancel = false;
    bool  f_refPictureComputed = false;
    bool  f_fileReadingSuccess;
    const QString thumbPrefix( QString(THUMBPREFIXE) + QString(PHOTOPREFIXE) );
    const QString thumbsPath( THUMBSPATH );
    const QString photosPath( QString(PHOTOSPATH) + "/" + RESOLUTIONPATH );
    
    CPhoto photoOriginal;    //Image originale. Trop grande pour tre utilise  chaque fois (trop lent), on en drive 2 "masters".
    CPhoto photoMaster;      //Master pour gnrer les photos = la photo de plus grande taille watermarke
    CPhoto photoThumbMaster; //Master pour gnrer les vignettes = la photo de plus grande taille AVANT watermarking
    CPhoto photoResized;     //Photo resize  sauver sur le disque
    t_watermark watermarkParams;
    QString filename;
//    string filenameLocal8; //filename converti en char 8bits du jeu de caractre local
    QString fileToWrite;
    QSize size;
    int saveQuality;
    int i = 0;
    
    //Annulation de la tche si demand
    m_p_mutexRemoteControl->lock();
    fCancel = *m_fStopRequested;
    m_p_mutexRemoteControl->unlock();
    if( fCancel ) { 
        m_generatedParameters.setExitStatus( stopped );
        emit processCompleted( m_generatedParameters );
        return;
    }

    //Ouverture du fichier
    m_mutexFileReading.lock(); //On locke pour optimiser les accs disque
    f_fileReadingSuccess = photoOriginal.load( m_inFilePath );
    m_mutexFileReading.unlock();
    if( !f_fileReadingSuccess ) //Si echec de la lecture du fichier
    {
        m_generatedParameters.setMessage( tr("Unable to open the file: ") + m_inFilePath + QString(" ")
                                        + tr("Error: ") + photoOriginal.error() );
        m_generatedParameters.setExitStatus( failure );
        emit processCompleted( m_generatedParameters );
        
    	return;
    }
    QDir fileOutPath = m_outPath;
   
    photoOriginal.orientationExif();  //A faire avant tout resize : sinon les portions width/height peuvent changer et bug...
    photoMaster = photoOriginal;
    	   
    // Attention :
    // -----------
    // L'ordre du traitement doit correspondre au contenu de la pile des tailles m_sizesQueue:
    // Vignette, res1, ..., resN
      

    //Gnration des photos.
    //La premire photo resize (+grande rsolution de la file), servira de base aux autres photos afin d'acclrer les traitements
    while( m_sizesQueue.size() > 1 ) //Safe :  y a au moins deux rsolutions et une vignette
    {
        //Annulation de la tche si demand
        m_p_mutexRemoteControl->lock();
        fCancel = *m_fStopRequested;
        m_p_mutexRemoteControl->unlock();
        //Annulation de la tche
        if( fCancel ) {
            m_generatedParameters.setExitStatus( stopped );
            emit processCompleted( m_generatedParameters );
            return;
        }
            	    	
        //Toutes les tailles
        fileOutPath = m_outPath;
        size = m_sizesQueue.dequeue();
        photoResized = photoMaster;
        //On ne resize que si la photo est plus grande que la taille souhaite
        if( photoMaster.size().width() > size.width() || photoMaster.size().height() > size.height() ) {
            photoResized.zoom( size ); //Filtre par dfaut : Lancsoz
        }
                    
        //Si c'est la photo de res max, elle est watermarke si besoin, et devient la rfrence (master) pour les photos suivantes
        if( !f_refPictureComputed )
        {
            photoThumbMaster = photoResized; //Sauvegarde du master pour les vignettes avant la signature...

            //-- Watermark
            watermarkParams = m_watermark.parameters( );
            //1 - Gnration si de type texte
            if( watermarkParams.enabled && watermarkParams.type == t_watermark::TEXT )
            {
                watermarkParams.text.setExifTags( photoMaster.exifTags( ) );
                watermarkParams.text.setFileInfo( m_photoProperties.fileInfo() );
                watermarkParams.text.setId( m_photoProperties.id() );
                m_watermark.setTaggedString( watermarkParams.text, QFont( watermarkParams.fontName ), QColor( watermarkParams.colorName ) );
            }
            //2 - Application si valide
            if( m_watermark.isValid() )
            {                
                photoResized.watermark( m_watermark, watermarkParams.position,
                                        watermarkParams.orientation, watermarkParams.relativeSize,
                                        watermarkParams.opacity );
            }

            photoMaster = photoResized; //Il est essentiel de sauver le master AVANT sharpening afin de maximiser la qualit d'image
            f_refPictureComputed = true;
            m_generatedParameters.setExifTags( photoMaster.exifTags( ) );
        }

        //sharpening
        photoResized.unsharpmask( m_sharpening.radius, m_sharpening.sigma, m_sharpening.amount, m_sharpening.threshold );
                
        //Sauvegarde
        saveQuality = m_qualityQueue.dequeue();
        fileOutPath.cd( photosPath + QString::number(++i) );
        filename = PHOTOPREFIXE + QString::number( m_photoProperties.id() );
        fileToWrite =  fileOutPath.absoluteFilePath( filename ) + ".jpg";
        if( photoResized.save( fileToWrite, saveQuality ) ){
            m_generatedParameters.enqueueSize( QSize( photoResized.size().width(), photoResized.size().height() ) );
        }
        else{ //Echec de la sauvegarde !
            m_generatedParameters.setMessage( MsgError.error(CError::FileSaving) + fileToWrite + tr(" error: ") + photoResized.error() );
            m_generatedParameters.setExitStatus( failure );
            emit processCompleted( m_generatedParameters );
    	    return;
        }

    }//Fin While - Gnration des photos

            
    //Gnration de la vignette
    size = m_sizesQueue.dequeue();
    filename = thumbPrefix + QString::number( m_photoProperties.id() );

    photoResized = photoThumbMaster;
    photoResized.zoom( size );
    photoResized.removeMetadata(); //Enlve les mtadonnes pour gagner en place

    //Sauvegarde vignette
    fileOutPath = m_outPath;
    fileOutPath.cd( thumbsPath );
    fileToWrite =  fileOutPath.absoluteFilePath( filename ) + ".jpg";
    saveQuality = m_qualityQueue.dequeue();
    if( !photoResized.save( fileToWrite, saveQuality ) )
    { //Echec de la sauvegarde !
        m_generatedParameters.setMessage( MsgError.error(CError::FileSaving) + fileToWrite + tr(" error: ") + photoResized.error() );
        m_generatedParameters.setExitStatus( failure );
        emit processCompleted( m_generatedParameters );
        return;
    }

    m_generatedParameters.setExitStatus( success );
    emit processCompleted( m_generatedParameters );

    return;
}



///////////////////////////////////////////////////////////////////
/////////////// CLASSE CGeneratedPhotoSetParameters ///////////////////
///////////////////////////////////////////////////////////////////
CGeneratedPhotoSetParameters::CGeneratedPhotoSetParameters( ) :
    QObject( )
{
    this->m_exitStatus = failure;
}

CGeneratedPhotoSetParameters::CGeneratedPhotoSetParameters( const CGeneratedPhotoSetParameters &other ) :
    QObject( )
{
    this->m_generatedSizesQueue = other.m_generatedSizesQueue;
    this->m_idPhoto = other.m_idPhoto;
    this->m_exitStatus = other.m_exitStatus;
    this->m_message = other.m_message;
    this->m_exifTags = other.m_exifTags;
}

CGeneratedPhotoSetParameters::~CGeneratedPhotoSetParameters( ){
}

void CGeneratedPhotoSetParameters::enqueueSize( QSize toQueue ){
    m_generatedSizesQueue.enqueue( toQueue );
}

void CGeneratedPhotoSetParameters::setIdPhoto( int id ){
    m_idPhoto = id;
}
void CGeneratedPhotoSetParameters::setMessage( const QString & message ){
    m_message = message;
}
void CGeneratedPhotoSetParameters::setExifTags( const QMap<QString,QString> & exifTags ){
    m_exifTags = exifTags;
}
void CGeneratedPhotoSetParameters::setExitStatus( const e_photoProcessStatus exitStatus ){
    m_exitStatus = exitStatus;
}

QQueue<QSize> CGeneratedPhotoSetParameters::generatedSizesQueue(){
    return m_generatedSizesQueue;
}
int CGeneratedPhotoSetParameters::idPhoto(){
    return m_idPhoto;
}
e_photoProcessStatus CGeneratedPhotoSetParameters::exitStatus( ){
    return m_exitStatus;
}
QString CGeneratedPhotoSetParameters::message( ){
    return m_message;
}
QMap<QString,QString> CGeneratedPhotoSetParameters::exifTags( ){
    return m_exifTags;
}
