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


#include <QtGlobal>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QLocale>
#include <QDir>
#include <QImageReader>
#include <QTextStream>
#include <QString>

#include "CPlatform.h"
#include "builddate.h"

/*************************** VARIABLES STATIQUES ****************/
const QRegExp CPlatform::m_forbiddenCharacters = QRegExp("[ '<>.%/\\*|\"()&:?]"); //Les caractères interdits du filesystem
const QStringList CPlatform::m_languageList = QStringList() << QObject::tr("English")   // !! Entrer ici toutes les langues suportées !!! //
                                                            << QObject::tr("French");   // !! L'ordre ne pas changer d'une version à une autre !! //
QString CPlatform::m_language = QString();

/*************************** METHODES *********************/
CPlatform::CPlatform()
{

}

/**************************
* revision()
* ----------
* Retourne le numéro de révision.
***************************/
QString CPlatform::revision()
{
    QString buildDate( BUILD_DATE );
    buildDate.replace('-','/');
    buildDate = buildDate.left( buildDate.indexOf(" ") );

    return buildDate;
}



/**************************
* revisionInt()
* ----------
* Retourne le numéro de révision.
***************************/
int CPlatform::revisionInt()
{
    QString buildDate( BUILD_DATE );
    buildDate.remove('-');
    buildDate = buildDate.left( buildDate.indexOf(" ") );

    return buildDate.toInt();
}


/**************************
* language()
* ----------
* Retourne la langue de l'ui si elle a été configurée
* sinon retourne la langue de l'os
***************************/
QString CPlatform::language( )
{
    if( m_language.isEmpty( ) ){
	    return QLocale::system().name().section('_', 0, 0);
    }
    else{
        return m_language;
    }
}



/**************************
* setLanguage( const QString &language  )
* ----------
* Indique la langue utilisée
***************************/
void CPlatform::setLanguage( const QString &language )
{
    m_language = language;
}


/**************************
* languageList()
* ----------
* Retourn la liste des langues supportées
***************************/
QStringList CPlatform::languageList( )
{
    return m_languageList;
}


/**************************
* defaultFont()
* ----------
* Retourne la police par défaut du system
***************************/
QFont CPlatform::defaultFont()
{
    // MacOS
#ifdef Q_WS_MAC
    return QFont("Tahoma",9); //Requires 10.5 Leopard
#endif
    //Linux (X11)
#ifdef Q_WS_X11
    return QFont("Nimbus Sans L",8);
#endif
    //Windows (par défaut)
    return QFont("MS Shell Dlg 2",8);
}

/**************************
* applicationDirPath()
* ----------
* Retourne le répertoire où se trouve le programme
***************************/
QString CPlatform::applicationDirPath()
{
    //Cas particulier : macOS
#ifdef Q_WS_MAC
    return QDir::currentPath();
#else
    return QCoreApplication::applicationDirPath();
#endif
}


/**************************
* applicationDirPath()
* ----------
* Retourne le répertoire où se trouvent les skins fournies
***************************/
QString CPlatform::skinDirPath()
{
    QString skinDir = applicationDirPath() + QString("/skins");
    return skinDir;
}


/**************************
* appDataDir()
* ----------
* Retourne le répertore où sont sauvées les données persistantes du programme
***************************/
QString CPlatform::appDataDir()
{
    QString directory = QDesktopServices::storageLocation( QDesktopServices::DataLocation );
    if( directory.isEmpty() ){ // embedded platforms currently don't define this location and you don't want to write to /foo
        directory = QDir::homePath();
    }
#ifdef Q_WS_WIN    
	return directory + QString( "/") + QCoreApplication::applicationName();
#else
    return directory + QString( "/." ) + QCoreApplication::applicationName();
#endif    
}


/**************************
* appDataDir()
* ----------
* Retourne les caractères interdits.
* ex: pour nommer un nom de fichier
***************************/
QRegExp CPlatform::forbiddenCharacters()
{
    return m_forbiddenCharacters;
}


/***************************** OUTILS *******************************/

#include "CErrorMessages.h"

//Outils suceptibles d'être utilisés par plusieurs classes

/*************************
* getImagesInDir( const QDir &dir, QString filter = "All" )
*----------------------
* Retourne la liste des images du répertoire "dir"
* Si filter = "All", tous les formats d'images supportés seront pris en compte
**************************/
QStringList CPlatform::getImagesInDir( const QDir &dir, QStringList filter )
{
    QStringList imagesFilter;
    QStringList photoList;

	//Construction de la liste de tous les formats supportés si aucun filtre n'a été spécifié
	if( filter.isEmpty() )
    {
            QByteArray supportedImageFormat;
            QList<QByteArray> supportedImageFormats = QImageReader::supportedImageFormats( );
            foreach( supportedImageFormat, supportedImageFormats){
                    imagesFilter << QString("*.") + supportedImageFormat;
            }
    }
    //Utilisation du filtre fourni en entrée
    else{
            imagesFilter = filter;
    }
            photoList = dir.entryList( imagesFilter, QDir::Readable | QDir::Files, QDir::Name ); //Récupération de la liste

	return photoList;
}



/*************************
* copyDirectory( QDir source, QDir destination, QString &errorMsg )
*----------------------
* Copie récursive du contenu du répertoire source dans le répertoire destination
* Renvoie true si succès ou false sinon
* In: QDir source - répertoire source
* In: QDir destination - répertoire destination
* Out: QString &errorMsg - message d'erreur si pb
* Return: true si succès, false si échec
**************************/
//Inspiré par niak74 : http://www.siteduzero.com/forum-83-434816-p1-qt---supprimer-un-dossier.html
bool CPlatform::copyDirectory( QDir source, QDir destination, QString &errorMsg )
{
    //QDir currentDestPath = destination;
    source.setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);

    foreach(QFileInfo fileInfo, source.entryInfoList())
    {
        if(fileInfo.isDir())
        {
            QDir currentDestPath = destination;
            QFileInfo destDirInfo( destination.absoluteFilePath(fileInfo.fileName()) );
            if( !destDirInfo.exists() ){ //Si le répertoire n'existe pas déjà, on le créé
                if ( !destination.mkdir( fileInfo.fileName() ) ){ //Erreur lors de la création du rep
                    errorMsg = CErrorMessages::error(DirectoryCreation) + destination.absolutePath() + "/" + fileInfo.fileName();
                    return false;
                }
            }
            currentDestPath.cd( fileInfo.fileName() ); //On entre dans le répertoire
            if( !copyDirectory(fileInfo.filePath(), currentDestPath, errorMsg) ){ //On le clone => récursivité !
                return false;
            }
        }
        else{
            // Si le fichier existe déjà : le supprimer
            QFileInfo destFileInfo( destination.absoluteFilePath(fileInfo.fileName()) );
            if( destFileInfo.exists() ){
                if( !destination.remove( destFileInfo.fileName() ) ){//Impossible de supprimer le fichier
                    errorMsg = CErrorMessages::error(FileCreation) + source.absoluteFilePath(fileInfo.fileName());
                    return false;
                }
            }
            if(!QFile::copy( fileInfo.absoluteFilePath(), destination.absoluteFilePath(fileInfo.fileName()) ) ){
                errorMsg = CErrorMessages::error(FileCreation) + source.absoluteFilePath(fileInfo.fileName());
                return false;
            }

        }

    }

    return true;
}

/*************************
* readTranslatedTextFile( QString baseName )
*----------------------
* Charge la bonne version (bonne langue) d'un fichier texte des ressources
* en fonction de la langue actuelle de l'ui.
* In: (QString) nom de base du fichier AVEC EXTENSION, sans le suffixe de langue
* Return: (QString) Texte chargé
**************************/
QString CPlatform::readTranslatedTextFile( const QString &baseName )
{
    QString langSuffix;
    QString trFileName;
    QString enFileName; //on calcule le nom du fichier anglais pour se rabattre dessus par défaut
    QString enSuffix( "_en" );
    int indexOf;
    
    //Détermination du suffixe en fonction de la langue
    if( m_language == "fr" ){
        langSuffix = QString("_fr" );
    }
    else{ //Par défaut : anglais
        langSuffix = enSuffix;
    }
    //Composition du nom
    trFileName = QString( ":/text/" ) + baseName;
    enFileName = trFileName;
    indexOf = trFileName.lastIndexOf( QChar('.') );
    if( indexOf != -1 ){
        enFileName = enFileName.insert( indexOf, enSuffix );
        trFileName = trFileName.insert( indexOf, langSuffix );
    }
    else{
        enFileName = enFileName.append( enSuffix );
        trFileName = trFileName.append( langSuffix );
    }

    QFile* fileToRead = new QFile( trFileName );
    if( !fileToRead->exists() ){ //Si le fichier traduit n'éxiste pas on se rabat sur l'Anglais
        delete fileToRead;
        fileToRead = new QFile( enFileName );
    }
    fileToRead->open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream textStream( fileToRead );
    textStream.setCodec("UTF-8");
    QString returnedValue = textStream.readAll();
    delete fileToRead;
    return returnedValue;
}
