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

#include "IUserInterface.h"
#include "mainwin.h"
#include "CTerminalUi.h"
#include "CPlatform.h"
#include "CGalleryGenerator.h"

using namespace Magick;



int main(int argc, char *argv[])
{
    int exitValue;

    //Init Magick++
    InitializeMagick(*argv);

    //Instanciation appli
    QApplication appli(argc, argv); //A INSTANCIER LE PLUS TOT POSSIBLE !
    QCoreApplication::setOrganizationName("EZWebGallery"); //Utile pour le stockage des QSettings
    QCoreApplication::setOrganizationDomain("ezwebgallery.org");
    QCoreApplication::setApplicationName("Gallery Designer");

    //Instanciation du gnrateur de galerie
    CGalleryGenerator* galleryGenerator = new CGalleryGenerator( );

    //Deux possibilits : ouverture de la fentre ou xection  partir d'un terminal
    // ----------------- MODE FENETRE
    if( argc == 1 )
    {
        //Instanciation fentre
        MainWin* appWindow = new MainWin( );

        //Connections UI<->Gnrateur
        QObject::connect( galleryGenerator, SIGNAL( debugSignal(QString)), appWindow, SLOT(onLogMsg(QString)) );
        QObject::connect( galleryGenerator, SIGNAL( progressBarSignal( int, QString, QString ) ), appWindow, SLOT( onProgressBar( int, QString, QString ) ) );
        QObject::connect( galleryGenerator, SIGNAL( generationFinishedSignal(QList<CPhotoProperties> ) ), appWindow, SLOT( onGalleryGenerationFinished( QList<CPhotoProperties> ) ) );
        QObject::connect( galleryGenerator, SIGNAL( forceStoppedFinishedSignal( QStringList ) ), appWindow, SLOT( onForceStoppedFinished( QStringList ) ) );

        appWindow->setGenerator( galleryGenerator );
 //     galleryGenerator->connectToUi( appWindow );

        //Affichage fentre et xcution
        appWindow->show( );
        exitValue = appli.exec();

        //Fin
        delete appWindow;
    }
    // ----------------- EXECUTION DANS UN TERMINAL
    else
    {
        //Instanciation UI
        CTerminalUi* appCLI = new CTerminalUi( appli.arguments().at(1) );

        //Connections UI<->Gnrateur
        QObject::connect( galleryGenerator, SIGNAL(debugSignal(QString)), appCLI, SLOT(onLogMsg(QString)) );
        QObject::connect( galleryGenerator, SIGNAL( progressBarSignal( int, QString, QString ) ), appCLI, SLOT( onProgressBar( int, QString, QString ) ) );
        QObject::connect( galleryGenerator, SIGNAL( generationFinishedSignal(QList<CPhotoProperties> ) ), appCLI, SLOT( onGalleryGenerationFinished( QList<CPhotoProperties> ) ) );
        QObject::connect( galleryGenerator, SIGNAL( forceStoppedFinishedSignal( QStringList ) ), appCLI, SLOT( onForceStoppedFinished( QStringList ) ) );

        appCLI->setGenerator( galleryGenerator );
 //       galleryGenerator->connectToUi( appCLI );

        //Excution
        QObject::connect( appCLI, SIGNAL(done()), &appli, SLOT(quit()));
        QTimer::singleShot( 1000, appCLI, SLOT(run()) );
        exitValue = appli.exec(); //La boucle d'xcution fait partir le timer qui lance terminalDisplay->run()

    }

    galleryGenerator->quit();
    galleryGenerator->deleteLater();
    return exitValue;
}


