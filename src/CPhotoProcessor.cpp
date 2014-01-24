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
#include <QImageReader>

#include <string>
#include <iostream>

#include "GlobalDefinitions.h"
#include "CPhotoProcessor.h"
#include "CLogger.h"
#include "CError.h"

using namespace std;

///////////////////////////////////////////////////////////////////
//////////////////// CLASSE CPhotoProcessor ///////////////////////
///////////////////////////////////////////////////////////////////

//--- Variable statique
QMutex CPhotoProcessor::m_mutexFileReading;  //Mutex partag pour viter les acces disques concurrents


//Copy of the parameters as the process will occur in its own thread
CPhotoProcessor::CPhotoProcessor(   CPhotoProperties photoProperties,
                                    QDir outPath,           //Path de la gallerie gnre
                                    const QMap<QString,QSize>& photoSizes,
                                    const QMap<QString,QSize>& thumbSizes,
                                    unsigned int quality,   //Qualit des Jpegs gnrs. Au moins deux: thumb + 1 jpeg de sortie                                    
                                    t_sharpening &sharpening, //paramtres d'accentuation
                                    const CWatermark &watermark,
                                    volatile bool* fStopRequested,    //Boolen demandant l'arrt des traitements
                                    QMutex* remoteControl )
{
    m_photoProperties = photoProperties,
    m_outPath = outPath;
    m_photoQuality = quality;
    m_photoSizes = photoSizes;
    m_thumbSizes = thumbSizes;
    m_watermark = watermark;
    m_sharpening = sharpening;
    m_fStopRequested = fStopRequested;
    m_p_mutexRemoteControl = remoteControl;
}

CPhotoProcessor::~CPhotoProcessor(){ }



////////////// SLOTS /////////////////////


////////////////// RUN ////////////////////
// This function process a SINGLE photo
// + Opens the file
// + Resize to the max size, add watermarking
// + Generate subsequent smaller sized photos
// + Generate the thumbnail
///////////////////////////////////////////
void CPhotoProcessor::run()
{
    bool  fCancel = false;
    //bool  f_refPictureComputed = false;
    bool  f_fileReadingSuccess;
    const QString thumbPrefix( THUMBPREFIXE );
    const QString thumbsPath( QString(THUMBSPATH) + "/" );
    const QString photosPath( QString(PHOTOSPATH) + "/" );
    
    CPhoto photoOriginal;    //Image originale. Trop grande pour tre utilisee  chaque fois (trop lent), on en drive 2 "masters".
    CPhoto photoMaster;      //Master pour gnrer les photos = la photo de plus grande taille watermarke
    CPhoto photoThumbMaster; //Master pour gnrer les vignettes = la photo de plus grande taille AVANT watermarking
    CPhoto photoResized;     //Photo resize  sauver sur le disque
    t_watermark watermarkParams;
    QString filename;
    QString fileToWrite;
    QSize size;

    //Annulation de la tache si demande
    m_p_mutexRemoteControl->lock();
    fCancel = *m_fStopRequested;
    m_p_mutexRemoteControl->unlock();
    if( fCancel ) { 
        m_generatedParameters.setExitStatus( stopped );
        m_generatedParameters.setPhotoProperties( m_photoProperties );
        emit processCompleted( m_generatedParameters );
        return;
    }

    try //catching exceptions from ImageMagick
    {
        //Ouverture du fichier
        m_mutexFileReading.lock(); //On locke pour optimiser les accs disque
        f_fileReadingSuccess = photoOriginal.load( m_photoProperties.fileInfo().absoluteFilePath() );
        m_mutexFileReading.unlock();
        if( !f_fileReadingSuccess ) //Si echec de la lecture du fichier
        {
            m_generatedParameters.setMessage( PtrMessage(new CError(CError::FileOpening,  m_photoProperties.fileInfo().absoluteFilePath() + " " +  photoOriginal.error())) );
            m_generatedParameters.setExitStatus( failure );
            m_generatedParameters.setPhotoProperties( m_photoProperties);
            emit processCompleted( m_generatedParameters );
        
    	    return;
        }
   
        photoOriginal.orientationExif();  //A faire avant tout resize : sinon les portions width/height peuvent changer et bug...
        photoMaster = photoOriginal;
    	   
      
        QDir fileOutPath = m_outPath;
        filename = m_photoProperties.encodedFilename();
        // 1 - Processing first photo (size max):
        //     will be used as a base for all the other to speed up the processings    
        //////////
        QString key = QString(RESOLUTIONPATH) + QString::number(1);
        size = m_photoSizes.value( key );

        // 1a - check if task was canceled
        m_p_mutexRemoteControl->lock();
        fCancel = *m_fStopRequested;
        m_p_mutexRemoteControl->unlock();
        // cancel if requested
        if( fCancel ) {
            m_generatedParameters.setExitStatus( stopped );
            m_generatedParameters.setPhotoProperties( m_photoProperties);
            emit processCompleted( m_generatedParameters );
            return;
        }
    
        // 1b - Resizing
        photoResized = photoMaster;
        // Performed only if the photo is bigger than the targeted size
        if( photoMaster.size().width() > size.width() || photoMaster.size().height() > size.height() ) {
            photoResized.zoom( size ); //Default filter : Lancsoz
        }
        photoThumbMaster = photoResized; //Saving a copy before watermarking to generate the thumbnails

        // 1c - Watermarking
        watermarkParams = m_watermark.parameters( );

        // 1c1 - Genereting if watermark is a string
        if( watermarkParams.enabled && watermarkParams.type == t_watermark::TEXT )
        {
            watermarkParams.text.setExifTags( photoMaster.exifTags( ) );
            watermarkParams.text.setFileInfo( m_photoProperties.fileInfo() );
            watermarkParams.text.setId( m_photoProperties.id() );
            m_watermark.setTaggedString( watermarkParams.text, QFont( watermarkParams.fontName ), QColor( watermarkParams.colorName ) );
        }
        // 1c2 - Applying watermark
        if( m_watermark.isValid() )
        {                
            photoResized.watermark( m_watermark, watermarkParams.position,
                                    watermarkParams.orientation, watermarkParams.relativeSize,
                                    watermarkParams.opacity );
        }

        photoMaster = photoResized; //Saving the master from which to derivate all the differently sized versions
        m_photoProperties.setExifTags( photoMaster.exifTags( ) );

        // 1d - Sharpening
        photoResized.unsharpmask( m_sharpening.radius, m_sharpening.sigma, m_sharpening.amount, m_sharpening.threshold );
        
        // 1e - Saving
        QString filedir = photosPath + key;
        fileOutPath.mkdir( filedir );
        fileOutPath.cd( filedir );
        fileToWrite =  fileOutPath.absoluteFilePath( filename );
        if( photoResized.save( fileToWrite, m_photoQuality ) ){
            m_generatedParameters.addSize( key, photoResized.size() );
        }
        else{ //Failure !
            m_generatedParameters.setMessage( PtrMessage(new CError(CError::FileSaving, fileToWrite + " " + photoResized.error())) );
            m_generatedParameters.setExitStatus( failure );
            m_generatedParameters.setPhotoProperties( m_photoProperties);
            emit processCompleted( m_generatedParameters );
	        return;
        }

        m_photoSizes.remove( key ); //removing the processed (max) size


        //2 - Iterating the remaining sizes (at least one)
        ////////////
        QMapIterator<QString,QSize> itPhotoSizes( m_photoSizes );
        while( itPhotoSizes.hasNext() )
        {

            // 2a - check if task was canceled
            m_p_mutexRemoteControl->lock();
            fCancel = *m_fStopRequested;
            m_p_mutexRemoteControl->unlock();
            // cancel if requested
            if( fCancel ) {
                m_generatedParameters.setExitStatus( stopped );
                m_generatedParameters.setPhotoProperties( m_photoProperties );
                emit processCompleted( m_generatedParameters );
                return;
            }

            itPhotoSizes.next();
            size = itPhotoSizes.value();
            key = itPhotoSizes.key();

            //2b - Resizing
            photoResized = photoMaster;
            // Performed only if the photo is bigger than the targeted size
            if( photoMaster.size().width() > size.width() || photoMaster.size().height() > size.height() ) {
                photoResized.zoom( size ); //Default filter : Lancsoz
            }

            //2c - Sharpening
            photoResized.unsharpmask( m_sharpening.radius, m_sharpening.sigma, m_sharpening.amount, m_sharpening.threshold );

            //2d - Saving
            fileOutPath = m_outPath;
            QString filedir = photosPath + key;
            fileOutPath.mkdir( filedir );
            fileOutPath.cd( filedir );
            fileToWrite =  fileOutPath.absoluteFilePath( filename );
            if( photoResized.save( fileToWrite, m_photoQuality ) ){
                m_generatedParameters.addSize( key, QSize( photoResized.size().width(), photoResized.size().height() ) );
            }
            else{ //Failure !
                m_generatedParameters.setMessage( PtrMessage(new CError(CError::FileSaving, fileToWrite + " " + photoResized.error())) );
                m_generatedParameters.setExitStatus( failure );
                m_generatedParameters.setPhotoProperties( m_photoProperties);
                emit processCompleted( m_generatedParameters );
    	        return;
            }
        } // while( itPhotoSizes.hasNext() )

    
        //3 - Iterating the required thumbnails
        ////////////
        QMapIterator<QString,QSize> itThumbSizes( m_thumbSizes );
        while( itThumbSizes.hasNext() )
        {
            // 3a - check if task was canceled
            m_p_mutexRemoteControl->lock();
            fCancel = *m_fStopRequested;
            m_p_mutexRemoteControl->unlock();
            //Cancelation
            if( fCancel ) {
                m_generatedParameters.setExitStatus( stopped );
                m_generatedParameters.setPhotoProperties( m_photoProperties);
                emit processCompleted( m_generatedParameters );
                return;
            }

            itThumbSizes.next();
            size = itThumbSizes.value();
            key = itThumbSizes.key();

            //3b - Resizing
            photoResized = photoThumbMaster;  //Without watermark
            photoResized.zoom( size );
            photoResized.removeMetadata();    //To gain some space

            //3c - Saving
            fileOutPath = m_outPath;
            QString filedir = thumbsPath + key;
            fileOutPath.mkdir( filedir );
            fileOutPath.cd( filedir );
            fileToWrite =  fileOutPath.absoluteFilePath( filename );
            if( !photoResized.save( fileToWrite, s_thumbQuality ) )
            { //Echec de la sauvegarde !
                m_generatedParameters.setMessage( PtrMessage(new CError(CError::FileSaving, fileToWrite + " " + photoResized.error())) );
                m_generatedParameters.setExitStatus( failure );
                m_generatedParameters.setPhotoProperties( m_photoProperties);
                emit processCompleted( m_generatedParameters );
                return;
            }

        } // while( itThumbSizes.hasNext() )
    }//try
    catch( std::exception &error ) { //Process any other exceptions derived from standard C++ exception
            PtrMessage err( new CError(CError::Internal, m_photoProperties.fileInfo().absoluteFilePath() + " " + error.what()));
            CLogger::getInstance().log( err );
            m_generatedParameters.setMessage( err );
            m_generatedParameters.setExitStatus( failure );
            m_generatedParameters.setPhotoProperties( m_photoProperties);
            emit processCompleted( m_generatedParameters );
    }  

    // Success !
    m_generatedParameters.setExitStatus( success );
    m_generatedParameters.setPhotoProperties( m_photoProperties);
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
    this->m_generatedSizes = other.m_generatedSizes;
    this->m_photoProperties = other.m_photoProperties;
    this->m_exitStatus = other.m_exitStatus;
    this->m_pMessage = other.m_pMessage;
}

CGeneratedPhotoSetParameters::~CGeneratedPhotoSetParameters( ){
}


void CGeneratedPhotoSetParameters::addSize(const QString& path, const QSize& size )
{
    m_generatedSizes.insert( path, size );
}


void CGeneratedPhotoSetParameters::setMessage( const PtrMessage & message ){
    m_pMessage = message;
}

void CGeneratedPhotoSetParameters::setExitStatus( const e_photoProcessStatus exitStatus ){
    m_exitStatus = exitStatus;
}

const QMap<QString, QSize>& CGeneratedPhotoSetParameters::generatedSizes( void ) const {
    return m_generatedSizes;
}

e_photoProcessStatus CGeneratedPhotoSetParameters::exitStatus( ){
    return m_exitStatus;
}
PtrMessage CGeneratedPhotoSetParameters::message( ){
    return m_pMessage;
}
