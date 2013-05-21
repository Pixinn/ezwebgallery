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



#include <QObject>
#include <QCoreApplication>
#include <QTimer>
#include <QtGui>
#include <QtPlugin>
#include <QString>
#include <QTranslator>
#include <QList>
#include <QFont>

#include <Magick++.h>
#include <cstdio>
#include <iostream>

#include "CLogger.h"
#include "CPhotoDatabase.h"
#include "CPhotoFeederDirectory.h"
#include "IUserInterface.h"
#include "mainwin.h"
#include "CTerminalUi.h"
#include "CPlatform.h"
#include "CGalleryGenerator.h"
#include "CProjectParameters.h"
#include "CMessage.h"

using namespace Magick;



int main(int argc, char *argv[])
{
    int exitValue;

    //Init Magick++
    InitializeMagick(*argv);
    QApplication appli(argc, argv); //A INSTANCIER LE PLUS TOT POSSIBLE !
    QCoreApplication::setOrganizationName("EZWebGallery"); //Utile pour le stockage des QSettings
    QCoreApplication::setOrganizationDomain("ezwebgallery.org");
    QCoreApplication::setApplicationName("EZWebGallery");

    //Instanciations
    CPhotoDatabase& photoDatabase = CPhotoDatabase::getInstance();
    photoDatabase.init();
    CProjectParameters& projectParameters = *new CProjectParameters( );
    CGalleryGenerator& galleryGenerator = *new CGalleryGenerator();

    //Qt
    qRegisterMetaType<PtrMessage>("PtrMessage");
    
    //Deux possibilités : ouverture de la fenêtre ou éxecution à partir d'un terminal
    // ----------------- MODE FENETRE
    if( argc == 1 )
    {
        //Instanciation fenetre
        MainWin* appWindow = new MainWin( galleryGenerator, projectParameters  );

        //Connections UI<->Générateur
        QObject::connect( &galleryGenerator, SIGNAL( progressBarSignal( int, QString, PtrMessage ) ), appWindow, SLOT( onProgressBar( int, QString, PtrMessage ) ) );
        QObject::connect( &galleryGenerator, SIGNAL( generationFinishedSignal(QList<CPhotoProperties> ) ), appWindow, SLOT( onGalleryGenerationFinished( QList<CPhotoProperties> ) ) );
        QObject::connect( &galleryGenerator, SIGNAL( forceStoppedFinishedSignal( PtrMessageList ) ), appWindow, SLOT( onForceStoppedFinished( PtrMessageList ) ) );
        // DB -> UI
        QObject::connect( &photoDatabase, SIGNAL( error( PtrMessage ) ), appWindow, SLOT( error( PtrMessage ) ) );
        QObject::connect( &photoDatabase, SIGNAL( warning( PtrMessage ) ), appWindow, SLOT( warning( PtrMessage ) ) );
        QObject::connect( &photoDatabase, SIGNAL( message( PtrMessage ) ), appWindow, SLOT( information( PtrMessage ) ) );
        // Logger
        QObject::connect( &(CLogger::getInstance()), SIGNAL( displayMessage(PtrMessage) ), appWindow, SLOT( onLogMsg(PtrMessage) ), Qt::QueuedConnection );

        //Affichage fenêtre et éxécution
        appWindow->init();
        appWindow->show( );
        exitValue = appli.exec();

        //Fin
        delete appWindow;
    }
    // ----------------- EXECUTION DANS UN TERMINAL
    else
    {  
        //Instanciation UI
        CTerminalUi* appCLI = new CTerminalUi( galleryGenerator, appli.arguments().at(1) );

        bool fConnected = true;
        //Connections UI<->Générateur
        fConnected &= QObject::connect( &galleryGenerator, SIGNAL( progressBarSignal( int, QString, PtrMessage ) ), appCLI, SLOT( onProgressBar( int, QString, PtrMessage ) ) );
        fConnected &= QObject::connect( &galleryGenerator, SIGNAL( generationFinishedSignal(QList<CPhotoProperties> ) ), appCLI, SLOT( onGalleryGenerationFinished( QList<CPhotoProperties> ) ) );
        fConnected &= QObject::connect( &galleryGenerator, SIGNAL( forceStoppedFinishedSignal( PtrMessageList ) ), appCLI, SLOT( onForceStoppedFinished( PtrMessageList ) ) );
        // DB -> UI
        fConnected &= QObject::connect( &photoDatabase, SIGNAL( error( PtrMessage ) ), appCLI, SLOT( error( PtrMessage ) ) );
        fConnected &= QObject::connect( &photoDatabase, SIGNAL( warning( PtrMessage ) ), appCLI, SLOT( warning( PtrMessage ) ) );
        fConnected &= QObject::connect( &photoDatabase, SIGNAL( message( PtrMessage ) ), appCLI, SLOT( information( PtrMessage ) ) );
        // Logger
        fConnected &= QObject::connect( &(CLogger::getInstance()), SIGNAL( displayMessage(PtrMessage) ), appCLI, SLOT( onLogMsg(PtrMessage) ) );
        //Excution
        fConnected &= QObject::connect( appCLI, SIGNAL(done()), &appli, SLOT(quit()), Qt::QueuedConnection);
        QTimer::singleShot( 1000, appCLI, SLOT(run()) );
        exitValue = appli.exec(); //La boucle d'excution fait partir le timer qui lance terminalDisplay->run()

        delete appCLI;
    }

    galleryGenerator.quit();
    galleryGenerator.deleteLater(); //Stopping thread can take some time
    delete &projectParameters;
    
    return exitValue;
}


