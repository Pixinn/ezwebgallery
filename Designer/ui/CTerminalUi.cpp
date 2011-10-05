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
* $Id: CTerminalUi.cpp 133 2011-10-04 20:05:28Z xtof $
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


/******************************************* Variables statiques *********************/
QTextStream CTerminalUi::cout( stdout, QIODevice::WriteOnly );
QTextStream CTerminalUi::cerr( stderr, QIODevice::WriteOnly );


/******************************************* Ralisation Interface **********************************************/
//IUserInterface::IUserInterface( ){}


CTerminalUi::CTerminalUi( const QString &projectFile )/* :  IUserInterface( parent )*/
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
    msg = ""; //Pour viter un warning
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

void CTerminalUi::onGalleryGenerationFinished( /*bool success*/QList<CPhotoProperties> propertiesList )
{
    bool success = true;
    foreach( CPhotoProperties photoProperties, propertiesList ){
        if( !photoProperties.processed() ){
            success = false;
        }
    }
    if( success ){
        cout << tr("Generation successfully completed.") << endl;
    }
    emit done();
}

void CTerminalUi::onProgressBar( int completion, QString color, QString msg, int timeout )
{
    completion = 0;
    color = "";
    timeout = 0; //Pour viter des warnings
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
       //  buildPhotoLists( );
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
 /*   //Mise  jour de la liste des photos : le contenu du rep a peut-tre chang depuis la sauvegarde
    QDir inputDir( m_projectParameters.m_galleryConfig.inputDir );
	QStringList photoFileTypes = (QStringList() << "*.jpeg" << "*.jpg" << "*.tiff" << "*.tif"); //Formats d'image supports en entre
    QStringList photoList = CPlatform::getImagesInDir( inputDir, photoFileTypes );
    QStringListIterator* p_photoListIterator = new QStringListIterator( photoList );
    m_projectParameters.m_photosList.clear();
    while( p_photoListIterator->hasNext() ){    //Construction du QMap<photoName,lastModified>
        QString photoName = p_photoListIterator->next();
        QFileInfo* p_photoFileInfo = new QFileInfo( inputDir.absoluteFilePath(photoName) );
        m_projectParameters.m_photosList.insert( photoName, p_photoFileInfo->lastModified() );
        onLogMsg( photoName + QString(": ") + p_photoFileInfo->lastModified().toString() );
        delete p_photoFileInfo;
    }
    delete p_photoListIterator;
    //Cration du QStringList de lgendes
    QMap<QString,CCaption> projectCaptionMap = m_captionManager.captionMap(); //List de lgende telle que lue  partir du fichier projet
    QList<CCaption> captionList;
    CCaption emptyCaption;
  //La liste des lgendes sera dans le mme ordre que la liste de photos : alphabtique
    foreach( QString photo, photoList ){
        if( projectCaptionList.contains( photo ) ){
            captionList << projectCaptionList.value( photo ) ;
        }
        //La photo n'tait pas auparavant dans le rpertoire. On met une lgende vide
        else{
            captionList << emptyCaption;
        }
    }
*/

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
            CPhotoProperties newProperties;
//          newProperties.setFileName( photoName );
            newProperties.setLastModificationTime( p_photoFileInfo->lastModified() );
            m_projectParameters.m_photoPropertiesMap.insert( photoName, newProperties );
            m_projectParameters.m_photosConfig.f_regeneration = true;
            m_projectParameters.m_thumbsConfig.f_regeneration = true;
        }
        //Si les infos de date du fichier sont diffrentes => on update et on demande la regration galement
        else{
            CPhotoProperties deprecatedProperties = m_projectParameters.m_photoPropertiesMap.value( photoName );
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
    m_p_galleryGenerator->generateGallery(  m_projectParameters,
                                            m_skinParameters  );

}

/****************************************** Fonctions protges **************************************/
//Ncessaire pour pouvoir annuler la gnration
//Malheureuseument sous Qt4, les applications non-graphiques ne peuvent recevoir d'evt clavier...
void CTerminalUi::keyPressEvent ( QKeyEvent * event )
{
    if( event->key() == Qt::Key_Escape ){
        cout << tr("Canceling...") << endl;
        m_p_galleryGenerator->abordGeneration( );
    }
}



/****************************************** Fonctions publiques *********************************/

void CTerminalUi::show(){}

void CTerminalUi::setGenerator( CGalleryGenerator *generator )
{
    m_p_galleryGenerator = generator;
}
