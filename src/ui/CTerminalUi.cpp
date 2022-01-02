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



#include <cstdio>
#include <QDomDocument>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QStringList>
#include <QStringListIterator>

#include "CTerminalUi.h"
#include "GlobalDefinitions.h"
#include "CGalleryGeneratorFeeder.h"
#include "CPlatform.h"
#include "CCaption.h"
#include "CPhotoDatabase.h"
#include "CPhotoFeederDirectory.h"
#include "CLogger.h"
#include "CWarning.h"


/******************************************* Variables statiques *********************/
QTextStream CTerminalUi::cout( stdout, QIODevice::WriteOnly );
QTextStream CTerminalUi::cerr( stderr, QIODevice::WriteOnly );


/******************************************* Ralisation Interface **********************************************/


CTerminalUi::CTerminalUi( CGalleryGenerator & galleryGenerator, const QString &projectFile ) :
    QObject(),    
    m_galleryGenerator( galleryGenerator )
{
    m_projectFile = projectFile;

    m_projectParameters.setcaptionManager( &m_captionManager );
    m_projectParameters.setSkin( &m_skinParameters );
    m_projectParameters.m_photosConfig.f_regeneration = true;
    m_projectParameters.m_thumbsConfig.f_regeneration = true;

    m_languageManager.translate();

    connect( &m_projectParameters, SIGNAL(warning(PtrMessage)), this, SLOT(warning(PtrMessage)));
}

CTerminalUi::~CTerminalUi( )
{ }

////////// SLOTS //////////

void CTerminalUi::onLogMsg( PtrMessage msg  ){
    QString message = msg->message();
    cout << message << endl;
}

void CTerminalUi::onForceStoppedFinished( PtrMessageList listMsg )
{
    if( listMsg.size() > 0){        
        for( PtrMessageList::iterator i = listMsg.begin(); i < listMsg.end(); i++ ){
            cerr << (*i)->message() << endl;
        }
    }
    cout << tr("Generation cancelled.") << endl;
    emit done();
}

void CTerminalUi::onGalleryGenerationFinished( QList<CPhotoProperties> )
{
    cout << tr("Generation successfully completed.") << endl;
    emit done();
}

void CTerminalUi::onProgressBar( int, QString, PtrMessage msg, int )
{
    cout << msg->message() << endl;
}

void CTerminalUi::information( PtrMessage message )
{
    QString info( "\n" + tr("INFO") + "\n" );
    CLogger::getInstance().log( PtrMessage(new CMessage(info,message->message())) );
}

void CTerminalUi::warning( PtrMessage message )
{
    QString warning( "\n" + tr("WARNING") + "\n" );
    warning += message->message() + "\n";
    CLogger::getInstance().log( PtrMessage(new CWarning(warning)) );
}

void CTerminalUi::error( PtrMessage message )
{
    QString err( "\n" + tr("ERROR") + "\n" );
    CLogger::getInstance().log( PtrMessage(new CError(err,message->message())) );
}

////////////////////////// RUN ///////////////////////
void CTerminalUi::run( )
{

    cout << tr("EZWebGallery started.") << endl;

    //Chargement de la dernire session
    QDomDocument projectDoc;
    QString filePath =  QDir::current().filePath( m_projectFile ); //can manage absolute and relative paths
    QFile file_xmlprojectDoc( filePath );
    if( QFile( filePath ).exists() && projectDoc.setContent( &file_xmlprojectDoc ) ){
        m_projectParameters.fromDomDocument( projectDoc );
        cout << m_projectFile << tr(" loaded.") << endl;
    }
    else{
        cerr << tr("Cannot load the project: ") << m_projectFile << endl;
        cerr << tr("Usage: ezwebgallery [project file to load]") << endl;
        emit done();
        return;
    }
    //Chargement de la skin
    if( !m_skinParameters.load( m_projectParameters.m_galleryConfig.skinPath ) ){
        PtrMessageList errs;
        errs.append( PtrMessage(new CError(m_skinParameters.errors().last())));
        onForceStoppedFinished( errs );
        return;

    }
    else{
        cout <<  m_projectParameters.m_galleryConfig.skinPath << tr(" loaded.") << endl;
    }

    //Cration du rpertoire de sortie si il n'existe pas
    QDir outputDir( m_projectParameters.m_galleryConfig.outputDir );
    if( !outputDir.exists() ){
        cout << tr("Creating the output folder: ") << m_projectParameters.m_galleryConfig.outputDir << endl;
        if( outputDir.mkpath( m_projectParameters.m_galleryConfig.outputDir )){
            cout << tr("Output folder successfully created.") << endl;
        }else{
            PtrMessageList errs;
            errs.append( PtrMessage(new CError(CError::DirectoryCreation, m_projectParameters.m_galleryConfig.outputDir)) );
            onForceStoppedFinished( errs );
            return;
        }
    }

    //Constructing photo properties list
    CPhotoDatabase& photoDB = CPhotoDatabase::getInstance();
    CPhotoFeederDirectory& photoFeeder = CPhotoFeederDirectory::getInstance();
    QStringList photoList = photoFeeder.getPhotoList();
    photoDB.refresh( photoList );
    QList<CPhotoProperties*> propertiesList;
    for( int i = 0; i < photoDB.size(); i++ ) {
        propertiesList << photoDB.properties(i);
    }

    //Displaying the project's photo' names
    cout << endl << tr("Photos to be processed:") << endl;
    foreach( CPhotoProperties* properties, propertiesList ) {
        cout << properties->fileInfo().absoluteFilePath() << endl;
    }
    cout << endl;

    //Launching generation
    if( !m_galleryGenerator.generateGallery( CGalleryGeneratorFeeder(m_projectParameters, m_skinParameters, propertiesList) ) )  {
        cout << tr("The gallery could not be properly generated.");
        exit(0);
    }    
}

/****************************************** Fonctions protges **************************************/
//Ncessaire pour pouvoir annuler la gnration
//Malheureuseument sous Qt4, les applications non-graphiques ne peuvent recevoir d'evt clavier...
void CTerminalUi::keyPressEvent ( QKeyEvent * event )
{
    if( event->key() == Qt::Key_Escape ){
        cout << tr("Canceling...") << endl;
        m_galleryGenerator.abordGeneration( );
    }
}


/****************************************** Fonctions publiques *********************************/

void CTerminalUi::show(){}

