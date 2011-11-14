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

#include <QApplication>
#include <QCoreApplication>
#include <QLocale>
#include <QLibraryInfo>
#include <QSettings>

#include "CLanguageManager.h"
#include "GlobalDefinitions.h"
#include "CPlatform.h"

//DEFINITIONS
#define LANGUAGE_PREFIX_APPLI   ":/languages/GalleryDesigner_"
#define LANGUAGE_PREFIX_QT      "qt_"

//INIT DES MEMBRES STATIQUES
QMap<QString,QString> CLanguageManager::m_supportedLanguages = QMap<QString,QString>();
QTranslator* CLanguageManager::m_p_trAppli = new QTranslator();
QTranslator* CLanguageManager::m_p_trQtLibs = new QTranslator();

// !! IL FAUT AU MOINS UNE INSTANCE QUI PUISSE RECEVOIR LE SIGNAL TRANSLATE !! //
CLanguageManager::CLanguageManager(QObject *parent) :
    QObject(parent)
{
    //translate();
}


/*************************
* initSupportedLanguages( )
* ---------------
* !!! INSERER ICI TOUTES LES LANGUES SUPPORTEES !!!
* Initialise la liste de toutes les langues supportées.
* Fonction appelée à chaque retraduction, car les langues apparaissent
* dans l'ui et doivent elles-même être traduites...
*************************/
void CLanguageManager::initSupportedLanguages()
{
    m_supportedLanguages.clear();
    //INSERER ICI TOUTES LES LANGUES SUPPORTEES !!!
    m_supportedLanguages.insert( "en", tr("English") );
    m_supportedLanguages.insert( "fr", tr("French") );
}

/*************************
* supportedLanguages( )
* ---------------
* Retourne la liste des langues supportées par l'ui
* traduites dans la langue actuelle.
*************************/
QStringList CLanguageManager::supportedLanguages( )
{
    return m_supportedLanguages.values();
}

/*************************
* languageCode( const QString &language )
* ---------------
* Retourne le code de la langue fournie en entrée
* Le code est vide si la langue n'est pas supportée
* In : (QString) language
* Return : (QString) code de la langue
*************************/
QString CLanguageManager::languageCode( const QString &language )
{
    return m_supportedLanguages.key( language );
}


/*************************
* currentLanguage( )
* Retourne la langue courante
*************************/
QString CLanguageManager::currentLanguage()
{
    QSettings settings;
    QString langCode = settings.value( SETTINGS_LANGUAGE ).toString();
    return m_supportedLanguages.value( langCode );
}


/*************************
* translate( QString )
* ---------------
* Traduit l'appli en fonction de QSettings si possible,
* sinon en fonction de la langue locale ou à défaut en anglais
*************************/
void CLanguageManager::translate( )
{
    QSettings settings;
    QString langCode;

    //On commence par retirer tout traducteur déjà présent
    QApplication::removeTranslator(m_p_trAppli);
    QApplication::removeTranslator(m_p_trQtLibs);
    //Si les settings contiennent une langue, on essaiera de la charger,
    //Sinon on essaie de charger la langue de l'OS
    if( settings.contains( SETTINGS_LANGUAGE ) ) {
        langCode = settings.value( SETTINGS_LANGUAGE ).toString();
    }
    else {
        langCode = QLocale::system().name().section('_', 0, 0); //récupération de la langue de l'os
    }

    //Si on n'a pas pu charger la langue, on bascule sur l'anglais
    if( !m_p_trAppli->load(QString( LANGUAGE_PREFIX_APPLI ) + langCode) ) { //Cela ne peut arriver que si settings JAMAIS encore configuré et sur un OS dont l langue n'est pas supportée
          langCode = QString("en");
          m_p_trAppli->load(QString( LANGUAGE_PREFIX_APPLI ) + langCode);
    }

    //Chargement du translator pour Qt et installation
    m_p_trQtLibs->load(QString( LANGUAGE_PREFIX_QT ) + langCode, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    QApplication::installTranslator(m_p_trAppli);
    QApplication::installTranslator(m_p_trQtLibs);

    initSupportedLanguages();//A placer après l'installation des translator afin d'avoir une traduction correcte

    //Indication au reste de l'application de la langue chargée
    CPlatform::setLanguage( langCode );

}


