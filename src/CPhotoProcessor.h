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


#ifndef CPHOTOPROCESSOR_H
#define CPHOTOPROCESSOR_H

#include <QRunnable>
#include <QMutex>
#include <QDir>
#include <QQueue>
#include <QSize>
#include <QString>
#include <QMutex>

#include "CPhoto.h"
#include "CPhotoProperties.h"
#include "CErrorMessages.h"
#include "CProjectParameters.h"


enum e_photoProcessStatus{
    success,
    failure,
    stopped
};

typedef struct{
    double amount;
    double radius;
    double sigma;
    double threshold;
}t_sharpening;


/*****************************
 * CGeneratedPhotoSetParameters
 * ----------------------
 * Paramtres du set de photo gnres
 ******************************/
class CGeneratedPhotoSetParameters : public QObject
{

    public:
        CGeneratedPhotoSetParameters( );
        CGeneratedPhotoSetParameters(const CGeneratedPhotoSetParameters &);
        ~CGeneratedPhotoSetParameters( );
        void enqueueSize( QSize );
        void setIdPhoto( int );
        void setMessage( const QString & );
        void setExitStatus( const e_photoProcessStatus );
        void setExifTags( const  QMap<QString,QString>  &);
        QQueue<QSize> generatedSizesQueue( );
        int idPhoto( );
        e_photoProcessStatus exitStatus( );
        QString message( );
        QMap<QString,QString> exifTags( );

    private:
        int m_idPhoto; //Numro de la photo traite
        QQueue<QSize> m_generatedSizesQueue;  
        QMap<QString,QString> m_exifTags; //Tags de la photo traite        
        QString m_message;
        e_photoProcessStatus m_exitStatus;
};



/*****************************
 * CPhotoProcessor
 * ----------------------
 * Classe permettant de traiter la gnration d'une photo.
 ******************************/

class CPhotoProcessor : public QObject, public QRunnable
{
    Q_OBJECT    

    //variable statiques
    static QMutex m_mutexFileReading; //Mutex partag pour viter lesacces disques concurrents
    static const CErrorMessages MsgError;
    
    signals:
        void processCompleted( CGeneratedPhotoSetParameters );
	
    public:
        CPhotoProcessor( CPhotoProperties photoProperties, //Proprits de la photo  traiter
                         QDir outPath,           //Path de la gallerie gnre
                         QQueue<QSize> &sizes,   //Fifo des tailles  gnrer. Au moins deux: thumb + 1 taille de sortie
                         QQueue<int> &quality,   //Qualit des Jpegs gnrs. Au moins deux: thumb + 1 jpeg de sortie
                         t_sharpening &sharpening,
                         const CWatermark &watermark,
                         volatile bool* fStopRequested,   //Boolen demandant l'arrt des traitements
                         QMutex* remoteControl);   //Mutex pour forcer l'arrt
        ~CPhotoProcessor( );
        void run( );
     
    private:        
        QMutex* m_p_mutexRemoteControl; //Mutex permettant de protger les arrts "forcs" des threads
        CPhotoProperties m_photoProperties;
        QString m_inFilePath;
        QDir m_outPath;
        QQueue<QSize> m_sizesQueue;
        QQueue<int> m_qualityQueue;
        CWatermark m_watermark;
        t_sharpening m_sharpening;
        CGeneratedPhotoSetParameters m_generatedParameters;        
//      int m_idPhoto;
//      bool m_fCancellationRequested;
        volatile bool* m_fStopRequested;
        

};

#endif // CPHOTOPROCESSOR_H
