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



#ifndef CPLATFORM_H
#define CPLATFORM_H

#include <QString>
#include <QStringList>
#include <QMap>
#include <QDir>
#include <QFont>
#include <QRegExp>
#include <QDate>

/*****************************
 * CPlatform
 * ----------------------
 * Classe permettant de grer les spcificits des diffrentes plateformes cibles
 ******************************/
class CPlatform
{
public:
    CPlatform();
    static QFont defaultFont();
    static QString applicationDirPath();        //Renvoie le rpertoire o a t install l'application
    static QString skinDirPath();               //Renvoie le rpertoire o se situent les skin fournies
    static QString revision();
    static int revisionInt();                   //Renvoie la rvision du projet
    static int buildDate();        //Renvoie la date de compilation
    static QString language();                  //Renvoie la langue de l'ui
    static QStringList languageList();          //Liste de toutes les langues disponibles
    static void setLanguage( const QString &language );       //Indique la langue de l'ui
	static QString appDataDir();				//Retourne le rpertoire o sont sauves les donnes persistantes du programme
    static QRegExp forbiddenCharacters();       //Retourne les caractres interdits (pour un nom de fichier par exemple)

    //Outils. Quand ils y en aura plus -> crer une classe CTools
    static QStringList getImagesInDir( const QDir &, QStringList filter = QStringList() ); //Retourne la liste des images d'un rpertoire
    static bool copyDirectory( QDir source, QDir destination, QString &errorMsg ); //Copie rcursive du contenu du rpertoire source vers le rpertoire destination
    static QString readTranslatedTextFile( const QString &baseName ); //Charge la bonne version d'un fichier texte prsent dans les ressources en fonction de la langue de l'ui

private:
    static const QRegExp m_forbiddenCharacters;
    static const QStringList m_languageList;
    static QString m_language;
    
};

#endif // CPLATFORM_H
