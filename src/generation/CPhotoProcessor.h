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
#include <QMap>
#include <QSize>
#include <QString>
#include <QMutex>

#include "CPhoto.h"
#include "CPhotoProperties.h"
#include "CError.h"
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
        inline void addSize(const QString&, const QSize& );
        void setPhotoProperties( const CPhotoProperties& properties )   {
            m_photoProperties = properties;
        }
        void setMessage( const PtrMessage & );
        void setExitStatus( const e_photoProcessStatus );
        const QMap<QString, QSize>& generatedSizes( void ) const;
        e_photoProcessStatus exitStatus( );
        PtrMessage message( );
        CPhotoProperties photoProperties( void ) { return m_photoProperties; }

    private:
        CPhotoProperties m_photoProperties;
        QMap<QString,QSize> m_generatedSizes;
        PtrMessage m_pMessage;
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
    static QMutex m_mutexFileReading; //Mutex partag pour viter les acces disques concurrents
    
    signals:
        void processCompleted( CGeneratedPhotoSetParameters );
	
    public:
        CPhotoProcessor( CPhotoProperties photoProperties, //Proprits de la photo  traiter
                         QDir outPath,           //Path de la gallerie gnre
                         const QMap<QString,QSize> & photoSizes,
                         const QMap<QString,QSize> & thumbSizes,
                         unsigned int quality,
                         t_sharpening &sharpening,
                         const CWatermark &watermark,
                         volatile bool* fStopRequested,   //Boolen demandant l'arrt des traitements
                         QMutex* remoteControl);   //Mutex pour forcer l'arrt
        ~CPhotoProcessor( );
        void run( );
     
    private:
        static const int s_thumbQuality = 80;

        QMutex* m_p_mutexRemoteControl; //Mutex permettant de protger les arrts "forcs" des threads
        CPhotoProperties m_photoProperties;
        QDir m_outPath;
        //QQueue<QSize> m_sizesQueue;
        QMap<QString,QSize> m_photoSizes;
        QMap<QString,QSize> m_thumbSizes;
        unsigned int m_photoQuality;
        CWatermark m_watermark;
        t_sharpening m_sharpening;
        CGeneratedPhotoSetParameters m_generatedParameters;        
        volatile bool* m_fStopRequested;
        

};

#endif // CPHOTOPROCESSOR_H
