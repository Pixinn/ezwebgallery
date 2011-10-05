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

/*
* This header is mandatory on all files of the project in order to track the global svn revision usin svnrev
* $Id: CGalleryGenerator.h 133 2011-10-04 20:05:28Z xtof $
*/

#ifndef CGalleryGenerator_H
#define CGalleryGenerator_H


#include <QString>
#include <QThread>
#include <QThreadPool>
#include <QMutex>
#include <QMap>
#include <QStringList>
#include <QStateMachine>
#include <QState>

//#include "IUserInterface.h"
#include "CPhotoProcessor.h"
#include "CErrorMessages.h"
#include "CProjectParameters.h"
#include "CPhotoProperties.h"
#include "CSkinParameters.h"
#include "CCaptionManager.h"
#include "CCaption.h"

/*****************************
 * CGalleryGenerator
 * ----------------------
 * Classe permettant de gnrer la galerie
 ******************************/

class CGalleryGenerator : public QThread
{
    Q_OBJECT

    const CErrorMessages MsgError;

    //// Signaux/Slots ////
signals:
    void debugSignal( QString );		      //Ecrire dans une fentre de Debug
    void progressBarSignal( int completion, QString color, QString message ); //Faire voluer la progressBar
    void generationFinishedSignal( /*bool success*/ QList<CPhotoProperties> );   //Gnration de toutes les photos termine
    void forceStoppedFinishedSignal( QStringList );        //L'arrt forc des thread est termin. Passage des messages d'erreur.
    void abordGenerationSignal( );
    void startGenerationSignal( );
    void jobDone( );

public slots:
    //La gnration des photos se droule dans des tches spares
    void onPhotoProcessDone( CGeneratedPhotoSetParameters photoGeneratedSizes ); //Un process photo termin
    void onAbordGeneration( );
    //Les "tches"
    void initGeneration( );
    bool generateFileStructure( );          //retour: true si tout c'est bien pass
    int  generatePhotos( );                  //retour: Nombre photos dont le traitement est plannifi
    bool generateJsFiles( );
    bool skinning( );


    ///// Oprations //////
public:
    CGalleryGenerator( );
    ~CGalleryGenerator( );
    //Interface avec UI
    void generateGallery( CProjectParameters &, const CSkinParameters &/*, const QMap<QString,CCaption> &*/ );
 //   void connectToUi( IUserInterface * );
    bool isGenerationInProgress( );
    void abordGeneration( );
    
protected:
    void run( );    

private:
    //-- interfaage UI
    void debugDisplay( QString );		//Affichage d'un message de debug
    void displayProgressBar( int completion, QString color, QString message ); //Affiche un % d'avancement sur la progressBar    
    //-- tools	
    bool photosAlreadyExist( ); //Vrifie la prsence des photos dans le rpertoire de sortie
    bool thumbsAlreadyExist( );  //Vrifie la prsence des vignettes dans le rpertoire de sortie
    

    ///// Attributs //////

    //IUserInterface* m_p_ui;

    //Machine Etat
    QStateMachine m_stateMachine;
    QState* m_p_waitingForOrders;
    QState* m_p_galleryGeneration;
    QState* m_p_init;
    QState* m_p_generatingFileStructure;
    QState* m_p_generatingPhotos;
    QState* m_p_generatingJSFiles;
    QState* m_p_skinning;
    QState* m_p_abording;
    //Paramtres de la galerie
    CProjectParameters m_parameters;    
    QStringList m_captionsList;
    CSkinParameters m_skinParameters;
    //Gnration photos
    QList<CPhotoProperties> m_photoPropertiesList;
    QThreadPool* m_p_photoProcessorPool;    //Pool des threads effectuant les traitements. Un thread par photo
    QStringList m_msgErrorList;
    QMap<int, QQueue<QSize> > m_photoSizes;            //Contient les files des tailles des photos gnres <id,file>
    QMap<int, QMap<QString,QString> > m_photoExifTags; //Contient les donnes exifs intressantes des photos <id,exif>
    volatile int m_nbPhotosToProcess;
    volatile int m_nbPhotosProcessed;
    volatile int m_nbPhotoProcessFailed;
    volatile bool m_f_WorkInProgress;
    volatile bool m_fStopRequested;       //Demande d'arrt des processors
    QString m_errorMsg;
    //Threading synchro
    QMutex m_mutex;                       //Mutex pour protger les valeurs des slots potentiellement accds par d'autres threads
    QMutex m_mutexControlProcessors;      //Mutex partag avec mes photoprocessors afin de les piloter
    
};

#endif // CGalleryGenerator_H
