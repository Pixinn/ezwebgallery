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
const QRegExp CPlatform::m_forbiddenCharacters = QRegExp("[ '<>.%/\\*|\"()&:?]"); //Les caract�res interdits du filesystem
const QStringList CPlatform::m_languageList = QStringList() << QObject::tr("English")   // !! Entrer ici toutes les langues suport�es !!! //
                                                            << QObject::tr("French");   // !! L'ordre ne pas changer d'une version � une autre !! //
QString CPlatform::m_language = QString();

/*************************** METHODES *********************/
CPlatform::CPlatform()
{

}

/**************************
* revision()
* ----------
* Retourne le num�ro de r�vision.
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
* Retourne le num�ro de r�vision.
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
* Retourne la langue de l'ui si elle a �t� configur�e
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
* Indique la langue utilis�e
***************************/
void CPlatform::setLanguage( const QString &language )
{
    m_language = language;
}


/**************************
* languageList()
* ----------
* Retourn la liste des langues support�es
***************************/
QStringList CPlatform::languageList( )
{
    return m_languageList;
}


/**************************
* defaultFont()
* ----------
* Retourne la police par d�faut du system
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
    //Windows (par d�faut)
    return QFont("MS Shell Dlg 2",8);
}

/**************************
* applicationDirPath()
* ----------
* Retourne le r�pertoire o� se trouve le programme
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
* Retourne le r�pertoire o� se trouvent les skins fournies
***************************/
QString CPlatform::skinDirPath()
{
    QString skinDir = applicationDirPath() + QString("/skins");
    return skinDir;
}


/**************************
* appDataDir()
* ----------
* Retourne le r�pertore o� sont sauv�es les donn�es persistantes du programme
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
* Retourne les caract�res interdits.
* ex: pour nommer un nom de fichier
***************************/
QRegExp CPlatform::forbiddenCharacters()
{
    return m_forbiddenCharacters;
}


/***************************** OUTILS *******************************/

#include "CErrorMessages.h"

//Outils suceptibles d'�tre utilis�s par plusieurs classes

/*************************
* getImagesInDir( const QDir &dir, QString filter = "All" )
*----------------------
* Retourne la liste des images du r�pertoire "dir"
* Si filter = "All", tous les formats d'images support�s seront pris en compte
**************************/
QStringList CPlatform::getImagesInDir( const QDir &dir, QStringList filter )
{
    QStringList imagesFilter;
    QStringList photoList;

	//Construction de la liste de tous les formats support�s si aucun filtre n'a �t� sp�cifi�
	if( filter.isEmpty() )
    {
            QByteArray supportedImageFormat;
            QList<QByteArray> supportedImageFormats = QImageReader::supportedImageFormats( );
            foreach( supportedImageFormat, supportedImageFormats){
                    imagesFilter << QString("*.") + supportedImageFormat;
            }
    }
    //Utilisation du filtre fourni en entr�e
    else{
            imagesFilter = filter;
    }
            photoList = dir.entryList( imagesFilter, QDir::Readable | QDir::Files, QDir::Name ); //R�cup�ration de la liste

	return photoList;
}



/*************************
* copyDirectory( QDir source, QDir destination, QString &errorMsg )
*----------------------
* Copie r�cursive du contenu du r�pertoire source dans le r�pertoire destination
* Renvoie true si succ�s ou false sinon
* In: QDir source - r�pertoire source
* In: QDir destination - r�pertoire destination
* Out: QString &errorMsg - message d'erreur si pb
* Return: true si succ�s, false si �chec
**************************/
//Inspir� par niak74 : http://www.siteduzero.com/forum-83-434816-p1-qt---supprimer-un-dossier.html
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
            if( !destDirInfo.exists() ){ //Si le r�pertoire n'existe pas d�j�, on le cr��
                if ( !destination.mkdir( fileInfo.fileName() ) ){ //Erreur lors de la cr�ation du rep
                    errorMsg = CErrorMessages::error(DirectoryCreation) + destination.absolutePath() + "/" + fileInfo.fileName();
                    return false;
                }
            }
            currentDestPath.cd( fileInfo.fileName() ); //On entre dans le r�pertoire
            if( !copyDirectory(fileInfo.filePath(), currentDestPath, errorMsg) ){ //On le clone => r�cursivit� !
                return false;
            }
        }
        else{
            // Si le fichier existe d�j� : le supprimer
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
* Return: (QString) Texte charg�
**************************/
QString CPlatform::readTranslatedTextFile( const QString &baseName )
{
    QString langSuffix;
    QString trFileName;
    QString enFileName; //on calcule le nom du fichier anglais pour se rabattre dessus par d�faut
    QString enSuffix( "_en" );
    int indexOf;
    
    //D�termination du suffixe en fonction de la langue
    if( m_language == "fr" ){
        langSuffix = QString("_fr" );
    }
    else{ //Par d�faut : anglais
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
    if( !fileToRead->exists() ){ //Si le fichier traduit n'�xiste pas on se rabat sur l'Anglais
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
