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
}

CTerminalUi::~CTerminalUi( )
{ }

////////// SLOTS //////////

void CTerminalUi::onLogMsg( QString msg ){
    msg = ""; //Pour virer un warning
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

void CTerminalUi::onProgressBar( int completion, QString color, QString msg, int timeout )
{
    cout << msg << endl;
}

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

    //Launching generation
    m_galleryGenerator.generateGallery( m_projectParameters, m_skinParameters, propertiesList );

    /* 
            A REVOIR !!!!!

   //Reconstruction de la liste des photos rellement prsentes dans le rpertoire d'entre
	//REM: copier/coller adapt de mainwin::buildPhotoList() -> voir les commentaires de cette fonction pour explication
	QString photoName;
	QDir inputDir( m_projectParameters.m_galleryConfig.inputDir );
	QStringList photoFileTypes (QStringList() << "*.jpeg" << "*.jpg" << "*.tiff" << "*.tif"); //Formats d'image supports en entre
    QStringList photoList = CPlatform::getImagesInDir( inputDir, photoFileTypes ); //Rcupration de la liste des photos du rpertoire d'entre
    QStringListIterator* p_photoListIterator = new QStringListIterator( photoList );
    while( p_photoListIterator->hasNext() ) //On vrifie que les paramtres des photos sont toujours  jour
    {    
        photoName = p_photoListIterator->next();
        QFileInfo* p_photoFileInfo = new QFileInfo( inputDir.absoluteFilePath( photoName ) );
        //Si les paramtres de la galerie ne comportaient le fichier, on met  jour et on demande la regnration
        if( !m_projectParameters.m_photoPropertiesMap.contains( photoName ) )
        {
            CPhotoPropertiesExtended newProperties;
            newProperties.setLastModificationTime( p_photoFileInfo->lastModified() );
            m_projectParameters.m_photoPropertiesMap.insert( photoName, newProperties );
            m_projectParameters.m_photosConfig.f_regeneration = true;
            m_projectParameters.m_thumbsConfig.f_regeneration = true;
        }
        //Si les infos de date du fichier sont diffrentes => on update et on demande la regration galement
        else{
            CPhotoPropertiesExtended deprecatedProperties = m_projectParameters.m_photoPropertiesMap.value( photoName );
            if(  deprecatedProperties.lastModificationTime().toString() != p_photoFileInfo->lastModified().toString() ) { //Les QDateTime non convertis ne semblent pas bien se comparer ???
                deprecatedProperties.setLastModificationTime( p_photoFileInfo->lastModified() );
                m_projectParameters.m_photosConfig.f_regeneration = true;
                m_projectParameters.m_thumbsConfig.f_regeneration = true;
            }
        }
        delete p_photoFileInfo;
    }
	//Suppression des entres qui ne correspondent pas  une photo trouve dans le rpertoire
	foreach( photoName, m_projectParameters.m_photoPropertiesMap.keys() ){
		if( !photoList.contains( photoName ) ){
			m_projectParameters.m_photoPropertiesMap.remove( photoName );
		}
	}

    //Lancement de la gnration
    m_galleryGenerator.generateGallery(  m_projectParameters, m_skinParameters  ); A AJOUTER !!!!!!
*/
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

