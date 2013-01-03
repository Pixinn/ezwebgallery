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



#include <cstdio>
#include <QDomDocument>
#include <QDir>
#include <QFile>
#include <QMap>
#include <QStringList>
#include <QStringListIterator>

#include "CTerminalUi.h"
#include "GlobalDefinitions.h"
#include "CPlatform.h"
#include "CCaption.h"
#include "CPhotoDatabase.h"
#include "CPhotoFeederDirectory.h"


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

    connect( &m_projectParameters, SIGNAL(message(CMessage)), this, SLOT(information(CMessage)));
}

CTerminalUi::~CTerminalUi( )
{ }

////////// SLOTS //////////

void CTerminalUi::onLogMsg( const IMessage &msg  ){
    msg.message();
}

void CTerminalUi::onForceStoppedFinished( QStringList listMsg )
{
    if( listMsg.size() > 0){
        QStringListIterator iterator = QStringListIterator( listMsg );
        while( iterator.hasNext() ){
            cerr << iterator.next() << endl;
        }
    }
    cout << tr("Generation cancelled.") << endl;
    emit done();
}

void CTerminalUi::onGalleryGenerationFinished( QList<CPhotoProperties> propertiesList )
{
    propertiesList.size(); //to avoid a warning
    cout << tr("Generation successfully completed.") << endl;
    emit done();
}

void CTerminalUi::onProgressBar( int, QString, QString msg, int )
{
    cout << msg << endl;
}

void CTerminalUi::information( CMessage message )
{
    QString summary = message.summary();
    QString details = message.details();
    cout << "INFO: " << message.informativeText() << endl;
    if( !summary.isEmpty() ) {
        cout << summary << endl;
    }
    if( !details.isEmpty() ) {
        cout << details << endl;
    }
}

void CTerminalUi::warning( CMessage message )
{
    QString summary = message.summary();
    QString details = message.details();
    cout << "WARNING: " << message.informativeText() << endl;
    if( !summary.isEmpty() ) {
        cout << summary << endl;
    }
    if( !details.isEmpty() ) {
        cout << details << endl;
    }
}

void CTerminalUi::error( CMessage message )
{
    QString summary = message.summary();
    QString details = message.details();
    cout << "ERROR: " << message.informativeText() << endl;
    if( !summary.isEmpty() ) {
        cout << summary << endl;
    }
    if( !details.isEmpty() ) {
        cout << details << endl;
    }
}

////////////////////////// RUN ///////////////////////
void CTerminalUi::run( )
{

    cout << tr("EZWebGallery started.") << endl;

    //Chargement de la dernire session
    QDomDocument projectDoc;
    QFile file_xmlprojectDoc( m_projectFile );
    if( projectDoc.setContent( &file_xmlprojectDoc ) ){
        m_projectParameters.fromDomDocument( projectDoc );
        cout << m_projectFile << tr(" loaded.") << endl;
    }
    else{
        cerr << tr("Cannot load the project: ") << m_projectFile << endl;
        cerr << tr("Usage: GalleryDesigner [project file to load]") << endl;
        emit done();
        return;
    }
    //Chargement de la skin
    if( !m_skinParameters.load( m_projectParameters.m_galleryConfig.skinPath ) ){
        cerr << tr("Cannot load the skin: ") << m_projectParameters.m_galleryConfig.skinPath << endl;
        emit done();
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
            cerr << tr("Folder creation impossible.") << endl;
            emit done();
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
    if( !m_galleryGenerator.generateGallery( m_projectParameters, m_skinParameters, propertiesList ) )  {
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

