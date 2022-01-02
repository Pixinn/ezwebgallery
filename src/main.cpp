/* 
 *  EZWebGallery:
 *  Copyright (C) 2011 Christophe Meneboeuf <christophe@xtof.info>
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
#include <memory>

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
#include "CPlatform.h"

using namespace Magick;



int main(int argc, char *argv[])
{
    int exitValue;
    bool f_gui = ( argc == 1 ); //no args -> launch gui

    //Init Magick++
    CPlatform::limitOpenMPThreads();
    InitializeMagick(*argv);

    //Instantiate QApplication
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling); //Before instanciating the QApplication!
    QCoreApplication::setOrganizationName("EZWebGallery"); //Utile pour le stockage des QSettings
    QCoreApplication::setOrganizationDomain("ezwebgallery.org");
    QCoreApplication::setApplicationName("EZWebGallery");
    auto appli = std::make_unique<QApplication>(argc, argv); //A INSTANCIER LE PLUS TOT POSSIBLE !

    //Instanciations
    CPhotoDatabase& photoDatabase = CPhotoDatabase::getInstance();
    photoDatabase.init();
    CProjectParameters& projectParameters = *new CProjectParameters( );
    CGalleryGenerator& galleryGenerator = *new CGalleryGenerator();

    //Qt
    qRegisterMetaType<PtrMessage>("PtrMessage");
    
    //Deux possibilités : ouverture de la fenêtre ou éxecution à partir d'un terminal
    // ----------------- MODE FENETRE
    if( f_gui )
    {
        //Instanciation fenetre
        auto appWindow = std::make_unique<MainWin>( galleryGenerator, projectParameters  );

        //Connections UI<->Générateur
        QObject::connect( &galleryGenerator, SIGNAL( progressBarSignal( int, QString, PtrMessage ) ), appWindow.get(), SLOT( onProgressBar( int, QString, PtrMessage ) ) );
        QObject::connect( &galleryGenerator, SIGNAL( generationFinishedSignal(QList<CPhotoProperties> ) ), appWindow.get(), SLOT( onGalleryGenerationFinished( QList<CPhotoProperties> ) ) );
        QObject::connect( &galleryGenerator, SIGNAL( forceStoppedFinishedSignal( PtrMessageList ) ), appWindow.get(), SLOT( onForceStoppedFinished( PtrMessageList ) ) );
        // DB -> UI
        QObject::connect( &photoDatabase, SIGNAL( error( PtrMessage ) ), appWindow.get(), SLOT( error( PtrMessage ) ) );
        QObject::connect( &photoDatabase, SIGNAL( warning( PtrMessage ) ), appWindow.get(), SLOT( warning( PtrMessage ) ) );
        QObject::connect( &photoDatabase, SIGNAL( message( PtrMessage ) ), appWindow.get(), SLOT( information( PtrMessage ) ) );
        // Logger
        QObject::connect( &(CLogger::getInstance()), SIGNAL( displayMessage(PtrMessage) ), appWindow.get(), SLOT( onLogMsg(PtrMessage) ), Qt::QueuedConnection );

        //Affichage fenêtre et éxécution
        appWindow->init();
        appWindow->show( );
        exitValue = appli->exec();
    }
    // ----------------- EXECUTION DANS UN TERMINAL
    else
    {  
        //Instanciation UI
        auto appCLI = std::make_unique<CTerminalUi>( galleryGenerator, appli->arguments().at(1) );

        //Connections UI<->Générateur
        QObject::connect( &galleryGenerator, SIGNAL( progressBarSignal( int, QString, PtrMessage ) ), appCLI.get(), SLOT( onProgressBar( int, QString, PtrMessage ) ) );
        QObject::connect( &galleryGenerator, SIGNAL( generationFinishedSignal(QList<CPhotoProperties> ) ), appCLI.get(), SLOT( onGalleryGenerationFinished( QList<CPhotoProperties> ) ) );
        QObject::connect( &galleryGenerator, SIGNAL( forceStoppedFinishedSignal( PtrMessageList ) ), appCLI.get(), SLOT( onForceStoppedFinished( PtrMessageList ) ) );
        // DB -> UI
        QObject::connect( &photoDatabase, SIGNAL( error( PtrMessage ) ), appCLI.get(), SLOT( error( PtrMessage ) ) );
        QObject::connect( &photoDatabase, SIGNAL( warning( PtrMessage ) ), appCLI.get(), SLOT( warning( PtrMessage ) ) );
        QObject::connect( &photoDatabase, SIGNAL( message( PtrMessage ) ), appCLI.get(), SLOT( information( PtrMessage ) ) );
        // Logger
        QObject::connect( &(CLogger::getInstance()), SIGNAL( displayMessage(PtrMessage) ), appCLI.get(), SLOT( onLogMsg(PtrMessage) ) );
        //Excution
        QObject::connect( appCLI.get(), SIGNAL(done()), appli.get(), SLOT(quit()), Qt::QueuedConnection);
        QTimer::singleShot( 1000, appCLI.get(), SLOT(run()) );
        exitValue = appli->exec(); //La boucle d'excution fait partir le timer qui lance terminalDisplay->run()
    }

    galleryGenerator.quit();
    galleryGenerator.deleteLater(); //Stopping thread can take some time
    delete &projectParameters;
    
    return exitValue;
}


