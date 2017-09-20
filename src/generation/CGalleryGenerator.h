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



#ifndef CGalleryGenerator_H
#define CGalleryGenerator_H

#include <memory>

#include <QString>
#include <QThread>
#include <QThreadPool>
#include <QMutex>
#include <QMap>
#include <QStringList>
#include <QStateMachine>
#include <QState>

#include "CGalleryGeneratorFeeder.h"
#include "CPhotoProcessor.h"
#include "CProjectParameters.h"
#include "CPhotoProperties.h"
#include "CSkinParameters.h"
#include "CToolbar.h"
#include "CCaptionManager.h"
#include "CCaption.h"
#include "CMessage.h"
#include "CPhotoProperties.h"
#include "Json.h"

/*****************************
 * CGalleryGenerator
 * ----------------------
 * Classe permettant de générer la galerie
 ******************************/

class CGalleryGenerator : public QThread
{
    Q_OBJECT

    //// Signaux/Slots ////
signals:
    void progressBarSignal( int completion, QString color, PtrMessage message ); //Faire voluer la progressBar
    void generationFinishedSignal( /*bool success*/ QList<CPhotoProperties> );   //Gnration de toutes les photos termine
    void forceStoppedFinishedSignal( PtrMessageList );        //L'arrt forc des thread est termin. Passage des messages d'erreur.
    void abordGenerationSignal( );
    void startGenerationSignal( );
    void jobDone( );

public slots:
    //La génération des photos se droule dans des tches spares
    void onPhotoProcessDone( CGeneratedPhotoSetParameters photoGeneratedSizes ); //Un process photo termin
    void onAbordGeneration( );
    //Les "tches"
    void initGeneration( );
    bool generateFileStructure( );          //retour: true si tout c'est bien pass
    int  generatePhotos( );                  //retour: Nombre photos dont le traitement est plannifi
    bool generateJsFiles( );
    bool optionnalBehaviors();
    bool skinning( );


    ///// Opérations /////

public:
    CGalleryGenerator( void );
    ~CGalleryGenerator( void );
    //Interface avec UI
    bool generateGallery( const CGalleryGeneratorFeeder & );
    bool isGenerationInProgress( void );
    void abordGeneration( void );

protected:
    void run( );

private:
    //-- interfaçage UI
    void debugDisplay( QString );		//Affichage d'un message de debug
    void displayProgressBar( int completion, QString color, const PtrMessage &message ); //Affiche un % d'avancement sur la progressBar

    //-- tools
    std::unique_ptr<QTextStream> readHtml(QFile& input) const; ///< Reads an html file. **Throws** an error if not possible.

    ///// Attributs //////
    //Machine Etat
    QStateMachine m_stateMachine;
    std::unique_ptr<QState> m_p_waitingForOrders;
    std::unique_ptr<QState> m_p_galleryGeneration;
    std::unique_ptr<QState> m_p_init;
    std::unique_ptr<QState> m_p_generatingFileStructure;
    std::unique_ptr<QState> m_p_generatingPhotos;
    std::unique_ptr<QState> m_p_generatingJSFiles;
    std::unique_ptr<QState> m_p_optionalBehaviors;
    std::unique_ptr<QState> m_p_skinning;
    std::unique_ptr<QState> m_p_abording;
    //Paramètres de la galerie
    CGalleryGeneratorFeeder m_feeder;
    QStringList m_captionsList;
    JSON::Root m_jsonRoot;
    CSkinParameters m_skinParameters;
    CToolbarBehavior::t_Buttons m_toolbarButtons;
    //Generation photos
    QThreadPool* m_p_photoProcessorPool;    //Pool des threads effectuant les traitements. Un thread par photo
    PtrMessageList m_msgErrorList;
    QMap<int, QMap<QString,QSize> > m_photoSizes;      //Contient les files des tailles des photos gnres <id,file>
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
